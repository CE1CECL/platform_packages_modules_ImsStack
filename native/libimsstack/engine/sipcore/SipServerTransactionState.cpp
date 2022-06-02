/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "ServiceTimer.h"
#include "Connector.h"
#include "ISipHeader.h"
#include "SipPrivate.h"
#include "SipFeatures.h"
#include "SipDebug.h"
#include "SipConfigProxy.h"
#include "SipFactoryProxy.h"
#include "SipManager.h"
#include "SipStackState.h"
#include "SipTxnContextData.h"
#include "SipTimerValuesHelper.h"
#include "SipUtils.h"
#include "SipHeaderName.h"
#include "SipHeaderUtils.h"
#include "SipAddress.h"
#include "SipDialogEx.h"
#include "SipMessage.h"
// SIP_MESSAGE_TRACKER
#include "SipMessageTracker.h"
#include "SipServerTransport.h"
#include "SipServerTransactionState.h"

// #define __IMS_SIP_REQUEST_URI_VALIDATION__

__IMS_TRACE_TAG_SIP__;

PUBLIC
SipServerTransactionState::SipServerTransactionState(IN IMS_SINT32 nSlotId,
        IN CONST SipTransportAddress& objNearEnd_, IN CONST SipTransportAddress& objFarEnd_) :
        SipTransactionState(),
        piTimer_100Trying(IMS_NULL)
{
    nType = TYPE_SERVER;

    pTransport = new SipServerTransport(nSlotId, objNearEnd_, objFarEnd_);
}

PUBLIC VIRTUAL SipServerTransactionState::~SipServerTransactionState()
{
    if (piTimer_100Trying != IMS_NULL)
    {
        piTimer_100Trying->KillTimer();
        TimerService::GetTimerService()->DestroyTimer(piTimer_100Trying);
        piTimer_100Trying = IMS_NULL;
    }

#ifdef __IMS_SIP_DEBUG__
    IMS_TRACE_D("Destructor :: SipServerTransactionState", 0, 0, 0);
#endif
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_SINT32 SipServerTransactionState::CheckMessageValidity()
{
    IMS_SINT32 nValidity;

    //---------------------------------------------------------------------------------------------

    // Update the transport information from the top Via header
    // (received & rport parameter handling)
    nValidity = pTransport->ValidateViaHeader(pstMessage);

    if (nValidity != SipPrivate::MESSAGE_VALID)
        return nValidity;

    // Set "received" & "rport" parameter if it needs to be set
    if (!pTransport->FormViaHeader(pstMessage, GetSipProfile()))
    {
        return SipPrivate::MESSAGE_FAILED;
    }

    return SipPrivate::MESSAGE_VALID;
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_BOOL SipServerTransactionState::FormMessage()
{
    //---------------------------------------------------------------------------------------------

    // Update the transport information
    if (!UpdateTransportDetails())
    {
        SipPrivate::SetLastError(SipError::GENERAL_ERROR);
        return IMS_FALSE;
    }

    SipMethod objMethod = SipStack::GetMethod(pstMessage);

    SetFlowControlOption(objMethod);

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_BOOL SipServerTransactionState::Send(IN SipTimerValues* pTV /* = IMS_NULL */)
{
    //---------------------------------------------------------------------------------------------

    StopTimer100Trying(this);

    if (!UpdateTxnDetails())
    {
        IMS_TRACE_E(0, "Updating transaction details failed", 0, 0, 0);
        return IMS_FALSE;
    }

    return SipTransactionState::Send(pTV);
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_BOOL SipServerTransactionState::UpdateTransportDetails()
{
    //---------------------------------------------------------------------------------------------

    return SipTransactionState::UpdateTransportDetails();
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipServerTransactionState::InitResponse(IN IMS_SINT32 nStatusCode)
{
    //---------------------------------------------------------------------------------------------

    return InitResponse(nStatusCode, pstMessage);
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipServerTransactionState::IsSameTransaction(
        IN CONST SipServerTransactionState* pSTState) const
{
    //---------------------------------------------------------------------------------------------

    if (pSTState == IMS_NULL)
    {
        return IMS_FALSE;
    }

    if ((pstTxnKey == IMS_NULL) || (pSTState->pstTxnKey == IMS_NULL))
    {
        IMS_TRACE_D("Transaction Key Does Not Exist", 0, 0, 0);
        return IMS_FALSE;
    }

    // Compares these values : Call-ID, CSeq number, From-Tag, Via branch parameter
    if (!SipStack::CompareTxnKeysForCancel(pstTxnKey, pSTState->pstTxnKey))
    {
        IMS_TRACE_D("Transaction Not Matched : Cancel (%s), Ongoing (%s:%s)",
                SipStack::TxnKey_GetViaBranch(pstTxnKey),
                SipStack::TxnKey_GetMethod(pSTState->pstTxnKey),
                SipStack::TxnKey_GetViaBranch(pSTState->pstTxnKey));

        return IMS_FALSE;
    }

    IMS_TRACE_D("Transaction Matched : Cancel (%s), Ongoing (%s:%s)",
            SipStack::TxnKey_GetViaBranch(pstTxnKey),
            SipStack::TxnKey_GetMethod(pSTState->pstTxnKey),
            SipStack::TxnKey_GetViaBranch(pSTState->pstTxnKey));

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 SipServerTransactionState::MatchTransaction(IN ::SipMessage* pSipMsg)
{
    SipTransportParameter objTranspParam;

    // Update a new incoming request
    UpdateMessage(pSipMsg);

    const SipTransportAddress& objFarEnd = pTransport->GetAddress(SipTransport::TA_FAR);

    /* Fill Transport details */
    objTranspParam.setHostAddress(objFarEnd.GetIpAddress().ToString().GetStr());
    objTranspParam.setPort(objFarEnd.GetPort());
    objTranspParam.setTranspProtocol(objFarEnd.GetProtocol());

    if (objFarEnd.GetIpAddress().IsIPv4Address() == IMS_TRUE)
    {
        objTranspParam.setTanspIpType(SipTransportInfo::NETWORK_IPV4);
    }
    else
    {
        objTranspParam.setTanspIpType(SipTransportInfo::NETWORK_IPV6);
    }

    /* Prepare User data */
    /* BSP_TODO: make sure stack store the pTxnContextData otherwise there is memory leak */
    SipMethod objMethod = SipStack::GetMethod(pSipMsg);

    SipTxnContext* pstTxnContext = SipStack::CreateTxnContext();
    if (pstTxnContext == IMS_NULL)
    {
        return SipPrivate::MESSAGE_DISCARDED;
    }

    SipTxnContextData* pTxnContextData = new SipTxnContextData();
    if (pTxnContextData != IMS_NULL)
    {
        pTxnContextData->SetTxnState(this);
    }

    pstTxnContext->pTxnContextData = (SIP_VOID*)pTxnContextData;
    ISipUserData objUserData((SIP_VOID*)pstTxnContext);

    /* BSP_TODO::
    To send internal 100 Trying on received of new INVITE, user data is required otherwise
    Send2Nw APIs will Fail */
    SipEn_TxnStatus eTxnStatus = ETXNSTATUS_INVALID;
    ::SipTxnKey* pTxnKey = IMS_NULL;
    IMS_UINT16 nError = 0;
    IMS_SINT32 nEncodingOptions = SipPrivate::GetEncodingOptions();
    IMS_UINT32 nMsgOptions = ESIPMSGOPT_NONE;

    if ((nEncodingOptions & SipPrivate::OPT_E_SHORTFORM) != 0)
    {
        nMsgOptions |= ESIPMSGOPT_ENCSHORTFORM;
    }

    objUserData.SetMsgOptions(nMsgOptions);

    SIP_BOOL bStatus = SipStackManager::GetInstance()->OnRecvMessage(pSipMsg, &objTranspParam,
            &objUserData, reinterpret_cast<SIP_INT32*>(&eTxnStatus), &pTxnKey, &nError);

    // If it's NULL, new transaction grab this object.
    if (objUserData.GetUserData() != IMS_NULL)
    {
        if (objUserData.GetDeleteFlag() == SIP_TRUE)
        {
            SipTxnContext* pOldTxnContext =
                    reinterpret_cast<SipTxnContext*>(objUserData.GetUserData());

            if (pOldTxnContext != pstTxnContext)
            {
                SipStack::DestroyTxnContext(pOldTxnContext);
            }
        }

        SipStack::DestroyTxnContext(pstTxnContext);
    }

    if (bStatus == SIP_FALSE)
    {
        IMS_TRACE_D("SIPServerTransactionstate::MatchTransaction -> OnRecvMessage() Failed (%d)",
                nError, 0, 0);
        return SipPrivate::MESSAGE_FAILED;
    }

    switch (eTxnStatus)
    {
        case ETXNSTATUS_NEWREQRECVD:
        {
            IMS_TRACE_I("__UAS__ :: _____ NEW REQUEST _____", 0, 0, 0);
            break;
        }
        case ETXNSTATUS_VALIDMESSAGE:
        {
            IMS_TRACE_I("__UAS__ :: _____ VALID MESSAGE _____", 0, 0, 0);
            break;
        }
        case ETXNSTATUS_IGNOREREQ:
        {
            IMS_TRACE_I("__UAS__ :: _____ IGNORE REQUEST _____", 0, 0, 0);
            return SipPrivate::MESSAGE_DISCARDED;
        }
        case ETXNSTATUS_IGNORERESP:
        {
            IMS_TRACE_I("__UAS__ :: _____ IGNORE RESPONSE _____", 0, 0, 0);
            return SipPrivate::MESSAGE_DISCARDED;
        }
        case ETXNSTATUS_STRAYRESP:
        {
            IMS_TRACE_I("__UAS__ :: _____ STRAY RESPONSE _____", 0, 0, 0);
            return SipPrivate::MESSAGE_DISCARDED;
        }
        case ETXNSTATUS_RETRANSMISSION:
        {
            IMS_TRACE_I("__UAS__ :: _____ REMOTE RETRANSMISSION _____", 0, 0, 0);
            return SipPrivate::MESSAGE_DISCARDED;
        }
        case ETXNSTATUS_ERRORONSEND:
        case ETXNSTATUS_INVALIDMESSAGE:
        case ETXNSTATUS_INVALID:
        {
            IMS_TRACE_I("__UAS__ :: _____ PROCESSING FAILED _____", 0, 0, 0);
            return SipPrivate::MESSAGE_FAILED;
        }
        case ETXNSTATUS_STRAYPRACK:
        {
            IMS_TRACE_I("__UAS__ :: _____ STRAY PRACK RECEIVED _____", 0, 0, 0);
            return SipPrivate::MESSAGE_INVALID_481;
        }
        default:
            IMS_TRACE_I("__UAS__ :: _____ PROCESSING FAILED _____", 0, 0, 0);
            return SipPrivate::MESSAGE_FAILED;
    }

    if (pTxnKey != IMS_NULL)
    {
        SipStack::FreeTxnKey(pstTxnKey);
        pstTxnKey = pTxnKey;
    }

    if (pstTxnKey != IMS_NULL)
    {
        if (pstTxnKey->GetTxnType() == ETXN_INVSERTXN)
            nClass = CLASS_INVITE;
        else if (pstTxnKey->GetTxnType() == ETXN_NONINVSERTXN)
            nClass = CLASS_REGULAR;
    }

    SipFactoryProxy* pFactoryProxy = SipFactoryProxy::GetInstance();
    /* NOTE::
    If the message is an ACK request for non-2xx response to INVITE request,
    then stack drop the message by returning ignore request */
    if ((pstTxnKey != IMS_NULL) && (pstTxnKey->GetTxnType() == ETXN_INVSERTXN))
    {
        SipMethod objMethod = SipStack::GetMethod(pstMessage);

        if (objMethod.Equals(SipMethod::ACK) && (pstTxnKey->GetRespCode() >= SipStatusCode::SC_300))
        {
            IMS_TRACE_I("__UAS__ :: ___ ACK (%s) TO UNSUCCESSFUL FINAL RESPONSE ___",
                    SipDebug::GetCharA1(pstTxnKey->GetCallId(), 8, '@'), 0, 0);

            // SIP_MESSAGE_TRACKER
            if (pFactoryProxy->IsMessageTrackerEnabled(GetSlotId()))
            {
                SipMessageTracker* pMessageTracker = pFactoryProxy->GetMessageTracker(GetSlotId());
                pMessageTracker->NotifyMessageReceived(objMethod, pstTxnKey->GetRespCode(),
                        AString(static_cast<const IMS_CHAR*>(pstTxnKey->GetCallId())));
            }
            return SipPrivate::MESSAGE_DISCARDED;
        }
    }

    // SIP_MESSAGE_TRACKER
    if (pFactoryProxy->IsMessageTrackerEnabled(GetSlotId()))
    {
        SipMessageTracker* pMessageTracker = pFactoryProxy->GetMessageTracker(GetSlotId());
        if (pstTxnKey != IMS_NULL)
        {
            pMessageTracker->NotifyMessageReceived(SipStack::GetMethod(pstMessage), 0,
                    AString(static_cast<const IMS_CHAR*>(pstTxnKey->GetCallId())));
        }
        else
        {
            pMessageTracker->NotifyMessageReceived(
                    SipStack::GetMethod(pstMessage), 0, AString::ConstNull());
        }
    }
    return SipPrivate::MESSAGE_VALID;
}

/*

Remarks

*/
PUBLIC
void SipServerTransactionState::RejectRequest(
        IN IMS_SINT32 nStatusCode, IN CONST AString& strReason /* = AString::ConstNull() */)
{
    SipMethod objMethod = SipStack::GetMethod(pstMessage);

    //---------------------------------------------------------------------------------------------

    if (objMethod.ToInt() == SipMethod::ACK)
    {
        // Do not anything ...
        return;
    }

    if (pstMessage != IMS_NULL)
    {
        SipStack::FreeMessage(pstMessage);
    }

    pstMessage = SipStack::CreateMessage(SipStack::SIP_MESSAGE_RESPONSE);

    if (pstMessage == IMS_NULL)
    {
        IMS_TRACE_E(0, "Creating a reject message failed", 0, 0, 0);
        return;
    }

    // Set the status code
    if (strReason.IsNULL())
    {
        if (!SipStack::SetStatusLine(
                    nStatusCode, SipStatusCode::GetReasonPhrase(nStatusCode), pstMessage))
        {
            IMS_TRACE_E(0, "Setting a status-line failed", 0, 0, 0);
            return;
        }
    }
    else
    {
        if (!SipStack::SetStatusLine(nStatusCode, strReason, pstMessage))
        {
            IMS_TRACE_E(0, "Setting a status-line failed", 0, 0, 0);
            return;
        }
    }

    if (!InitResponse(nStatusCode))
    {
        IMS_TRACE_E(0, "Initializing a response failed", 0, 0, 0);
        return;
    }

    IMS_BOOL bIsRetryAfterPresent = IMS_FALSE;

    // Retry-After header field
    //  In case of a second INVITE received before it sends the final response
    //  to a first INVITE with a lower
    //  CSeq sequence number on the same dialog.
    if (objMethod.Equals(SipMethod::INVITE) && (nStatusCode == SipStatusCode::SC_500) &&
            (pDialogEx->GetState() != SipDState::STATE_CONFIRMED))
    {
        // Sets randomly chosen value of between 1 and 10 seconds.
        AString strRA_Value;
        strRA_Value.SetNumber(SipHeaderUtils::GenerateRetryAfterSeconds(10));

        SipHeaderBase* pstHeader = SipStack::DecodeHeader(ISipHeader::RETRY_AFTER_SEC, strRA_Value);

        if (pstHeader != IMS_NULL)
        {
            if (!SipStack::SetHeader(pstHeader, pstMessage))
            {
                SipStack::FreeHeaderEx(pstHeader);

                IMS_TRACE_E(0, "Setting Retry-After header field failed", 0, 0, 0);
                return;
            }

            SipStack::FreeHeaderEx(pstHeader);

            bIsRetryAfterPresent = IMS_TRUE;
        }
    }

    //// Add the specific headers according to the reject code

    // Retry-After : 404, 413, 480, 486, 500, 503, 600, 603
    // Allow : 200, 405
    // Supported : 200
    // Accept, Accept-Encoding, Accept-Language : 415
    // Unsupported : 420
    // Require : 421

    // Retry-After
    if ((!bIsRetryAfterPresent) &&
            ((nStatusCode == SipStatusCode::SC_404) || (nStatusCode == SipStatusCode::SC_413) ||
                    (nStatusCode == SipStatusCode::SC_480) ||
                    (nStatusCode == SipStatusCode::SC_486) ||
                    (nStatusCode == SipStatusCode::SC_500) ||
                    (nStatusCode == SipStatusCode::SC_503) ||
                    (nStatusCode == SipStatusCode::SC_600) ||
                    (nStatusCode == SipStatusCode::SC_603)))
    {
        // Sets randomly chosen value of between 1 and 10 seconds.
        AString strRA_Value;
        strRA_Value.SetNumber(SipHeaderUtils::GenerateRetryAfterSeconds(10));

        SipHeaderBase* pstHeader = SipStack::DecodeHeader(ISipHeader::RETRY_AFTER_SEC, strRA_Value);

        if (pstHeader != IMS_NULL)
        {
            if (!SipStack::SetHeader(pstHeader, pstMessage))
            {
                SipStack::FreeHeaderEx(pstHeader);

                IMS_TRACE_E(0, "Setting Retry-After header field failed", 0, 0, 0);
                return;
            }

            SipStack::FreeHeaderEx(pstHeader);
        }
    }

    if (nStatusCode == SipStatusCode::SC_405)
    {
        // Sets the Allow header fields with all the supported methods
        for (IMS_SINT32 i = 0; i < SipMethod::MAX; ++i)
        {
            if ((i == SipMethod::REGISTER) || (i == SipMethod::PUBLISH) ||
                    (i == SipMethod::UNKNOWN))
            {
                continue;
            }

            SipHeaderBase* pstHeader =
                    SipStack::DecodeHeader(ISipHeader::ALLOW, SipMethod::ToName(i));

            if (pstHeader != IMS_NULL)
            {
                if (!SipStack::AppendHeader(pstHeader, pstMessage))
                {
                    SipStack::FreeHeaderEx(pstHeader);

                    IMS_TRACE_E(0, "Setting Allow header field failed", 0, 0, 0);
                    return;
                }

                SipStack::FreeHeaderEx(pstHeader);
            }
        }
    }

    // Set a default User-Agent or Server header field
    if (SipConfigProxy::IsUserAgentConfigured(GetSlotId(), GetSipProfile()))
    {
        AString strUAString = SipConfigProxy::GetUaString(GetSlotId(), GetSipProfile());

        if (strUAString.GetLength() > 0)
        {
            AString strHName;

            if (SipConfigProxy::IsUserAgentSetByContext(GetSlotId(), GetSipProfile()))
            {
                strHName = SipHeaderName::SERVER;
            }
            else
            {
                strHName = SipHeaderName::USER_AGENT;
            }

            SipHeaderBase* pstUserAgent =
                    SipStack::DecodeHeader(ISipHeader::UNKNOWN, strHName, strUAString);

            if (pstUserAgent != IMS_NULL)
            {
                (void)SipStack::SetUnknownHeader(pstUserAgent, strHName, pstMessage);
                SipStack::FreeHeaderEx(pstUserAgent);
            }
        }
    }

    IMS_TRACE_I("RejectRequest - Sending a %d response ...", nStatusCode, 0, 0);

    if (!FormMessage())
    {
        IMS_TRACE_E(0, "FormMessage failed", 0, 0, 0);
        return;
    }

    // Update the transaction timer values if the timer values need to be updated on runtime
    SipStackState::GetInstance()->SetTransactionTimerValues(GetSlotId(), GetSipProfile());

    IMS_SINT32 nTxnType = SipTimerValuesHelper::NON_INVITE_SERVER;

    if (objMethod.Equals(SipMethod::INVITE))
    {
        nTxnType = SipTimerValuesHelper::INVITE_SERVER;
    }

    SipTimerValues objTVs = SipTimerValuesHelper::GetValues(GetSlotId(), GetSipProfile(), nTxnType);

    Send(&objTVs);
}

/*

Remarks

*/
PUBLIC
void SipServerTransactionState::SetDefaultContact(IN CONST AString& strContact)
{
    //---------------------------------------------------------------------------------------------

    strDefaultContact = strContact;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 SipServerTransactionState::HandleRequest(OUT RCPtr<SipDialogEx>& pOrigDialogEx)
{
    IMS_SINT32 nValidity;

    //--------------------------------------------------------------------------------------------

    // Store the last request message
    SipStack::FreeMessage(pstLastMessage);
    pstLastMessage = SipStack::CloneMessage(pstMessage);

    // Check the address type of Contact header for the response message (TLS)

    RCPtr<SipDialogState> pTempDState = new SipDialogState(IMS_FALSE);

    if (!pTempDState->InitDialogDetails(pstMessage))
    {
        return SipPrivate::MESSAGE_FAILED;
    }

    IMS_BOOL bIsForked = IMS_FALSE;
    SipManager* pSIPMngr = SipManager::GetInstance();
    SipMethod objMethod = SipStack::GetMethod(pstMessage);
    SipMessageInfo objMInfo(GetSlotId(), objMethod, pstMessage, SipMessageInfo::DIRECTION_INCOMING);

    // Look up to identify if the dialog-state already exists or not
    RCPtr<SipDialogState> pOrigDState = pSIPMngr->LookupDialogState(pTempDState.Get(), pstMessage);

    // Checking for the case where the 2xx to SUBSCRIBE is received after NOTIFY
    // to the subscription, each of them being from different users i.e. SUBSCRIBE was forked.
    if ((pOrigDState.IsNull()) && objMethod.Equals(SipMethod::NOTIFY))
    {
        pOrigDState =
                pSIPMngr->LookupDialogState(pTempDState.Get(), pstMessage, IMS_TRUE, &bIsForked);

#if 0
        if (!pOrigDState.IsNull())
        {
            // When a forked NOTIFY received ...
            if (bIsForked)
            {
            }
            // When an ealry NOTIFY received ...
            else
            {
            }
        }
#endif
    }

    // The request is received outside of the dialog.
    if (pOrigDState.IsNull())
    {
        // If the method can create a dialog, make the dialog state to a permanent state.
        pDialogEx = SipDialogEx::CreateDialog(pTempDState.Get(), objMInfo);

        if (pDialogEx.IsNull())
        {
            IMS_TRACE_E(0, "NULL POINTER : Instantiating a dialog usage for %s failed",
                    objMethod.ToString().GetStr(), 0, 0);
            return SipPrivate::MESSAGE_FAILED;
        }

        // STRAY message handling
        // 1) ACK request will be discarded after its INVITE dialog is destroyed.
        // 2) The below requests are received after an INVITE dialog is destroyed.
        //   BYE, INFO, UPDATE, PRACK
        // 3) NOTIFY request is received after its SUBSCRIBE dialog is destroyed.
        if (objMethod.Equals(SipMethod::ACK))
        {
            // 4 Send it to application ??? : 2xx is handled by UA Core
            //  TRACE :: ACK request is received - DISCARD or JUST BYPASS ???
            return SipPrivate::MESSAGE_DISCARDED;
        }
        else if (objMethod.Equals(SipMethod::NOTIFY) || objMethod.Equals(SipMethod::BYE) ||
                objMethod.Equals(SipMethod::CANCEL) || objMethod.Equals(SipMethod::INFO) ||
                objMethod.Equals(SipMethod::UPDATE) || objMethod.Equals(SipMethod::PRACK))
        {
            RejectRequest(SipStatusCode::SC_481);
            SipPrivate::SetLastError(SipError::DIALOG_NOT_EXIST);

            return SipPrivate::MESSAGE_DISCARDED;
        }
        else
        {
            SipDialogState* pDialogState = pDialogEx->GetDialogState();
            AString strToTag =
                    (pDialogState != IMS_NULL) ? pDialogState->GetLocalTag() : AString::ConstNull();

            if (strToTag.GetLength() > 0)
            {
                IMS_TRACE_E(0, "To-tag is already present, but no existing dialog.", 0, 0, 0);
                RejectRequest(SipStatusCode::SC_481);
                SipPrivate::SetLastError(SipError::DIALOG_NOT_EXIST);

                return SipPrivate::MESSAGE_DISCARDED;
            }
        }

        // Checks if CSeq value is exceeded
        IMS_UINT32 nSeqNum = SipStack::GetCSeqNumber(pstMessage);

        if (nSeqNum > SipPrivate::MAX_CSEQ_VALUE)
        {
            IMS_TRACE_E(0, "CSeq number is exceeded", 0, 0, 0);

            RejectRequest(SipStatusCode::SC_400);
            SipPrivate::SetLastError(SipError::CSEQ_VALUE_EXCEEDED);

            return SipPrivate::MESSAGE_DISCARDED;
        }
    }
    // The request is received within the existing dialog.
    else
    {
        // The dialog is in EARLY state and this CANCEL is not related to the dialog,
        // so, the dialog for CANCEL is created with a default information...
        if (objMethod.Equals(SipMethod::CANCEL))
        {
            // Update the dialog details from INVITE-created dialog...
            pTempDState->InitDialogDetails(SipDialogState::DIALOG_CANCELLED, pOrigDState.Get());

            pDialogEx = SipDialogEx::CreateDialog(pTempDState.Get(), objMInfo);
        }
        else if (bIsForked && objMethod.Equals(SipMethod::NOTIFY))
        {
            // Update the dialog details from NOTIFY-created dialog...
            pTempDState->InitDialogDetails(
                    SipDialogState::DIALOG_FORKED_REQUEST, pOrigDState.Get());

            pDialogEx = SipDialogEx::CreateDialog(pTempDState.Get(), objMInfo);

            pOrigDialogEx = pOrigDState->GetDialogUsage(objMInfo);
        }
        // For abnormal case, BYE request received before receiving 1xx response to INVITE request
        else if (objMethod.Equals(SipMethod::BYE) && pOrigDState->IsCaller() &&
                (pOrigDState->GetState() == SipDState::STATE_INIT))
        {
            // Update the dialog details from INVITE-created dialog...
            pTempDState->InitDialogDetails(SipDialogState::DIALOG_CANCELLED, pOrigDState.Get());

            pDialogEx = SipDialogEx::CreateDialog(pTempDState.Get(), objMInfo);

            IMS_TRACE_D("___ BYE request received before receiving 1xx response "
                        "to INVITE request ___",
                    0, 0, 0);
        }
        else
        {
            // Find an existing dialog usage from this server transaction if present...
            pDialogEx = pOrigDState->GetDialogUsage(objMInfo);

            if (pDialogEx.IsNull())
            {
                // If the subscribe dialog usage is not found for this NOTIFY request,
                // then reject the request with 481 ...
                if (objMethod.Equals(SipMethod::NOTIFY))
                {
                    // Create a new dialog usage from this server transaction...
                    pDialogEx = SipDialogEx::CreateDialog(pTempDState.Get(), objMInfo);

                    if (!pDialogEx.IsNull())
                    {
                        RejectRequest(SipStatusCode::SC_481);
                    }

                    SipPrivate::SetLastError(SipError::DIALOG_NOT_EXIST);

                    return SipPrivate::MESSAGE_DISCARDED;
                }

                IMS_TRACE_D("_____ CREATE A NEW DIALOG USAGE (%s) _____",
                        objMethod.ToString().GetStr(), 0, 0);

                // Create a new dialog usage from this server transaction...
                pDialogEx = SipDialogEx::CreateDialog(pOrigDState.Get(), objMInfo);
            }
            else
            {
                IMS_TRACE_D("_____ REUSE AN EXISTING DIALOG USAGE (%s) _____",
                        objMethod.ToString().GetStr(), 0, 0);
            }
        }

        // Request-URI VALIDATION
#ifdef __IMS_SIP_REQUEST_URI_VALIDATION__
        if (!objMethod.Equals(SipMethod::CANCEL))
        {
            SipAddrSpec* pstAddrSpec = SipStack::GetRequestUri(pstMessage);
            SipAddrSpec* pstLocalAddrSpec = SipStack::GetAddrSpec(pOrigDState->GetLocalTargetUri());

            if ((pstAddrSpec != IMS_NULL) && (pstLocalAddrSpec != IMS_NULL))
            {
                AString strAddrSpec = AString::ConstNull();
                AString strLocalAddrSpec = AString::ConstNull();

                SipStack::EncodeAddrSpec(pstAddrSpec, IMS_TRUE, strAddrSpec);
                SipStack::EncodeAddrSpec(pstLocalAddrSpec, IMS_TRUE, strLocalAddrSpec);

                SipAddress objAddress(strAddrSpec);
                SipAddress objLocalAddress(strLocalAddrSpec);

                if (!objAddress.Equals(objLocalAddress))
                {
                    SipStack::FreeAddrSpec(pstAddrSpec);
                    SipStack::FreeAddrSpec(pstLocalAddrSpec);

                    IMS_TRACE_D("Request-URI (%s, %s) is not matched",
                            SipDebug::GetUri1(strLocalAddrSpec).GetStr(),
                            SipDebug::GetUri2(strAddrSpec).GetStr(), 0);

                    if (!objMethod.Equals(SipMethod::ACK))
                    {
                        RejectRequest(SipStatusCode::SC_404);
                    }

                    return SipPrivate::MESSAGE_INVALID;
                }
            }
            else
            {
                SipStack::FreeAddrSpec(pstAddrSpec);
                SipStack::FreeAddrSpec(pstLocalAddrSpec);

                IMS_TRACE_D("Request-URI is not matched", 0, 0, 0);

                if (!objMethod.Equals(SipMethod::ACK))
                {
                    RejectRequest(SipStatusCode::SC_404);
                }

                return SipPrivate::MESSAGE_INVALID;
            }

            SipStack::FreeAddrSpec(pstAddrSpec);
            SipStack::FreeAddrSpec(pstLocalAddrSpec);
        }
#endif

        // Validate a CSeq header
        nValidity = pOrigDState->ValidateRemoteCSeq(pstMessage,
                ((pstTxnKey != IMS_NULL) ? SipStack::TxnKey_GetStatusCode(pstTxnKey) : 0));

        if (nValidity != SipPrivate::MESSAGE_VALID)
        {
            IMS_TRACE_E(0, "CSeq number is out of sequence", 0, 0, 0);

            switch (nValidity)
            {
                case SipPrivate::MESSAGE_INVALID_400:
                    RejectRequest(SipStatusCode::SC_400);
                    break;

                case SipPrivate::MESSAGE_INVALID_500:
                    RejectRequest(SipStatusCode::SC_500);
                    break;

                default:
                    break;
            }

            return nValidity;
        }
    }

    // METHOD INSPECTION
    //  In case of REGISTER & PUBLISH, it is not allowed in our device.
    //  So, if the method is REGISTER & PUBLISH, it will be rejected by the UA core.
    if (objMethod.Equals(SipMethod::REGISTER) || objMethod.Equals(SipMethod::PUBLISH))
    {
        RejectRequest(SipStatusCode::SC_405);
        return SipPrivate::MESSAGE_INVALID;
    }

    // VIA & RECORD-ROUTE header(s) handling
    // TIMESTAMP header handling

    nValidity = pDialogEx->UpdateDialogDetails(objMInfo);

    if (nValidity != SipPrivate::MESSAGE_VALID)
    {
        IMS_TRACE_E(0, "Updating the dialog details failed (%d)", nValidity, 0, 0);

        switch (nValidity)
        {
            case SipPrivate::MESSAGE_INVALID_400:
                RejectRequest(SipStatusCode::SC_400);
                break;

            case SipPrivate::MESSAGE_INVALID_481:
                RejectRequest(SipStatusCode::SC_481);
                break;

            case SipPrivate::MESSAGE_INVALID_500:
                RejectRequest(SipStatusCode::SC_500);
                break;

            default:
                break;
        }

        return nValidity;
    }

    if (Is100TryingResponseRequired(objMethod))
    {
        IMS_SINT32 nTV_100Trying = SipConfigProxy::GetTimerValue100Trying(GetSlotId());

        if (nTV_100Trying > 0)
        {
            // Start a timer for 100 Trying response transmission.
            StartTimer100Trying(this, nTV_100Trying);
        }
        else
        {
            // Send 100 Trying response immediately
            SendResponse100Trying(this);
        }
    }

    if (bIsForked)
    {
        return SipPrivate::MESSAGE_VALID_FORKED;
    }

    return SipPrivate::MESSAGE_VALID;
}

/*

Remarks

*/
PRIVATE VIRTUAL void SipServerTransactionState::Timer_TimerExpired(IN ITimer* piTimer)
{
    //---------------------------------------------------------------------------------------------

    if (piTimer_100Trying == IMS_NULL)
        return;

    if (piTimer_100Trying != piTimer)
        return;

    IMS_TRACE_I("SipServerTransactionState - 100 Trying response timer expired", 0, 0, 0);

    SendResponse100Trying(this);

    piTimer_100Trying->KillTimer();
    TimerService::GetTimerService()->DestroyTimer(piTimer_100Trying);
    piTimer_100Trying = IMS_NULL;
}

/*

Remarks

*/
PRIVATE
IMS_BOOL SipServerTransactionState::InitResponse(
        IN IMS_SINT32 nStatusCode, OUT ::SipMessage*& pstOutMessage)
{
    SipHeaderBase* pstGetHdr;
    SipHeaderBase* pstHeader;

    SipMethod objMethod = SipStack::GetMethod(pstLastMessage);

    //---------------------------------------------------------------------------------------------

    // CSeq
    pstGetHdr = SipStack::GetHeader(pstLastMessage, ISipHeader::CSEQ);

    pstHeader = SipStack::CloneHeader(pstGetHdr);
    SipStack::FreeHeaderEx(pstGetHdr);

    if (pstHeader != IMS_NULL)
    {
        if (!SipStack::SetHeader(pstHeader, pstOutMessage))
        {
            SipStack::FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        SipStack::FreeHeaderEx(pstHeader);
    }

    // From
    pstGetHdr = SipStack::GetHeader(pstLastMessage, ISipHeader::FROM);

    pstHeader = SipStack::CloneHeader(pstGetHdr);
    SipStack::FreeHeaderEx(pstGetHdr);

    if (pstHeader != IMS_NULL)
    {
        if (!SipStack::SetHeader(pstHeader, pstOutMessage))
        {
            SipStack::FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        SipStack::FreeHeaderEx(pstHeader);
    }

    // To
    pstGetHdr = SipStack::GetHeader(pstLastMessage, ISipHeader::TO);

    pstHeader = SipStack::CloneHeader(pstGetHdr);
    SipStack::FreeHeaderEx(pstGetHdr);

    if (pstHeader != IMS_NULL)
    {
        // Check if the tag is present, and if not present, insert a new tag value
        if ((nStatusCode != SipStatusCode::SC_100) &&
                !SipStack::HasParameter(pstHeader, Sip::STR_TAG))
        {
            AString strTagVal = pDialogEx->GetDialogState()->GetLocalTag();

            if (strTagVal.IsNULL())
            {
                strTagVal = SipUtils::GenerateTag(
                        SipConfigProxy::GetTagPrefix(GetSlotId(), GetSipProfile()));
            }

            if (!SipStack::SetParameter(pstHeader, Sip::STR_TAG, strTagVal))
            {
                SipStack::FreeHeaderEx(pstHeader);
                return IMS_FALSE;
            }
        }

        if (!SipStack::SetHeader(pstHeader, pstOutMessage))
        {
            SipStack::FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        SipStack::FreeHeaderEx(pstHeader);
    }

    // Call-ID
    pstGetHdr = SipStack::GetHeader(pstLastMessage, ISipHeader::CALL_ID);

    pstHeader = SipStack::CloneHeader(pstGetHdr);
    SipStack::FreeHeaderEx(pstGetHdr);

    if (pstHeader != IMS_NULL)
    {
        if (!SipStack::SetHeader(pstHeader, pstOutMessage))
        {
            SipStack::FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        SipStack::FreeHeaderEx(pstHeader);
    }

    // Via headers with the same order
    IMS_SINT32 nHCount = SipStack::GetHeaderCount(pstLastMessage, ISipHeader::VIA);

    for (IMS_SINT32 i = 0; i < nHCount; ++i)
    {
        pstGetHdr = SipStack::GetHeader(pstLastMessage, ISipHeader::VIA, i);

        if (SipStack::IsValidHeader(pstGetHdr))
        {
            if (!SipStack::InsertHeader(pstGetHdr, i, pstOutMessage))
            {
                SipStack::FreeHeaderEx(pstGetHdr);
                return IMS_FALSE;
            }
        }

        SipStack::FreeHeaderEx(pstGetHdr);
    }

    // Record-Route headers with the same order
    if (SipStatusCode::IsProvisional(nStatusCode) || SipStatusCode::IsFinalSuccess(nStatusCode))
    {
        nHCount = SipStack::GetHeaderCount(pstLastMessage, ISipHeader::RECORD_ROUTE);

        for (IMS_SINT32 i = 0; i < nHCount; ++i)
        {
            pstGetHdr = SipStack::GetHeader(pstLastMessage, ISipHeader::RECORD_ROUTE, i);

            if (SipStack::IsValidHeader(pstGetHdr))
            {
                if (!SipStack::InsertHeader(pstGetHdr, i, pstOutMessage))
                {
                    SipStack::FreeHeaderEx(pstGetHdr);
                    return IMS_FALSE;
                }
            }

            SipStack::FreeHeaderEx(pstGetHdr);
        }
    }

    // Contact
    IMS_BOOL bContactInAll1xxRequired =
            SipConfigProxy::IsContactInAll1xxRequired(GetSlotId(), GetSipProfile());

    if (SipDialogState::IsContactMandatory(sipcore::SipMessage::TYPE_RESPONSE, objMethod,
                nStatusCode, bContactInAll1xxRequired))
    {
        IMS_BOOL bContactHeaderInserted = IMS_FALSE;
        IMS_SINT32 nState = pDialogEx->GetState();

        if ((nState == SipDState::STATE_EARLY) || (nState == SipDState::STATE_CONFIRMED))
        {
            pstHeader = pDialogEx->GetDialogState()->GetLocalTargetUri();

            if (pstHeader != IMS_NULL)
            {
                if (SipStack::IsValidHeader(pstHeader))
                {
                    if (!SipStack::SetHeader(pstHeader, pstOutMessage))
                    {
                        return IMS_FALSE;
                    }

                    pstHeader = IMS_NULL;
                    bContactHeaderInserted = IMS_TRUE;
                }
            }
        }

        // If Contact header is not inserted, sets the Contact header to a default contact.
        if (!bContactHeaderInserted && !objMethod.Equals(SipMethod::MESSAGE) &&
                (strDefaultContact.GetLength() > 0))
        {
            pstHeader = SipStack::DecodeHeader(ISipHeader::CONTACT_NORMAL, strDefaultContact);

            if (pstHeader != IMS_NULL)
            {
                if (!SipStack::SetHeader(pstHeader, pstOutMessage))
                {
                    SipStack::FreeHeaderEx(pstHeader);
                    return IMS_FALSE;
                }

                SipStack::FreeHeaderEx(pstHeader);
            }
        }
    }

    // Timestamp header field, RFC 3261
    if (nStatusCode == SipStatusCode::SC_100)
    {
        pstGetHdr = SipStack::GetHeader(pstLastMessage, ISipHeader::TIMESTAMP);

        if (SipStack::IsValidHeader(pstGetHdr))
        {
            if (!SipStack::SetHeader(pstGetHdr, pstOutMessage))
            {
                SipStack::FreeHeaderEx(pstGetHdr);
                return IMS_FALSE;
            }
        }

        SipStack::FreeHeaderEx(pstGetHdr);
    }
    // HEADER_REQ_SESSION-ID
    else if (SipFeatures::IsHeaderSessionIdRequired(GetSlotId()))
    {
        SipDialogState* pDState = pDialogEx->GetDialogState();
        AString strSessionId = pDState->GetSessionId();

        if (strSessionId.GetLength() == 0)
        {
            strSessionId = SipUtils::GenerateSessionId(GetSlotId(), pDState->GetCallId());
        }

        if (strSessionId.GetLength() > 0)
        {
            const AString SESSION_ID(SipHeaderName::SESSION_ID);
            SipHeaderBase* pstSessionId =
                    SipStack::DecodeHeader(ISipHeader::UNKNOWN, SESSION_ID, strSessionId);

            if (pstSessionId != IMS_NULL)
            {
                (void)SipStack::SetUnknownHeader(pstSessionId, SESSION_ID, pstMessage);
                SipStack::FreeHeaderEx(pstSessionId);
            }
        }
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PRIVATE
IMS_BOOL SipServerTransactionState::UpdateTxnDetails()
{
    SipMethod objMethod = SipStack::GetMethod(pstMessage);
    SipMessageInfo objMInfo(GetSlotId(), objMethod, pstMessage, SipMessageInfo::DIRECTION_OUTGOING);

    //---------------------------------------------------------------------------------------------

    IMS_SINT32 nValidity = pDialogEx->UpdateDialogDetails(objMInfo);

    if (nValidity != SipPrivate::MESSAGE_VALID)
    {
        return IMS_FALSE;
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PRIVATE GLOBAL IMS_BOOL SipServerTransactionState::Is100TryingResponseRequired(
        IN CONST SipMethod& objMethod)
{
    //---------------------------------------------------------------------------------------------

    // The INVITE server transaction MUST generate a 100 (Trying) response unless it knows
    // that the TU will generate a provisional or final response within 200 ms, in which case
    // it MAY generate a 100 (Trying) response.
    return objMethod.Equals(SipMethod::INVITE);
}

/*

Remarks

*/
PRIVATE GLOBAL IMS_RESULT SipServerTransactionState::SendResponse100Trying(
        IN SipServerTransactionState* pSTState)
{
    ::SipMessage* pstMessage100Trying = SipStack::CreateMessage(SipStack::SIP_MESSAGE_RESPONSE);

    //---------------------------------------------------------------------------------------------

    if (pstMessage100Trying == IMS_NULL)
    {
        IMS_TRACE_E(0, "Creating a 100 Trying response message failed", 0, 0, 0);
        return IMS_FAILURE;
    }

    // Set the status code
    AString strPhrase = SipStatusCode::GetReasonPhrase(SipStatusCode::SC_100);

    if (!SipStack::SetStatusLine(SipStatusCode::SC_100, strPhrase, pstMessage100Trying))
    {
        SipStack::FreeMessage(pstMessage100Trying);

        IMS_TRACE_E(0, "Setting a status-line of 100 Trying response failed", 0, 0, 0);
        return IMS_FAILURE;
    }

    // Initialize a default header field from the previous request message
    if (!pSTState->InitResponse(SipStatusCode::SC_100, pstMessage100Trying))
    {
        SipStack::FreeMessage(pstMessage100Trying);

        IMS_TRACE_E(0, "Initializing a 100 Trying response message failed", 0, 0, 0);
        return IMS_FAILURE;
    }

    IMS_TRACE_I("___ SENDING 100 \"TRYING\" ...", 0, 0, 0);

    // Update the transport information to route the response to the proper network entity
    if (pSTState->pTransport != IMS_NULL)
    {
        pSTState->pTransport->UpdateDestinationInfo(pstMessage100Trying);
    }

    // Send SIP message to the network
    pSTState->SipTransactionState::Send(pstMessage100Trying, IMS_NULL);

    SipStack::FreeMessage(pstMessage100Trying);

    return IMS_SUCCESS;
}

/*

Remarks

*/
PRIVATE GLOBAL void SipServerTransactionState::StartTimer100Trying(
        IN SipServerTransactionState* pSTState, IN IMS_SINT32 nTimerInterval /* milli-seconds */)
{
    //---------------------------------------------------------------------------------------------

    if (nTimerInterval > 0)
    {
        pSTState->piTimer_100Trying = TimerService::GetTimerService()->CreateTimer();

        if (pSTState->piTimer_100Trying != IMS_NULL)
        {
            // 200 ms
            pSTState->piTimer_100Trying->SetTimer(nTimerInterval, pSTState);
        }
    }
}

/*

Remarks

*/
PRIVATE GLOBAL void SipServerTransactionState::StopTimer100Trying(
        IN SipServerTransactionState* pSTState)
{
    //---------------------------------------------------------------------------------------------

    if (pSTState->piTimer_100Trying != IMS_NULL)
    {
        IMS_TRACE_I("___ Stopping 100 Trying response timer ...", 0, 0, 0);

        pSTState->piTimer_100Trying->KillTimer();
        TimerService::GetTimerService()->DestroyTimer(pSTState->piTimer_100Trying);
        pSTState->piTimer_100Trying = IMS_NULL;
    }
}
