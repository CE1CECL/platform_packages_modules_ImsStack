/*
   Author
   <table>
   date      author                 description
   --------  --------------         ----------
   20100000  syed.malgimani@        Created
   20170110  vijay.nair@            Modified
   </table>

   Description

 */

#include "sip_pf_datatypes.h"
#include "platform/sip_pf_memory.h"
#include "platform/sip_pf_string.h"

#include "msg/sip_comdef.h"
#include "SipConfiguration.h"

#include "sip_error.h"
#include "SipTrace.h"
#include "sip_debug.h"

#include "txn/SipTimeoutData.h"
#include "txn/SipTxn.h"
#include "txn/SipTxnKey.h"
#include "txn/SipTxnFsmData.h"
#include "txn/SipTxnTimerValues.h"

#include "SipConfiguration.h"
#include "SipUtil.h"
#include "txn/SipTxnUtil.h"

/* Timer D value */
#define SIP_32Sec    32000

extern SIP_VOID* sip_cbk_createAckRequest(IN SIP_VOID* pvRespMsg, IN ISipUserData* pUserData);

static SIP_BOOL InvCliFsm_NullFxn
(
    IN     SipTxn        *pobjTxn,
    IN     SIP_VOID     *pvData,
    OUT SIP_UINT16     *pusError
)
{
    (void)pvData; (void)pobjTxn;
    SIP_DEBUG_STACKBUG(ESIPTRACE_MODTXN,
            "InvCliFsm_NullFxn: Invalid Handling",SIP_ZERO,SIP_ZERO);
    *pusError = ETXN_FSMEVENTERROR;
    return SIP_FALSE;
}

static SIP_BOOL sipInvCli_HandleFailureResp
(
    IN         SipTxn         *pobjTxn,
    IN_OUT     SipTxnFsmData *pobjFsmData,
    OUT        SIP_UINT16     *pusNewTxnState,
    OUT        SIP_UINT16     *pusError
)
{
    SipMessage* pobjSipAckMsg = reinterpret_cast<SipMessage*>(sip_cbk_createAckRequest(
            pobjFsmData->m_pobjSipMsgIn, pobjTxn->GetUserData()));

    if (pobjSipAckMsg == SIP_NULL)
    {
        pobjTxn->PrepareACK(pobjFsmData->m_pobjSipMsgIn, SIP_FALSE, &pobjSipAckMsg);
    }

    if (pobjSipAckMsg == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "sipInvCli_HandleFailureResp:Failed \n",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    const SipTxnTimerValues& pobjSipTxnTimers = pobjTxn->GetSipTxnTimers();
    SIP_UINT32 usDurationTD = pobjSipTxnTimers.GetTimerValue(SipTxn::TIMERD);

    /* On basis of Transport Start Timer */
    SIP_INT32 eTranspMsgSentProtocol = pobjTxn->GetMsgSentProto();

    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
            "sipInvCli_HandleFailureResp: Transport %d , TimerD: %d",
            eTranspMsgSentProtocol, usDurationTD);

    /* For Unreliable Transport */
    if (eTranspMsgSentProtocol == SipTransportInfo::PROTOCOL_UDP)
    {
        if (pobjTxn->StartTxnTimer(SipTxn::TIMERD, usDurationTD, pusError) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "sipInvCli_HandleFailureResp:Failed \n",
                    SIP_ZERO,SIP_ZERO);
            delete pobjSipAckMsg;
            return SIP_FALSE;
        }
        /* State Transition */
        *pusNewTxnState = SipTxn::INV_CLI_COMPLETED_ST;
    }
    else /* For Relaible Transport */
    {
        /* State Transition for Reliable Transport */
        *pusNewTxnState = SipTxn::INV_CLI_TERMINATED_ST;
    }

    /* OUT Paramet Set */
    pobjFsmData->m_pobjSendSipMsg = pobjSipAckMsg;

    return SIP_TRUE;
}

static SIP_BOOL InvCliFsm_IdleStSendInvReqEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    SipTxnKey* pobjNewTxnKey = new SipTxnKey(pobjTxn->GetTxnKey(), pusError);

    if ((pobjNewTxnKey == SIP_NULL) || (*pusError == E_ERR_PF_MALLOCFAILED))
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "InvCliFsm_IdleStSendInvReqEvt:pobjNewTxnKey memory fail",SIP_ZERO,SIP_ZERO);

        if (pobjNewTxnKey != SIP_NULL)
        {
            delete pobjNewTxnKey;
        }
        return SIP_FALSE;
    }

    if (sip_cbk_fetchTransaction(reinterpret_cast<SIP_VOID*>(pobjNewTxnKey), TXN_OPT_CREATE,
            SIP_NULL, reinterpret_cast<SIP_VOID**>(&pobjTxn)) == SIP_FALSE)
    {
        delete pobjNewTxnKey;
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "InvCliFsm_IdleStSendInvReqEvt:Adding Txn into DB Fails \n",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    /* TxnObj is added to hash, hence increment ref count */
    pobjTxn->increment();

    /* Start Timers
       1. Retx Timer : Retransmission Timer for UDP only
       2. Txn Timer  : Req Timeout Timer for any Transport
     */
    const SipTxnTimerValues& pobjSipTxnTimers = pobjTxn->GetSipTxnTimers();
    SIP_UINT32 usDurationT1 = pobjSipTxnTimers.GetTimerValue(SipTxn::TIMER1);
    SIP_UINT32 usDurationTB = pobjSipTxnTimers.GetTimerValue(SipTxn::TIMERB);
    SipTxnFsmData *pobjFsmData = (SipTxnFsmData *)pvData;
    SipTransportParameter    *pobjTranspParam = pobjFsmData->m_pobjTranspParam;
    SIP_INT32 eTranspProtocol = pobjTranspParam->GetTranspProtocol();

    /* For Unreliable Transport : Start Timer A*/
    if (eTranspProtocol == SipTransportInfo::PROTOCOL_UDP)
    {
        if (pobjTxn->StartTxnTimer(SipTxn::TIMERA,usDurationT1,pusError) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "InvCliFsm_IdleStSendInvReqEvt:StartTxnTimer A:Failed \n",
                    SIP_ZERO,SIP_ZERO);

            pobjTxn->RemoveFromTxnPool();
            return SIP_FALSE;
        }
    }
    else /* For Reliable Transport : Start Timer B*/
    {
        if (pobjTxn->StartTxnTimer(SipTxn::TIMERB,usDurationTB,pusError) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "StartTxnTimer B:Failed \n",
                    SIP_ZERO,SIP_ZERO);

            pobjTxn->RemoveFromTxnPool();
            return SIP_FALSE;
        }
    }
    pobjTxn->SetMaxDuration(usDurationTB);

    /* Set Userdata into Txn object */
    if (pobjFsmData->m_pobjUserData != SIP_NULL)
    {
        SIP_VOID        *pvTUdata = pobjFsmData->m_pobjUserData->GetUserData();
        ISipUserData    *pobjUserData = new ISipUserData(pvTUdata);
        if (pobjUserData == SIP_NULL)
        {
            pobjTxn->RemoveFromTxnPool();
            return SIP_FALSE;
        }
        pobjTxn->SetUserData(pobjUserData);
    }

    /* Fill FSM data for stack manager */
    pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    pobjFsmData->bTxnCreated = SIP_TRUE;

    /* State Transition */
    pobjTxn->SetTxnState(SipTxn::INV_CLI_CALLING_ST);

    return SIP_TRUE;
}

static SIP_BOOL InvCliFsm_CallingStTimerA_B_TimeoutEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    SipTimeoutData *pobjTimeoutData = (SipTimeoutData *)pvData;

    (void)pobjTimeoutData;

    SIP_UINT32 usDurationExpired = pobjTxn->GetDurationExpired();

    const SipTxnTimerValues& objSipTxnTimers = pobjTxn->GetSipTxnTimers();
    SIP_UINT32 usT1Val = objSipTxnTimers.GetTimerValue(SipTxn::TIMER1);

    if (usDurationExpired == 0)
    {
       //Timer T1 already fired.
       pobjTxn->IncrDurationExpired(usT1Val);
       usDurationExpired = usT1Val;
    }

    SIP_INT32 eTranspMsgSentProtocol = pobjTxn->GetMsgSentProto();
    SIP_UINT16 usReTxCount   = pobjTxn->GetReTxCount();
    SIP_UINT32 uiDuration = SIP_ZERO;
    SIP_UINT32 usMaxDuration = pobjTxn->GetMaxDuration();

    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
            "InvCliFSM_TimerA_B_TimeoutEvt: Transport %d, Duration: %d"
            ,eTranspMsgSentProtocol,usDurationExpired);

    /* For Unreliable Transport : Restart Timer */
    if (eTranspMsgSentProtocol == SipTransportInfo::PROTOCOL_UDP)
    {
        /* Check Max Retransmission Limit or Total expired
           time has crossed the Max duration limit or not */
        if (usDurationExpired >= usMaxDuration)
        {
            /* Stop Retransmissions : May notify StackUSer on Termination of Txn */
            SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                    "TimerA_B_TimeoutEvt: Txn Ends RextCount %d Duration %d",
                    usReTxCount,usDurationExpired);
            pobjTxn->SetTxnState(SipTxn::INV_CLI_TERMINATED_ST);
            uiDuration = SIP_ZERO; //Transaction to be timedout immediately.
        }
        else
        {
            /* Start Timer A with updated Duration */
            /* RFC 3261:17.1.1.2    Request is retransmitted with
               intervals that double after each transmission.
            */
            usReTxCount             = usReTxCount + SIP_ONE;
            SIP_UINT32 uiPow2     = SipNPower(SIP_TWO, usReTxCount);
            uiDuration = uiPow2 * usT1Val;

            //Update the timer duration.
            if ((usDurationExpired + uiDuration) >= usMaxDuration)
            {
                 uiDuration = usMaxDuration - usDurationExpired;
            }
        }
   }
   else /* For Relaible Transport */
   {
       SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
               "TimerA_B_TimeoutEvt: TCP Txn Ends  RextCount %d MaxDuration %d",
               usReTxCount,usMaxDuration);
       /* Terminate Transaction */
       pobjTxn->SetTxnState(SipTxn::INV_CLI_TERMINATED_ST);
       uiDuration = SIP_ZERO; //Transaction to be timedout immediately.
    }

    /* Start Timer A with updated Duration */
    if (uiDuration > SIP_ZERO)
    {
        if (pobjTxn->StartTxnTimer(SipTxn::TIMERA,uiDuration,pusError) == SIP_FALSE)
        {
           SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                   "StartTxnTimer A:Failed \n",
                   SIP_ZERO,SIP_ZERO);
           return SIP_FALSE;
        }
        pobjTxn->IncrTxnCount();
        pobjTxn->IncrDurationExpired(uiDuration);
        pobjTxn->SetCurrentDuration(uiDuration);
    }

    return SIP_TRUE;
}

static SIP_BOOL InvCliFsm_CallingStRecv1xxRespEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError;

    /* Stop existing Txn timer */
    pobjTxn->StopTxnTimer();

    /* Get user data */
    SipTxnFsmData *pobjFsmData = (SipTxnFsmData *)pvData;
    pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();

    /* Fill FSM data for stack manager */
    pobjFsmData->eTxnStatus = SipTxn::STATUS_VALID_MESSAGE;

    // RPR handling for retransmission
    SipMessage* pobjMsgIn = pobjFsmData->m_pobjSipMsgIn;
    SIP_INT16 uiStatusCode = SIP_ZERO;
    SipStatusLine* pobjStatusLine = pobjMsgIn->GetStatusLine();

    if (pobjStatusLine != SIP_NULL)
    {
        pobjStatusLine->GetStatusCode(&uiStatusCode);
        pobjStatusLine->SipDelete();
    }

    if (uiStatusCode != 100)
    {
        SIP_BOOL bRSeqExist = pobjMsgIn->HasHeader(SipHeaderBase::RSEQ);

        if (bRSeqExist == SIP_TRUE)
        {
            SipTxnKey* pobjRprTxnKey = new SipTxnKey(pobjMsgIn, pusError);

            if (SipTxnUtil::GetInstance()->AddTxnKey(pobjRprTxnKey) == SIP_FALSE)
            {
                delete pobjRprTxnKey;

                SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                        "InvCliFsm_CallingStRecv1xxRespEvt: RprTxnKey insertion failed",
                        SIP_ZERO, SIP_ZERO);
            }
        }
    }

    /* State Transition */
    pobjTxn->SetTxnState(SipTxn::INV_CLI_PROCEEDING_ST);

    return SIP_TRUE;
}

static SIP_BOOL InvCliFsm_CallingStRecv2xxRespEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError;

    /* Stop existing Txn timer */
    pobjTxn->StopTxnTimer();

    /* Get user data */
    SipTxnFsmData *pobjFsmData = (SipTxnFsmData *)pvData;
    pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();

    /* Fill FSM data for stack manager */
    pobjFsmData->eTxnStatus = SipTxn::STATUS_VALID_MESSAGE;
    pobjFsmData->bTxnTerminated = SIP_TRUE;

    /* State Transition */
    pobjTxn->SetTxnState(SipTxn::INV_CLI_TERMINATED_ST);

    return SIP_TRUE;
}

static SIP_BOOL InvCliFsm_CallingStRecv3xx6xxRespEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    /* Stop Timer A/B */
    pobjTxn->StopTxnTimer();

    /* Prepare failure ACK message and fill into pobjTxnFsmData.
       state txn is done in this fxn */
    SipTxnFsmData *pobjFsmData = (SipTxnFsmData *)pvData;
    SIP_UINT16     usNewTxnState = SIP_ZERO;

    if (sipInvCli_HandleFailureResp(pobjTxn, pobjFsmData, &usNewTxnState, pusError) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "InvCliFsm_CallingStRecv3xx6xxRespEvt :Failed \n",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    /* NOTE:
       pobjTxnFsmData is updated with failure ACK message that needs to be send to network.
     */

    if (usNewTxnState == SipTxn::INV_CLI_TERMINATED_ST)
    {
        pobjFsmData->bTxnTerminated = SIP_TRUE;
    }

    pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    pobjFsmData->eTxnStatus = SipTxn::STATUS_VALID_MESSAGE;

    /* state transition*/
    pobjTxn->SetTxnState(usNewTxnState);

    return SIP_TRUE;
}

static SIP_BOOL InvCliFsm_CallingStTranspErrorEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError;
    (void)pvData ; (void)pobjTxn;
    pobjTxn->StopTxnTimer();

    /* State Transition */
    pobjTxn->SetTxnState(SipTxn::INV_CLI_TERMINATED_ST);
    return SIP_TRUE;
}

static SIP_BOOL InvCliFsm_ProceedingStRecv1xxRespEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError;
    SipTxnFsmData *pobjFsmData = (SipTxnFsmData *)pvData;

    /* Get user data */
    pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    pobjFsmData->eTxnStatus = SipTxn::STATUS_VALID_MESSAGE;
    SipMessage *pobjMsgIn   = pobjFsmData->m_pobjSipMsgIn;

    SIP_INT16 uiStatusCode = SIP_ZERO;
    SipStatusLine* pobjStatusLine = pobjMsgIn->GetStatusLine();

    if (pobjStatusLine != SIP_NULL)
    {
        pobjStatusLine->GetStatusCode(&uiStatusCode);
        pobjStatusLine->SipDelete();
    }

    if (uiStatusCode != 100)
    {
        SIP_BOOL bRSeqExist = pobjMsgIn->HasHeader(SipHeaderBase::RSEQ);
        if ( bRSeqExist == SIP_TRUE)
        {
            SipTxnUtil *pObjSipTxnUtil = SipTxnUtil::GetInstance();
            SipTxnKey *pobjTempTxnKey = new SipTxnKey(pobjMsgIn, pusError);
            SipTxnKey *pobjINVTxnKey = pObjSipTxnUtil->SearchTxnKey(pobjTempTxnKey);
            if (pobjINVTxnKey != SIP_NULL)
            {
                pobjFsmData->eTxnStatus = SipTxn::STATUS_RETRANSMISSION;
                if (pobjTempTxnKey != SIP_NULL)
                {
                    delete pobjTempTxnKey;
                    pobjTempTxnKey = SIP_NULL;
                }
                SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                           "InvCliFsm_ProceedingStSendNon100ProvRespEvt: Retransmitted message.",
                           SIP_ZERO,SIP_ZERO);
            }
            else
            {
                pobjINVTxnKey = pObjSipTxnUtil->SearchTxnKey(pobjTempTxnKey, SIP_FALSE);
                if (pobjINVTxnKey != SIP_NULL)
                {
                    SIP_UINT32 nRseq = pobjTempTxnKey->GetRSeq();
                    delete pobjTempTxnKey;
                    pobjTempTxnKey = SIP_NULL;
                    if (pobjINVTxnKey->GetRSeq() + 1 != nRseq)
                    {
                        pobjFsmData->eTxnStatus = SipTxn::STATUS_STRAY_RESP;
                        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                                     "InvCliFsm_ProceedingStSendNon100ProvRespEvt: Stray message.",
                                     SIP_ZERO,SIP_ZERO);
                    }
                    else
                    {
                        pobjINVTxnKey->SetRSeq(nRseq);
                        pobjFsmData->eTxnStatus = SipTxn::STATUS_VALID_MESSAGE;
                        SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                                     "InvCliFsm_ProceedingStSendNon100ProvRespEvt: Rseq is proper.",
                                     SIP_ZERO,SIP_ZERO);
                    }
                }
                else
                {
                    if (pObjSipTxnUtil->AddTxnKey(pobjTempTxnKey) == SIP_FALSE)
                    {
                        delete pobjTempTxnKey;
                        pobjTempTxnKey = SIP_NULL;
                        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                                  "InvCliFsm_ProceedingStSendNon100ProvRespEvt: TxnKey insertion failed",
                               SIP_ZERO,SIP_ZERO);
                    }
                    pobjFsmData->eTxnStatus = SipTxn::STATUS_VALID_MESSAGE;
                }
            }
        }
    }
    /* Do Noting Update the Response Handle free Previous one */
    return SIP_TRUE;
}

static SIP_BOOL InvCliFsm_ProceedingStRecv2xxRespEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError;

    SipTxnUtil::GetInstance()->DeleteTxnKey(pobjTxn->GetTxnKey());

    /* Fill FSM data for stack manager */
    SipTxnFsmData *pobjFsmData = (SipTxnFsmData *)pvData;
    pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    pobjFsmData->eTxnStatus = SipTxn::STATUS_VALID_MESSAGE;
    pobjFsmData->bTxnTerminated = SIP_TRUE;

    /* State Transition */
    pobjTxn->SetTxnState(SipTxn::INV_CLI_TERMINATED_ST);

    return SIP_TRUE;
}

static SIP_BOOL InvCliFsm_ProceedingStRecv3xx6xxRespEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    SipTxnUtil::GetInstance()->DeleteTxnKey(pobjTxn->GetTxnKey());

    SipTxnFsmData *pobjFsmData = (SipTxnFsmData*) pvData;
    SIP_UINT16     usNewTxnState = SIP_ZERO;
    /* Prepare failure ACK message and fill into pobjTxnFsmData.
       state txn is done in this fxn */
    if (sipInvCli_HandleFailureResp(pobjTxn, pobjFsmData, &usNewTxnState, pusError) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "InvCliFsm_ProceedingStRecv3xx6xxRespEvt :Fail",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    /* NOTE:
       pobjTxnFsmData is updated with failure ACK message that needs to be send to network.
     */

    if (usNewTxnState == SipTxn::INV_CLI_TERMINATED_ST)
    {
        pobjFsmData->bTxnTerminated = SIP_TRUE;
    }

    pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    pobjFsmData->eTxnStatus = SipTxn::STATUS_VALID_MESSAGE;

    /* state transition*/
    pobjTxn->SetTxnState(usNewTxnState);

    return SIP_TRUE;
}

static SIP_BOOL InvCliFsm_ProceedingStTranspErrorEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError;
    (void)pvData;
    /* Transition to Terminated State */
    pobjTxn->SetTxnState(SipTxn::INV_CLI_TERMINATED_ST);

    return SIP_TRUE;
}

static SIP_BOOL InvCliFsm_CompletedStTimerD_TimeoutEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    SipTimeoutData *pobjTimeoutData = (SipTimeoutData *)pvData;

    (void)pusError;
    (void)pobjTimeoutData;

    /* Transition to Terminated State */
    pobjTxn->SetTxnState(SipTxn::INV_CLI_TERMINATED_ST);

    return SIP_TRUE;
}

static SIP_BOOL InvCliFsm_CompletedStRecv1xxRespEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError;
    SipTxnFsmData *pobjFsmData = (SipTxnFsmData *)pvData;

    /* Old response received , simply obsorved the messages*/

    pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    pobjFsmData->eTxnStatus = SipTxn::STATUS_IGNORE_RESP;

    /* old response received. remain in same state*/
    return SIP_TRUE;
}

static SIP_BOOL InvCliFsm_CompletedStRecv3xx6xxRespEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError;
    SipTxnFsmData *pobjFsmData = (SipTxnFsmData*) pvData;

    /* This is receive of re-transmitted failure response. Failure ACK shall be sent */
    pobjFsmData->eTxnStatus = SipTxn::STATUS_RETRANSMISSION;
    pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    pobjFsmData->m_pobjTranspInfo = pobjTxn->GetTranspInfo();

    /* Remain in same state*/
    return SIP_TRUE;
}

static SIP_BOOL InvCliFsm_CompletedStTranspErrorEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError;
    (void)pvData;

    pobjTxn->StopTxnTimer();
    pobjTxn->SetTxnState(SipTxn::INV_CLI_TERMINATED_ST);

    return SIP_TRUE;
}

SIP_BOOL (*gpfSipInvClientTxnFsm[SipTxn::INV_CLI_INVALID_ST+1][SipTxn::INV_CLI_INVALID_EVT+1])
(
     SipTxn    *pobjTxn,
     SIP_VOID    *pvData, /* Event specific data */
     SIP_UINT16 *pusError
) =
{
    /* IDLE State:: S0*/
    {
        InvCliFsm_IdleStSendInvReqEvt,    /* SipTxn::INV_CLI_SEND_INV_REQ_EVT */
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_TIMERA_B_TIME_OUT_EVT */
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_TIMERD_TIME_OUT_EVT */
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_RECV_1XX_RESP_EVT */
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_RECV_2XX_RESP_EVT */
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_RECV_3XX_6XX_RESP_EVT */
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_TRANSP_ERROR_EVT */
        InvCliFsm_NullFxn
    },
    /* CALLING State:: S1*/
    {
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_SEND_INV_REQ_EVT */
        InvCliFsm_CallingStTimerA_B_TimeoutEvt,/*TimerA_B_TimeoutEvt */
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_TIMERD_TIME_OUT_EVT */
        InvCliFsm_CallingStRecv1xxRespEvt,/* SipTxn::INV_CLI_RECV_1XX_RESP_EVT */
        InvCliFsm_CallingStRecv2xxRespEvt,/* SipTxn::INV_CLI_RECV_2XX_RESP_EVT */
        InvCliFsm_CallingStRecv3xx6xxRespEvt,/* Recv3xx6xxRespEvt */
        InvCliFsm_CallingStTranspErrorEvt,/* SipTxn::INV_CLI_TRANSP_ERROR_EVT */
        InvCliFsm_NullFxn
    },
    /* PROCEEDING State:: S2*/
    {
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_SEND_INV_REQ_EVT */
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_TIMERA_B_TIME_OUT_EVT */
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_TIMERD_TIME_OUT_EVT */
        InvCliFsm_ProceedingStRecv1xxRespEvt,/*Recv1xxRespEvt */
        InvCliFsm_ProceedingStRecv2xxRespEvt,/* Recv2xxRespEvt */
        InvCliFsm_ProceedingStRecv3xx6xxRespEvt,/* Recv3xx6xxRespEvt */
        InvCliFsm_ProceedingStTranspErrorEvt,/* TranspErrorEvt */
        InvCliFsm_NullFxn
    },
    /* COMPLETED State:: S3*/
    {
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_SEND_INV_REQ_EVT */
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_TIMERA_B_TIME_OUT_EVT */
        InvCliFsm_CompletedStTimerD_TimeoutEvt,    /* SipTxn::INV_CLI_TIMERD_TIME_OUT_EVT */
        InvCliFsm_CompletedStRecv1xxRespEvt,/* SipTxn::INV_CLI_RECV_1XX_RESP_EVT */
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_RECV_2XX_RESP_EVT */
        InvCliFsm_CompletedStRecv3xx6xxRespEvt,/* Recv3xx6xxRespEvt */
        InvCliFsm_CompletedStTranspErrorEvt,/* TranspErrorEvt */
        InvCliFsm_NullFxn
    },

    /* TERMINATED State:: S4*/
    {
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_SEND_INV_REQ_EVT */
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_TIMERA_B_TIME_OUT_EVT */
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_TIMERD_TIME_OUT_EVT */
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_RECV_1XX_RESP_EVT */
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_RECV_2XX_RESP_EVT */
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_RECV_3XX_6XX_RESP_EVT */
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_TRANSP_ERROR_EVT */
        InvCliFsm_NullFxn
    },
    /* Invalid State:: S5*/
    {
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_SEND_INV_REQ_EVT */
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_TIMERA_B_TIME_OUT_EVT */
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_TIMERD_TIME_OUT_EVT */
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_RECV_1XX_RESP_EVT */
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_RECV_2XX_RESP_EVT */
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_RECV_3XX_6XX_RESP_EVT */
        InvCliFsm_NullFxn,                    /* SipTxn::INV_CLI_TRANSP_ERROR_EVT */
        InvCliFsm_NullFxn
    }
};
