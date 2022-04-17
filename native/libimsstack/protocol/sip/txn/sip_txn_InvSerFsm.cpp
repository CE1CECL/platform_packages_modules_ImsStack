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
#include "sip_error.h"
#include "SipTrace.h"
#include "sip_debug.h"

#include "txn/SipTimeoutData.h"
#include "txn/SipTxn.h"
#include "txn/SipTxnKey.h"
#include "txn/SipTxnFsmData.h"
#include "txn/SipTxnUtil.h"
#include "txn/SipTxnHandler.h"


#include "SipUtil.h"

#define MIN(a,b) (a < b) ? a : b

/* RFC 3261: Sec 17.2.1
   The server transaction MUST generate a 100
   (Trying) response unless it knows that the TU will generate a
   provisional or final response within 200 ms */
#define SIP_MAX_TU_RESPONSE_TIME 200

SIP_BOOL InvSerFsm_NullFxn
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError;
    (void)pvData;
    (void)pobjTxn;
    SIP_DEBUG_STACKBUG(ESIPTRACE_MODTXN,
            "InvSerFsm_NullFxn: Invalid Handling", SIP_ZERO,SIP_ZERO);
    return SIP_FALSE;
}

static SIP_BOOL InvSerFsm_IdleStRecvInvReqEvt
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
                "InvSerFsm_IdleStRecvInvReqEvt:pobjNewTxnKey memory fail",SIP_ZERO,SIP_ZERO);

        if (pobjNewTxnKey != SIP_NULL)
        {
            delete pobjNewTxnKey;
        }
        return SIP_FALSE;
    }

    SIP_BOOL bStatus = sip_cbk_fetchTransaction(
            reinterpret_cast<SIP_VOID*>(pobjNewTxnKey),
            TXN_OPT_CREATE,
            SIP_NULL,
            reinterpret_cast<SIP_VOID**>(&pobjTxn));

    if (bStatus == SIP_FALSE)
    {
        delete pobjNewTxnKey;

        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "InvSerFsm_IdleStRecvInvReqEvt:Adding Txn into DB Fails \n",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    /* TxnObj is added to hash, hence increment ref count */
    pobjTxn->increment();

    /* Set Userdata into Txn object */
    SipTxnFsmData    *pobjFsmData = (SipTxnFsmData *)pvData;
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
    pobjFsmData->eTxnStatus = SipTxn::STATUS_NEW_REQ_RECVD;

    /* State Transition */
    pobjTxn->SetTxnState(SipTxn::INV_SER_PROCEEDING_ST);

    return SIP_TRUE;
}

static SIP_BOOL InvSerFsm_ProceedingStRecvInvReqEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16    *pusError
)
{
    (void)pusError;

    SipTxnFsmData    *pobjFsmData = (SipTxnFsmData *)pvData;
    /* This is receive of re-transmitted INVITE request. stack manager to send last response */
    pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    pobjFsmData->m_pobjTranspInfo = pobjTxn->GetTranspInfo();

    /* Check if TU has already send any 1xx response for INVITE else ignore the INVITE */
    if (pobjFsmData->m_pobjTranspInfo != SIP_NULL)
    {
        pobjFsmData->eTxnStatus = SipTxn::STATUS_RETRANSMISSION;
    }
    else
    {
        pobjFsmData->eTxnStatus = SipTxn::STATUS_IGNORE_REQ;
    }
    /* Remain in same state */
    return SIP_TRUE;
}

static SIP_BOOL InvSerFsm_ProceedingStSendNon100ProvRespEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    /* Stop existing Txn timer*/
    /* Delete the txn key in the util list*/
    pobjTxn->StopTxnTimer();
    SipTxnUtil::GetInstance()->DeleteTxnKey(pobjTxn->GetTxnKey());

    // Initialize the retransmission related information
    pobjTxn->InitRetransmissionInfo();

    /*Check whether RSeq header present or not*/
    SipTxnFsmData *pobjFsmData     = (SipTxnFsmData *)pvData;
    SipMessage *pobjMsgIn          = pobjFsmData->m_pobjSipMsgIn;
    SIP_BOOL bRSeqExist = pobjMsgIn->HasHeader(SipHeaderBase::RSEQ);

    /*Get the status code*/
    SIP_INT16 uiStatusCode = SIP_ZERO;
    SipStatusLine* pobjStatusLine = pobjMsgIn->GetStatusLine();

    if (pobjStatusLine != SIP_NULL)
    {
        pobjStatusLine->GetStatusCode(&uiStatusCode);
        pobjStatusLine->SipDelete();
    }

    SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
         "InvSerFsm_ProceedingStSendNon100ProvRespEvt, Status Code: %d, RSeq: %d.",
         uiStatusCode, bRSeqExist);
    // 18x / 199 response
    if (((uiStatusCode > 100) && (uiStatusCode < 200)) && (bRSeqExist == SIP_TRUE))
    {
        /*Create a duplicate txn key and store in a transaction list to match with PRACK*/
        SipTxnKey *pobjTxnKey = new SipTxnKey(pobjTxn->GetTxnKey(), pusError);
        if (pobjTxnKey != SIP_NULL)
        {
            SipTxnUtil *pObjSipTxnUtil = SipTxnUtil::GetInstance();
            SIP_UINT32 uiRseqNum = GetRSeqNum(pobjMsgIn, SipHeaderBase::RSEQ);
            pobjTxnKey->SetRSeq(uiRseqNum);
            if (pObjSipTxnUtil->AddTxnKey(pobjTxnKey) == SIP_FALSE)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "InvSerFsm_ProceedingStSendNon100ProvRespEvt: TxnKey insertion failed",
                    SIP_ZERO,SIP_ZERO);
            }
            else
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "InvSerFsm_ProceedingStSendNon100ProvRespEvt, Txnkey added to the utility list.",
                    SIP_ZERO,SIP_ZERO);
            }
        }

        const SipTxnTimerValues& objSipTxnTimers = pobjTxn->GetSipTxnTimers();
        SIP_UINT32 usDurationT1 = objSipTxnTimers.GetTimerValue(SipTxn::TIMER1);
        SIP_UINT32 usDurationTH = objSipTxnTimers.GetTimerValue(SipTxn::TIMERH);

        /*RFC 3262 mentions to start retransmission timer irrespective of transport being used*/
        /*Start retransmission timer*/
        if (pobjTxn->StartTxnTimer(SipTxn::TIMERG, usDurationT1, pusError) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
            "InvSerFsm_ProceedingStSendNon100ProvRespEvt: Start Timer Failed",
            SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }
        pobjTxn->SetMaxDuration(usDurationTH);
    }

    pobjFsmData->eTxnStatus = SipTxn::STATUS_VALID_MESSAGE;

    /* No Change in State, Be in proceeding state*/

    return SIP_TRUE;
}

static SIP_BOOL InvSerFsm_ProceedingStSend3xx6xxFailureRespEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    /* Stop existing Txn timer*/
    /* Delete the txn key in the util list*/
    pobjTxn->StopTxnTimer();
    SipTxnUtil::GetInstance()->DeleteTxnKey(pobjTxn->GetTxnKey());

    // Initialize the retransmission related information
    pobjTxn->InitRetransmissionInfo();

    const SipTxnTimerValues& objSipTxnTimers = pobjTxn->GetSipTxnTimers();
    SIP_UINT32 usDurationT1 = objSipTxnTimers.GetTimerValue(SipTxn::TIMER1);
    SIP_UINT32 usDurationTH = objSipTxnTimers.GetTimerValue(SipTxn::TIMERH);

    SipTxnFsmData    *pobjFsmData = (SipTxnFsmData *)pvData;
    SipTransportParameter *pobjTranspParam     = pobjFsmData->m_pobjTranspParam;
    SIP_INT32 eTranspProtocol = pobjTranspParam->GetTranspProtocol();

    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
            "InvSerFsm_ProceedingStSend3xx6xxFailureRespEvt: Transport %d",
            eTranspProtocol,SIP_ZERO);

    /* For Unreliable Transport : Start Timer G*/
    if (eTranspProtocol == SipTransportInfo::PROTOCOL_UDP)
    {
        if (pobjTxn->StartTxnTimer(SipTxn::TIMERG, usDurationT1, pusError) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "InvSerFsm_ProceedingStSend3xx6xxFailureRespEvt: Start Timer Failed",
                    SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }
    }
    else /*Reliable transport*/
    {
        if (pobjTxn->StartTxnTimer(SipTxn::TIMERH,usDurationTH,pusError) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "InvSerFsm_ProceedingStSend3xx6xxFailureRespEvt:Start Timer Failed",
                    SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }

        pobjTxn->IncrDurationExpired(usDurationTH);
    }

    pobjTxn->SetMaxDuration(usDurationTH);

    pobjFsmData->eTxnStatus = SipTxn::STATUS_VALID_MESSAGE;

    /* State Transition */
    pobjTxn->SetTxnState(SipTxn::INV_SER_COMPLETED_ST);

    return SIP_TRUE;
}

static SIP_BOOL InvSerFsm_ProceedingStSend2xxSuccessRespEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    /* Stop existing Txn timer*/
    /* Delete the txn key in the util list*/
    pobjTxn->StopTxnTimer();
    SipTxnUtil::GetInstance()->DeleteTxnKey(pobjTxn->GetTxnKey());

    // Initialize the retransmission related information
    pobjTxn->InitRetransmissionInfo();

    const SipTxnTimerValues& objSipTxnTimers = pobjTxn->GetSipTxnTimers();
    SIP_UINT32 usDurationTH = objSipTxnTimers.GetTimerValue(SipTxn::TIMERH);

    SipTxnFsmData    *pobjFsmData = (SipTxnFsmData *)pvData;
    if (pobjTxn->StartTxnTimer(SipTxn::TIMERH, usDurationTH, pusError) != SIP_FALSE)
    {
        pobjFsmData->eTxnStatus = SipTxn::STATUS_VALID_MESSAGE;
        pobjTxn->SetMaxDuration(usDurationTH);
        pobjTxn->IncrDurationExpired(usDurationTH);
        pobjTxn->SetTxnState(SipTxn::INV_SER_COMPLETED_ST);
    }
    else
    {
        pobjFsmData->eTxnStatus = SipTxn::STATUS_VALID_MESSAGE;
        pobjFsmData->bTxnTerminated = SIP_TRUE;
        pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
        pobjTxn->SetTxnState(SipTxn::INV_SER_TERMINATED_ST);
    }

    return SIP_TRUE;
}

static SIP_BOOL InvSerFsm_ProceedingStTranspErrorEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
 )
{
    (void)pvData;
    (void)pusError;
    pobjTxn->SetTxnState(SipTxn::INV_SER_TERMINATED_ST);
    return SIP_TRUE;
}

static SIP_BOOL InvSerFsm_ProceedingStTimerG_H_TimeoutEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    SipTimeoutData* pobjTimeoutData = (SipTimeoutData *)pvData;

    if (pobjTimeoutData == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "InvSerFsm_ProceedingStTranspErrorEvt : pobjTimeoutData is NULL",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_UINT32 uiDuration = SIP_ZERO;

    SIP_UINT32 usDurationExpired = pobjTxn->GetDurationExpired();
    const SipTxnTimerValues& objSipTxnTimers = pobjTxn->GetSipTxnTimers();
    SIP_UINT32 usT1Val = objSipTxnTimers.GetTimerValue(SipTxn::TIMER1);

    if (usDurationExpired == 0)
    {
       //Timer T1 already fired.
       pobjTxn->IncrDurationExpired(usT1Val);
       usDurationExpired = usT1Val;
    }

    SIP_UINT32 usMaxDuration = pobjTxn->GetMaxDuration();

    if (pobjTimeoutData->GetTimerType() == SipTxn::TIMERG)
    {
        /* Check Max Retransmission Limit or Total expired
           time has crossed the Max duration limit or not */
        if (usDurationExpired >= usMaxDuration)
        {
            pobjTxn->SetTxnState(SipTxn::INV_SER_TERMINATED_ST);
            uiDuration = SIP_ZERO;
            return SIP_TRUE;
        }
        else
        {
            SIP_UINT32 usCurrentDuration = pobjTxn->GetCurrentDuration();

            /* Double of timeout value for RPR */
            uiDuration = usCurrentDuration << 1;

            //Update the timer duration.
            if ((usDurationExpired + uiDuration) >= usMaxDuration)
            {
                 uiDuration = usMaxDuration - usDurationExpired;
            }
        }

        if (uiDuration > SIP_ZERO)
        {
            if (pobjTxn->StartTxnTimer(SipTxn::TIMERG, uiDuration,pusError) == SIP_FALSE)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                        "InvSerFsm_ProceedingStTimerG_H_TimeoutEvt: Start Timer Failed",
                        SIP_ZERO,SIP_ZERO);
                return SIP_FALSE;
            }
            pobjTxn->IncrTxnCount();
            pobjTxn->IncrDurationExpired(uiDuration);
            pobjTxn->SetCurrentDuration(uiDuration);
        }
    }

    return SIP_TRUE;
}

static SIP_BOOL InvSerFsm_CompletedStRecvInvReqEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16    *pusError
)
{
    (void)pusError;

    SipTxnFsmData    *pobjFsmData = (SipTxnFsmData *)pvData;

    /* This is receive of re-transmitted INVITE request. stack manager to send last response */
    pobjFsmData->eTxnStatus = SipTxn::STATUS_RETRANSMISSION;
    pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    pobjFsmData->m_pobjTranspInfo = pobjTxn->GetTranspInfo();

    /* Remain in same state */

    return SIP_TRUE;
}

static SIP_BOOL InvSerFsm_CompletedStTranspErrorEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pvData;
    (void)pusError;

    pobjTxn->StopTxnTimer();
    pobjTxn->SetTxnState(SipTxn::INV_SER_TERMINATED_ST);
    return SIP_TRUE;
}

static SIP_BOOL InvSerFsm_CompletedStRecvAckReqEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    pobjTxn->StopTxnTimer();

    SIP_INT32 eTranspMsgSentProtocol = pobjTxn->GetMsgSentProto();

    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
            "InvSerFsm_CompletedStRecvAckReqEvt: Transport %d",
            eTranspMsgSentProtocol,SIP_ZERO);

    SIP_UINT16    usNextState;
    SIP_UINT32    usDurationTI = 0;

    SipTxnFsmData    *pobjFsmData = (SipTxnFsmData *)pvData;
    /* For Unreliable Transport */
    if (eTranspMsgSentProtocol == SipTransportInfo::PROTOCOL_UDP)
    {
        /* Start Timer I */
        const SipTxnTimerValues& objSipTxnTimers = pobjTxn->GetSipTxnTimers();
        usDurationTI = objSipTxnTimers.GetTimerValue(SipTxn::TIMERI);

        usNextState = SipTxn::INV_SER_CONFIRMED_ST;

        pobjFsmData->eTxnStatus = SipTxn::STATUS_VALID_MESSAGE;
        pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    }
    else /* For Relaible Transport */
    {
        //TimerI = 0, for TCP.
        pobjFsmData->bTxnTerminated = SIP_TRUE;
        pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
        usNextState = SipTxn::INV_SER_TERMINATED_ST;
    }

    pobjTxn->SetTxnState(usNextState);

    SIP_BOOL bStatus = pobjTxn->StartTxnTimer(SipTxn::TIMERI, usDurationTI, pusError);
    if (bStatus == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "InvSerFsm_CompletedStRecvAckReqEvt:Start Timer Failed \n",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    return SIP_TRUE;
}

static SIP_BOOL InvSerFsm_CompletedStTimerG_H_TimeoutEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pvData;

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
    SIP_UINT32 usMaxDuration = pobjTxn->GetMaxDuration();

    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
            "InvSerFsm_CompletedStTimerG_H_TimeoutEvt: Transport %d",
            eTranspMsgSentProtocol,SIP_ZERO);

    SIP_UINT32 uiDuration = SIP_ZERO;
    /* For Unreliable Transport */
    if (eTranspMsgSentProtocol == SipTransportInfo::PROTOCOL_UDP)
    {
        /* Check Max Retransmission Limit or Total expired
           time has crossed the Max duration limit or not */
        if (usDurationExpired >= usMaxDuration)
        {
            /* Stop Retransmissions : May notify StackUSer on Termination of Txn */
            SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                    "CompletedStTimerG_H_TimeoutEvt: Txn Ends RextCount %d Duration %d",
                    pobjTxn->GetReTxCount(), usDurationExpired);

            pobjTxn->SetTxnState(SipTxn::INV_SER_TERMINATED_ST);
            uiDuration = SIP_ZERO; //Transaction to be timedout immediately.
        }
        else
        {
            SIP_UINT32 usCurrentDuration = pobjTxn->GetCurrentDuration();
            SIP_UINT32 usNextDuration = usCurrentDuration << 1;
            SIP_UINT32 usDurationT2 = objSipTxnTimers.GetTimerValue(SipTxn::TIMER2);

            /* MIN(2*T1, T2) seconds*/
            uiDuration = MIN(usNextDuration, usDurationT2);

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
                "CompletedStTimerG_H_TimeoutEvt: TCP Txn Ends RextCount %d MaxDuration %d",
                pobjTxn->GetReTxCount(), usMaxDuration);
        pobjTxn->SetTxnState(SipTxn::INV_SER_TERMINATED_ST);
        uiDuration = SIP_ZERO; //Transaction to be timedout immediately.
    }

    if (uiDuration > SIP_ZERO)
    {
        SIP_BOOL bStatus = pobjTxn->StartTxnTimer(SipTxn::TIMERG, uiDuration, pusError);
        if (bStatus == SIP_FALSE)
        {
            return SIP_FALSE;
        }
        pobjTxn->IncrTxnCount();
        pobjTxn->IncrDurationExpired(uiDuration);
        pobjTxn->SetCurrentDuration(uiDuration);
    }

    return SIP_TRUE;
}

static SIP_BOOL InvSerFsm_ConfirmedStRecvAckReqEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError;

    SipTxnFsmData    *pobjFsmData = (SipTxnFsmData *)pvData;

    /* Absorb ACK retranmsissions.*/
    /* RFC 3261: 17.2.1 INVITE Server Transaction
       The purpose of the "Confirmed" state is to absorb any additional ACK
       messages that arrive, triggered from retransmissions of the final
       response */

    /* Re-transmitted failure ACK is received. Simply absorved the message */

    /* Fill FSM data for stack manager */
    pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    pobjFsmData->eTxnStatus = SipTxn::STATUS_VALID_MESSAGE;

    /* Remain in same state */
    return SIP_TRUE;
}

static SIP_BOOL InvSerFsm_ConfirmedStTimerI_TimeoutEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError;
    (void)pvData;

    pobjTxn->SetTxnState(SipTxn::INV_SER_TERMINATED_ST);
    return SIP_TRUE;
}

SIP_BOOL (*gpfSipInvSerTxnFsm [SipTxn::INV_SER_INVALID_ST+1][SipTxn::INV_SER_INVALID_EVT+1])
(
    SipTxn        *pobjTxn,
    SIP_VOID        *pvData,
    SIP_UINT16    *pusError
) =
{
    /* IDLE State:: S0*/
    {
        InvSerFsm_IdleStRecvInvReqEvt,     /* SipTxn::INV_SER_RECV_INV_REQ_EVT */
        InvSerFsm_NullFxn,                 /* SipTxn::INV_SER_SEND_NON_100_PROV_RESP_EVT*/
        InvSerFsm_NullFxn,                 /* SipTxn::INV_SER_SEND_3XX_6XX_FAILURE_RESP_EVT*/
        InvSerFsm_NullFxn,                 /* SipTxn::INV_SER_SEND_2XX_SUCCESS_RESP_EVT*/
        InvSerFsm_NullFxn,                 /* SipTxn::INV_SER_TRANSP_ERROR_EVT*/
        InvSerFsm_NullFxn,                 /* SipTxn::INV_SER_RECV_ACK_REQ_EVT*/
        InvSerFsm_NullFxn,                /* SipTxn::INV_SER_TIMER_G_H_TIME_OUT_EVT*/
        InvSerFsm_NullFxn,                /* SipTxn::INV_SER_TIMER_I_TIME_OUT_EVT*/
        InvSerFsm_NullFxn
    },
    /* PROCEEDING State:: S1*/
    {
        InvSerFsm_ProceedingStRecvInvReqEvt, /* InvSerFsm_ProceedingStRecvInvReqEvt,
                                                SipTxn::INV_SER_RECV_INV_REQ_EVT */
        InvSerFsm_ProceedingStSendNon100ProvRespEvt, /* SendNon100ProvRespEvt*/
        InvSerFsm_ProceedingStSend3xx6xxFailureRespEvt, /* Send3xx6xxFailureRespEvt*/
        InvSerFsm_ProceedingStSend2xxSuccessRespEvt, /*Send2xxSuccessRespEvt*/
        InvSerFsm_ProceedingStTranspErrorEvt, /* SipTxn::INV_SER_TRANSP_ERROR_EVT*/
        InvSerFsm_NullFxn,                     /* SipTxn::INV_SER_RECV_ACK_REQ_EVT*/
        InvSerFsm_ProceedingStTimerG_H_TimeoutEvt, /* SipTxn::INV_SER_TIMER_G_H_TIME_OUT_EVT*/
        InvSerFsm_NullFxn,                    /* SipTxn::INV_SER_TIMER_I_TIME_OUT_EVT*/
        InvSerFsm_NullFxn
    },
    /* COMPLETE State:: S2*/
    {
        InvSerFsm_CompletedStRecvInvReqEvt,     /*InvSerFsm_CompletedStRecvInvReqEvt,
                                                  SipTxn::INV_SER_RECV_INV_REQ_EVT */
        InvSerFsm_NullFxn,                         /* SipTxn::INV_SER_SEND_NON_100_PROV_RESP_EVT*/
        InvSerFsm_NullFxn,                        /* SipTxn::INV_SER_SEND_3XX_6XX_FAILURE_RESP_EVT*/
        InvSerFsm_NullFxn,                        /* SipTxn::INV_SER_SEND_2XX_SUCCESS_RESP_EVT*/
        InvSerFsm_CompletedStTranspErrorEvt,     /* SipTxn::INV_SER_TRANSP_ERROR_EVT*/
        InvSerFsm_CompletedStRecvAckReqEvt,     /* SipTxn::INV_SER_RECV_ACK_REQ_EVT*/
        InvSerFsm_CompletedStTimerG_H_TimeoutEvt, /* TimerG_H_TimeoutEvt*/
        InvSerFsm_NullFxn,                        /* SipTxn::INV_SER_TIMER_I_TIME_OUT_EVT*/
        InvSerFsm_NullFxn
    },
    /* CONFIRMED State:: S3*/
    {
        InvSerFsm_NullFxn,                    /* SipTxn::INV_SER_RECV_INV_REQ_EVT */
        InvSerFsm_NullFxn,                     /* SipTxn::INV_SER_SEND_NON_100_PROV_RESP_EVT*/
        InvSerFsm_NullFxn,                     /* SipTxn::INV_SER_SEND_3XX_6XX_FAILURE_RESP_EVT*/
        InvSerFsm_NullFxn,                     /* SipTxn::INV_SER_SEND_2XX_SUCCESS_RESP_EVT*/
        InvSerFsm_NullFxn,                     /* SipTxn::INV_SER_TRANSP_ERROR_EVT*/
        InvSerFsm_ConfirmedStRecvAckReqEvt, /* SipTxn::INV_SER_RECV_ACK_REQ_EVT*/
        InvSerFsm_NullFxn,                     /* TimerG_H_TimeoutEvt*/
        InvSerFsm_ConfirmedStTimerI_TimeoutEvt,    /* SipTxn::INV_SER_TIMER_I_TIME_OUT_EVT*/
        InvSerFsm_NullFxn
    },

    /* TERMINATED State:: S4*/
    {
        InvSerFsm_NullFxn,                     /* SipTxn::INV_SER_RECV_INV_REQ_EVT */
        InvSerFsm_NullFxn,                     /* SipTxn::INV_SER_SEND_NON_100_PROV_RESP_EVT*/
        InvSerFsm_NullFxn,                     /* SipTxn::INV_SER_SEND_3XX_6XX_FAILURE_RESP_EVT*/
        InvSerFsm_NullFxn,                     /* SipTxn::INV_SER_SEND_2XX_SUCCESS_RESP_EVT*/
        InvSerFsm_NullFxn,                     /* SipTxn::INV_SER_TRANSP_ERROR_EVT*/
        InvSerFsm_NullFxn,                     /* SipTxn::INV_SER_RECV_ACK_REQ_EVT*/
        InvSerFsm_NullFxn,                     /* SipTxn::INV_SER_TIMER_G_H_TIME_OUT_EVT*/
        InvSerFsm_NullFxn,                    /* SipTxn::INV_SER_TIMER_I_TIME_OUT_EVT*/
        InvSerFsm_NullFxn
    },
    /* Invalid State:: S4*/
    {
        InvSerFsm_NullFxn,                     /* SipTxn::INV_SER_RECV_INV_REQ_EVT */
        InvSerFsm_NullFxn,                    /* SipTxn::INV_SER_SEND_NON_100_PROV_RESP_EVT*/
        InvSerFsm_NullFxn,                     /* SipTxn::INV_SER_SEND_3XX_6XX_FAILURE_RESP_EVT*/
        InvSerFsm_NullFxn,                     /* SipTxn::INV_SER_SEND_2XX_SUCCESS_RESP_EVT*/
        InvSerFsm_NullFxn,                     /* SipTxn::INV_SER_TRANSP_ERROR_EVT*/
        InvSerFsm_NullFxn,                     /* SipTxn::INV_SER_RECV_ACK_REQ_EVT*/
        InvSerFsm_NullFxn,                     /* SipTxn::INV_SER_TIMER_G_H_TIME_OUT_EVT*/
        InvSerFsm_NullFxn,                    /* SipTxn::INV_SER_TIMER_I_TIME_OUT_EVT*/
        InvSerFsm_NullFxn
    }
};
