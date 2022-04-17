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

#include "txn/SipTxn.h"
#include "txn/sip_txn_fsm.h"

#include "SipConfiguration.h"
#include "sip_debug.h"
#include "SipTrace.h"
#include "SipUtil.h"
#include "platform/sip_pf_string.h"
#include "txn/SipTimeoutData.h"

#ifdef SIP_TRACE_ENABLE
static SIP_CHAR szInvClientTxnFsmSt[SipTxn::INV_CLI_INVALID_ST + 1][SIP_15] =
{
    "IdleSt",
    "CallingSt",
    "ProceedingSt",
    "CompletedSt",
    "TerminatedSt",
    "InvalidSt"
};

static SIP_CHAR szInvClientTxnFsmEvt[SipTxn::INV_CLI_INVALID_EVT+1][SIP_20] =
{
    "SendInvReqEvt",
    "TimerA_B_TimeoutEvt",
    "TimerD_TimeoutEvt",
    "Recv1xxRespEvt",
    "Recv2xxRespEvt",
    "Recv3xx6xxRespEvt",
    "TranspErrorEvt",
    "InvalidEvt"
};

static SIP_CHAR szInvSerTxnFsmSt[SipTxn::INV_SER_INVALID_ST + 1][SIP_15] =
{
    "IdleSt",
    "ProceedingSt",
    "CompletedSt",
    "ConfirmedSt",
    "TerminatedSt",
    "InvalidSt"
};

static SIP_CHAR szInvSerTxnFsmEvt[SipTxn::INV_SER_INVALID_EVT + 1][SIP_25] =
{
    "RecvInvReqEvt",
    "SendNon100ProvRespEvt",
    "Send3xx6xxFailureRespEvt",
    "Send2xxSuccessRespEvt",
    "TranspErrorEvt",
    "RecvAckReqEvt",
    "TimerG_H_TimeoutEvt",
    "TimerI_TimeoutEvt",
    "InvalidEvt"
};

static SIP_CHAR szNonInvClientTxnFsmSt[SipTxn::NON_INV_CLI_INVALID_ST + 1][SIP_15] =
{
    "IdleSt",
    "TryingSt",
    "ProceedingSt",
    "CompletedSt",
    "TerminatedSt",
    "InvalidSt"
};

static SIP_CHAR szNonInvClientTxnFsmEvt[SipTxn::NON_INV_CLI_INVALID_EVT + 1][SIP_22] =
{
    "SendNonInvReq_Evt",
    "Timer_E_F_TimeoutEvt",
    "Recv1xxRespEvt",
    "Recv2xx6xxRespEvt",
    "TranspErrorEvt",
    "Timer_K_TimeoutEvt",
    "InvalidEvt"
};

static SIP_CHAR szSipNonInvSerTxnFsmSt[SipTxn::NON_INV_SER_INVALID_ST + 1][SIP_15] =
{
    "IdleSt",
    "TryingSt",
    "ProceedingSt",
    "CompletedSt",
    "TerminatedSt",
    "InvalidSt"
};

static SIP_CHAR szSipNonInvSerTxnFsmEvt[SipTxn::NON_INV_SER_INVALID_EVT + 1][SIP_20] =
{
    "RecvNonInvReqEvt",
    "Send1xxRespEvt",
    "Send2xx6xxRespEvt",
    "TranspErrorEvt",
    "Timer_J_TimeoutEvt",
    "InvalidEvt"
};

static SIP_CHAR szSipTxnTimer[SipTxn::TIMER_TYPE_INVALID + 1][SIP_14] =
{
    "Timer1",
    "Timer2",
    "Timer4",
    "TimerA",
    "TimerB",
    "TimerC",
    "TimerD",
    "TimerE",
    "TimerF",
    "TimerG",
    "TimerH",
    "TimerI",
    "TimerJ",
    "TimerK",
    "TimerOther",
    "TimerInvalid"
};
#endif // #ifdef SIP_TRACE_ENABLE



SipTxn::SipTxn()
    :m_eTxnType(SipTxn::INVALID_TXN),m_pobjTxnKey(SIP_NULL),
    m_pobjSipMsg(SIP_NULL), m_pobjTranspInfo(SIP_NULL),
    m_pobjUserData(SIP_NULL),m_usTxnState(SIP_ZERO),m_usReTxCount(SIP_ZERO),
    m_eTimerType(SipTxn::TIMER_TYPE_INVALID),m_pvTimerId(SIP_NULL),
    m_usMaxDuration(SIP_ZERO),m_usDurationExpired(SIP_ZERO),
    m_usCurrentDuration(SIP_ZERO)
{
}

SipTxn::SipTxn
(
 IN     SIP_INT32        eTxnType,
 IN     SipTxnKey            *pobjTxnKey,
 IN     SipMessage        *pobjSipMsg,
 IN     SipTimerContext   *pobjSipTxnTimerContext,
 OUT SIP_UINT16         *pusError
)
    :m_eTxnType(SipTxn::INVALID_TXN),m_pobjTxnKey(SIP_NULL),
    m_pobjSipMsg(SIP_NULL), m_pobjTranspInfo(SIP_NULL),
    m_pobjUserData(SIP_NULL),m_usTxnState(SIP_ZERO),m_usReTxCount(SIP_ZERO),
    m_eTimerType(SipTxn::TIMER_TYPE_INVALID),m_pvTimerId(SIP_NULL),
    m_usMaxDuration(SIP_ZERO),m_usDurationExpired(SIP_ZERO),
    m_usCurrentDuration(SIP_ZERO)
{
    m_eTxnType             = eTxnType;
    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN, "+SipTxn Newly Add m_eTxnType[%d] eTxnType[%d]",
        m_eTxnType,eTxnType);
    m_pobjTxnKey         = new SipTxnKey(pobjTxnKey, pusError);

    if ((pobjSipTxnTimerContext != SIP_NULL) &&
        (pobjSipTxnTimerContext->pTxnSipTxnTimers != SIP_NULL))
    {
        objTxnTimerValues.UpdateSipTimers(pobjSipTxnTimerContext->nTimerOptions,
            pobjSipTxnTimerContext->pTxnSipTxnTimers);
    }

    if (m_pobjTxnKey == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SipTxn SipTxnKey NULL \n",SIP_ZERO,SIP_ZERO);
        return;
    }

    if (*pusError == E_ERR_PF_MALLOCFAILED)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SipTxn Malloc Failed \n",SIP_ZERO,SIP_ZERO);
        delete m_pobjTxnKey;
        m_pobjTxnKey = SIP_NULL;
    }

    m_pobjSipMsg = new SipMessage(*pobjSipMsg);

    /* IDLE State */
    m_usTxnState        = SIP_ZERO;
    m_usReTxCount        = SIP_ZERO;

}

SipTxn::~SipTxn()
{
    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,"SipTxn Destructor[%x]",this,0);

    if (m_pobjTranspInfo != SIP_NULL)
    {
        delete m_pobjTranspInfo;
        m_pobjTranspInfo = SIP_NULL;
    }

    if (m_pobjUserData != SIP_NULL)
    {
        delete m_pobjUserData;
        m_pobjUserData = SIP_NULL;
    }

    if (m_pobjTxnKey != SIP_NULL)
    {
        m_pobjTxnKey->SipDelete();
        m_pobjTxnKey = SIP_NULL;
    }

    if (m_pobjSipMsg != SIP_NULL)
    {
        m_pobjSipMsg->SipDelete();
        m_pobjSipMsg = SIP_NULL;
    }
}

SIP_BOOL SipTxn::InvokeFsm(SIP_UINT16 usEvent, SIP_VOID *pvData,SIP_UINT16 *pusError)
{
    SIP_INT32    eTxnType = m_eTxnType;
    SIP_UINT32    usTxnState = m_usTxnState;

    switch (eTxnType)
    {
        case SipTxn::INV_CLI_TXN:
            {
                SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                        "SipTxn::InvokeFsm:InvCliTxn [%s][%s]",
                        szInvClientTxnFsmSt[m_usTxnState],szInvClientTxnFsmEvt[usEvent]);

                if (gpfSipInvClientTxnFsm[usTxnState][usEvent](this, pvData,
                        pusError) == SIP_FALSE)
                {
                    SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                            "gpfSipInvClientTxnFsm Fails",SIP_ZERO,SIP_ZERO);
                    return SIP_FALSE;
                }
            }
            break;

        case SipTxn::INV_SER_TXN:
            {
                SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                        "SipTxn::InvokeFsm:InvSerTxn [%s][%s]",
                        szInvSerTxnFsmSt[m_usTxnState],szInvSerTxnFsmEvt[usEvent]);

                if (gpfSipInvSerTxnFsm[usTxnState][usEvent](this, pvData, pusError) == SIP_FALSE)
                {
                    SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                            "gpfSipInvSerTxnFsm Fails",SIP_ZERO,SIP_ZERO);
                    return SIP_FALSE;
                }
            }
            break;

        case SipTxn::NON_INV_CLI_TXN:
            {
                SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                        "SipTxn::InvokeFsm:NonInvCliTxn [%s][%s]",
                        szNonInvClientTxnFsmSt[m_usTxnState],szNonInvClientTxnFsmEvt[usEvent]);

                if (gpfSipNonInvClientTxnFsm[usTxnState][usEvent](this, pvData,
                        pusError) == SIP_FALSE)
                {
                    SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                            "gpfSipNonInvClientTxnFsm Fail",SIP_ZERO,SIP_ZERO);
                    return SIP_FALSE;
                }
            }
            break;

        case SipTxn::NON_INV_SER_TXN:
            {
                SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                        "SipTxn::InvokeFsm:NonInvSerTxn [%s][%s]",
                        szSipNonInvSerTxnFsmSt[m_usTxnState],szSipNonInvSerTxnFsmEvt[usEvent]);

                if (gpfSipNonInvSerTxnFsm[usTxnState][usEvent](this, pvData,
                        pusError) == SIP_FALSE)
                {
                    SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                            "gpfSipNonInvSerTxnFsm Fail",SIP_ZERO,SIP_ZERO);
                    return SIP_FALSE;
                }
            }
            break;

        default:
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                        "SipTxn::Invalid Txn Type eTxnType[%d] usTxnState[%d]",eTxnType,usTxnState);
                return SIP_FALSE;
            }
    }

    return SIP_TRUE;
}

/*
   This fxn stop the running txn timer and inform to user to abort the existing NW transaction
*/
SIP_BOOL SipTxn::AbortTxn()
{
    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN, "SipTxn::AbortTxn Invoked",0,0);

    SipUtil *pobjUtil = SipUtil_GetInstance();
    if (pobjUtil == SIP_NULL)
    {
        return SIP_FALSE;
    }

    ISipTimerUtil *pobjTimer = pobjUtil->GetTimer();
    if (m_pvTimerId != SIP_NULL)
    {
        SipTimeoutData *pobjTimeoutData = (SipTimeoutData *)pobjTimer->StopTimerEx
            (m_pvTimerId);
        m_pvTimerId = SIP_NULL;

        if (pobjTimeoutData != SIP_NULL)
        {
            delete pobjTimeoutData;
        }
    }

    /* Inform to user for closing NW txn */
    ISipNetworkUtil *pobjNetworkUtil = pobjUtil->GetNetwork();
    if (pobjNetworkUtil != SIP_NULL)
    {
        if (m_pobjTranspInfo != SIP_NULL)
        {
            pobjNetworkUtil->AbortTransmission(
                    m_pobjTranspInfo->GetMsgSentTranspParam(),
                    m_pobjUserData);
        }
    }

    return SIP_TRUE;
}

SIP_BOOL SipTxn::StartTxnTimer(SIP_UINT32 uieTimerType, SIP_UINT32 uiDuration, SIP_UINT16 *pusError)
{
    SipTimeoutData* pobjTimeoutData = new SipTimeoutData(m_eTxnType,
            (SIP_INT32)uieTimerType,
            m_pobjTxnKey);

    if (pobjTimeoutData == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SipTxn::StartTxnTimer: Memory Failed \n", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    if (*pusError == EERR_MALLOCFAILED)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SipTxn::StartTxnTimer: Memory Failed \n",SIP_ZERO,SIP_ZERO);
        delete pobjTimeoutData;
        return SIP_FALSE;
    }

    if (uiDuration > SIP_ZERO)
    {
        SipUtil *pobjUtil = SipUtil_GetInstance();

        if (pobjUtil == SIP_NULL)
        {
            SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                    "SipTxn::StartTxnTimer:pobjUtil is NULL ",
                    SIP_ZERO,SIP_ZERO);
            delete pobjTimeoutData;
            return SIP_FALSE;
        }

        ISipTimerUtil* pobjTimer = pobjUtil->GetTimer();

        if (pobjTimer == SIP_NULL)
        {
            SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                    "SipTxn::StartTxnTimer:pobjTimer is NULL ",
                    SIP_ZERO,SIP_ZERO);
            delete pobjTimeoutData;
            return SIP_FALSE;
        }

        if (pobjTimer->StartTimer(&m_pvTimerId, uiDuration, SIP_FALSE, CbkTxnTimeout,
                pobjTimeoutData) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "StartTxnTimer: StartTimer Failed",SIP_ZERO,SIP_ZERO);

            delete pobjTimeoutData;
            return SIP_FALSE;
        }

        SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                "SipTxn::StartTxnTimer[%s,%d]",
                szSipTxnTimer[uieTimerType],uiDuration);

        m_eTimerType = (SIP_INT32)uieTimerType;
        m_usCurrentDuration = uiDuration;
    }
    else
    {
        m_eTimerType = (SIP_INT32)uieTimerType;
        m_usCurrentDuration = uiDuration;
        SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                "SipTxn::StartTxnTimer, uiDuration is zero, invoke FSM directly.",
                SIP_ZERO, SIP_ZERO);
        /*make the timer id null & pass null as timer id in callback
          function to avoid Timer ID Mismatch in callback function*/
        SetTimerId(SIP_NULL);
        CbkTxnTimeout(pobjTimeoutData,SIP_NULL);
    }
    return SIP_TRUE;
}

SIP_BOOL SipTxn::StopTxnTimer()
{
    if (m_pvTimerId == SIP_NULL)
    {
        return SIP_TRUE;
    }

    SipUtil    *pobjUtil = SipUtil_GetInstance();
    if (pobjUtil == SIP_NULL)
    {
        SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                "SipTxn::StopTxnTimer:pobjUtil is NULL ",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    ISipTimerUtil    *pobjTimer = pobjUtil->GetTimer();
    if (pobjTimer == SIP_NULL)
    {
        SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                "SipTxn::StopTxnTimer:pobjTimer is NULL ",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SipTimeoutData    *pobjTimeoutData = SIP_NULL;
    pobjTimeoutData = (SipTimeoutData *)pobjTimer->StopTimerEx(
            m_pvTimerId);
    m_pvTimerId = SIP_NULL;
    delete pobjTimeoutData;

    return SIP_TRUE;
}

SIP_BOOL    SipTxn::PrepareACK
(
 SipMessage    *pobjSipRespMsg,    /* IN */
 SIP_BOOL        bSetMsgBody,        /* IN */
 SipMessage    **ppobjSipAckMsg    /* OUT */
)
{
    /* Ref:RFC 3261 Sec.17.1.1.3 Construction of the ACK Request*/
    SipMessage        *pobjSipAckMsg = new SipMessage(SipMessage::REQ_TYPE);
    SIP_BOOL         bStatus = SIP_TRUE;

    (void)bSetMsgBody;

    /* Set 'Call-ID' from INVITE Request     */
    SipHeaderBase *pobjCallIdHdr = m_pobjSipMsg->GetHdrObj(SipHeaderBase::CALL_ID);
    if (pobjCallIdHdr != SIP_NULL)
    {
        SipHeaderBase *pobjCallId = new SipHeaderBase(*pobjCallIdHdr);
        pobjCallIdHdr->SipDelete();

        bStatus = pobjSipAckMsg->SetHeader(pobjCallId);
        pobjCallId->SipDelete();
        if (bStatus == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "PrepareACK: Set CallID Failed \n",SIP_ZERO,SIP_ZERO);
            pobjSipAckMsg->SipDelete();
            return SIP_FALSE;
        }
    }

    /* Set 'From'     from INVITE Request */
    SipFromHeader *pobjFromHdr = (SipFromHeader *) pobjSipRespMsg->GetHdrObj(SipHeaderBase::FROM);
    if (SIP_NULL!= pobjFromHdr)
    {
        SipFromHeader *pobjFrom = new SipFromHeader(*pobjFromHdr);
        pobjFromHdr->SipDelete();

        bStatus = pobjSipAckMsg->SetHeader(pobjFrom);
        pobjFrom->SipDelete();
        if (bStatus == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "PrepareACK: Set From Failed \n",SIP_ZERO,SIP_ZERO);
            pobjSipAckMsg->SipDelete();
            return SIP_FALSE;
        }
    }
    /* Set Request-Uri from INVITE Request */
    SipRequestLine *pobjReqLine = SIP_NULL;
    pobjReqLine     = m_pobjSipMsg->GetReqLine();

    if (pobjReqLine != SIP_NULL)
    {
        SipAddrSpec    *pobjReqUri = pobjReqLine->GetReqUri();
        pobjReqLine->SipDelete();

        if (pobjReqUri != SIP_NULL)
        {
            SipAddrSpec *pNewObjReqUri = new SipAddrSpec(*pobjReqUri);
            pobjReqUri->SipDelete();
            SipRequestLine *pobjReqLine = new SipRequestLine((SIP_CHAR *)ACK_METHOD,
                pNewObjReqUri, (SIP_CHAR *)SIP_SIPVERSION);
            if (pobjReqLine == SIP_NULL)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "PrepareACK: SipRequestLine NULL \n",SIP_ZERO,SIP_ZERO);
                pobjSipAckMsg->SipDelete();
                return SIP_FALSE;
            }
            pobjSipAckMsg->SetRequestline(pobjReqLine);
        }
    }

    /* Set 'To' from INVITE Response */
    SipToHeader *pobjToHdr =(SipToHeader *) pobjSipRespMsg->GetHdrObj(SipHeaderBase::TO);
    if (pobjToHdr != SIP_NULL)
    {
        SipToHeader *pobjTo = new SipToHeader(*pobjToHdr);
        pobjToHdr->SipDelete();

        bStatus = pobjSipAckMsg->SetHeader(pobjTo);
        pobjTo->SipDelete();
        if (bStatus == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "PrepareACK: Set To Failed \n",SIP_ZERO,SIP_ZERO);
            pobjSipAckMsg->SipDelete();
            return SIP_FALSE;
        }
    }

    /* Set 'Via' from INVITE Request    */
    SipViaHeader *pobjViaHdr = (SipViaHeader *) m_pobjSipMsg->GetHdrObj(SipHeaderBase::VIA);
    if (pobjViaHdr != SIP_NULL)
    {
        SipViaHeader *pobjVia = new SipViaHeader(*pobjViaHdr);
        pobjViaHdr->SipDelete();

        bStatus = pobjSipAckMsg->SetHeader(pobjVia);
        pobjVia->SipDelete();
        if (bStatus == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "PrepareACK: Set Via Failed \n",SIP_ZERO,SIP_ZERO);
            pobjSipAckMsg->SipDelete();
            return SIP_FALSE;
        }
    }

    /* Set CSeq    -Num    from INVITE Request */
    SipCSeqHeader *pobjCSeqHdr = (SipCSeqHeader *) m_pobjSipMsg->GetHdrObj(SipHeaderBase::CSEQ);
    if (pobjCSeqHdr != SIP_NULL)
    {
        SipCSeqHeader *pobjCseq = new SipCSeqHeader(*pobjCSeqHdr);
        pobjCSeqHdr->SipDelete();

        /* Set CSeq-Method     as ACk */
        pobjCseq->SetMethod((SIP_CHAR *)"ACK");

        bStatus = pobjSipAckMsg->SetHeader(pobjCseq);
        pobjCseq->SipDelete();
        if (bStatus == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "PrepareACK: Set CSeq Failed \n",SIP_ZERO,SIP_ZERO);
            pobjSipAckMsg->SipDelete();
            return SIP_FALSE;
        }
    }

    /* Set MaxForward Header */
    SipIntegerHeader *pobjMaxForward = new SipIntegerHeader(SipHeaderBase::MAX_FORWARDS);
    bStatus = pobjSipAckMsg->SetHeader(pobjMaxForward);
    pobjMaxForward->SipDelete();
    if (bStatus == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "PrepareACK: Set MaxForward Failed \n",SIP_ZERO,SIP_ZERO);
        pobjSipAckMsg->SipDelete();
        return SIP_FALSE;
    }

    /* Set UserAgent from INVITE Request */
    SipUserAgentHeader *pobjUserAgentHdr =
        (SipUserAgentHeader*) m_pobjSipMsg->GetHdrObj(SipHeaderBase::USER_AGENT);
    if (pobjUserAgentHdr != SIP_NULL)
    {
        SipUserAgentHeader *pobjUserAgent = new SipUserAgentHeader(*pobjUserAgentHdr);
        pobjUserAgentHdr->SipDelete();

        bStatus = pobjSipAckMsg->SetHeader(pobjUserAgent);
        pobjUserAgent->SipDelete();
        if (bStatus == SIP_FALSE)
        {
                SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                        "PrepareACK: Set UserAgent Failed \n",SIP_ZERO,SIP_ZERO);
                pobjSipAckMsg->SipDelete();
                return SIP_FALSE;
        }
    }
    if (SipConfiguration::GetInstance()->IsPANIHeaderReqdForACK()){
    /* Set PANI header from INVITE Request*/
        SipHeaderBase *pobjPANIHdr = m_pobjSipMsg->GetHdrObj(SipHeaderBase::P_ACCESS_NETWORK_INFO);
        if (pobjPANIHdr != SIP_NULL) {
            SipHeaderBase *pobjPANI = new SipHeaderBase(*pobjPANIHdr);
            pobjPANIHdr->SipDelete();
            bStatus = pobjSipAckMsg->SetHeader(pobjPANI);
            pobjPANI->SipDelete();
            if (bStatus == SIP_FALSE) {
                SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "PrepareACK: Set PANI Failed \n",SIP_ZERO,SIP_ZERO);
                pobjSipAckMsg->SipDelete();
                return SIP_FALSE;
            }
        }
    }

    /* If Route is present in INVITE Request ,Copy Route Header*/
    /* Need to Clone */
    SipHeaderList *pobjRouteList = m_pobjSipMsg->GetHdrList(SipHeaderBase::ROUTE);
    if (pobjRouteList != SIP_NULL)
    {
        bStatus = pobjSipAckMsg->SetHdrList(pobjRouteList);
        pobjRouteList->SipDelete();
        if (bStatus == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "PrepareACK: Set Route Failed \n",SIP_ZERO,SIP_ZERO);
            pobjSipAckMsg->SipDelete();
            return SIP_FALSE;
        }
    }

    /* ###TODO Setting of Msg Body in Response currently not supported */
    *ppobjSipAckMsg = pobjSipAckMsg;
    return SIP_TRUE;
}

SIP_VOID* SipTxn::GetTimerId()
{
    return m_pvTimerId;
}

SIP_BOOL SipTxn::UpdateTranspInfo(SipTransportInfo *pobjTranspInfo)
{
    if (m_pobjTranspInfo != SIP_NULL)
    {
        delete m_pobjTranspInfo;
    }

    m_pobjTranspInfo = pobjTranspInfo;
    return SIP_TRUE;
}

/* Remove Txn entry from DB. NOTE-> it does not delete Txn obj */
SIP_VOID SipTxn::RemoveFromTxnPool()
{
    SipTxn_RemoveFromTxnPool(m_pobjTxnKey);
}

SipTransportInfo*     SipTxn::GetTranspInfo()
{
    return m_pobjTranspInfo;
}

ISipUserData*         SipTxn::GetUserData()
{
    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,"SipTxn::GetUserData Called",SIP_ZERO,SIP_ZERO);
    return m_pobjUserData;
}

SIP_INT32 SipTxn::GetMsgSentProto()
{
    if (m_pobjTranspInfo == SIP_NULL)
    {
        /*  stack error */
        SIP_DEBUG_STACKBUG(ESIPTRACE_MODTRANSP,
                "SipTransp_OnReceiveTranspLayer: sipFetchElement Error \n",
                SIP_ZERO,SIP_ZERO);
        return SipTransportInfo::PROTOCOL_INVALID;
    }

    SipTransportParameter *pobjMsgSentTransParam = m_pobjTranspInfo->GetMsgSentTranspParam();
    if (pobjMsgSentTransParam == SIP_NULL)
    {
        /*  stack error */
        SIP_DEBUG_STACKBUG(ESIPTRACE_MODTRANSP,
                "SipTransp_OnReceiveTranspLayer: sipFetchElement Error \n",
                SIP_ZERO,SIP_ZERO);
        return SipTransportInfo::PROTOCOL_INVALID;
    }

    SIP_INT32 eTranspMsgSentProtocol = pobjMsgSentTransParam->GetTranspProtocol();

    return eTranspMsgSentProtocol;
}

SIP_BOOL SipTxn::SetUserData(ISipUserData    *pobjUserData)
{
    if (pobjUserData == SIP_NULL)
    {
        return SIP_TRUE;
    }

    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,"SipTxn::SetUserData Called",SIP_ZERO,SIP_ZERO);

    if (m_pobjUserData != SIP_NULL)
    {
        delete m_pobjUserData;
    }

    m_pobjUserData = pobjUserData;

    return SIP_TRUE;
}

SIP_BOOL SipTxn::IsTxnTerminated()
{
    switch (m_eTxnType)
    {
        case SipTxn::INV_CLI_TXN:
            {
                if (m_usTxnState == SipTxn::INV_CLI_TERMINATED_ST)
                {
                    return SIP_TRUE;
                }
            }
            break;

        case SipTxn::NON_INV_CLI_TXN:
            {
                if (m_usTxnState == SipTxn::NON_INV_CLI_TERMINATED_ST)
                {
                    return SIP_TRUE;
                }
            }
            break;

        case SipTxn::INV_SER_TXN:
            {
                if (m_usTxnState == SipTxn::INV_SER_TERMINATED_ST)
                {
                    return SIP_TRUE;
                }
            }
            break;
        case SipTxn::NON_INV_SER_TXN:
            {
                if (m_usTxnState == SipTxn::NON_INV_SER_TERMINATED_ST)
                {
                    return SIP_TRUE;
                }
            }
            break;

        default:
            {
                return SIP_FALSE;
            }
            break;
    }

    return SIP_FALSE;
}

SIP_VOID SipTxn::InitRetransmissionInfo()
{
    m_usReTxCount = SIP_ZERO;
    m_usMaxDuration = SIP_ZERO;
    m_usDurationExpired = SIP_ZERO;
    m_usCurrentDuration = SIP_ZERO;
}

SIP_VOID SipTxn::SetRespCode(SIP_UINT16 usRespCode)
{
    if (m_pobjTxnKey != SIP_NULL)
    {
        m_pobjTxnKey->SetRespCode(usRespCode);
    }
}

SIP_VOID CbkTxnTimeout
(
 SIP_VOID        *pvobjTimeoutData,
 SIP_VOID        *pvTimerId
 )
{
    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                      "\n***CbkTxnTimeout --> Processing Started***",SIP_ZERO,SIP_ZERO);

    SipTimeoutData    *pobjTimeoutData = (SipTimeoutData *)pvobjTimeoutData;
    if (pobjTimeoutData == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "CbkTxnTimeout: pobjTimeoutData is NULL",SIP_ZERO,SIP_ZERO);
        return;
    }

    SipTxnKey * pobjTxnKey = pobjTimeoutData->GetTxnKey();
    SipTxn             *pobjTxn = SIP_NULL;
    SIP_BOOL bTxnExist = sip_cbk_fetchTransaction(
            reinterpret_cast<SIP_VOID*>(pobjTxnKey),
            TXN_OPT_FETCH,
            SIP_NULL,
            reinterpret_cast<SIP_VOID**>(&pobjTxn));

    if (bTxnExist == SIP_YES)
    {
        if (pobjTxn == SIP_NULL)
        {
            /*  stack error */
            SIP_DEBUG_STACKBUG(ESIPTRACE_MODTXN,
                    "CbkTxnTimeout: Stack Error,Db Status Error",
                    SIP_ZERO,SIP_ZERO);
            delete pobjTimeoutData;
            return;
        }
    }
    else
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "CbkTxnTimeout: TXN Not Exists\n",
                SIP_ZERO,SIP_ZERO);
        delete pobjTimeoutData;
        return;
    }

    pobjTxn->increment();

    /* Timer Id is no longer valid. So remove from Transaction Object */

    if (pvTimerId != pobjTxn->GetTimerId())
    {
        SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                "CbkTxnTimeout --> Timer ID Mismatch",SIP_ZERO,SIP_ZERO);

        delete pobjTimeoutData;
        pobjTxn->SipDelete();
        return;
    }

    pobjTxn->SetTimerId(SIP_NULL);

    SipUtil *pobjUtil = SipUtil_GetInstance();
    if (pobjUtil == SIP_NULL)
    {
        delete pobjTimeoutData;
        pobjTxn->SipDelete();
        return;
    }

    ISipUserData    *pobjUserData = pobjTxn->GetUserData();

    /*No need to notify txntimeout to listener if the txn is already terminated*/
    /* Notify user is txn is terminated */
    ISipTxnListener *pobjTxnListener = pobjUtil->GetTxnListener();

    SIP_INT32 uieTimerType = SipTxn::TIMER_TYPE_INVALID;
    if (pobjTxn->IsTxnTerminated() == SIP_TRUE)
    {
        if (pobjTxnListener != SIP_NULL)
        {
            pobjTxnListener->TxnTimeout(pobjUserData, (SIP_INT32)uieTimerType);
        }

        SipTxn_RemoveFromTxnPool(pobjTxnKey);

        delete pobjTimeoutData;
        pobjTxn->SipDelete();

        SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                "***CbkTxnTimeout --> TxnTerminated***\n",SIP_ZERO,SIP_ZERO);
        return;
    }

    SIP_UINT16 usEvent = SIP_INVALID;
    SIP_INT32 eTxnType = pobjTxn->GetTxnType();
    uieTimerType = pobjTimeoutData->GetTimerType();
    SIP_BOOL bStatus = SIP_TRUE;

    switch (eTxnType)
    {
        case SipTxn::INV_CLI_TXN:
            {
                if ((uieTimerType == SipTxn::TIMERA) ||
                        (uieTimerType == SipTxn::TIMERB))
                {
                    usEvent = SipTxn::INV_CLI_TIMERA_B_TIME_OUT_EVT;
                }
                else if (uieTimerType == SipTxn::TIMERD)
                {
                    usEvent = SipTxn::INV_CLI_TIMERD_TIME_OUT_EVT;
                }
                else
                {
                    usEvent = SipTxn::INV_CLI_INVALID_EVT;
                    SIP_DEBUG_STACKBUG(ESIPTRACE_MODTXN,
                        "CbkTxnTimeout[InvCliTxn]: Invalid Timer Evt[%d]",uieTimerType,SIP_ZERO);

                    bStatus = SIP_FALSE;
                }
            }
            break;

        case SipTxn::NON_INV_CLI_TXN:
            {
                if ((uieTimerType == SipTxn::TIMERE) ||
                        (uieTimerType == SipTxn::TIMERF))
                {
                    usEvent = SipTxn::NON_INV_CLI_TIMER_E_F_TIME_OUT_EVT;
                }
                else if (uieTimerType == SipTxn::TIMERK)
                {
                    usEvent = SipTxn::NON_INV_CLI_TIMER_K_TIME_OUT_EVT;
                }
                else
                {
                    usEvent = SipTxn::NON_INV_CLI_INVALID_EVT;
                    SIP_DEBUG_STACKBUG(ESIPTRACE_MODTXN,
                        "CbkTxnTimeout[NonInvCliTxn]: Invalid Timer Evt[%d]",uieTimerType,SIP_ZERO);

                    bStatus = SIP_FALSE;
                }
            }
            break;

        case SipTxn::INV_SER_TXN:
            {
                if ((uieTimerType == SipTxn::TIMERG) ||
                        (uieTimerType == SipTxn::TIMERH))
                {
                    usEvent = SipTxn::INV_SER_TIMER_G_H_TIME_OUT_EVT;
                }
                else if (uieTimerType == SipTxn::TIMERI)
                {
                    usEvent = SipTxn::INV_SER_TIMER_I_TIME_OUT_EVT;
                }
                else
                {
                    usEvent = SipTxn::INV_SER_INVALID_EVT;
                    SIP_DEBUG_STACKBUG(ESIPTRACE_MODTXN,
                        "CbkTxnTimeout[InvSerTxn]: Invalid Timer Evt[%d]",uieTimerType,SIP_ZERO);

                    bStatus = SIP_FALSE;
                }
            }
            break;
        case SipTxn::NON_INV_SER_TXN:
            {
                if (uieTimerType == SipTxn::TIMERJ)
                {
                    usEvent = SipTxn::NON_INV_SER_TIMER_J_TIME_OUT_EVT;
                }
                else
                {
                    usEvent = SipTxn::NON_INV_SER_INVALID_EVT;
                    SIP_DEBUG_STACKBUG(ESIPTRACE_MODTXN,
                        "CbkTxnTimeout[NonInvSerTxn]: Invalid Timer Evt[%d]",uieTimerType,SIP_ZERO);

                    bStatus = SIP_FALSE;
                }
            }
            break;

        default:
            {
                SIP_DEBUG_STACKBUG(ESIPTRACE_MODTXN,
                        "CbkTxnTimeout: Invalid Txn type",SIP_ZERO,SIP_ZERO);

                bStatus = SIP_FALSE;
            }
            break;
    }


    if (bStatus == SIP_FALSE)
    {
        delete pobjTimeoutData;
        pobjTxn->SipDelete();
        return;
    }

    SIP_UINT16         usError = SIP_ZERO;
    bStatus = pobjTxn->InvokeFsm(usEvent,pobjTimeoutData,&usError);
    if (bStatus == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "CbkTxnTimeout: Invoke FSM Fail",SIP_ZERO,SIP_ZERO);

        delete pobjTimeoutData;
        pobjTxn->SipDelete();
        return;
    }

    /*No need to notify txntimeout to listener if the txn is already terminated*/
    if (pobjTxn->IsTxnTerminated() == SIP_TRUE)
    {
        if (pobjTxnListener != SIP_NULL)
        {
            pobjTxnListener->TxnTimeout(pobjUserData, (SIP_INT32)uieTimerType);
        }

        SipTxn_RemoveFromTxnPool(pobjTxnKey);

        delete pobjTimeoutData;
        pobjTxn->SipDelete();

        SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                "***CbkTxnTimeout --> TxnTerminated***\n",SIP_ZERO,SIP_ZERO);
        return;
    }

    delete pobjTimeoutData;

    /* On transaction timer expiry, if txn is not terminated, it mean there are message
    to re-transmit */
    ISipNetworkUtil* pobjNetworkUtil = pobjUtil->GetNetwork();
    SipTransportInfo* pobjTranspInfo = pobjTxn->GetTranspInfo();

    if (pobjTranspInfo != SIP_NULL)
    {
        SipTransportBuffer* pobjTransSipBuffer = pobjTranspInfo->GetTranspSipBuffer();
        SipTransportParameter* pobjActualDestParam = pobjTranspInfo->GetMsgSentTranspParam();

        bStatus = pobjNetworkUtil->SendToNetwork(
                pobjTransSipBuffer,
                pobjActualDestParam,
                pobjUserData );

        pobjTxn->SipDelete();

        if (bStatus == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "CbkTxnTimeout: SendToNetwork Fail",SIP_ZERO,SIP_ZERO);
            return;
        }
    }
    else
    {
        pobjTxn->SipDelete();
    }

    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,"CbkTxnTimeout Processing Done",SIP_ZERO,SIP_ZERO);
}

SIP_VOID SipTxn_RemoveFromTxnPool(SipTxnKey* pTxnKey)
{
    SipTxn* pTempTxn = SIP_NULL;
    SipTxnKey* pTempTxnKey = SIP_NULL;

    if (sip_cbk_releaseTransaction(
            reinterpret_cast<SIP_VOID*>(pTxnKey),
            TXN_OPT_REMOVE,
            reinterpret_cast<SIP_VOID**>(&pTempTxnKey),
            reinterpret_cast<SIP_VOID**>(&pTempTxn)) == SIP_FALSE)
    {
        SIP_DEBUG_STACKBUG(ESIPTRACE_MODTXN,
                "SipTxn_RemoveFromTxnPool:\n",
                SIP_ZERO, SIP_ZERO);
        return;
    }

    if (pTempTxnKey != SIP_NULL)
    {
        pTempTxnKey->SipDelete();
    }

    if (pTempTxn != SIP_NULL)
    {
        pTempTxn->SipDelete();
    }
}
