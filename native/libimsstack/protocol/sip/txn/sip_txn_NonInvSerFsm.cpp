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
#include "txn/SipTxnUtil.h"
#include "txn/SipTxnHandler.h"

#include "SipUtil.h"

static SIP_BOOL NonInvSerFsm_NullFxn
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError; (void)pvData; (void)pobjTxn;
    SIP_DEBUG_STACKBUG(ESIPTRACE_MODTXN,
            "NonInvSerFsm_NullFxn: Invalid Handling",
            SIP_ZERO,SIP_ZERO);
    return SIP_FALSE;
}

static SIP_BOOL NonInvSer_Send2xx6xxResp
(
    SipTxn        *pobjTxn,
    void            *pvData,
    SIP_UINT16    *pusNewTxnState,
    SIP_UINT16     *pusError
)
{
    SipTxnFsmData *pobjFsmData     = (SipTxnFsmData*) pvData;
    SipTransportParameter *pobjTransparam = pobjFsmData->m_pobjTranspParam;

    const SipTxnTimerValues& objSipTxnTimers = pobjTxn->GetSipTxnTimers();
    SIP_UINT32 uiDurationTJ = objSipTxnTimers.GetTimerValue(SipTxn::TIMERJ);

    SIP_INT32 eTranspMsgSentProtocol = pobjTransparam->GetTranspProtocol();
    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
            "NonInvSer_Send2xx6xxResp: Transport %d, TimerJ %d",
            eTranspMsgSentProtocol,uiDurationTJ);

    /* For Unreliable Transport : Start Timer J*/
    if (eTranspMsgSentProtocol == SipTransportInfo::PROTOCOL_UDP)
    {
        if (pobjTxn->StartTxnTimer(SipTxn::TIMERJ,uiDurationTJ,pusError) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "NonInvSer_Send2xx6xxResp: StartTxnTimer:Failed \n",
                    SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }
        pobjTxn->SetCurrentDuration(uiDurationTJ);
        *pusNewTxnState = SipTxn::NON_INV_SER_COMPLETED_ST;
    }
    /* For Reliable Transport : Terminate Txn */
    else
    {
        *pusNewTxnState = SipTxn::NON_INV_SER_TERMINATED_ST;
        return SIP_TRUE;
    }
    return SIP_TRUE;
}

static SIP_BOOL NonInvSerFsm_IdleStRecvNonInvReqEvt
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
                "NonInvSerFsm_IdleStRecvNonInvReqEvt:pobjNewTxnKey memory fail",
                SIP_ZERO,SIP_ZERO);

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
                "NonInvSerFsm_IdleStRecvNonInvReqEvt:Adding Txn into DB Fails \n",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    /* TxnObj is added to hash, hence increment ref count */
    pobjTxn->increment();

    SipTxnFsmData *pobjFsmData     = (SipTxnFsmData*) pvData;
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

    /* Fill FSM data for stack manager */
    pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    pobjFsmData->bTxnCreated = SIP_TRUE;
    pobjFsmData->eTxnStatus = SipTxn::STATUS_NEW_REQ_RECVD;

    /* State Transition */
    pobjTxn->SetTxnState(SipTxn::NON_INV_SER_TRYING_ST);

    SipMessage *pobjMsgIn = pobjFsmData->m_pobjSipMsgIn;
    SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
         "NonInvSerFsm_IdleStRecvNonInvReqEvt: Method : %d",
         pobjMsgIn->GetMethodType(),SIP_ZERO);

    /*If PRACK message received, stop 18x retransmission timer if any*/
    if (pobjMsgIn->GetMethodType() == SipMessage::METHOD_PRACK)
    {
        SipTxn *pobjInvSerTxn = SIP_NULL;//Corresponding INV Txn.

        SipTxnUtil *pObjSipTxnUtil = SipTxnUtil::GetInstance();
        SIP_UINT32 uiRseqNum = GetRSeqNum(pobjMsgIn, SipHeaderBase::RACK);
        pobjNewTxnKey->SetRSeq(uiRseqNum);
        SipTxnKey *pobjINVTxnKey = pObjSipTxnUtil->SearchTxnKey(pobjNewTxnKey);

        if (pobjINVTxnKey != SIP_NULL)
        {
            if (sip_cbk_fetchTransaction(reinterpret_cast<SIP_VOID*>(pobjINVTxnKey), TXN_OPT_FETCH,
                    SIP_NULL, reinterpret_cast<SIP_VOID**>(&pobjInvSerTxn)) == SIP_TRUE)
            {
                if (pobjInvSerTxn != SIP_NULL)
                {
                    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                        "NonInvSerFsm_IdleStRecvNonInvReqEvt: FETCH Success Stopping Timer.",
                        SIP_ZERO,SIP_ZERO);
                    pobjInvSerTxn->StopTxnTimer();
                }
                if (pObjSipTxnUtil->DeleteTxnKey(pobjINVTxnKey, SIP_TRUE) == SIP_FALSE)
                {
                    SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                        "NonInvSerFsm_IdleStRecvNonInvReqEvt:Deleting Txn Key from list Failed \n",
                        SIP_ZERO,SIP_ZERO);
                }
                pobjINVTxnKey = SIP_NULL;
            }
            else
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                     "NonInvSerFsm_IdleStRecvNonInvReqEvt, FETCH failure, can not stop Timer.",
                     SIP_ZERO,SIP_ZERO);
            }
        }
        else
        {
            pobjINVTxnKey = pObjSipTxnUtil->SearchTxnKey(pobjNewTxnKey, SIP_FALSE);

            if (pobjINVTxnKey != SIP_NULL)
            {
                // INVITE transaction exists,
                // but the PRACK request which is not matched with RSeq is received.
                // It needs to send 481 response.
                pobjFsmData->eTxnStatus = SipTxn::STATUS_STRAY_PRACK;
            }
            else
            {
                // INVITE server transaction is terminated by sending 200 OK,
                // but UA MUST be prepared to process PRACK request
                // if the processing of UA core is possible.
            }
        }
    }
    return SIP_TRUE;
}

static SIP_BOOL NonInvSerFsm_TryingStRecvNonInvReqEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError;
    SipTxnFsmData *pobjFsmData = (SipTxnFsmData*) pvData;

    /* This is receive of re-transmitted non-INVITE request.
       since previous req is already passed to TU and response is not been send, ignore the request */
    pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    pobjFsmData->eTxnStatus = SipTxn::STATUS_IGNORE_REQ;

    /* Remain in same state */
    return SIP_TRUE;
}

static SIP_BOOL NonInvSerFsm_TryingStSend1xxRespEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
 )
{
    (void)pusError;
    SipTxnFsmData *pobjFsmData     = (SipTxnFsmData*) pvData;

    /* Fill FSM data for stack manager */
    pobjFsmData->eTxnStatus = SipTxn::STATUS_VALID_MESSAGE;

    pobjTxn->SetTxnState(SipTxn::NON_INV_SER_PROCEEDING_ST);
    return SIP_TRUE;
}

static SIP_BOOL NonInvSerFsm_TryingStSend2xx6xxRespEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    SIP_UINT16    usNewTxnState = SIP_ZERO;

    /* handling of response and state transition occur inside the fxn */
    if (NonInvSer_Send2xx6xxResp(pobjTxn,pvData,&usNewTxnState,pusError) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "NonInvSerFsm_TryingStSend2xx6xxRespEvt: response processing fail",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SipTxnFsmData *pobjFsmData     = (SipTxnFsmData*) pvData;
    /* Fill FSM data for stack manager */
    if (usNewTxnState == SipTxn::NON_INV_SER_TERMINATED_ST)
    {
        pobjFsmData->bTxnTerminated = SIP_TRUE;
        pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    }
    pobjFsmData->eTxnStatus = SipTxn::STATUS_VALID_MESSAGE;

    pobjTxn->SetTxnState(usNewTxnState);

    return SIP_TRUE;
}

static SIP_BOOL NonInvSerFsm_TryingStTranspErrorEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError; (void) pvData; (void) pobjTxn;
    pobjTxn->SetTxnState(SipTxn::NON_INV_SER_TERMINATED_ST);
    return SIP_TRUE;
}

static SIP_BOOL NonInvSerFsm_ProceedingStRecvNonInvReqEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError;
    SipTxnFsmData *pobjFsmData = (SipTxnFsmData*) pvData;

    /* This is receive of re-transmitted non-INVITE request.
       stack manager to send last response */
    pobjFsmData->eTxnStatus = SipTxn::STATUS_RETRANSMISSION;
    pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    pobjFsmData->m_pobjTranspInfo = pobjTxn->GetTranspInfo();

    /* Remain in same state */
    return SIP_TRUE;
}

static SIP_BOOL NonInvSerFsm_ProceedingStSend1xxRespEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pobjTxn;
    (void)pusError;
    SipTxnFsmData *pobjFsmData     = (SipTxnFsmData*) pvData;

    pobjFsmData->eTxnStatus = SipTxn::STATUS_VALID_MESSAGE;

    /* No Change in State */
    return SIP_TRUE;
}

static SIP_BOOL NonInvSerFsm_ProceedingStSend2xx6xxRespEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    SIP_UINT16    usNewTxnState;

    /* handling of response and state transition occur inside the fxn */
    if (NonInvSer_Send2xx6xxResp(pobjTxn,pvData,&usNewTxnState,pusError) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "NonInvSerFsm_ProceedingStSend2xx6xxRespEvt: response processing fail",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SipTxnFsmData *pobjFsmData = (SipTxnFsmData*)pvData;
    /* Fill FSM data for stack manager */
    if (usNewTxnState == SipTxn::NON_INV_SER_TERMINATED_ST)
    {
        pobjFsmData->bTxnTerminated = SIP_TRUE;
        pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    }
    pobjFsmData->eTxnStatus = SipTxn::STATUS_VALID_MESSAGE;

    pobjTxn->SetTxnState(usNewTxnState);
    return SIP_TRUE;
}

static SIP_BOOL NonInvSerFsm_ProceedingStTranspErrorEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError; (void) pvData; (void)pobjTxn;
    pobjTxn->SetTxnState(SipTxn::NON_INV_SER_TERMINATED_ST);
    return SIP_TRUE;
}

static SIP_BOOL NonInvSerFsm_CompletedStRecvNonInvReqEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError;
    SipTxnFsmData *pobjFsmData = (SipTxnFsmData*) pvData;

    /* This is receive of re-transmitted non-INVITE request.
       stack manager to send last response */
    pobjFsmData->eTxnStatus = SipTxn::STATUS_RETRANSMISSION;
    pobjFsmData->m_pobjOutUserData = pobjTxn->GetUserData();
    pobjFsmData->m_pobjTranspInfo = pobjTxn->GetTranspInfo();

    /* Remain in same state */
    return SIP_TRUE;
}

static SIP_BOOL NonInvSerFsm_CompletedStTranspErrorEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    (void)pusError; (void)pvData, (void)pobjTxn;
    pobjTxn->StopTxnTimer();
    pobjTxn->SetTxnState(SipTxn::NON_INV_SER_TERMINATED_ST);
    return SIP_TRUE;
}

static SIP_BOOL NonInvSerFsm_CompletedStTimer_J_TimeoutEvt
(
    SipTxn        *pobjTxn,
    SIP_VOID     *pvData,
    SIP_UINT16     *pusError
)
{
    SipTimeoutData *pobjTimeoutData = (SipTimeoutData *)pvData;

    (void)pusError;
    (void)pobjTimeoutData;

    /* State Transition */
    pobjTxn->SetTxnState(SipTxn::NON_INV_SER_TERMINATED_ST);
    return SIP_TRUE;
}

SIP_BOOL (*gpfSipNonInvSerTxnFsm[SipTxn::NON_INV_SER_INVALID_ST+1]
                                 [SipTxn::NON_INV_SER_INVALID_EVT+1])
(
     SipTxn        *pobjTxn,
     SIP_VOID     *pvData,
     SIP_UINT16     *pusError
) =
{
    /* Idle State:: S0*/
    {
        NonInvSerFsm_IdleStRecvNonInvReqEvt,    /* SipTxn::NON_INV_SER_RECV_NON_INV_REQ_EVT */
        NonInvSerFsm_NullFxn,                    /* SipTxn::NON_INV_SER_SEND_1XX_RESP_EVT */
        NonInvSerFsm_NullFxn,                    /* SipTxn::NON_INV_SER_SEND_2XX_6XX_RESP_EVT */
        NonInvSerFsm_NullFxn,                    /* SipTxn::NON_INV_SER_TRANSP_ERROR_EVT */
        NonInvSerFsm_NullFxn,                    /* SipTxn::NON_INV_SER_TIMER_J_TIME_OUT_EVT */
        NonInvSerFsm_NullFxn
    },
    /* TRYING State:: S1*/
    {
        NonInvSerFsm_TryingStRecvNonInvReqEvt,    /* SipTxn::NON_INV_SER_RECV_NON_INV_REQ_EVT */
        NonInvSerFsm_TryingStSend1xxRespEvt,    /* SipTxn::NON_INV_SER_SEND_1XX_RESP_EVT */
        NonInvSerFsm_TryingStSend2xx6xxRespEvt,/* Send2xx6xxRespEvt */
        NonInvSerFsm_TryingStTranspErrorEvt,    /* SipTxn::NON_INV_SER_TRANSP_ERROR_EVT */
        NonInvSerFsm_NullFxn,                    /* SipTxn::NON_INV_SER_TIMER_J_TIME_OUT_EVT */
        NonInvSerFsm_NullFxn
    },
    /* PROCEEDING State:: S2*/
    {
        NonInvSerFsm_ProceedingStRecvNonInvReqEvt,    /* RecvNonInvReqEvt */
        NonInvSerFsm_ProceedingStSend1xxRespEvt,    /* Send1xxRespEvt */
        NonInvSerFsm_ProceedingStSend2xx6xxRespEvt,/* Send2xx6xxRespEvt */
        NonInvSerFsm_ProceedingStTranspErrorEvt,/* ranspErrorEvt */
        NonInvSerFsm_NullFxn,                    /* SipTxn::NON_INV_SER_TIMER_J_TIME_OUT_EVT */
        NonInvSerFsm_NullFxn
    },
    /* COMPLETED State:: S3*/
    {
        NonInvSerFsm_CompletedStRecvNonInvReqEvt, /* RecvNonInvReqEvt */
        NonInvSerFsm_NullFxn,                    /* SipTxn::NON_INV_SER_SEND_1XX_RESP_EVT */
        NonInvSerFsm_NullFxn,                    /* SipTxn::NON_INV_SER_SEND_2XX_6XX_RESP_EVT */
        NonInvSerFsm_CompletedStTranspErrorEvt,    /* SipTxn::NON_INV_SER_TRANSP_ERROR_EVT */
        NonInvSerFsm_CompletedStTimer_J_TimeoutEvt,/* Timer_J_TimeoutEvt */
        NonInvSerFsm_NullFxn
    },
    /* TERMINATED State:: S4*/
    {
        NonInvSerFsm_NullFxn,                    /* SipTxn::NON_INV_SER_RECV_NON_INV_REQ_EVT */
        NonInvSerFsm_NullFxn,                    /* SipTxn::NON_INV_SER_SEND_1XX_RESP_EVT */
        NonInvSerFsm_NullFxn,                    /* SipTxn::NON_INV_SER_SEND_2XX_6XX_RESP_EVT */
        NonInvSerFsm_NullFxn,                    /* SipTxn::NON_INV_SER_TRANSP_ERROR_EVT */
        NonInvSerFsm_NullFxn,                    /* SipTxn::NON_INV_SER_TIMER_J_TIME_OUT_EVT */
        NonInvSerFsm_NullFxn
    },
    /* Invalid State:: S5*/
    {
        NonInvSerFsm_NullFxn,                    /* SipTxn::NON_INV_SER_RECV_NON_INV_REQ_EVT */
        NonInvSerFsm_NullFxn,                    /* SipTxn::NON_INV_SER_SEND_1XX_RESP_EVT */
        NonInvSerFsm_NullFxn,                    /* SipTxn::NON_INV_SER_SEND_2XX_6XX_RESP_EVT */
        NonInvSerFsm_NullFxn,                    /* SipTxn::NON_INV_SER_TRANSP_ERROR_EVT */
        NonInvSerFsm_NullFxn,                    /* SipTxn::NON_INV_SER_TIMER_J_TIME_OUT_EVT */
        NonInvSerFsm_NullFxn
    }
};
