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
#include "ISipHeader.h"
#include "SipPrivate.h"
#include "SipDebug.h"
#include "SipHeaderName.h"
#include "SipFeatures.h"
#include "SipConfigProxy.h"
#include "SipStackHeaders.h"
#include "SipStackState.h"
#include "SipTxnContextData.h"
#include "SipUtils.h"
#include "SipDialogEx.h"
#include "ISipClientTransactionStateListener.h"
#include "SipAckPackage.h"
#include "SipFactoryProxy.h"
// SIP_MESSAGE_TRACKER
#include "SipMessageTracker.h"
#include "SipClientTransport.h"
#include "PAccessNetworkInfoHeader.h"
#include "SipClientTransactionState.h"

__IMS_TRACE_TAG_SIP__;

PUBLIC
SipClientTransactionState::SipClientTransactionState(IN IMS_SINT32 nSlotId) :
        SipTransactionState(),
        nRoutingType(TARGET_LR),
        pstImplicitRoute(IMS_NULL),
        piListener(IMS_NULL)
{
    nType = TYPE_CLIENT;

    pTransport = new SipClientTransport(nSlotId);
}

PUBLIC VIRTUAL SipClientTransactionState::~SipClientTransactionState()
{
    SipStack::FreeAddrSpec(pstImplicitRoute);

#ifdef __IMS_SIP_DEBUG__
    IMS_TRACE_D("Destructor :: SipClientTransactionState", 0, 0, 0);
#endif
}

/*

Remarks

*/
PUBLIC VIRTUAL void SipClientTransactionState::Abort()
{
    //---------------------------------------------------------------------------------------------

    // FORKED_RESPONSE
    if (RemoveForkedTransaction() > 0)
        return;

    SipTransactionState::Abort();
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_SINT32 SipClientTransactionState::CheckMessageValidity()
{
    IMS_SINT32 nValidity;

    //---------------------------------------------------------------------------------------------

    // Check the validity of transport information from the top Via header
    nValidity = pTransport->ValidateViaHeader(pstMessage);

    if (nValidity != SipPrivate::MESSAGE_VALID)
        return nValidity;

    return SipPrivate::MESSAGE_VALID;
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_BOOL SipClientTransactionState::FormMessage()
{
    // Update the transport information
    if (!UpdateTransportDetails())
    {
        SipPrivate::SetLastError(SipError::GENERAL_ERROR);
        return IMS_FALSE;
    }

    SipMethod objMethod = SipStack::GetMethod(pstMessage);

    if (!SetDialogRelatedHeaders(objMethod))
    {
        IMS_TRACE_E(0, "Setting a dialog-related headers failed", 0, 0, 0);
        return IMS_FALSE;
    }

    if (!pTransport->FormViaHeader(pstMessage, GetSipProfile()))
    {
        return IMS_FALSE;
    }

    SetFlowControlOption(objMethod);

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_BOOL SipClientTransactionState::FormMessageForResubmissionRequest()
{
    //---------------------------------------------------------------------------------------------

    // Update the transport information
    // 4 Checks if the sec-agree is supported or not
    if (!UpdateTransportDetails())
    {
        SipPrivate::SetLastError(SipError::GENERAL_ERROR);
        return IMS_FALSE;
    }

    // Remove the previous Via header from the message
    SipStack::RemoveHeader(ISipHeader::VIA, pstMessage);

    // Remove the from-tag
    IMS_BOOL bFromTagRemovalRequired = IMS_TRUE;

    if (!pDialogEx.IsNull())
    {
        if (pDialogEx->GetState() == SipDState::STATE_CONFIRMED)
        {
            // Do not remove "from-tag" from From header within SIP dialog
            bFromTagRemovalRequired = IMS_FALSE;
        }
    }

    if (bFromTagRemovalRequired)
    {
        SipHeaderBase* pstFromHeader = SipStack::GetHeader(pstMessage, ISipHeader::FROM);
        SipStack::RemoveParameter(Sip::STR_TAG, pstFromHeader);
        SipStack::FreeHeaderEx(pstFromHeader);
    }

    SipMethod objMethod = SipStack::GetMethod(pstMessage);
    AString strHBody;

    // Sets CSeq header field
    strHBody.SetNumber(++nCSeqNumber);
    strHBody.Append(TextParser::CHAR_SP);
    strHBody.Append(objMethod.ToString());

    SipHeaderBase* pstHeader = SipStack::DecodeHeader(ISipHeader::CSEQ, strHBody);

    if (!SipStack::SetHeader(pstHeader, pstMessage))
    {
        SipStack::FreeHeaderEx(pstHeader);
        return IMS_FALSE;
    }

    SipStack::FreeHeaderEx(pstHeader);

    if (!SetDialogRelatedHeaders(objMethod))
    {
        IMS_TRACE_E(0, "Setting a dialog-related headers failed", 0, 0, 0);
        return IMS_FALSE;
    }

    if (!pTransport->FormViaHeader(pstMessage, GetSipProfile()))
    {
        return IMS_FALSE;
    }

    SetFlowControlOption(objMethod);

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_BOOL SipClientTransactionState::InitTxnDetails(
        IN CONST SipTransactionState* pTState)
{
    const SipClientTransactionState* pCTState =
            DYNAMIC_CAST(const SipClientTransactionState*, pTState);

    //---------------------------------------------------------------------------------------------

    if (pCTState == IMS_NULL)
    {
        return IMS_FALSE;
    }

    if (!SipTransactionState::InitTxnDetails(pCTState))
    {
        return IMS_FALSE;
    }

    nRoutingType = pCTState->nRoutingType;

    // IMPLICIT_ROUTE
    SipStack::FreeAddrSpec(pstImplicitRoute);

    if (pCTState->pstImplicitRoute != IMS_NULL)
    {
        pstImplicitRoute = pCTState->pstImplicitRoute;
        SipStack::AddReference(pstImplicitRoute);
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_BOOL SipClientTransactionState::Send(IN SipTimerValues* pTV /* = IMS_NULL */)
{
    SipMethod objMethod = SipStack::GetMethod(pstMessage);

    //---------------------------------------------------------------------------------------------

    if (!UpdateTxnDetails(objMethod))
    {
        IMS_TRACE_E(0, "Updating transaction details failed", 0, 0, 0);
        return IMS_FALSE;
    }

    return SipTransactionState::Send(pTV);
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_BOOL SipClientTransactionState::UpdateTransportDetails()
{
    IMS_BOOL bRoutingLR = (nRoutingType == TARGET_LR) ? IMS_TRUE : IMS_FALSE;
    IMS_BOOL bImplicitRouteRequired = IMS_FALSE;
    IMS_BOOL bCheckImplicitRouteUsage = IMS_FALSE;
    IMS_BOOL bIgnoreLR = IMS_FALSE;

    //---------------------------------------------------------------------------------------------

    if (pstImplicitRoute != IMS_NULL)
    {
        if (nRoutingType == TARGET_NO_ROUTE)
        {
            bCheckImplicitRouteUsage = IMS_TRUE;
        }
        else if (!pSIPProfile.IsNull() &&
                pSIPProfile->IsConfigurationSet(
                        SipProfile::CONFIG_IGNORE_RR_ON_IMPLICIT_ROUTE_REQUIRED))
        {
            bCheckImplicitRouteUsage = IMS_TRUE;
            bIgnoreLR = IMS_TRUE;
        }
    }

    // IMPLICIT_ROUTE
    if (bCheckImplicitRouteUsage)
    {
        IMS_SINT32 nDialogState = pDialogEx->GetState();

        if (nDialogState == SipDState::STATE_INIT)
        {
            bImplicitRouteRequired = IMS_TRUE;
        }
        // IMPLICIT_ROUTING_FOR_MID_DIALOG
        else if (pDialogEx->IsInviteUsage() && (nDialogState == SipDState::STATE_CONFIRMED))
        {
            bImplicitRouteRequired = IMS_TRUE;
        }
        // For routing all mid-dialog requests to the first outbound proxy
        else if ((nDialogState == SipDState::STATE_EARLY) ||
                (nDialogState == SipDState::STATE_CONFIRMED))
        {
            bImplicitRouteRequired = IMS_TRUE;
        }
        // BYE_REQUEST_ON_DIALOG_TERMINATED
        else if (pDialogEx->IsInviteUsage() && (nDialogState == SipDState::STATE_TERMINATED))
        {
            SipMethod objMethod = SipStack::GetMethod(pstMessage);

            if (objMethod.Equals(SipMethod::BYE))
            {
                bImplicitRouteRequired = IMS_TRUE;
            }
        }

        if (bImplicitRouteRequired && bIgnoreLR)
        {
            bRoutingLR = IMS_FALSE;
            IMS_TRACE_I("ImplicitRoute :: Ignore route-set", 0, 0, 0);
        }
    }

    if (!pTransport->UpdateDestinationInfo(pstMessage, bRoutingLR,
                (bImplicitRouteRequired == IMS_TRUE) ? pstImplicitRoute : IMS_NULL))
    {
        return IMS_FALSE;
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipClientTransactionState::AdjustTransportProtocolAsUdp()
{
    if (pTransport == IMS_NULL)
    {
        return IMS_FALSE;
    }

    SipStack::RemoveHeader(ISipHeader::VIA, pstMessage);

    pTransport->SetProtocol(SipTransportAddress::PROTOCOL_UDP, SipTransport::TA_NEAR);
    pTransport->SetProtocol(SipTransportAddress::PROTOCOL_UDP, SipTransport::TA_FAR);
    pTransport->SetExplicitTargetProtocol(IMS_TRUE);

    if (!pTransport->FormViaHeader(pstMessage, GetSipProfile()))
    {
        return IMS_FALSE;
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
::SipMessage* SipClientTransactionState::CreateAckRequest(IN ::SipMessage* pstRespMessage)
{
    IMS_TRACE_D("CreateAckRequest", 0, 0, 0);

    ::SipMessage* pstAckMessage = SipStack::CreateMessage(SipStack::SIP_MESSAGE_REQUEST);

    if (pstAckMessage == IMS_NULL)
    {
        IMS_TRACE_E(0, "Creating an ACK request message failed", 0, 0, 0);
        return IMS_NULL;
    }

    if (pstRespMessage == IMS_NULL)
    {
        pstRespMessage = pstMessage;
    }

    // Initializes ACK request message
    if (!InitAck(pstAckMessage, pstRespMessage))
    {
        IMS_TRACE_E(0, "Initializing ACK request failed", 0, 0, 0);
        SipStack::FreeMessage(pstAckMessage);
        return IMS_NULL;
    }

    return pstAckMessage;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipClientTransactionState::InitCancel(IN CONST SipClientTransactionState* pInviteTState)
{
    SipHeaderBase* pstGetHdr;
    SipHeaderBase* pstHeader;
    SipMethod objMethod(SipMethod::CANCEL);

    //---------------------------------------------------------------------------------------------

    nClass = CLASS_REGULAR;

    SetSipProfile(pInviteTState->GetSipProfile());

    // Sets a SIP method name
    SipStack::SetMethod(objMethod, pstMessage);

    // Sets a Request-URI field
    SipAddrSpec* pstAddrSpec = SipStack::GetRequestUri(pInviteTState->pstLastMessage);

    (void)SipStack::SetRequestUri(pstAddrSpec, pstMessage);
    SipStack::FreeAddrSpec(pstAddrSpec);

    AString strHBody;

    // Sets CSeq header field
    strHBody.SetNumber(pInviteTState->nCSeqNumber);
    strHBody.Append(TextParser::CHAR_SP);
    strHBody.Append(SipMethod::NAME[SipMethod::CANCEL]);

    pstHeader = SipStack::DecodeHeader(ISipHeader::CSEQ, strHBody);

    if (!SipStack::SetHeader(pstHeader, pstMessage))
    {
        SipStack::FreeHeaderEx(pstHeader);
        return IMS_FALSE;
    }

    SipStack::FreeHeaderEx(pstHeader);

    // Sets Max-Forwards header field
    strHBody.SetNumber(SipPrivate::MAX_HOP);
    pstHeader = SipStack::DecodeHeader(ISipHeader::MAX_FORWARDS, strHBody);

    if (!SipStack::SetHeader(pstHeader, pstMessage))
    {
        SipStack::FreeHeaderEx(pstHeader);
        return IMS_FALSE;
    }

    SipStack::FreeHeaderEx(pstHeader);

    // Sets From header field
    pstGetHdr = SipStack::GetHeader(pInviteTState->pstLastMessage, ISipHeader::FROM);

    pstHeader = SipStack::CloneHeader(pstGetHdr);
    SipStack::FreeHeaderEx(pstGetHdr);

    if (pstHeader != IMS_NULL)
    {
        if (!SipStack::SetHeader(pstHeader, pstMessage))
        {
            SipStack::FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        SipStack::FreeHeaderEx(pstHeader);
    }

    // Sets To header field
    pstGetHdr = SipStack::GetHeader(pInviteTState->pstLastMessage, ISipHeader::TO);

    pstHeader = SipStack::CloneHeader(pstGetHdr);
    SipStack::FreeHeaderEx(pstGetHdr);

    if (pstHeader != IMS_NULL)
    {
        // Remove To-Tag if the CANCEL transaction is for initial INVITE request
        IMS_BOOL bInitialINVITE = IMS_TRUE;

        if (!pInviteTState->pDialogEx.IsNull())
        {
            SipDialogState* pDState = pInviteTState->pDialogEx->GetDialogState();

            if (pDState != IMS_NULL)
            {
                if (pDState->GetState() == SipDState::STATE_CONFIRMED)
                {
                    bInitialINVITE = IMS_FALSE;
                }
            }
        }

        if (bInitialINVITE)
        {
            (void)SipStack::RemoveParameter(AString(Sip::STR_TAG), pstHeader);
        }

        if (!SipStack::SetHeader(pstHeader, pstMessage))
        {
            SipStack::FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        SipStack::FreeHeaderEx(pstHeader);
    }

    // Sets Call-ID header field
    pstGetHdr = SipStack::GetHeader(pInviteTState->pstLastMessage, ISipHeader::CALL_ID);

    pstHeader = SipStack::CloneHeader(pstGetHdr);
    SipStack::FreeHeaderEx(pstGetHdr);

    if (pstHeader != IMS_NULL)
    {
        if (!SipStack::SetHeader(pstHeader, pstMessage))
        {
            SipStack::FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        SipStack::FreeHeaderEx(pstHeader);
    }

    // Sets Via header field
    pstGetHdr = SipStack::GetHeader(pInviteTState->pstLastMessage, ISipHeader::VIA);

    pstHeader = SipStack::CloneHeader(pstGetHdr);
    SipStack::FreeHeaderEx(pstGetHdr);

    if (pstHeader != IMS_NULL)
    {
        if (!SipStack::SetHeader(pstHeader, pstMessage))
        {
            SipStack::FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        SipStack::FreeHeaderEx(pstHeader);
    }

    // Route
    IMS_SINT32 nHCount = SipStack::GetHeaderCount(pInviteTState->pstLastMessage, ISipHeader::ROUTE);

    for (IMS_SINT32 i = 0; i < nHCount; i++)
    {
        pstGetHdr = SipStack::GetHeader(pInviteTState->pstLastMessage, ISipHeader::ROUTE, i);

        if (SipStack::IsValidHeader(pstGetHdr))
        {
            if (!SipStack::AppendHeader(pstGetHdr, pstMessage))
            {
                SipStack::FreeHeaderEx(pstGetHdr);
                return IMS_FALSE;
            }
        }

        SipStack::FreeHeaderEx(pstGetHdr);
    }

    // IMPLICIT_ROUTE
    SipStack::FreeAddrSpec(pstImplicitRoute);

    if (pInviteTState->pstImplicitRoute != IMS_NULL)
    {
        pstImplicitRoute = pInviteTState->pstImplicitRoute;
        SipStack::AddReference(pstImplicitRoute);
    }

    // Update the transport information
    const SipTransportAddress& objNearTA =
            pInviteTState->pTransport->GetAddress(SipTransport::TA_NEAR);
    const SipTransportAddress& objFarTA =
            pInviteTState->pTransport->GetAddress(SipTransport::TA_FAR);

    // RFC5626_FLOW_CONTROL
    pTransport->SetTransportTuple(objNearTA.GetIpAddress(), objNearTA.GetPort(),
            pInviteTState->pTransport->GetPortC(), pInviteTState->pTransport->GetPortFlowControl(),
            pInviteTState->pTransport->GetTransportExt());

    // FIX_TRANSPORT_PROTOCOL
    pTransport->SetProtocol(objNearTA.GetProtocol(), SipTransport::TA_NEAR);
    pTransport->SetProtocol(objFarTA.GetProtocol(), SipTransport::TA_FAR);

    // Create a dialog
    pDialogEx = SipDialogEx::CreateDialog(objMethod);

    if (pDialogEx.IsNull())
    {
        return IMS_FALSE;
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipClientTransactionState::InitRequest(IN CONST SipMethod& objMethod)
{
    //---------------------------------------------------------------------------------------------

    // Set a mandatory headers in the INITIAL state
    //    - CSeq, Max-Forwards headers
    if (!SetMandatoryHeaders(objMethod))
    {
        return IMS_FALSE;
    }

    // Create a dialog usage according to the method
    pDialogEx = SipDialogEx::CreateDialog(objMethod);

    if (pDialogEx.IsNull())
    {
        return IMS_FALSE;
    }

    // FORKED_RESPONSE
    if (objMethod.Equals(SipMethod::INVITE))
    {
        pForkedTxnMngr = new SipForkedTransactionManager();

        if (pForkedTxnMngr.IsNull())
        {
            return IMS_FALSE;
        }

        pForkedTxnMngr->Add(this);

        // FORKED_RESPONSE_TO_SUPPORT_EARLY_DIALOG_TERMINATION
        // Grab the forked response manager when initiating the INVITE transaction
        pPersistentForkedTxnMngr = pForkedTxnMngr;
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipClientTransactionState::InitRequest(
        IN CONST SipMethod& objMethod, IN SipDialogEx* pDialogEx)
{
    SipDialogState* pDState;

    //---------------------------------------------------------------------------------------------

    if (!objMethod.Equals(SipMethod::ACK))
    {
        this->pDialogEx = pDialogEx;
        pDState = this->pDialogEx->GetDialogState();

        nCSeqNumber = pDState->GetNextCSeqNumber();
    }
    else
    {
        pDState = this->pDialogEx->GetDialogState();

        // For ACK transaction, reset the transport protocol to a default transport protocol
        pTransport->SetProtocol(SipTransportAddress::PROTOCOL_UDP, SipTransport::TA_NEAR);
        pTransport->SetProtocol(SipTransportAddress::PROTOCOL_UDP, SipTransport::TA_FAR);
    }

    // FORKED_RESPONSE
    if (objMethod.Equals(SipMethod::INVITE))
    {
        pForkedTxnMngr = new SipForkedTransactionManager();

        if (pForkedTxnMngr.IsNull())
        {
            return IMS_FALSE;
        }

        pForkedTxnMngr->Add(this);

        // FORKED_RESPONSE_TO_SUPPORT_EARLY_DIALOG_TERMINATION
        // Grab the forked response manager when initiating the INVITE transaction
        pPersistentForkedTxnMngr = pForkedTxnMngr;
    }

    // Set a mandatory headers in the INITIAL state
    //    - CSeq, Max-Forwards headers
    if (!SetMandatoryHeaders(objMethod))
    {
        return IMS_FALSE;
    }

    if (!pDState->InitRequest(objMethod, pstMessage))
    {
        return IMS_FALSE;
    }

    if (!CorrectRouteHeader(pstMessage))
    {
        return IMS_FALSE;
    }

    // P-Access-Network-Info header if required
    if (objMethod.Equals(SipMethod::ACK) && SipFeatures::IsPaniHeaderForAckRequired(GetSlotId()))
    {
        SetPANIHeader(objMethod, pstMessage);
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 SipClientTransactionState::RemoveForkedTransaction()
{
    //---------------------------------------------------------------------------------------------

    if (!pForkedTxnMngr.IsNull())
    {
        pForkedTxnMngr->Remove(this);

        if (!pForkedTxnMngr->IsEmpty())
        {
            pForkedTxnMngr = IMS_NULL;
            return 1;
        }

        // FIX_NO_ACK_RETRANSMISSION
        if (pForkedTxnMngr->IsTransactionCompleted())
        {
            pForkedTxnMngr = IMS_NULL;
            return 1;
        }

        pForkedTxnMngr = IMS_NULL;
    }

    return 0;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipClientTransactionState::SendWithCredentials(IN SipTimerValues* pTV /* = IMS_NULL */)
{
    SipMethod objMethod = SipStack::GetMethod(pstMessage);

    //---------------------------------------------------------------------------------------------

    if (!UpdateTxnDetails(objMethod))
    {
        IMS_TRACE_E(0, "Updating transaction details failed", 0, 0, 0);
        return IMS_FALSE;
    }

    // Updates the sequence number on the dialog state, EARLY / CONFIRMED
    if (!pDialogEx.IsNull())
    {
        if ((pDialogEx->GetState() == SipDState::STATE_EARLY) ||
                (pDialogEx->GetState() == SipDState::STATE_CONFIRMED))
        {
            pDialogEx->GetDialogState()->UpdateLocalCSeq(nCSeqNumber);
        }
        // BYE_REQUEST_ON_DIALOG_TERMINATED
        else if (objMethod.Equals(SipMethod::BYE) &&
                (pDialogEx->GetState() == SipDState::STATE_TERMINATED))
        {
            pDialogEx->GetDialogState()->UpdateLocalCSeq(nCSeqNumber);
        }
    }

    return SipTransactionState::Send(pTV);
}

/*

Remarks

*/
PUBLIC
void SipClientTransactionState::SetExtensionTokenForViaBranch(IN CONST AString& strToken)
{
    SipClientTransport* pClientTransport = DYNAMIC_CAST(SipClientTransport*, pTransport);

    //---------------------------------------------------------------------------------------------

    if (pClientTransport == IMS_NULL)
    {
        return;
    }

    pClientTransport->SetExtensionTokenForViaBranch(strToken);
}

/*

Remarks
 IMPLICIT_ROUTE
*/
PUBLIC
void SipClientTransactionState::SetImplicitRouteHeader(IN CONST AString& strRouteHeader)
{
    //---------------------------------------------------------------------------------------------

    SipStack::FreeAddrSpec(pstImplicitRoute);
    pstImplicitRoute = IMS_NULL;

    if (strRouteHeader.GetLength() > 0)
    {
        pstImplicitRoute = SipStack::DecodeAddrSpec(strRouteHeader);
    }
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipClientTransactionState::UpdateRouteDetails(IN CONST SipMethod& objMethod)
{
    //---------------------------------------------------------------------------------------------

    if (!CorrectRouteHeader(pstMessage))
    {
        return IMS_FALSE;
    }

    SipAddrSpec* pstAddrSpec = SipStack::GetRequestUri(pstMessage);

    if (objMethod.Equals(SipMethod::REGISTER))
    {
        // Remove userinfo & password field if present
        SipStack::RemoveUserAndPassword(pstAddrSpec);
    }

    SipStack::FreeAddrSpec(pstAddrSpec);

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 SipClientTransactionState::HandleResponse(IN ::SipMessage* pstMessage)
{
    // Update the transaction information for incoming response
    IMS_SINT32 nValidity;

    //---------------------------------------------------------------------------------------------

    // Check the message validity - CSeq number
    IMS_UINT32 nCSeqNum = SipStack::GetCSeqNumber(pstMessage);

    if (nCSeqNumber != nCSeqNum)
    {
        // notify the error ???
        IMS_TRACE_E(0, "Sequence number is not ordered - Current (%d), New (%d)", nCSeqNumber,
                nCSeqNum, 0);
        return SipPrivate::MESSAGE_DISCARDED;
    }

    SipMethod objMethod = SipStack::GetMethod(pstMessage);
    SipMessageInfo objMInfo(GetSlotId(), objMethod, pstMessage, SipMessageInfo::DIRECTION_INCOMING);

    nValidity = pDialogEx->GetDialogState()->CheckToTagValidity(objMInfo);

    // FORKED_RESPONSE
    if (nValidity == SipPrivate::MESSAGE_VALID_FORKED)
    {
        // Handle forked response
        IMS_TRACE_I("__UAC__ :: _____ FORKED RESPONSE (%s) RECEIVED _____",
                objMethod.ToString().GetStr(), 0, 0);

        if (!HandleForkedResponse(objMInfo))
        {
            IMS_TRACE_E(0, "Handling a forked response failed", 0, 0, 0);
            return SipPrivate::MESSAGE_DISCARDED;
        }

        return SipPrivate::MESSAGE_VALID;
    }
    else
    {
        if (nValidity != SipPrivate::MESSAGE_VALID)
        {
            return nValidity;
        }
    }

    // If the message is valid, then update the response message
    UpdateMessage(pstMessage);

    // Initialize the transport info. when the response message is received
    pTransport->InitTransportOnMessageReceived(pstMessage);

    // Validate the transport information
    nValidity = CheckMessageValidity();

    if (nValidity != SipPrivate::MESSAGE_VALID)
    {
        IMS_TRACE_E(nValidity, "Checking the message validity failed", 0, 0, 0);
        return nValidity;
    }

    nValidity = pDialogEx->UpdateDialogDetails(objMInfo);

    if (nValidity != SipPrivate::MESSAGE_VALID)
    {
        IMS_TRACE_E(nValidity, "Updating a dialog details failed", 0, 0, 0);
        return nValidity;
    }

    // Check if non-2xx response to INVITE, then the client transaction needs to send ACK request.
    if (objMethod.Equals(SipMethod::INVITE))
    {
        CheckNSendAck();
    }

    // FIX_NO_ACK_RETRANSMISSION
    if (!pForkedTxnMngr.IsNull())
    {
        IMS_SINT32 nStatusCode = SipStack::GetStatusCode(pstMessage);

        if (SipStatusCode::IsFinal(nStatusCode))
        {
            pForkedTxnMngr->SetTransactionCompleted(nStatusCode);
        }
    }

    // Notify the response to SipClientConnection
    if (piListener != IMS_NULL)
    {
        piListener->ClientTransactionState_ResponseReceived(pstMessage);
    }
    else
    {
        IMS_TRACE_E(0, "No listener (%s)",
                SipDebug::GetCharA1(pDialogEx->GetDialogState()->GetCallId().GetStr(), 8, '@'), 0,
                0);
    }

    return SipPrivate::MESSAGE_VALID;
}

/*

Remarks

*/
PUBLIC GLOBAL IMS_SINT32 SipClientTransactionState::MatchTransaction(IN ::SipMessage* pSipMsg,
        IN CONST SipTransportAddress& objFarEnd, OUT RCPtr<SipClientTransactionState>& pCTState)
{
    IMS_TRACE_I("SipClientTransactionState::MatchTransaction ... ", 0, 0, 0);

    SipMethod objMethod = SipStack::GetMethod(pSipMsg);

    if (objMethod.Equals(SipMethod::ACK) && !SipStack::IsRequestMessage(pSipMsg))
    {
        IMS_TRACE_I("__UAC__ :: __ACK RESPONSE RECEIVED__", 0, 0, 0);
        return SipPrivate::MESSAGE_DISCARDED;
    }

    SipTransportParameter objTranspParam;

    /* Fill transport details */
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
    ISipUserData objUserData;
    ::SipTxnKey* pTxnKey = IMS_NULL;
    SipEn_TxnStatus eTxnStatus = ETXNSTATUS_INVALID;
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
    if (bStatus == SIP_FALSE)
    {
        IMS_TRACE_E(nError, "OnRecvMessage() Failed", 0, 0, 0);
        return SipPrivate::MESSAGE_FAILED;
    }

    switch (eTxnStatus)
    {
        case ETXNSTATUS_NEWREQRECVD:
        {
            IMS_TRACE_I("__UAC__ :: _____ NEW REQUEST _____", 0, 0, 0);
            break;
        }
        case ETXNSTATUS_VALIDMESSAGE:
        {
            IMS_TRACE_I("__UAC__ :: _____ VALID MESSAGE _____", 0, 0, 0);
            break;
        }
        case ETXNSTATUS_2XX_STRAYRESP:
        {
            IMS_TRACE_I("__UAC__ :: _____STRAY 2XX RESPONSE _____", 0, 0, 0);
            if (!SipAckPackage::HandleStray2xx(pSipMsg))
            {
                IMS_TRACE_E(0, "__UAC__ :: 2XX RETRANSMISSION HANDLING FAILED.", 0, 0, 0);
            }
            return SipPrivate::MESSAGE_DISCARDED;
        }
        case ETXNSTATUS_IGNOREREQ:
        {
            IMS_TRACE_I("__UAC__ :: _____ IGNORE REQUEST _____", 0, 0, 0);
            return SipPrivate::MESSAGE_DISCARDED;
        }
        case ETXNSTATUS_IGNORERESP:
        {
            IMS_TRACE_I("__UAC__ :: _____ IGNORE RESPONSE _____", 0, 0, 0);
            return SipPrivate::MESSAGE_DISCARDED;
        }
        case ETXNSTATUS_STRAYRESP:
        {
            IMS_TRACE_I("__UAC__ :: _____ STRAY RESPONSE _____", 0, 0, 0);
            return SipPrivate::MESSAGE_DISCARDED;
        }
        case ETXNSTATUS_RETRANSMISSION:
        {
            IMS_TRACE_I("__UAC__ :: _____ REMOTE RETRANSMISSION _____", 0, 0, 0);
            return SipPrivate::MESSAGE_DISCARDED;
        }
        case ETXNSTATUS_ERRORONSEND:
        case ETXNSTATUS_INVALIDMESSAGE:
        case ETXNSTATUS_INVALID:
        {
            IMS_TRACE_I("__UAC__ :: _____ PROCESSING FAILED _____", 0, 0, 0);
            return SipPrivate::MESSAGE_FAILED;
        }
        default:
            IMS_TRACE_I("__UAC__ :: _____ PROCESSING FAILED _____", 0, 0, 0);
            return SipPrivate::MESSAGE_FAILED;
    }

    SipTxnContext* pTxnContext = (SipTxnContext*)objUserData.GetUserData();

    if (pTxnContext == IMS_NULL)
    {
        IMS_TRACE_E(0, "pTxnContext is NULL", 0, 0, 0);
        return SipPrivate::MESSAGE_FAILED;
    }

    SipTxnContextData* pTxnContextData = (SipTxnContextData*)pTxnContext->pTxnContextData;

    if (pTxnContextData == IMS_NULL)
    {
        if (objUserData.GetDeleteFlag() == SIP_TRUE)
        {
            SipStack::DestroyTxnContext(pTxnContext);
        }

        SipStack::FreeTxnKey(pTxnKey);
        // fatal error
        IMS_TRACE_E(0, "Getting the transaction context data failed", 0, 0, 0);
        return SipPrivate::MESSAGE_FAILED;
    }

    pCTState = DYNAMIC_CAST(SipClientTransactionState*, pTxnContextData->GetTxnState());

    if (pCTState.IsNull())
    {
        if (objUserData.GetDeleteFlag() == SIP_TRUE)
        {
            SipStack::DestroyTxnContext(pTxnContext);
        }

        SipStack::FreeTxnKey(pTxnKey);

        // fatal error
        IMS_TRACE_E(
                0, "The transaction context data is missing the transaction state info.", 0, 0, 0);
        return SipPrivate::MESSAGE_FAILED;
    }

    // FORKED_RESPONSE :: Check & look up the appropriate client transaction ...
    // FORKED_RESPONSE_TO_SUPPORT_EARLY_DIALOG_TERMINATION
    RCPtr<SipForkedTransactionManager> pTmpFTM;

    if (!pCTState->pForkedTxnMngr.IsNull())
    {
        pTmpFTM = pCTState->pForkedTxnMngr;
        IMS_TRACE_D("FTM :: Association with this transaction", 0, 0, 0);
    }
    else if (!pCTState->pPersistentForkedTxnMngr.IsNull())
    {
        pTmpFTM = pCTState->pPersistentForkedTxnMngr;
        IMS_TRACE_D("FTM :: No association with this transaction, but choose it", 0, 0, 0);
    }

    if (objMethod.Equals(SipMethod::INVITE) && !pTmpFTM.IsNull())
    {
        SipClientTransactionState* pTmpCTState = pTmpFTM->Lookup(pSipMsg);

        if ((pTmpCTState != IMS_NULL) && (pTmpCTState != pCTState.Get()))
        {
            IMS_TRACE_D("FTM :: The response is received via the other transaction", 0, 0, 0);

            pCTState = pTmpCTState;
        }
    }

    // Update the transaction key if it is different
    if ((pTxnKey != IMS_NULL) && (pTxnKey != pCTState->pstTxnKey))
    {
        SipStack::FreeTxnKey(pCTState->pstTxnKey);

        pCTState->pstTxnKey = pTxnKey;
        SipStack::AddReference(pCTState->pstTxnKey);
    }
    else
    {
        SipStack::FreeTxnKey(pTxnKey);
    }

    // Release transaction context if transaction is terminated
    if (objUserData.GetDeleteFlag() == SIP_TRUE)
    {
        SipStack::DestroyTxnContext(pTxnContext);
    }

    // SIP_MESSAGE_TRACKER
    SipFactoryProxy* pFactoryProxy = SipFactoryProxy::GetInstance();

    if (pFactoryProxy->IsMessageTrackerEnabled(pCTState->GetSlotId()))
    {
        SipMessageTracker* pMessageTracker =
                pFactoryProxy->GetMessageTracker(pCTState->GetSlotId());

        pMessageTracker->NotifyMessageReceived(objMethod, SipStack::GetStatusCode(pSipMsg),
                AString(SipStack::TxnKey_GetCallId(pCTState->pstTxnKey)));
    }

    return SipPrivate::MESSAGE_VALID;
}

/*

Remarks

*/
PRIVATE
IMS_BOOL SipClientTransactionState::CorrectRouteHeader(IN_OUT ::SipMessage*& pstMessage)
{
    // Get the topmost Route entry
    SipAddrSpec* pstAddrSpec = SipStack::GetAddrSpec(pstMessage, ISipHeader::ROUTE);

    //---------------------------------------------------------------------------------------------

    if (pstAddrSpec == IMS_NULL)
    {
        if (SipStack::IsLastErrorNoExist())
        {
            nRoutingType = TARGET_NO_ROUTE;
            return IMS_TRUE;
        }

        return IMS_FALSE;
    }

    // The top Route entry is a loose router : no changes
    //    --> Request-URI is already set by the application or from To header
    const AString LR(Sip::STR_LR);

    if (SipStack::HasParameter(pstAddrSpec, LR))
    {
        AString strLRValue = SipStack::GetParameter(pstAddrSpec, LR);

        if ((strLRValue.GetLength() == 0) || strLRValue.EqualsIgnoreCase("true"))
        {
            nRoutingType = TARGET_LR;

            SipStack::FreeAddrSpec(pstAddrSpec);
            return IMS_TRUE;
        }
    }

    nRoutingType = TARGET_SR;

    // Here, the top Route entry is a strict router:
    //    --> This addr-spec SHOULD be set in the Request-Line
    SipAddrSpec* pstReqLineAddrSpec = SipStack::GetRequestUri(pstMessage);

    SipHeaderBase* pstRouteHeader = SipStack::CreateHeader(ISipHeader::ROUTE, pstReqLineAddrSpec);
    SipStack::FreeAddrSpec(pstReqLineAddrSpec);

    if (!SipStack::AppendHeader(pstRouteHeader, pstMessage))
    {
        SipStack::FreeHeaderEx(pstRouteHeader);
        SipStack::FreeAddrSpec(pstAddrSpec);
        return IMS_FALSE;
    }

    SipStack::FreeHeaderEx(pstRouteHeader);

    // Set the Request-URI from the addr-spec of the top Route entry
    if (!SipStack::SetRequestUri(pstAddrSpec, pstMessage))
    {
        SipStack::FreeAddrSpec(pstAddrSpec);
        return IMS_FALSE;
    }

    SipStack::FreeAddrSpec(pstAddrSpec);

    return IMS_TRUE;
}

/*

Remarks

*/
PRIVATE
void SipClientTransactionState::CheckNSendAck()
{
    //---------------------------------------------------------------------------------------------

    if (!SipStack::IsAckTransmissionRequiredForNon2XX())
    {
        // SIP stack takes care of the failure response for INVITE and sends ACK
        return;
    }

    // Check if non-2xx response to INVITE, then the client transaction needs to send ACK request.
    IMS_SINT32 nStatusCode = SipStack::GetStatusCode(pstMessage);

    if (nStatusCode >= SipStatusCode::SC_300)
    {
        ::SipMessage* pstAckMessage = SipStack::CreateMessage(SipStack::SIP_MESSAGE_REQUEST);

        if (pstAckMessage == IMS_NULL)
        {
            IMS_TRACE_E(0, "Creating an ACK request message failed", 0, 0, 0);
            return;
        }

        // Initializes ACK request message
        if (!InitAck(pstAckMessage, pstMessage))
        {
            IMS_TRACE_E(0, "Initializing ACK request failed", 0, 0, 0);
            SipStack::FreeMessage(pstAckMessage);
            return;
        }

        SipMethod objMethod(SipMethod::ACK);
        SipMessageInfo objMInfo(
                GetSlotId(), objMethod, pstAckMessage, SipMessageInfo::DIRECTION_OUTGOING);

        // Update the Contact information
        pDialogEx->UpdateDialogDetails(objMInfo);

        // Store the last request message
        // SipStack::FreeMessage(pstLastMessage);
        // pstLastMessage = SipStack::CloneMessage(pstAckMessage);

        IMS_TRACE_I("___ SENDING ACK REQUEST .....", 0, 0, 0);
        SipTransactionState::Send(pstAckMessage, IMS_NULL);

        SipStack::FreeMessage(pstAckMessage);
    }
}

/*

Remarks

*/
PRIVATE VIRTUAL SipTransactionState* SipClientTransactionState::Clone()
{
    SipClientTransactionState* pTxnState = new SipClientTransactionState(GetSlotId());

    //---------------------------------------------------------------------------------------------

    if (pTxnState == IMS_NULL)
    {
        return IMS_NULL;
    }

    if (!pTxnState->InitTxnDetails(this))
    {
        delete pTxnState;
        return IMS_NULL;
    }

    return pTxnState;
}

/*

Remarks

*/
PRIVATE
IMS_BOOL SipClientTransactionState::HandleForkedResponse(IN CONST SipMessageInfo& objMInfo)
{
    //---------------------------------------------------------------------------------------------

    if (piListener == IMS_NULL)
    {
        IMS_TRACE_E(0, "No listener (%s)",
                SipDebug::GetCharA1(pDialogEx->GetDialogState()->GetCallId().GetStr(), 8, '@'), 0,
                0);
        return IMS_FALSE;
    }

    RCPtr<SipClientTransactionState> pForkedCTState = new SipClientTransactionState(GetSlotId());

    if (pForkedCTState.IsNull())
    {
        return IMS_FALSE;
    }

    // Add the forked transaction to the forked transaction manager...
    pForkedCTState->pForkedTxnMngr = pForkedTxnMngr;

    if (!pForkedCTState->pForkedTxnMngr.IsNull())
    {
        pForkedCTState->pForkedTxnMngr->Add(pForkedCTState.Get());
    }

    if (!pForkedCTState->InitTxnDetails(this))
    {
        IMS_TRACE_E(0, "Initializing SIP client transaction state failed", 0, 0, 0);
        return IMS_FALSE;
    }

    RCPtr<SipDialogState> pDState = new SipDialogState();

    if (pDState.IsNull())
    {
        return IMS_FALSE;
    }

    // Initialize the dialog details from the previous request message
    if (!pDState->InitDialogDetails(GetLastMessage()))
    {
        IMS_TRACE_E(0, "Initializing a dialog details from the previous request message", 0, 0, 0);
        return IMS_FALSE;
    }

    // Initialize the dialog details for the forked message
    if (!pDState->InitDialogDetails(
                SipDialogState::DIALOG_FORKED_RESPONSE, pDialogEx->GetDialogState()))
    {
        IMS_TRACE_E(0, "Initializing a dialog details from the forked response message", 0, 0, 0);
        return IMS_FALSE;
    }

    pForkedCTState->pDialogEx = SipDialogEx::CreateDialog(pDState.Get(), objMInfo);

    if (pForkedCTState->pDialogEx.IsNull())
    {
        return IMS_FALSE;
    }

    SipMessageInfo objPrevMInfo(GetSlotId(), objMInfo.GetMethod(), GetLastMessage(),
            SipMessageInfo::DIRECTION_OUTGOING);

    if (pForkedCTState->pDialogEx->UpdateDialogDetails(objPrevMInfo) != SipPrivate::MESSAGE_VALID)
    {
        IMS_TRACE_E(0, "Updating a dialog details from the previous request message", 0, 0, 0);
        return IMS_FALSE;
    }

    if (pForkedCTState->pDialogEx->UpdateDialogDetails(objMInfo) != SipPrivate::MESSAGE_VALID)
    {
        IMS_TRACE_E(0, "Updating a dialog details from the forked response message", 0, 0, 0);
        return IMS_FALSE;
    }

    pForkedCTState->UpdateMessage(objMInfo.GetMessage());

    // Initialize the transport info. when the response message is received
    pForkedCTState->pTransport->InitTransportOnMessageReceived(objMInfo.GetMessage());

    // Validate the transport information
    IMS_SINT32 nValidity = pForkedCTState->CheckMessageValidity();

    if (nValidity != SipPrivate::MESSAGE_VALID)
    {
        IMS_TRACE_E(nValidity, "Checking the message validity failed", 0, 0, 0);
        return IMS_FALSE;
    }

    // Check if non-2xx response to INVITE, then the client transaction needs to send ACK request.
    if (objMInfo.GetMethod().Equals(SipMethod::INVITE))
    {
        pForkedCTState->CheckNSendAck();
    }

    // FIX_NO_ACK_RETRANSMISSION
    if (!pForkedCTState->pForkedTxnMngr.IsNull())
    {
        IMS_SINT32 nStatusCode = SipStack::GetStatusCode(objMInfo.GetMessage());

        if (SipStatusCode::IsFinal(nStatusCode))
        {
            pForkedCTState->pForkedTxnMngr->SetTransactionCompleted(nStatusCode);
        }
    }

    piListener->ClientTransactionState_ForkedResponseReceived(pForkedCTState.Get());

    return IMS_TRUE;
}

/*

Remarks

*/
PRIVATE
IMS_BOOL SipClientTransactionState::InitAck(
        IN_OUT ::SipMessage*& pstAckMessage, IN ::SipMessage* pstRespMessage)
{
    SipHeaderBase* pstGetHdr;
    SipHeaderBase* pstHeader;
    SipMethod objMethodACK(SipMethod::ACK);

    //---------------------------------------------------------------------------------------------

    // Sets SIP method name
    SipStack::SetMethod(objMethodACK, pstAckMessage);

    // Sets a Request-URI field
    SipAddrSpec* pstAddrSpec = SipStack::GetRequestUri(pstLastMessage);
    (void)SipStack::SetRequestUri(pstAddrSpec, pstAckMessage);
    SipStack::FreeAddrSpec(pstAddrSpec);

    AString strHBody;

    // Sets CSeq header field
    strHBody.SetNumber(nCSeqNumber);
    strHBody += TextParser::CHAR_SP;
    strHBody += SipMethod::NAME[SipMethod::ACK];
    pstHeader = SipStack::DecodeHeader(ISipHeader::CSEQ, strHBody);

    if (!SipStack::SetHeader(pstHeader, pstAckMessage))
    {
        SipStack::FreeHeaderEx(pstHeader);
        return IMS_FALSE;
    }

    SipStack::FreeHeaderEx(pstHeader);

    // Sets Max-Forwards header field
    strHBody.SetNumber(SipPrivate::MAX_HOP);
    pstHeader = SipStack::DecodeHeader(ISipHeader::MAX_FORWARDS, strHBody);

    if (!SipStack::SetHeader(pstHeader, pstAckMessage))
    {
        SipStack::FreeHeaderEx(pstHeader);
        return IMS_FALSE;
    }

    SipStack::FreeHeaderEx(pstHeader);

    // Sets From header field
    pstGetHdr = SipStack::GetHeader(pstLastMessage, ISipHeader::FROM);

    pstHeader = SipStack::CloneHeader(pstGetHdr);
    SipStack::FreeHeaderEx(pstGetHdr);

    if (pstHeader != IMS_NULL)
    {
        if (!SipStack::SetHeader(pstHeader, pstAckMessage))
        {
            SipStack::FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        SipStack::FreeHeaderEx(pstHeader);
    }

    // Sets To header field
    pstGetHdr = SipStack::GetHeader(pstLastMessage, ISipHeader::TO);

    pstHeader = SipStack::CloneHeader(pstGetHdr);
    SipStack::FreeHeaderEx(pstGetHdr);

    if (pstHeader != IMS_NULL)
    {
        if (!SipStack::SetHeader(pstHeader, pstAckMessage))
        {
            SipStack::FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        // Update To-Tag from the last response message
        pstGetHdr = SipStack::GetHeader(pstRespMessage, ISipHeader::TO);

        AString strToTag = SipStack::GetParameter(pstGetHdr, Sip::STR_TAG);

        if (!strToTag.IsNULL())
        {
            (void)SipStack::SetParameter(pstHeader, Sip::STR_TAG, strToTag);
        }

        SipStack::FreeHeaderEx(pstGetHdr);

        SipStack::FreeHeaderEx(pstHeader);
    }

    // Sets Call-ID header field
    pstGetHdr = SipStack::GetHeader(pstLastMessage, ISipHeader::CALL_ID);

    pstHeader = SipStack::CloneHeader(pstGetHdr);
    SipStack::FreeHeaderEx(pstGetHdr);

    if (pstHeader != IMS_NULL)
    {
        if (!SipStack::SetHeader(pstHeader, pstAckMessage))
        {
            SipStack::FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        SipStack::FreeHeaderEx(pstHeader);
    }

    // Sets Via header field
    pstGetHdr = SipStack::GetHeader(pstLastMessage, ISipHeader::VIA);

    pstHeader = SipStack::CloneHeader(pstGetHdr);
    SipStack::FreeHeaderEx(pstGetHdr);

    if (pstHeader != IMS_NULL)
    {
        if (!SipStack::SetHeader(pstHeader, pstAckMessage))
        {
            SipStack::FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        SipStack::FreeHeaderEx(pstHeader);
    }

    // Sets Route header field
    IMS_SINT32 nHCount = SipStack::GetHeaderCount(pstLastMessage, ISipHeader::ROUTE);

    for (IMS_SINT32 i = 0; i < nHCount; i++)
    {
        pstGetHdr = SipStack::GetHeader(pstLastMessage, ISipHeader::ROUTE, i);

        if (SipStack::IsValidHeader(pstGetHdr))
        {
            if (!SipStack::AppendHeader(pstGetHdr, pstAckMessage))
            {
                SipStack::FreeHeaderEx(pstGetHdr);
                return IMS_FALSE;
            }
        }

        SipStack::FreeHeaderEx(pstGetHdr);
    }

    // Sets User-Agent header if it is present in the previous request
    pstGetHdr = SipStack::GetUnknownHeader(pstLastMessage, SipHeaderName::USER_AGENT);

    if (SipStack::IsValidHeader(pstGetHdr))
    {
        pstHeader = SipStack::CloneHeader(pstGetHdr);

        if (pstHeader != IMS_NULL)
        {
            // Even if setting User-Agent is failed, it will proceed to send ACK request.
            SipStack::SetHeader(pstHeader, pstAckMessage);
            SipStack::FreeHeaderEx(pstHeader);
        }
    }

    SipStack::FreeHeaderEx(pstGetHdr);

    // P-Access-Network-Info header if required
    if (SipFeatures::IsPaniHeaderForAckRequired(GetSlotId()))
    {
        SetPANIHeader(objMethodACK, pstAckMessage);
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PRIVATE
IMS_BOOL SipClientTransactionState::SetDialogRelatedHeaders(IN CONST SipMethod& objMethod)
{
    //---------------------------------------------------------------------------------------------

    if (pDialogEx->GetState() == SipDState::STATE_INIT)
    {
        // Check if the Call-ID field is present or not
        if (!SipStack::IsHeaderPresent(pstMessage, ISipHeader::CALL_ID))
        {
            // Add Call-ID in here
            const IPAddress& objAddress = pTransport->GetIpAddress();
            AString strCallId = SipUtils::GenerateCallId(objAddress.ToString());
            SipHeaderBase* pstHeader = SipStack::DecodeHeader(ISipHeader::CALL_ID, strCallId);

            if (pstHeader == IMS_NULL)
                return IMS_FALSE;

            if (!SipStack::SetHeader(pstHeader, pstMessage))
            {
                SipStack::FreeHeaderEx(pstHeader);
                return IMS_FALSE;
            }

            SipStack::FreeHeaderEx(pstHeader);

            // HEADER_REQ_SESSION-ID
            if (SipFeatures::IsHeaderSessionIdRequired(GetSlotId()))
            {
                AString strSessionId = SipUtils::GenerateSessionId(GetSlotId(), strCallId);

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
        }

        // Set From-Tag
        if (!objMethod.Equals(SipMethod::CANCEL) && !objMethod.Equals(SipMethod::ACK))
        {
            SipHeaderBase* pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::FROM);

            if (!SipStack::HasParameter(pstHeader, Sip::STR_TAG))
            {
                AString strTagVal = SipUtils::GenerateTag(
                        SipConfigProxy::GetTagPrefix(GetSlotId(), GetSipProfile()));

                if (!SipStack::SetParameter(pstHeader, Sip::STR_TAG, strTagVal))
                {
                    SipStack::FreeHeaderEx(pstHeader);
                    return IMS_FALSE;
                }
            }

            SipStack::FreeHeaderEx(pstHeader);
        }
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PRIVATE
IMS_BOOL SipClientTransactionState::SetMandatoryHeaders(IN CONST SipMethod& objMethod)
{
    SipHeaderBase* pstHeader;

    //---------------------------------------------------------------------------------------------

    if (objMethod.Equals(SipMethod::INVITE) || objMethod.Equals(SipMethod::ACK))
        nClass = CLASS_INVITE;
    else if (objMethod.Equals(SipMethod::PRACK))
        nClass = CLASS_OVERLAP;
    else
        nClass = CLASS_REGULAR;

    AString strHBody;

    // Sets CSeq header field
    strHBody.SetNumber(nCSeqNumber);
    strHBody.Append(TextParser::CHAR_SP);
    strHBody.Append(objMethod.ToString());

    pstHeader = SipStack::DecodeHeader(ISipHeader::CSEQ, strHBody);

    if (!SipStack::SetHeader(pstHeader, pstMessage))
    {
        SipStack::FreeHeaderEx(pstHeader);
        return IMS_FALSE;
    }

    SipStack::FreeHeaderEx(pstHeader);

    // Sets Max-Forwards header field
    strHBody.SetNumber(SipPrivate::MAX_HOP);
    pstHeader = SipStack::DecodeHeader(ISipHeader::MAX_FORWARDS, strHBody);

    if (!SipStack::SetHeader(pstHeader, pstMessage))
    {
        SipStack::FreeHeaderEx(pstHeader);
        return IMS_FALSE;
    }

    SipStack::FreeHeaderEx(pstHeader);

    return IMS_TRUE;
}

/*

Remarks

*/
PRIVATE
void SipClientTransactionState::SetPANIHeader(
        IN CONST SipMethod& objMethod, IN_OUT ::SipMessage*& pstMessage)
{
    AString strPANI;

    if (PAccessNetworkInfoHeader::FormHeaderForOperatorSpecific(
                GetSlotId(), pTransport->GetIpAddress(), objMethod, GetSipProfile(), strPANI))
    {
        if (strPANI.GetLength() > 0)
        {
            SipHeaderBase* pstHeader = SipStack::DecodeHeader(
                    ISipHeader::P_ACCESS_NETWORK_INFO, AString::ConstNull(), strPANI);

            if (pstHeader != IMS_NULL)
            {
                SipStack::SetHeader(pstHeader, pstMessage);
                SipStack::FreeHeaderEx(pstHeader);
            }
        }
    }
}

/*

Remarks

*/
PRIVATE
IMS_BOOL SipClientTransactionState::UpdateTxnDetails(IN CONST SipMethod& objMethod)
{
    // Update the transaction information for outgoing request
    SipMessageInfo objMInfo(GetSlotId(), objMethod, pstMessage, SipMessageInfo::DIRECTION_OUTGOING);

    //---------------------------------------------------------------------------------------------

    if (pDialogEx->GetState() == SipDState::STATE_INIT)
    {
        if (!pDialogEx->GetDialogState()->InitDialogDetails(pstMessage))
        {
            return IMS_FALSE;
        }

        // Update the sequence number in the CSeq header if changed.
        IMS_UINT32 nSeqNum = SipStack::GetCSeqNumber(pstMessage);

        if (nSeqNum != SipPrivate::INVALID_SEQ_NUM)
            nCSeqNumber = nSeqNum;
    }

    // If the request can create a dialog usage, then update the state if changed
    if (SipDialogBase::IsDialogCreatable(objMethod))
    {
        SipDialogState* pDState = pDialogEx->GetDialogState();
        SipDialogEx* pTmpDialogEx = pDState->GetDialogUsage(objMInfo);

        // Not present; So, use a new dialog
        if (pTmpDialogEx == IMS_NULL)
        {
            pDialogEx->InitDialogWithDelay(objMInfo);
        }
        // Use an existing dialog
        else
        {
            IMS_TRACE_D("_____ UPDATE DIALOG  (%s:%s) _____", objMethod.ToString().GetStr(),
                    SipDebug::GetCharA1(pDState->GetCallId().GetStr(), 8, '@'), 0);

            pDialogEx = pTmpDialogEx;
        }
    }

    // Update the Contact information
    pDialogEx->UpdateDialogDetails(objMInfo);

    // Store the last request message
    if (!objMethod.Equals(SipMethod::ACK))
    {
        SipStack::FreeMessage(pstLastMessage);
        pstLastMessage = SipStack::CloneMessage(pstMessage);
    }

    return IMS_TRUE;
}
