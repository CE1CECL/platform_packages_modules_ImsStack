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
#include "txn/SipTxnHandler.h"

#include "SipUtil.h"
#include "txn/SipTxnUtil.h"

#define MIN(a,b) (a < b) ? a : b

static SIP_BOOL NonInvCliFsm_NullFxn
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError;
    (void)pobjTxn;
    (void)pvData;

    SIP_DEBUG_STACKBUG(ESIPTRACE_MODTXN,
            "NonInvCliFsm_NullFxn: Invalid Handling",
            SIP_ZERO,SIP_ZERO);
    return SIP_FALSE;
}

static SIP_BOOL NonInvClient_TimeoutHandling
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void) pvData;

    const SipTxnTimerValues& objSipTxnTimers = pobjTxn->GetSipTxnTimers();
    SIP_UINT32 usT1Val = objSipTxnTimers.GetTimerValue(SipTxn::TIMER1);

    SIP_UINT32 usDurationExpired = pobjTxn->GetDurationExpired();
    if (usDurationExpired == 0)
    {
       //Timer T1 already fired.
       pobjTxn->IncrDurationExpired(usT1Val);
       usDurationExpired = usT1Val;
    }

    SIP_INT32 eTranspMsgSentProtocol = pobjTxn->GetMsgSentProto();
    SIP_UINT32 usMaxDuration = pobjTxn->GetMaxDuration();

    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
            "NonInvClient_TimeoutHandling: Transport %d, Duration: %d"
            ,eTranspMsgSentProtocol,usDurationExpired);

    SIP_UINT32 uiDuration     = SIP_ZERO;
    /* For Unreliable Transport */
    if (eTranspMsgSentProtocol == SipTransportInfo::PROTOCOL_UDP)
    {
        /* Check Max Retransmission Limit or Total expired
           time has crossed the Max duration limit or not */
        if (usDurationExpired >= usMaxDuration)
        {
            /* Stop Retransmissions : May notify StackUSer on Termination of Txn */
            SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                    "NonInvClient_TimeoutHandling: Txn Ends RextCount %d Duration %d",
                    pobjTxn->GetReTxCount(), usDurationExpired);
            pobjTxn->SetTxnState(SipTxn::NON_INV_CLI_TERMINATED_ST);
            uiDuration = SIP_ZERO; //Transaction to be timedout immediately.
        }
        else
        {
            /* Restarting of Timer is based on the current state
               RFC 3261: Sec17.1.2.1
               If timer E fires while Trying state, then MIN(2*T1, T2).
               If Timer E fires while in the "Proceeding" state,then reset with a value of T2 seconds.
            */
            SIP_UINT16 usCurTxnState = pobjTxn->GetTxnState();
            SIP_UINT32 usDurationT2 = objSipTxnTimers.GetTimerValue(SipTxn::TIMER2);
            if (usCurTxnState == SipTxn::NON_INV_CLI_TRYING_ST)
            {
                SIP_UINT32 usCurrentDuration = pobjTxn->GetCurrentDuration();
                SIP_UINT32 usNextDuration = usCurrentDuration << 1;

                /* MIN(2*T1, T2) seconds*/
                uiDuration = MIN(usNextDuration, usDurationT2);
             }
             else /* SipTxn::NON_INV_CLI_PROCEEDING_ST */
             {
                uiDuration = usDurationT2;
             }

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
                "NonInvClient_TimeoutHandling_E_H: TCP Txn Ends RextCount %d MaxDuration %d",
                pobjTxn->GetReTxCount(), usMaxDuration);
        pobjTxn->SetTxnState(SipTxn::NON_INV_CLI_TERMINATED_ST);
        uiDuration = SIP_ZERO; //Transaction to be timedout immediately.
    }

    /* Start Timer E with updated Duration */
    if (uiDuration > SIP_ZERO)
    {
        SIP_BOOL bTimerStatus = pobjTxn->StartTxnTimer(SipTxn::TIMERE, uiDuration,pusError);
        if (bTimerStatus == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                   "NonInvCli_Recv2xx6xxResp:StartTxnTimer E, Failed\n",
                   SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }
        pobjTxn->IncrTxnCount();
        pobjTxn->IncrDurationExpired(uiDuration);
        pobjTxn->SetCurrentDuration(uiDuration);
    }

    return SIP_TRUE;
}

static SIP_BOOL NonInvCli_Recv2xx6xxResp
(
    SipTxn        *pobjTxn,
    SIP_UINT16    *pusNewTxnState,
    SIP_UINT16     *pusError
)
{
    pobjTxn->StopTxnTimer();

    SIP_INT32 eTranspMsgSentProtocol =
        pobjTxn->GetMsgSentProto();
    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
            "NonInvCli_Recv2xx6xxResp: Transp Protocol %d",
            eTranspMsgSentProtocol,SIP_ZERO);

    /* For Unreliable Transport */
    if (eTranspMsgSentProtocol == SipTransportInfo::PROTOCOL_UDP)
    {
        /* Start Timer K */
        const SipTxnTimerValues& objSipTxnTimers = pobjTxn->GetSipTxnTimers();
        SIP_UINT32 usDurationTK = objSipTxnTimers.GetTimerValue(SipTxn::TIMERK);
        SIP_BOOL bStatus  = pobjTxn->StartTxnTimer(SipTxn::TIMERK,usDurationTK,pusError);
        SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,"NonInvCli_Recv2xx6xxResp: Timer start returned",
                  SIP_ZERO,SIP_ZERO);
        if (bStatus == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                  "NonInvCli_Recv2xx6xxResp:StartTxnTimer K, Failed\n",
                  SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }
        pobjTxn->SetCurrentDuration(usDurationTK);
        *pusNewTxnState = SipTxn::NON_INV_CLI_COMPLETED_ST;
    }
    else /* For Relaible Transport */
    {
        *pusNewTxnState = SipTxn::NON_INV_CLI_TERMINATED_ST;
        return SIP_TRUE;
    }

    return SIP_TRUE;
}

static SIP_BOOL NonInvCliFsm_IdleStSendNonInvReqEvt
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
                "NonInvCliFsm_IdleStSendNonInvReqEvt:pobjNewTxnKey memory fail",SIP_ZERO,SIP_ZERO);

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
                "NonInvCliFsm_IdleStSendNonInvReqEvt:Adding Txn into DB Fails \n",
                SIP_ZERO,SIP_ZERO);

        return SIP_FALSE;
    }

    /* TxnObj is added to hash, hence increment ref count */
    pobjTxn->increment();

    /* Start Timer
       1. Retx Timer : for UDP only
       2. Transaction Timer : for TCP
     */
    const SipTxnTimerValues& objSipTxnTimers = pobjTxn->GetSipTxnTimers();
    SIP_UINT32 usDurationT1 = objSipTxnTimers.GetTimerValue(SipTxn::TIMER1);
    SIP_UINT32 usDurationTF = objSipTxnTimers.GetTimerValue(SipTxn::TIMERF);
    SipTxnFsmData *pobjFsmData     = (SipTxnFsmData *)pvData;
    SipTransportParameter *pobjTranspParam = pobjFsmData->m_pobjTranspParam;
    SIP_INT32 eTranspProtocol = pobjTranspParam->GetTranspProtocol();

    /* For Unreliable Transport : Start Timer E*/
    if (eTranspProtocol == SipTransportInfo::PROTOCOL_UDP)
    {
        bStatus = pobjTxn->StartTxnTimer(SipTxn::TIMERE,usDurationT1,pusError);
        if (bStatus == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "NonInvCliFsm_IdleStSendNonInvReqEvt:StartTxnTimer:Failed \n",
                    SIP_ZERO,SIP_ZERO);
            pobjTxn->RemoveFromTxnPool();
            return SIP_FALSE;
        }
    }
    else /* For Reliable Transport : Start Timer F*/
    {
        bStatus = pobjTxn->StartTxnTimer(SipTxn::TIMERF,usDurationTF,pusError);
        if (bStatus == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "NonInvCliFsm_IdleStSendNonInvReqEvt:StartTxnTimer:Failed \n",
                    SIP_ZERO,SIP_ZERO);
            pobjTxn->RemoveFromTxnPool();
            return SIP_FALSE;
        }
    }

    pobjTxn->SetMaxDuration(usDurationTF);

    /* Set user data in Txn Obj */
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
    else
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "pobjFsmData->m_pobjUserData is NULL",SIP_ZERO,SIP_ZERO);
    }

    /* Fill FSM data for stack manager */
    pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    pobjFsmData->bTxnCreated = SIP_TRUE;

    /* State Transition */
    pobjTxn->SetTxnState(SipTxn::NON_INV_CLI_TRYING_ST);

    if (SipPf_Strcmp(pobjNewTxnKey->GetMethod(), CANCEL_METHOD) == SIP_EQUALS)
    {
        SipTxnUtil::GetInstance()->DeleteTxnKey(pobjTxn->GetTxnKey());
    }
    return SIP_TRUE;
}

static SIP_BOOL NonInvCliFsm_TryingStTimer_E_F_TimeoutEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    SipTimeoutData *pobjTimeoutData = (SipTimeoutData *)pvData;

    (void)pobjTimeoutData;

    /* handling of timeout and state transition occure inside the function */
    SIP_BOOL bStatus = NonInvClient_TimeoutHandling(pobjTxn,pvData,pusError);

    if (bStatus == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "NonInvCli:Timeout handling failed",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    return SIP_TRUE;
}


static SIP_BOOL NonInvCliFsm_TryingStRecv1xxRespEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError;
    SipTxnFsmData *pobjFsmData = (SipTxnFsmData *)pvData;

    /* Fill FSM data for stack manager */
    pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    pobjFsmData->eTxnStatus = SipTxn::STATUS_VALID_MESSAGE;
    pobjTxn->SetTxnState(SipTxn::NON_INV_CLI_PROCEEDING_ST);
    return SIP_TRUE;
}

static SIP_BOOL NonInvCliFsm_TryingStRecv2xx6xxRespEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    SIP_UINT16    usNewTxnState = SIP_ZERO;

    /* state transition occur inside the function */
    SIP_BOOL bStatus = NonInvCli_Recv2xx6xxResp(pobjTxn,&usNewTxnState,pusError);
    if (bStatus == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "NonInvCli:2xx-6xx handling failed",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SipTxnFsmData *pobjFsmData = (SipTxnFsmData *)pvData;
    /* Fill FSM data for stack manager */
    if (usNewTxnState == SipTxn::NON_INV_CLI_TERMINATED_ST)
    {
        pobjFsmData->bTxnTerminated = SIP_TRUE;
    }

    pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    pobjFsmData->eTxnStatus = SipTxn::STATUS_VALID_MESSAGE;

    pobjTxn->SetTxnState(usNewTxnState);

    return SIP_TRUE;
}

static SIP_BOOL NonInvCliFsm_TryingStTranspErrorEvt
(
 SipTxn        *pobjTxn,
 SIP_VOID     *pvData,
 SIP_UINT16     *pusError
)
{
    (void)pusError;
    (void)pvData;

    pobjTxn->StopTxnTimer();
    pobjTxn->SetTxnState(SipTxn::NON_INV_CLI_TERMINATED_ST);
    return SIP_TRUE;
}

static SIP_BOOL NonInvCliFsm_ProceedingStTimer_E_F_TimeoutEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    SipTimeoutData *pobjTimeoutData = (SipTimeoutData *)pvData;
    (void)pobjTimeoutData;

    /* handling of timeout and state transition occure inside the function */
    SIP_BOOL bStatus = NonInvClient_TimeoutHandling(pobjTxn,pvData,pusError);

    if (bStatus == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "NonInvCliFsm_ProceedingStTimer_E_F_TimeoutEvt:Timeout handling failed \n",
                SIP_ZERO,SIP_ZERO);

        return SIP_FALSE;
    }
    return SIP_TRUE;
}

static SIP_BOOL NonInvCliFsm_ProceedingStRecv1xxRespEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError;
    SipTxnFsmData *pobjFsmData = (SipTxnFsmData *)pvData;

    /* Fill FSM data for stack manager */
    pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    pobjFsmData->eTxnStatus = SipTxn::STATUS_VALID_MESSAGE;

    /* remain in same state*/
    return SIP_TRUE;
}

static SIP_BOOL NonInvCliFsm_ProceedingStRecv2xx6xxRespEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    SIP_UINT16    usNewTxnState = SIP_ZERO;

    /* state transition occur inside the function */
    SIP_BOOL bStatus = NonInvCli_Recv2xx6xxResp(pobjTxn,&usNewTxnState,pusError);
    if (bStatus == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "NonInvCliFsm_ProceedingStRecv2xx6xxRespEvt:Resp handling failed \n",
                SIP_ZERO,SIP_ZERO);

        return SIP_FALSE;
    }

    SipTxnFsmData *pobjFsmData = (SipTxnFsmData *)pvData;
    /* Fill FSM data for stack manager */
    if (usNewTxnState == SipTxn::NON_INV_CLI_TERMINATED_ST)
    {
        pobjFsmData->bTxnTerminated = SIP_TRUE;
    }

    pobjFsmData->eTxnStatus = SipTxn::STATUS_VALID_MESSAGE;
    pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    pobjTxn->SetTxnState(usNewTxnState);
    return SIP_TRUE;
}

static SIP_BOOL NonInvCliFsm_ProceedingStTranspErrorEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError;
    (void)pvData;
    pobjTxn->StopTxnTimer();
    pobjTxn->SetTxnState(SipTxn::NON_INV_CLI_TERMINATED_ST);
    return SIP_TRUE;
}

static SIP_BOOL NonInvCliFsm_CompletedStTimer_K_TimeoutEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError;
    SipTimeoutData *pobjTimeoutData = (SipTimeoutData *)pvData;
    (void)pobjTimeoutData;

    /* State Transition */
    pobjTxn->SetTxnState(SipTxn::NON_INV_CLI_TERMINATED_ST);
    return SIP_TRUE;
}

static SIP_BOOL NonInvCliFsm_CompletedStRecv1xxRespEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError;
    SipTxnFsmData *pobjFsmData = (SipTxnFsmData *)pvData;

    /* Fill FSM data for stack manager */
    pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    pobjFsmData->eTxnStatus = SipTxn::STATUS_IGNORE_RESP;

    /* remain in same state*/
    return SIP_TRUE;
}

static SIP_BOOL NonInvCliFsm_CompletedStRecv2xx6xxRespEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError;
    SipTxnFsmData *pobjFsmData = (SipTxnFsmData *)pvData;

    /* Fill FSM data for stack manager */
    pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    pobjFsmData->eTxnStatus = SipTxn::STATUS_IGNORE_RESP;

    /* remain in same state*/
    return SIP_TRUE;
}

static SIP_BOOL NonInvCliFsm_CompletedStTranspErrorEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError;
    (void)pvData;
    pobjTxn->StopTxnTimer();
    pobjTxn->SetTxnState(SipTxn::NON_INV_CLI_TERMINATED_ST);

    return SIP_TRUE;
}

SIP_BOOL (*gpfSipNonInvClientTxnFsm[SipTxn::NON_INV_CLI_INVALID_ST+1]
                                    [SipTxn::NON_INV_CLI_INVALID_EVT+1])
(
     SipTxn        *pobjTxn,
     SIP_VOID     *pvData,
     SIP_UINT16     *pusError
) =
{
    /* Idle State:: S0*/
    {
        NonInvCliFsm_IdleStSendNonInvReqEvt,     /* SendNonInvReq_Evt */
        NonInvCliFsm_NullFxn,                    /* SipTxn::NON_INV_CLI_TIMER_E_F_TIME_OUT_EVT */
        NonInvCliFsm_NullFxn,                    /* SipTxn::NON_INV_CLI_RECV_1XX_RESP_EVT */
        NonInvCliFsm_NullFxn,                    /* SipTxn::NON_INV_CLI_RECV_2XX_6XX_RESP_EVT */
        NonInvCliFsm_NullFxn,                    /* SipTxn::NON_INV_CLI_TRANSP_ERROR_EVT */
        NonInvCliFsm_NullFxn,                    /* SipTxn::NON_INV_CLI_TIMER_K_TIME_OUT_EVT */
        NonInvCliFsm_NullFxn

    },
    /* TRYING State:: S1*/
    {
        NonInvCliFsm_NullFxn,                     /* SipTxn::NON_INV_CLI_SEND_NON_INV_REQ_EVT */
        NonInvCliFsm_TryingStTimer_E_F_TimeoutEvt,/* Timer_E_F_TimeoutEvt */
        NonInvCliFsm_TryingStRecv1xxRespEvt,    /* SipTxn::NON_INV_CLI_RECV_1XX_RESP_EVT */
        NonInvCliFsm_TryingStRecv2xx6xxRespEvt,    /* Recv2xx6xxRespEvt */
        NonInvCliFsm_TryingStTranspErrorEvt,    /* SipTxn::NON_INV_CLI_TRANSP_ERROR_EVT */
        NonInvCliFsm_NullFxn,                    /* SipTxn::NON_INV_CLI_TIMER_K_TIME_OUT_EVT */
        NonInvCliFsm_NullFxn

    },
    /* PROCEEDING State:: S2*/
    {
        NonInvCliFsm_NullFxn,                     /* SipTxn::NON_INV_CLI_SEND_NON_INV_REQ_EVT */
        NonInvCliFsm_ProceedingStTimer_E_F_TimeoutEvt,/* Timer_E_F_TimeoutEvt */
        NonInvCliFsm_ProceedingStRecv1xxRespEvt,/* Recv1xxRespEvt */
        NonInvCliFsm_ProceedingStRecv2xx6xxRespEvt,/* Recv2xx6xxRespEvt */
        NonInvCliFsm_ProceedingStTranspErrorEvt,/* TranspErrorEvt */
        NonInvCliFsm_NullFxn,                    /* SipTxn::NON_INV_CLI_TIMER_K_TIME_OUT_EVT */
        NonInvCliFsm_NullFxn

    },
    /* COMPLETED State:: S3*/
    {
        NonInvCliFsm_NullFxn,                     /* SipTxn::NON_INV_CLI_SEND_NON_INV_REQ_EVT */
        NonInvCliFsm_NullFxn,                    /* SipTxn::NON_INV_CLI_TIMER_E_F_TIME_OUT_EVT */
        NonInvCliFsm_CompletedStRecv1xxRespEvt,/* SipTxn::NON_INV_CLI_RECV_1XX_RESP_EVT */
        NonInvCliFsm_CompletedStRecv2xx6xxRespEvt,/* SipTxn::NON_INV_CLI_RECV_2XX_6XX_RESP_EVT */
        NonInvCliFsm_CompletedStTranspErrorEvt,    /* SipTxn::NON_INV_CLI_TRANSP_ERROR_EVT*/
        NonInvCliFsm_CompletedStTimer_K_TimeoutEvt,/* Timer_K_TimeoutEvt*/
        NonInvCliFsm_NullFxn
    },
    /* TERMINATED State:: S4*/
    {
        NonInvCliFsm_NullFxn,                     /* SipTxn::NON_INV_CLI_SEND_NON_INV_REQ_EVT */
        NonInvCliFsm_NullFxn,                    /* SipTxn::NON_INV_CLI_TIMER_E_F_TIME_OUT_EVT */
        NonInvCliFsm_NullFxn,                    /* SipTxn::NON_INV_CLI_RECV_1XX_RESP_EVT */
        NonInvCliFsm_NullFxn,                    /* SipTxn::NON_INV_CLI_RECV_2XX_6XX_RESP_EVT */
        NonInvCliFsm_NullFxn,                    /* SipTxn::NON_INV_CLI_TRANSP_ERROR_EVT*/
        NonInvCliFsm_NullFxn,                    /* SipTxn::NON_INV_CLI_TIMER_K_TIME_OUT_EVT */
        NonInvCliFsm_NullFxn
    },
    /* Invalid State:: S5*/
    {
        NonInvCliFsm_NullFxn,                     /* SipTxn::NON_INV_CLI_SEND_NON_INV_REQ_EVT */
        NonInvCliFsm_NullFxn,                    /* SipTxn::NON_INV_CLI_TIMER_E_F_TIME_OUT_EVT */
        NonInvCliFsm_NullFxn,                    /* SipTxn::NON_INV_CLI_RECV_1XX_RESP_EVT */
        NonInvCliFsm_NullFxn,                    /* SipTxn::NON_INV_CLI_RECV_2XX_6XX_RESP_EVT */
        NonInvCliFsm_NullFxn,                    /* SipTxn::NON_INV_CLI_TRANSP_ERROR_EVT*/
        NonInvCliFsm_NullFxn,                    /* SipTxn::NON_INV_CLI_TIMER_K_TIME_OUT_EVT */
        NonInvCliFsm_NullFxn
    }
};
