/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090302  toastops@                 Created
    </table>

    Description
    This class contains constants representing SIP response codes as defined in RFC 3261
    and extensions.
*/

#include "ServiceMemory.h"
#include "SipPrivate.h"
#include "SipFeatures.h"
#include "SipDebug.h"
// HEADER_REQ_SESSION-ID
#include "SipHeaderName.h"
#include "SipMessage.h"
#include "SipDialogSharedState.h"
#include "SipDialogState.h"

#define __IMS_SIP_DIALOG_COMPONENT_BY_REFERENCE__

__IMS_TRACE_TAG_SIP__;

PUBLIC
SipDialogState::PendingRemoteTarget::PendingRemoteTarget() :
        strKey(AString::ConstNull()),
        pstHeader(IMS_NULL)
{
}

PUBLIC
SipDialogState::PendingRemoteTarget::PendingRemoteTarget(
        IN CONST AString& strKey_, IN SipHeaderBase* pstHeader_) :
        strKey(strKey_),
        pstHeader(pstHeader_)
{
}

PUBLIC
SipDialogState::PendingRemoteTarget::~PendingRemoteTarget()
{
    if (pstHeader != IMS_NULL)
    {
        SipStack::FreeHeaderEx(pstHeader);
    }
}

PUBLIC
SipDialogState::SipDialogState(IN IMS_BOOL bIsCaller_ /* = IMS_TRUE */) :
        RCObject(),
        bIsCaller(bIsCaller_),
        pstLocalURI(IMS_NULL),
        pstRemoteURI(IMS_NULL),
        bSecure(IMS_FALSE),
        pstLocalTargetURI(IMS_NULL),
        pstRemoteTargetURI(IMS_NULL),
        objPendingRemoteTargets(IMSList<PendingRemoteTarget*>()),
        nLocalCSeq(0),
        nRemoteCSeq(0),
        nRemoteCSeqForINVITE(0),
        nState(SipDState::STATE_INIT),
        bPreloadedSet(IMS_FALSE),
        pSharedState(IMS_NULL),
        pLocalContactHeader(IMS_NULL),
        strSessionId(AString::ConstNull())  // HEADER_REQ_SESSION-ID
        ,
        nFromChangeOption(FROM_CHANGE_NONE)
{
    pstLocalURI = SipStack::CreateHeader(SipStack::GetAnyHeaderType());
    pstRemoteURI = SipStack::CreateHeader(SipStack::GetAnyHeaderType());

    // Local & Remote Contact information
    pstLocalTargetURI = SipStack::CreateHeader(SipStack::GetAnyHeaderType());
    pstRemoteTargetURI = SipStack::CreateHeader(SipStack::GetAnyHeaderType());
}

PUBLIC
SipDialogState::SipDialogState(IN CONST SipDialogState& objRHS) :
        RCObject(objRHS)
{
    // NOTE: If reference count is not used, you MUST implement this copy constructor
}

PUBLIC VIRTUAL SipDialogState::~SipDialogState()
{
    //---------------------------------------------------------------------------------------------

    if (pstLocalURI != IMS_NULL)
        SipStack::FreeHeaderEx(pstLocalURI);

    if (pstRemoteURI != IMS_NULL)
        SipStack::FreeHeaderEx(pstRemoteURI);

    if (pstLocalTargetURI != IMS_NULL)
        SipStack::FreeHeaderEx(pstLocalTargetURI);

    if (pstRemoteTargetURI != IMS_NULL)
        SipStack::FreeHeaderEx(pstRemoteTargetURI);

    ClearRouteSet();

    if (pSharedState != IMS_NULL)
    {
        delete pSharedState;
    }

    if (pLocalContactHeader != IMS_NULL)
    {
        delete pLocalContactHeader;
        pLocalContactHeader = IMS_NULL;
    }

    // REMOTE_TARGET_UPDATE_FROM_MID_DIALOG_REQUEST
    RemoveAllPendingRemoteTargets();

    IMS_TRACE_D("Destructor :: SipDialogState (%s : %s)", bIsCaller ? "__CALLER__" : "__CALLEE__",
            SipDebug::GetCharA1(strCallId.GetStr(), 8, '@'), 0);
}

/*
 Checks if the message received has a valid To-Tag value.
If the dialog state has To-Tag and incoming message has no To-Tag,
then the validation will be failed.
If the invalid message is a request, then it rejects the request with 481 response.

Remarks

*/
PUBLIC
IMS_SINT32 SipDialogState::CheckToTagValidity(IN CONST SipMessageInfo& objMInfo)
{
    ::SipMessage* pstMessage = objMInfo.GetMessage();

    const SipMethod& objMethod = objMInfo.GetMethod();

    //---------------------------------------------------------------------------------------------

    if (objMethod.Equals(SipMethod::INVALID))
        return SipPrivate::MESSAGE_VALID;

    // For CANCEL & non-2xx responses to CANCEL, relax the To-Tag validation.
    // As these are hop-by-hop responses, they can have different To-Tags.
    if (objMethod.Equals(SipMethod::CANCEL))
        return SipPrivate::MESSAGE_VALID;

    if (!SipStack::IsRequestMessage(pstMessage))
    {
        if (objMethod.Equals(SipMethod::INVITE) &&
                (SipStack::GetStatusCode(pstMessage) >= SipStatusCode::SC_300))
        {
            return SipPrivate::MESSAGE_VALID;
        }
    }

    IMS_BOOL bIsInvalid = IMS_FALSE;
    IMS_BOOL bLocalTagMismatched = IMS_FALSE;
    IMS_BOOL bRemoteTagMismatched = IMS_FALSE;

    AString strLocalTag = SipStack::GetParameter(pstLocalURI, Sip::STR_TAG);
    AString strRemoteTag = SipStack::GetParameter(pstRemoteURI, Sip::STR_TAG);
    AString strNewLocalTag;
    AString strNewRemoteTag;

    SipHeaderBase* pstHeader;

    if (SipStack::IsRequestMessage(pstMessage))
    {
        // Get local tag
        pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::TO);

        strNewLocalTag = SipStack::GetParameter(pstHeader, Sip::STR_TAG);
        SipStack::FreeHeaderEx(pstHeader);

        // Get remote tag
        pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::FROM);

        strNewRemoteTag = SipStack::GetParameter(pstHeader, Sip::STR_TAG);
        SipStack::FreeHeaderEx(pstHeader);
    }
    else
    {
        // Get local tag
        pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::FROM);

        strNewLocalTag = SipStack::GetParameter(pstHeader, Sip::STR_TAG);
        SipStack::FreeHeaderEx(pstHeader);

        // Get remote tag
        pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::TO);

        strNewRemoteTag = SipStack::GetParameter(pstHeader, Sip::STR_TAG);
        SipStack::FreeHeaderEx(pstHeader);
    }

    // Check the local tag validation
    if ((strLocalTag.GetLength() != 0) && (strNewLocalTag.GetLength() == 0))
    {
        bIsInvalid = IMS_TRUE;
        bLocalTagMismatched = IMS_TRUE;
    }
    else if ((strLocalTag.GetLength() != 0) && (strNewLocalTag.GetLength() != 0))
    {
        if (!strLocalTag.Equals(strNewLocalTag))
        {
            bIsInvalid = IMS_TRUE;
            bLocalTagMismatched = IMS_TRUE;
        }
    }

    // Check the remote tag validation
    if ((strRemoteTag.GetLength() != 0) && (strNewRemoteTag.GetLength() == 0))
    {
        bIsInvalid = IMS_TRUE;
        bRemoteTagMismatched = IMS_TRUE;
    }
    else if ((strRemoteTag.GetLength() != 0) && (strNewRemoteTag.GetLength() != 0))
    {
        if (!strRemoteTag.Equals(strNewRemoteTag))
        {
            bIsInvalid = IMS_TRUE;
            bRemoteTagMismatched = IMS_TRUE;
        }
    }

    if (bIsInvalid == IMS_TRUE)
    {
        // Error : Tag Mismatch Error
        if (SipStack::IsRequestMessage(pstMessage) && !objMethod.Equals(SipMethod::INVITE))
        {
            // If the local tag is not matching,
            // then set the current local tag to this failed response.
            if (bLocalTagMismatched == IMS_TRUE)
            {
                SipPrivate::SetLastError(SipError::LOCAL_TAG_MISMATCH);
            }

            return SipPrivate::MESSAGE_INVALID_481;
        }
        else if (!SipStack::IsRequestMessage(pstMessage) && objMethod.Equals(SipMethod::INVITE) &&
                (bRemoteTagMismatched == IMS_TRUE))
        {
            // Handle the forked response to INVITE request.
            return SipPrivate::MESSAGE_VALID_FORKED;
        }

        return SipPrivate::MESSAGE_INVALID;
    }

    return SipPrivate::MESSAGE_VALID;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 SipDialogState::CompareTo(IN SipDialogState* pDState, IN ::SipMessage* pstMessage,
        IN IMS_BOOL bCheckForked /* = IMS_FALSE */)
{
    //---------------------------------------------------------------------------------------------

    // Check if the Call-ID's match
    if (!strCallId.Equals(pDState->strCallId))
    {
        // Call-Id's does not match
        return NOT_MATCHED;
    }

    IMS_SINT32 nComparisonResult = NOT_MATCHED;
    IMS_BOOL bToTagLenient = IMS_FALSE;
    IMS_SINT32 nForkedMessage = FORKED_ANY;
    SipMethod objMethod = SipStack::GetMethod(pstMessage);

    if (pstMessage != IMS_NULL)
    {
        IMS_SINT32 nStatusCode = SipStatusCode::SC_INVALID;

        if (!SipStack::IsRequestMessage(pstMessage))
        {
            nStatusCode = SipStack::GetStatusCode(pstMessage);
        }

        // For response to CANCEL and non-2xx response to INVITE,
        // the comparison should be lenient against the To-Tags.

        // Fix for CSR 1-1316815
        // The To-Tag leniency should be present for CANCEL request,
        // response for CANCEL, and failure responses for INVITE.
        if (objMethod.Equals(SipMethod::CANCEL))
            bToTagLenient = IMS_TRUE;

        else if (!SipStack::IsRequestMessage(pstMessage) && (objMethod.Equals(SipMethod::INVITE)) &&
                (nStatusCode >= SipStatusCode::SC_300))
        {
            bToTagLenient = IMS_TRUE;
        }
        else if ((bCheckForked) && !SipStack::IsRequestMessage(pstMessage) &&
                (objMethod.Equals(SipMethod::INVITE)) && (nStatusCode >= SipStatusCode::SC_100) &&
                (nStatusCode < SipStatusCode::SC_300))
        {
            bToTagLenient = IMS_TRUE;
            nForkedMessage = FORKED_INVITE;
        }
        // If the forking flag in enabled (i.e. Sdf_en_forkedSubscribe)
        // and it's a 1xx/2xx response to SUBSCRIBE, then set the bToTagLenient flag.
        // Also, reset the matching result since it is used only for detecting forked NOTIFYs.
        else if ((bCheckForked) && !SipStack::IsRequestMessage(pstMessage) &&
                (objMethod.Equals(SipMethod::SUBSCRIBE)) &&
                (nStatusCode >= SipStatusCode::SC_100) && (nStatusCode < SipStatusCode::SC_300))
        {
            bToTagLenient = IMS_TRUE;
        }
        else if ((bCheckForked) && SipStack::IsRequestMessage(pstMessage) &&
                (objMethod.Equals(SipMethod::NOTIFY)))
        {
            nForkedMessage = FORKED_SUBSCRIBE;
        }
// FIXME: Strict comparison - UPDATE request
#if 0
        else if (SipStack::IsRequestMessage(pstMessage)
                && objMethod.Equals(SipMethod::UPDATE))
        {
            IMS_SINT32 nDState = GetState();

            if ((nDState != SipDState::STATE_EARLY)
                    && (nDState != SipDState::STATE_CONFIRMED))
            {
                // If the dialog state is not proper, we don't need to compare the remains.
                return NOT_MATCHED;
            }
        }
#endif
    }

    // Compares a local & remote headers on basis of
    // { (From : From) (To : To) } OR { (From : To) (To : From) }

    // Compares the first case (The local-address of each dialog-state).
    nComparisonResult =
            CompareHeaders(pDState->pstLocalURI, pstLocalURI, bToTagLenient, nForkedMessage);

    if (nComparisonResult == MATCHED)
    {
        // First pair of headers matched.
        // Now, go in for matching the second set of headers viz. the remote-address
        // of each dialog-state and return back whatever this comparison returns.
        nComparisonResult =
                CompareHeaders(pDState->pstRemoteURI, pstRemoteURI, bToTagLenient, nForkedMessage);
    }

    // If the SIP message is not NULL, use the type of the SIP message (Request/Response)
    // and the caller member variable in the existing dialog-state, to decide whether to do
    // (From:From, To:To) OR (From:To, To:From) [exact match]
    if ((pstMessage != IMS_NULL) && (nComparisonResult != NOT_MATCHED))
    {
        // If the comparison returned MATCHED_OVERLAP_DIALING, we check whether this is an INVITE.
        // If it's not an INVITE, we return NOT_MATCHED.
        // Similarly, if extra digits were added in a response, we return NOT_MATCHED.
        if (nComparisonResult == MATCHED_OVERLAP_DIALING)
        {
            if (!SipStack::IsRequestMessage(pstMessage) ||
                    (SipStack::IsRequestMessage(pstMessage) &&
                            !objMethod.Equals(SipMethod::INVITE)))
            {
                nComparisonResult = NOT_MATCHED;
            }
        }

        if (nComparisonResult != NOT_MATCHED)
        {
            // If the request is CANCEL, then Equalss the Via branch parameter
            // also to have the exact match of dialog-state.
            if (SipStack::IsRequestMessage(pstMessage) && objMethod.Equals(SipMethod::CANCEL))
            {
                if (!pDState->strRemoteViaBranch.IsNULL())
                {
                    if (!strRemoteViaBranch.IsNULL())
                    {
                        if (!strRemoteViaBranch.Equals(pDState->strRemoteViaBranch))
                            nComparisonResult = NOT_MATCHED;
                    }
                    else
                        nComparisonResult = NOT_MATCHED;
                }
                else
                {
                    if (!strRemoteViaBranch.IsNULL())
                        nComparisonResult = NOT_MATCHED;
                }
            }
        }
    }

    return nComparisonResult;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipDialogState::Equals(IN SipDialogState* pDState)
{
    //---------------------------------------------------------------------------------------------

    if (pDState == IMS_NULL)
        return IMS_FALSE;

    if (!strCallId.Equals(pDState->strCallId))
        return IMS_FALSE;

    AString strTagVal = SipStack::GetParameter(pstLocalURI, Sip::STR_TAG);
    AString strOtherTagVal = SipStack::GetParameter(pDState->pstLocalURI, Sip::STR_TAG);

    if (!strTagVal.Equals(strOtherTagVal))
        return IMS_FALSE;

    strTagVal = SipStack::GetParameter(pstRemoteURI, Sip::STR_TAG);
    strOtherTagVal = SipStack::GetParameter(pDState->pstRemoteURI, Sip::STR_TAG);

    if (!strTagVal.Equals(strOtherTagVal))
        return IMS_FALSE;

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
const ISipHeader* SipDialogState::GetContactHeader() const
{
    //---------------------------------------------------------------------------------------------

    return pLocalContactHeader;
}

/*

Remarks

*/
PUBLIC
AString SipDialogState::GetLocalTag() const
{
    //---------------------------------------------------------------------------------------------

    return SipStack::GetParameter(pstLocalURI, Sip::STR_TAG);
}

/*

Remarks

*/
PUBLIC
AString SipDialogState::GetRemoteTag() const
{
    //---------------------------------------------------------------------------------------------

    return SipStack::GetParameter(pstRemoteURI, Sip::STR_TAG);
}

/*

Remarks

*/
PUBLIC
SipHeaderBase* SipDialogState::GetLocalTargetUri() const
{
    //---------------------------------------------------------------------------------------------

    return pstLocalTargetURI;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipDialogState::InitDialogDetails(IN ::SipMessage* pstMessage)
{
    SipHeaderBase* pstHeader;
    SipHeaderBase* pstTmpHeader;

    //---------------------------------------------------------------------------------------------

    // Call Id
    pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::CALL_ID);
    SipStack::EncodeHeaderBody(pstHeader, IMS_FALSE, strCallId);
    SipStack::FreeHeaderEx(pstHeader);

    // The dialog state is created by the outgoing dialog-creatable request
    if (bIsCaller)
    {
        // Local URI & Tag
        SipStack::FreeHeader(pstLocalURI);

        pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::FROM);

#ifdef __IMS_SIP_DIALOG_COMPONENT_BY_REFERENCE__
        pstTmpHeader = pstHeader;
        pstHeader = IMS_NULL;
#else
        pstTmpHeader = SipStack::CloneHeader(pstHeader);
#endif

        if (pstTmpHeader != IMS_NULL)
        {
            pstLocalURI = SipStack::CopyHeader(pstLocalURI, pstTmpHeader);
            SipStack::FreeHeaderEx(pstTmpHeader);
        }

        SipStack::FreeHeaderEx(pstHeader);

        // Remote URI & Tag
        SipStack::FreeHeader(pstRemoteURI);

        pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::TO);

#ifdef __IMS_SIP_DIALOG_COMPONENT_BY_REFERENCE__
        pstTmpHeader = pstHeader;
        pstHeader = IMS_NULL;
#else
        pstTmpHeader = SipStack::CloneHeader(pstHeader);
#endif

        if (pstTmpHeader != IMS_NULL)
        {
            pstRemoteURI = SipStack::CopyHeader(pstRemoteURI, pstTmpHeader);
            SipStack::FreeHeaderEx(pstTmpHeader);
        }

        SipStack::FreeHeaderEx(pstHeader);

        // Via branch parameter
        pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::VIA);
        strLocalViaBranch = SipStack::GetParameter(pstHeader, Sip::STR_BRANCH);
        SipStack::FreeHeaderEx(pstHeader);
    }
    // The dialog state is created by the incoming dialog-creatable request
    else
    {
        // Local URI & Tag
        SipStack::FreeHeader(pstLocalURI);

        pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::TO);

#ifdef __IMS_SIP_DIALOG_COMPONENT_BY_REFERENCE__
        pstTmpHeader = pstHeader;
        pstHeader = IMS_NULL;
#else
        pstTmpHeader = SipStack::CloneHeader(pstHeader);
#endif

        if (pstTmpHeader != IMS_NULL)
        {
            pstLocalURI = SipStack::CopyHeader(pstLocalURI, pstTmpHeader);
            SipStack::FreeHeaderEx(pstTmpHeader);
        }

        SipStack::FreeHeaderEx(pstHeader);

        // Remote URI & Tag
        SipStack::FreeHeader(pstRemoteURI);

        pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::FROM);

#ifdef __IMS_SIP_DIALOG_COMPONENT_BY_REFERENCE__
        pstTmpHeader = pstHeader;
        pstHeader = IMS_NULL;
#else
        pstTmpHeader = SipStack::CloneHeader(pstHeader);
#endif

        if (pstTmpHeader != IMS_NULL)
        {
            pstRemoteURI = SipStack::CopyHeader(pstRemoteURI, pstTmpHeader);
            SipStack::FreeHeaderEx(pstTmpHeader);
        }

        SipStack::FreeHeaderEx(pstHeader);

        // Via branch parameter
        pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::VIA);
        strRemoteViaBranch = SipStack::GetParameter(pstHeader, Sip::STR_BRANCH);
        SipStack::FreeHeaderEx(pstHeader);
    }

    // Create a shared dialog state
    if (pSharedState == IMS_NULL)
    {
        pSharedState = new SipDialogSharedState();

        if (pSharedState == IMS_NULL)
        {
            IMS_TRACE_E(0, "Creating a shared dialog state failed", 0, 0, 0);
            return IMS_FALSE;
        }
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipDialogState::InitDialogDetails(IN IMS_SINT32 nTrigger, IN SipDialogState* pDState)
{
    //---------------------------------------------------------------------------------------------

    if (nTrigger == DIALOG_CANCELLED)
    {
        // Copy the remote sequence number from INVITE dialog...
        nRemoteCSeq = pDState->nRemoteCSeq;
        nRemoteCSeqForINVITE = pDState->nRemoteCSeqForINVITE;
    }
    else if (nTrigger == DIALOG_FORKED_REQUEST)
    {
        // Copy the information from the original dialog info.
        bIsCaller = pDState->bIsCaller;
        nLocalCSeq = pDState->nLocalCSeq;
    }
    else if (nTrigger == DIALOG_FORKED_RESPONSE)
    {
        // It will be updated from the previous request later...
        nLocalCSeq = 1;

#ifdef __IMS_SIP_DIALOG_COMPONENT_BY_REFERENCE__
        SipHeaderBase* pstTmpHeader = SipStack::CloneHeader(pstRemoteURI);

        if (pstTmpHeader != IMS_NULL)
        {
            SipStack::FreeHeader(pstRemoteURI);

            pstRemoteURI = SipStack::CopyHeader(pstRemoteURI, pstTmpHeader);
            SipStack::FreeHeaderEx(pstTmpHeader);
        }

        // Remove the remote-tag because it is updated by the previous response
        // which is creating a dialog (early)
        SipStack::RemoveParameter(Sip::STR_TAG, pstRemoteURI);
#else
        // Remove the remote-tag because it is updated by the previous response
        // which is creating a dialog (early)
        SipStack::RemoveParameter(Sip::STR_TAG, pstRemoteURI);
#endif
    }
    else
    {
        return IMS_FALSE;
    }

    // HEADER_REQ_SESSION-ID
    strSessionId = pDState->strSessionId;

    if ((pDState->nFromChangeOption & FROM_CHANGE_ON_INVITE_REQUEST) != 0)
    {
        SetFromChangeOption(FROM_CHANGE_ON_INVITE_REQUEST);
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipDialogState::InitRequest(
        IN CONST SipMethod& objMethod, IN_OUT ::SipMessage*& pstMessage)
{
    SipHeaderBase* pstHeader;

    //---------------------------------------------------------------------------------------------

    // Method
    SipStack::SetMethod(objMethod.ToString(), pstMessage);

    // Request-URI : Set the remote target URI
    SipAddrSpec* pstAddrSpec = SipStack::GetAddrSpec(pstRemoteTargetURI);

    if (pstAddrSpec == IMS_NULL)
    {
        IMS_TRACE_E(0, "There is no remote target information (no contact)", 0, 0, 0);
        return IMS_FALSE;
    }

    SipStack::SetRequestUri(pstAddrSpec, pstMessage);
    SipStack::FreeAddrSpec(pstAddrSpec);

    // From
    pstHeader = SipStack::CloneHeader(pstLocalURI);
    SipStack::SetHeaderType(ISipHeader::FROM, pstHeader);

    if (!SipStack::SetHeader(pstHeader, pstMessage))
    {
        SipStack::FreeHeaderEx(pstHeader);
        return IMS_FALSE;
    }

    SipStack::FreeHeaderEx(pstHeader);

    // To
    pstHeader = SipStack::CloneHeader(pstRemoteURI);
    SipStack::SetHeaderType(ISipHeader::TO, pstHeader);

    if (!SipStack::SetHeader(pstHeader, pstMessage))
    {
        SipStack::FreeHeaderEx(pstHeader);
        return IMS_FALSE;
    }

    SipStack::FreeHeaderEx(pstHeader);

    // Call-ID
    pstHeader = SipStack::DecodeHeader(ISipHeader::CALL_ID, strCallId);

    if (!SipStack::SetHeader(pstHeader, pstMessage))
    {
        SipStack::FreeHeaderEx(pstHeader);
        return IMS_FALSE;
    }

    SipStack::FreeHeaderEx(pstHeader);

    // Via : Transport Layer will set the header field

    // Route
    for (IMS_UINT32 i = 0; i < objRouteSet.GetSize(); ++i)
    {
        SipHeaderBase* pstHeader = objRouteSet.GetAt(i);

        if (pstHeader != IMS_NULL)
        {
            if (!SipStack::InsertHeader(pstHeader, i, pstMessage))
            {
                return IMS_FALSE;
            }
        }
    }

    // Contact
    if (IsContactMandatory(
                sipcore::SipMessage::TYPE_REQUEST, objMethod, SipStatusCode::SC_INVALID, IMS_FALSE))
    {
        pstHeader = SipStack::CloneHeader(pstLocalTargetURI);

        if (!SipStack::SetHeader(pstHeader, pstMessage))
        {
            SipStack::FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        SipStack::FreeHeaderEx(pstHeader);
    }

    // HEADER_REQ_SESSION-ID
    if (strSessionId.GetLength() > 0)
    {
        const AString SESSION_ID(SipHeaderName::SESSION_ID);

        pstHeader = SipStack::DecodeHeader(ISipHeader::UNKNOWN, SESSION_ID, strSessionId);

        (void)SipStack::SetUnknownHeader(pstHeader, SESSION_ID, pstMessage);
        SipStack::FreeHeaderEx(pstHeader);
    }

    return IMS_TRUE;
}

/*

Remarks
 CONTACT_HEADER_PARAMETER_CONTROL_FOR_MID_DIALOG_REQUEST

*/
PUBLIC
IMS_RESULT SipDialogState::SetContactParameter(
        IN CONST AString& strParameter, IN IMS_SINT32 nOperation /* = 0 (0: ADD, 1: REMOVE) */)
{
    //---------------------------------------------------------------------------------------------

    if ((nOperation != 0) && (nOperation != 1))
    {
        return IMS_FAILURE;
    }

    AString strName;
    AString strValue = AString::ConstNull();
    IMS_SINT32 nCount = strParameter.SplitF(TextParser::CHAR_EQUAL, strName, strValue);

    if (nCount == 0)
    {
        return IMS_FAILURE;
    }

    if (nOperation == 0 /* ADD */)
    {
        if (strValue.IsEmpty())
        {
            strValue = AString::ConstNull();
        }

        if (!SipStack::SetParameter(pstLocalTargetURI, strName, strValue))
        {
            return IMS_FAILURE;
        }

        if ((pLocalContactHeader != IMS_NULL) &&
                (pLocalContactHeader->SetParameter(strName, strValue) != IMS_SUCCESS))
        {
            return IMS_FAILURE;
        }
    }
    else
    {
        SipStack::RemoveParameter(strName, pstLocalTargetURI);

        if (pLocalContactHeader != IMS_NULL)
        {
            pLocalContactHeader->RemoveParameter(strName);
        }
    }

    return IMS_SUCCESS;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 SipDialogState::UpdateDialogDetails(IN CONST SipMessageInfo& objMInfo,
        IN IMS_SINT32 nUsageState, IN IMS_SINT32 nAction, IN IMS_SINT32 nTrigger)
{
    //---------------------------------------------------------------------------------------------

    // If the dialog's state is already in TERMINATED state, skip the below procedures ...
    if (nState == SipDState::STATE_TERMINATED)
    {
        IMS_TRACE_D("INVALID STATE : Dialog (%s) is already in TERMINATED state",
                SipDebug::GetCharA1(strCallId.GetStr(), 8, '@'), 0, 0);
        return SipPrivate::MESSAGE_VALID;
    }

    ::SipMessage* pstMessage = objMInfo.GetMessage();

    if (objMInfo.IsOutgoingMessage())
    {
        if (!SipStack::IsRequestMessage(pstMessage))
        {
            if (GetState() == SipDState::STATE_INIT)
            {
                // Update To-Tag when sending the response to the incoming SIP request
                if (!UpdateComponents(objMInfo))
                {
                    return SipPrivate::MESSAGE_FAILED;
                }

                // HEADER_REQ_SESSION-ID
                if (SipFeatures::IsHeaderSessionIdRequired(objMInfo.GetSlotId()))
                {
                    UpdateSessionId(objMInfo);
                }
            }
        }
        else
        {
            if (GetState() == SipDState::STATE_INIT)
            {
                // Update the local command sequence number
                nLocalCSeq = SipStack::GetCSeqNumber(pstMessage);

                // HEADER_REQ_SESSION-ID
                if (SipFeatures::IsHeaderSessionIdRequired(objMInfo.GetSlotId()))
                {
                    UpdateSessionId(objMInfo);
                }
            }
        }

        if (GetState() != SipDState::STATE_CONFIRMED)
        {
            UpdateFromChangeOption(objMInfo);
        }
    }
    else
    {
        IMS_BOOL bIsINVITEConfirmed = IMS_FALSE;

        // Update the dialog identifiers if it does not already set
        if (!SipStack::IsRequestMessage(pstMessage))
        {
            // If this is a response, then update only the To-Tag in the dialog state
            // if it has not been set as yet.
            // The From, Call-ID and other fields have already been set at the time
            // of sending out the request.
            // NOTE: The to-tag updates should only be done for a >= 2xx response.
            // Only RPR's with a different tag cause formation of different call legs.
            // As we don't support multiple call legs, for now we will update the to-tag
            // for any incoming response.
            SipHeaderBase* pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::TO);
            AString strMsgTag = SipStack::GetParameter(pstHeader, Sip::STR_TAG);
            SipStack::FreeHeaderEx(pstHeader);

            AString strDialogTag = SipStack::GetParameter(pstRemoteURI, Sip::STR_TAG);

            if ((strDialogTag.GetLength() == 0) && (strMsgTag.GetLength() > 0))
            {
                if (!SipStack::SetParameter(pstRemoteURI, Sip::STR_TAG, strMsgTag))
                    return SipPrivate::MESSAGE_FAILED;
            }

            // The route set for the dialog MUST be re-computed based on the 2xx response
            IMS_SINT32 nStatusCode = SipStack::GetStatusCode(pstMessage);

            if (objMInfo.GetMethod().Equals(SipMethod::INVITE) &&
                    (SipStatusCode::IsFinalSuccess(nStatusCode)))
            {
                bIsINVITEConfirmed = IMS_TRUE;
            }
        }
        else
        {
            // Validate Cseq:
            // It will be handled in the HandleRequest(...) in SipServerTransactionState
            /*
            IMS_SINT32 nValidity = ValidateRemoteCSeq(pstMessage);

            if (nValidity != SipPrivate::MESSAGE_VALID)
            {
                IMS_TRACE_E(0, "Validating CSeq failed", 0, 0, 0);
                return nValidity;
            }
            */

            nRemoteCSeq = SipStack::GetCSeqNumber(pstMessage);

            if (objMInfo.GetMethod().Equals(SipMethod::INVITE))
            {
                nRemoteCSeqForINVITE = nRemoteCSeq;
            }

            // It is a request that has been received. If the From header has a tag, and
            //  a) If the original call was from us and the to-tag in the dialog state is NULL,
            //    then we need to update it with the received tag.
            //  b) If the original call was from the other end,
            //    and the From tag in the dialog state is NULL,
            //    then we need to update it with the received tag.
            SipHeaderBase* pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::FROM);
            AString strMsgTag = SipStack::GetParameter(pstHeader, Sip::STR_TAG);
            SipStack::FreeHeaderEx(pstHeader);

            if (strMsgTag.GetLength() > 0)
            {
                AString strDialogTag = SipStack::GetParameter(pstRemoteURI, Sip::STR_TAG);

                if (strDialogTag.GetLength() == 0)
                {
                    if (!SipStack::SetParameter(pstRemoteURI, Sip::STR_TAG, strMsgTag))
                        return SipPrivate::MESSAGE_FAILED;
                }
            }

            if (objMInfo.GetMethod().Equals(SipMethod::INVITE))
            {
                pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::VIA);
                strRemoteViaBranch = SipStack::GetParameter(pstHeader, AString(Sip::STR_BRANCH));
                SipStack::FreeHeaderEx(pstHeader);
            }
        }

        if (GetState() != SipDState::STATE_CONFIRMED)
        {
            // HEADER_REQ_SESSION-ID
            if (SipFeatures::IsHeaderSessionIdRequired(objMInfo.GetSlotId()))
            {
                UpdateSessionId(objMInfo);
            }

            UpdateFromChangeOption(objMInfo);

            if (objRouteSet.GetSize() == 0)
            {
                if (!CreateRouteSet(objMInfo))
                    return SipPrivate::MESSAGE_FAILED;
            }
            else
            {
                // Route headers already exist in the route set.
                // Update only the contact header if required.

                // The route set for the dialog MUST be re-computed based on the 2xx response
                if (bIsINVITEConfirmed)
                {
                    bPreloadedSet = IMS_FALSE;

                    ClearRouteSet();

                    if (!CreateRouteSet(objMInfo))
                        return SipPrivate::MESSAGE_FAILED;
                }
                else
                {
                    if (!UpdateRouteSet(objMInfo))
                        return SipPrivate::MESSAGE_FAILED;
                }
            }
        }
    }

    // Update the Contact information according to the mode
    UpdateContact(objMInfo);

    // Update a dialog state
    UpdateState(nUsageState, nAction, nTrigger);

    // Update "remote-URI" if changed
    UpdateRemoteURI(objMInfo);

    return SipPrivate::MESSAGE_VALID;
}

/*

Remarks

*/
PUBLIC
void SipDialogState::UpdateLocalCSeq(IN IMS_UINT32 nCSeq)
{
    //---------------------------------------------------------------------------------------------

    IMS_TRACE_I("Dialog (%s) - CSeq: %d >> %d", SipDebug::GetCharA1(strCallId.GetStr(), 8, '@'),
            nLocalCSeq, nCSeq);

    nLocalCSeq = nCSeq;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 SipDialogState::ValidateRemoteCSeq(
        IN ::SipMessage* pstMessage, IN IMS_SINT32 nPrevStatusCode /* = 0*/)
{
    IMS_UINT32 nSeqNum;
    SipMethod objMethod = SipStack::GetMethod(pstMessage);

    //---------------------------------------------------------------------------------------------

    nSeqNum = SipStack::GetCSeqNumber(pstMessage);

    if (nSeqNum == SipPrivate::INVALID_SEQ_NUM)
    {
        return SipPrivate::MESSAGE_FAILED;
    }

    // If a retransmitted ACK of lower CSeq appears after a fresh INVITE of higher CSeq
    // has been received (for example, after the first INVITE was rejected with a 401/407),
    // it can be dropped.

    // For an ACK, if the CSeq is lower than the remote CSeq, it can be dropped.
    // This may happen if the ACK to a 401 appears after a fresh INVITE
    // with higher CSeq has been received by the F/W.
    if (objMethod.Equals(SipMethod::ACK))
    {
        if (nRemoteCSeqForINVITE != nSeqNum)
        {
            // ACK request of incoming re-INVITE can be received
            // after new re-INVITE request is received.
            if (!SipStatusCode::IsFinalSuccess(nPrevStatusCode))
            {
                IMS_TRACE_E(0, "OUT OF SEQUENCE :: ACK (%d, %d)", nRemoteCSeqForINVITE, nSeqNum, 0);
                return SipPrivate::MESSAGE_DISCARDED;
            }
        }

        return SipPrivate::MESSAGE_VALID;
    }

    // Check if the CSeq is higher.
    // Note that for ACK and CANCEL, the CSeq will be same as that in initial INVITE.
    if (objMethod.Equals(SipMethod::CANCEL))
    {
        if (nRemoteCSeqForINVITE == nSeqNum)
        {
            return SipPrivate::MESSAGE_VALID;
        }
        else if (nRemoteCSeqForINVITE < nSeqNum)
        {
            SipPrivate::SetLastError(SipError::CSEQ_VALUE_MISMATCHED);
            return SipPrivate::MESSAGE_INVALID_400;
        }
        else
        {
            SipPrivate::SetLastError(SipError::REQUEST_OUT_OF_ORDER);
            return SipPrivate::MESSAGE_INVALID_500;
        }
    }

    // Checks for PRACK.
    if (objMethod.Equals(SipMethod::PRACK))
    {
        // Application will handle the procedure for PRACK.
    }

    // 3 Loop detection ????

    // a) If CSeq is higher, it should be responded with a 400 Bad Request.
    // b) If CSeq is the same, it is a retransmission, so it can be dropped.
    // c) If CSeq is the lower, it should be responded with a 500 response.
    if (nRemoteCSeq > nSeqNum)
    {
        SipPrivate::SetLastError(SipError::REQUEST_OUT_OF_ORDER);
        return SipPrivate::MESSAGE_INVALID_500;
    }
    else if (nRemoteCSeq == nSeqNum)
    {
        // It needs to be checked if the request is looped or not.
        // CheckRequestLoop(pstMessage);
        SipPrivate::SetLastError(SipError::LOOP_DETECTED);
        return SipPrivate::MESSAGE_INVALID_400;
    }

    return SipPrivate::MESSAGE_VALID;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipDialogState::AddDialogUsage(IN SipDialogEx* pDialogEx)
{
    //---------------------------------------------------------------------------------------------

    if (pSharedState == IMS_NULL)
        return IMS_FALSE;

    return pSharedState->AddDialog(pDialogEx);
}

/*

Remarks

*/
PUBLIC
void SipDialogState::RemoveDialogUsage(IN SipDialogEx* pDialogEx)
{
    //---------------------------------------------------------------------------------------------

    if (pSharedState == IMS_NULL)
        return;

    pSharedState->RemoveDialog(pDialogEx);
}

/*

Remarks

*/
PUBLIC
SipDialogEx* SipDialogState::GetDialogUsage(IN CONST SipMessageInfo& objMInfo)
{
    //---------------------------------------------------------------------------------------------

    if (pSharedState == IMS_NULL)
        return IMS_NULL;

    return pSharedState->GetDialog(objMInfo);
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipDialogState::HasMultipleDialogUsages() const
{
    //---------------------------------------------------------------------------------------------

    if (pSharedState == IMS_NULL)
    {
        return IMS_FALSE;
    }

    return pSharedState->HasMultipleDialogUsages();
}

/*

Remarks

*/
PUBLIC GLOBAL IMS_BOOL SipDialogState::IsContactMandatory(IN IMS_SINT32 nMsgType,
        IN CONST SipMethod& objMethod, IN IMS_SINT32 nStatusCode,
        IN IMS_BOOL bContactInAll1xxRequired)
{
    //---------------------------------------------------------------------------------------------

    if (nMsgType == sipcore::SipMessage::TYPE_REQUEST)
    {
        if (objMethod.Equals(SipMethod::INVITE) || objMethod.Equals(SipMethod::SUBSCRIBE) ||
                objMethod.Equals(SipMethod::NOTIFY) || objMethod.Equals(SipMethod::REFER) ||
                objMethod.Equals(SipMethod::UPDATE))
        {
            return IMS_TRUE;
        }
        // REGISTER, OPTIONS, and PUBLISH is added by the JSR 281 requirements.
        else if (objMethod.Equals(SipMethod::REGISTER) || objMethod.Equals(SipMethod::OPTIONS) ||
                objMethod.Equals(SipMethod::PUBLISH))
        {
            return IMS_TRUE;
        }
    }
    else
    {
        if ((nStatusCode >= SipStatusCode::SC_200) && (nStatusCode < SipStatusCode::SC_300))
        {
            if (objMethod.Equals(SipMethod::INVITE) || objMethod.Equals(SipMethod::SUBSCRIBE) ||
                    objMethod.Equals(SipMethod::REFER) || objMethod.Equals(SipMethod::UPDATE))
            {
                return IMS_TRUE;
            }
            // OPTIONS & NOTIFY is added by the JSR 281 requirements.
            else if (objMethod.Equals(SipMethod::OPTIONS) || objMethod.Equals(SipMethod::NOTIFY))
            {
                return IMS_TRUE;
            }
        }
        else if ((nStatusCode >= SipStatusCode::SC_300) && (nStatusCode < SipStatusCode::SC_400))
        {
            if (objMethod.Equals(SipMethod::SUBSCRIBE) || objMethod.Equals(SipMethod::NOTIFY))
            {
                return IMS_TRUE;
            }
        }
        // PATCH_FOR_CONTACT_HEADER_IN_ALL_1XX
        else if (SipStatusCode::IsProvisional(nStatusCode))
        {
            if (objMethod.Equals(SipMethod::INVITE))
            {
                if (bContactInAll1xxRequired)
                {
                    return IMS_TRUE;
                }
                else if (nStatusCode >= SipStatusCode::SC_180)
                {
                    // 3GPP Profile: 18x / 19x
                    return IMS_TRUE;
                }
            }
        }
    }

    return IMS_FALSE;
}

/*

Remarks

*/
PRIVATE
void SipDialogState::ClearRouteSet()
{
    //---------------------------------------------------------------------------------------------

    if (objRouteSet.IsEmpty())
        return;

    for (IMS_UINT32 i = 0; i < objRouteSet.GetSize(); ++i)
    {
        SipHeaderBase* pstHeader = objRouteSet.GetAt(i);

        SipStack::FreeHeaderEx(pstHeader);
    }

    objRouteSet.Clear();
}

/*

Remarks

*/
PRIVATE
IMS_BOOL SipDialogState::CreateRouteSet(IN CONST SipMessageInfo& objMInfo)
{
    ::SipMessage* pstMessage = objMInfo.GetMessage();

    //---------------------------------------------------------------------------------------------

    if (SipDialogState::IsTargetRefreshMessage(pstMessage))
    {
        IMS_SINT32 nHCount = SipStack::GetHeaderCount(pstMessage, ISipHeader::RECORD_ROUTE);

        if (nHCount != 0)
        {
            SipHeaderBase* pstRRHdr;
            SipHeaderBase* pstHeader;

            IMS_SINT32 nIndex;

            // Reverse the record-routes into the route list only
            // if a response message is received (UAC end).
            // Else, copy the record-routes into routes as they are (UAS end).
            if (!SipStack::IsRequestMessage(pstMessage))
            {
                IMS_SINT32 nStatusCode = SipStack::GetStatusCode(pstMessage);

                if (!SipStack::IsMessageRpr(pstMessage) &&
                        SipStatusCode::IsProvisional(nStatusCode))
                {
                    bPreloadedSet = IMS_TRUE;
                }

                for (nIndex = nHCount - 1; nIndex >= 0; nIndex--)
                {
                    pstRRHdr = SipStack::GetHeader(pstMessage, ISipHeader::RECORD_ROUTE, nIndex);

                    SipStack::SetHeaderType(ISipHeader::ROUTE, pstRRHdr);

                    pstHeader = SipStack::CloneHeader(pstRRHdr);

                    SipStack::FreeHeaderEx(pstRRHdr);

                    if (!objRouteSet.InsertAt(pstHeader, (nHCount - 1) - nIndex))
                    {
                        SipStack::FreeHeaderEx(pstHeader);
                        return IMS_FALSE;
                    }
                }
            }
            else
            {
                for (nIndex = 0; nIndex < nHCount; nIndex++)
                {
                    pstRRHdr = SipStack::GetHeader(pstMessage, ISipHeader::RECORD_ROUTE, nIndex);

                    SipStack::SetHeaderType(ISipHeader::ROUTE, pstRRHdr);

                    pstHeader = SipStack::CloneHeader(pstRRHdr);

                    SipStack::FreeHeaderEx(pstRRHdr);

                    if (!objRouteSet.Append(pstHeader))
                    {
                        SipStack::FreeHeaderEx(pstHeader);
                        return IMS_FALSE;
                    }
                }
            }
        }

        // 3 Contact or From or Request-URI need to be extracted ???
        if (!UpdateContact(objMInfo))
        {
            return IMS_FALSE;
        }
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PRIVATE
IMS_BOOL SipDialogState::UpdateComponents(IN CONST SipMessageInfo& objMInfo)
{
    SipHeaderBase* pstHeader;
    SipHeaderBase* pstTmpHeader;
    ::SipMessage* pstMessage = objMInfo.GetMessage();

    IMS_SINT32 nHType;
    AString strTmpVal;

    //---------------------------------------------------------------------------------------------

    // Call Id
    pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::CALL_ID);
    SipStack::EncodeHeaderBody(pstHeader, IMS_FALSE, strTmpVal);

    if ((strCallId.GetLength() == 0) || (!strCallId.Equals(strTmpVal)))
    {
        strCallId = strTmpVal;
    }

    SipStack::FreeHeaderEx(pstHeader);

    // Local URI & Tag
    SipStack::FreeHeader(pstLocalURI);

    if (SipStack::IsRequestMessage(pstMessage))
    {
        if (objMInfo.IsOutgoingMessage())
            nHType = ISipHeader::FROM;
        else
            nHType = ISipHeader::TO;
    }
    else
    {
        if (objMInfo.IsOutgoingMessage())
            nHType = ISipHeader::TO;
        else
            nHType = ISipHeader::FROM;
    }

    pstHeader = SipStack::GetHeader(pstMessage, nHType);

#ifdef __IMS_SIP_DIALOG_COMPONENT_BY_REFERENCE__
    pstTmpHeader = pstHeader;
    pstHeader = IMS_NULL;
#else
    pstTmpHeader = SipStack::CloneHeader(pstHeader);
#endif

    if (pstTmpHeader != IMS_NULL)
    {
        pstLocalURI = SipStack::CopyHeader(pstLocalURI, pstTmpHeader);
        SipStack::FreeHeaderEx(pstTmpHeader);
    }

    SipStack::FreeHeaderEx(pstHeader);

    // Remote URI & Tag
    SipStack::FreeHeader(pstRemoteURI);

    if (SipStack::IsRequestMessage(pstMessage))
    {
        if (objMInfo.IsOutgoingMessage())
            nHType = ISipHeader::TO;
        else
            nHType = ISipHeader::FROM;
    }
    else
    {
        if (objMInfo.IsOutgoingMessage())
            nHType = ISipHeader::FROM;
        else
            nHType = ISipHeader::TO;
    }

    pstHeader = SipStack::GetHeader(pstMessage, nHType);

#ifdef __IMS_SIP_DIALOG_COMPONENT_BY_REFERENCE__
    pstTmpHeader = pstHeader;
    pstHeader = IMS_NULL;
#else
    pstTmpHeader = SipStack::CloneHeader(pstHeader);
#endif

    if (pstTmpHeader != IMS_NULL)
    {
        pstRemoteURI = SipStack::CopyHeader(pstRemoteURI, pstTmpHeader);
        SipStack::FreeHeaderEx(pstTmpHeader);
    }

    SipStack::FreeHeaderEx(pstHeader);

    return IMS_TRUE;
}

/*

Remarks

*/
PRIVATE
IMS_BOOL SipDialogState::UpdateContact(IN CONST SipMessageInfo& objMInfo)
{
    ::SipMessage* pstMessage = objMInfo.GetMessage();

    //---------------------------------------------------------------------------------------------

    if (!IsTargetRefreshMessage(pstMessage))
    {
        // REMOTE_TARGET_UPDATE_FROM_MID_DIALOG_REQUEST
        if (objMInfo.IsOutgoingMessage() && !SipStack::IsRequestMessage(pstMessage))
        {
            IMS_SINT32 nStatusCode = SipStack::GetStatusCode(pstMessage);

            if (nStatusCode != SipStatusCode::SC_100)
            {
                RemovePendingRemoteTarget(objMInfo);
            }
        }
        return IMS_TRUE;
    }

    IMS_SINT32 nHCount = SipStack::GetHeaderCount(pstMessage, ISipHeader::CONTACT_NORMAL);

    if (nHCount > 0)
    {
        if (objMInfo.IsOutgoingMessage())
        {
            SipStack::FreeHeader(pstLocalTargetURI);

            SipHeaderBase* pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::CONTACT_NORMAL);

            if (pstHeader != IMS_NULL)
            {
                pstLocalTargetURI = SipStack::CopyHeader(pstLocalTargetURI, pstHeader);
                SipStack::FreeHeaderEx(pstHeader);
            }

            if (pLocalContactHeader != IMS_NULL)
            {
                delete pLocalContactHeader;
                pLocalContactHeader = IMS_NULL;
            }

            if (SipStack::IsValidHeader(pstLocalTargetURI))
            {
                pLocalContactHeader = new SipHeader(pstLocalTargetURI);
            }

            // REMOTE_TARGET_UPDATE_FROM_MID_DIALOG_REQUEST
            if (!SipStack::IsRequestMessage(pstMessage))
            {
                UpdateAndRemovePendingRemoteTarget(objMInfo);
            }
        }
        else
        {
            // REMOTE_TARGET_UPDATE_FROM_MID_DIALOG_REQUEST
            if ((nState != SipDState::STATE_INIT) && SipStack::IsRequestMessage(pstMessage))
            {
                // Store the remote target and it will be updated after sending 2xx response.
                AddPendingRemoteTarget(objMInfo);
            }
            else
            {
                SipStack::FreeHeader(pstRemoteTargetURI);

                SipHeaderBase* pstHeader =
                        SipStack::GetHeader(pstMessage, ISipHeader::CONTACT_NORMAL);

                if (pstHeader != IMS_NULL)
                {
                    pstRemoteTargetURI = SipStack::CopyHeader(pstRemoteTargetURI, pstHeader);
                    SipStack::FreeHeaderEx(pstHeader);
                }
            }
        }
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PRIVATE
IMS_BOOL SipDialogState::UpdateRemoteURI(IN CONST SipMessageInfo& objMInfo)
{
    ::SipMessage* pstMessage = objMInfo.GetMessage();
    IMS_BOOL bFromChangeable = IMS_FALSE;

    //---------------------------------------------------------------------------------------------

    if ((GetState() != SipDState::STATE_INIT) && IsFromChangeCapable() &&
            (objMInfo.GetMethod().Equals(SipMethod::INVITE) ||
                    objMInfo.GetMethod().Equals(SipMethod::UPDATE)) &&
            !SipStack::IsRequestMessage(pstMessage))
    {
        IMS_SINT32 nStatusCode = SipStack::GetStatusCode(pstMessage);

        if (SipStatusCode::IsFinalSuccess(nStatusCode))
        {
            bFromChangeable = IMS_TRUE;
        }
    }

    if (!bFromChangeable)
    {
        return IMS_FALSE;
    }

    SipHeaderBase* pstTmpHeader;
    IMS_SINT32 nHType;

    if (SipStack::IsRequestMessage(pstMessage))
    {
        if (objMInfo.IsOutgoingMessage())
            nHType = ISipHeader::TO;
        else
            nHType = ISipHeader::FROM;
    }
    else
    {
        if (objMInfo.IsOutgoingMessage())
            nHType = ISipHeader::FROM;
        else
            nHType = ISipHeader::TO;
    }

    // Remote URI & Tag
    SipStack::FreeHeader(pstRemoteURI);

    SipHeaderBase* pstHeader = SipStack::GetHeader(pstMessage, nHType);

#ifdef __IMS_SIP_DIALOG_COMPONENT_BY_REFERENCE__
    pstTmpHeader = pstHeader;
    pstHeader = IMS_NULL;
#else
    pstTmpHeader = SipStack::CloneHeader(pstHeader);
#endif

    if (pstTmpHeader != IMS_NULL)
    {
        pstRemoteURI = SipStack::CopyHeader(pstRemoteURI, pstTmpHeader);
        SipStack::FreeHeaderEx(pstTmpHeader);
    }

    SipStack::FreeHeaderEx(pstHeader);

    IMS_TRACE_D("DialogState :: from-change is done", 0, 0, 0);

    return IMS_TRUE;
}

/*

Remarks

*/
PRIVATE
IMS_BOOL SipDialogState::UpdateRouteSet(IN CONST SipMessageInfo& objMInfo)
{
    ::SipMessage* pstMessage = objMInfo.GetMessage();

    //---------------------------------------------------------------------------------------------

    if (SipDialogState::IsTargetRefreshMessage(pstMessage))
    {
        if (bPreloadedSet)
        {
            // This indicates that the existing route set is only the preloaded one.
            // So, delete the preloaded one and form the fresh route set.
            ClearRouteSet();

            if (!CreateRouteSet(objMInfo))
                return IMS_FALSE;

            bPreloadedSet = IMS_FALSE;
        }
        else
        {
            // This is an established route set.
            // Only last entry which indicates the contact address can be changed.
            if (!UpdateContact(objMInfo))
                return IMS_FALSE;
        }
    }

    return IMS_TRUE;
}

/*

Remarks
 HEADER_REQ_SESSION-ID
*/
PRIVATE
void SipDialogState::UpdateSessionId(IN CONST SipMessageInfo& objMInfo)
{
    //---------------------------------------------------------------------------------------------

    if (strSessionId.GetLength() > 0)
    {
        // Session-ID is already set by the previous request or response...
        return;
    }

    SipHeaderBase* pstHeader =
            SipStack::GetUnknownHeader(objMInfo.GetMessage(), SipHeaderName::SESSION_ID);

    if (SipStack::IsValidHeader(pstHeader))
    {
        strSessionId = SipStack::GetUnknownHeaderBody(pstHeader);
    }

    SipStack::FreeHeaderEx(pstHeader);
}

/*

Remarks

*/
PRIVATE
void SipDialogState::UpdateState(
        IN IMS_SINT32 nUsageState, IN IMS_SINT32 nAction, IN IMS_SINT32 nTrigger)
{
    IMS_SINT32 nNextState = SipDState::STATE_MAX;

    //---------------------------------------------------------------------------------------------

    // 3 TODO
    (void)nTrigger;

    switch (nAction)
    {
        case SipDState::ACTION_TRANSIT_STATE:
            // If the shared state has a multiple dialog usages, do not transit the state.
            nNextState = nUsageState;

            if (pSharedState->IsShared() && (nUsageState == SipDState::STATE_TERMINATED))
            {
                // Do not transit the state...
                nNextState = SipDState::STATE_MAX;
            }
            break;

        case SipDState::ACTION_DESTROY_USAGE:
            // If the shared state has a multiple dialog usages, do not transit the state.
            if (!pSharedState->IsShared())
            {
                nNextState = SipDState::STATE_TERMINATED;
            }
            break;

        case SipDState::ACTION_DESTROY_DIALOG:
            nNextState = SipDState::STATE_TERMINATED;
            break;

        case SipDState::ACTION_IGNORE:
        default:
            return;
    }

    if ((nNextState >= SipDState::STATE_INIT) && (nNextState < SipDState::STATE_MAX))
    {
        static const IMS_CHAR* STATE[SipDState::STATE_MAX] = {
                "INITIALIZED", "TERMINATED", "EARLY", "CONFIRMED"};

        IMS_TRACE_I(
                "_____ DIALOG STATE : (%s) >>> (%s) _____", STATE[nState], STATE[nNextState], 0);

        nState = nNextState;
    }
}

/*

Remarks
 REMOTE_TARGET_UPDATE_FROM_MID_DIALOG_REQUEST
*/
PRIVATE
void SipDialogState::AddPendingRemoteTarget(IN CONST SipMessageInfo& objMInfo)
{
    ::SipMessage* pstMessage = objMInfo.GetMessage();

    //---------------------------------------------------------------------------------------------

    AString strKey = SipStack::GetViaBranchParameter(pstMessage);

    if (strKey.GetLength() == 0)
    {
        // no-op
        return;
    }

    // Checks if the pending remote target is already present or not
    for (IMS_UINT32 i = 0; i < objPendingRemoteTargets.GetSize(); ++i)
    {
        PendingRemoteTarget* pRemoteTarget = objPendingRemoteTargets.GetAt(i);

        if (pRemoteTarget == IMS_NULL)
        {
            continue;
        }

        if (strKey.Equals(pRemoteTarget->strKey))
        {
            IMS_TRACE_D("PendingRemoteTarget(add) :: key(dup)=%s, count=%d",
                    SipDebug::GetCharA1(strKey.GetStr(), 8, '@'), objPendingRemoteTargets.GetSize(),
                    0);
            return;
        }
    }

    SipHeaderBase* pstContact = IMS_NULL;
    SipHeaderBase* pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::CONTACT_NORMAL);

    if (SipStack::IsValidHeader(pstHeader))
    {
        pstContact = SipStack::CloneHeader(pstHeader);
    }

    SipStack::FreeHeaderEx(pstHeader);

    if (pstContact == IMS_NULL)
    {
        // no-op
        return;
    }

    objPendingRemoteTargets.Append(new PendingRemoteTarget(strKey, pstContact));

    IMS_TRACE_D("PendingRemoteTarget(add) :: key=%s, count=%d",
            SipDebug::GetCharA1(strKey.GetStr(), 8, '@'), objPendingRemoteTargets.GetSize(), 0);
}

/*

Remarks
 REMOTE_TARGET_UPDATE_FROM_MID_DIALOG_REQUEST
*/
PRIVATE
void SipDialogState::RemoveAllPendingRemoteTargets()
{
    //---------------------------------------------------------------------------------------------

    if (!objPendingRemoteTargets.IsEmpty())
    {
        for (IMS_UINT32 i = 0; i < objPendingRemoteTargets.GetSize(); ++i)
        {
            PendingRemoteTarget* pRemoteTarget = objPendingRemoteTargets.GetAt(i);

            if (pRemoteTarget != IMS_NULL)
            {
                delete pRemoteTarget;
            }
        }

        objPendingRemoteTargets.Clear();
    }
}

/*

Remarks
 REMOTE_TARGET_UPDATE_FROM_MID_DIALOG_REQUEST
*/
PRIVATE
void SipDialogState::RemovePendingRemoteTarget(IN CONST SipMessageInfo& objMInfo)
{
    //---------------------------------------------------------------------------------------------

    if (objPendingRemoteTargets.IsEmpty())
    {
        // no-op
        return;
    }

    AString strKey = SipStack::GetViaBranchParameter(objMInfo.GetMessage());

    if (strKey.GetLength() == 0)
    {
        // no-op
        return;
    }

    for (IMS_UINT32 i = 0; i < objPendingRemoteTargets.GetSize(); ++i)
    {
        PendingRemoteTarget* pRemoteTarget = objPendingRemoteTargets.GetAt(i);

        if (pRemoteTarget == IMS_NULL)
        {
            continue;
        }

        if (strKey.Equals(pRemoteTarget->strKey))
        {
            objPendingRemoteTargets.RemoveAt(i);
            delete pRemoteTarget;

            IMS_TRACE_D("PendingRemoteTarget(remove) :: key=%s, count=%d",
                    SipDebug::GetCharA1(strKey.GetStr(), 8, '@'), objPendingRemoteTargets.GetSize(),
                    0);
            break;
        }
    }
}

/*

Remarks
 REMOTE_TARGET_UPDATE_FROM_MID_DIALOG_REQUEST
*/
PRIVATE
void SipDialogState::UpdateAndRemovePendingRemoteTarget(IN CONST SipMessageInfo& objMInfo)
{
    //---------------------------------------------------------------------------------------------

    if (objPendingRemoteTargets.IsEmpty())
    {
        // no-op
        return;
    }

    AString strKey = SipStack::GetViaBranchParameter(objMInfo.GetMessage());

    if (strKey.GetLength() == 0)
    {
        // no-op
        return;
    }

    PendingRemoteTarget* pRemoteTarget = IMS_NULL;
    IMS_UINT32 nMatchedIndex = 0;

    for (IMS_UINT32 i = 0; i < objPendingRemoteTargets.GetSize(); ++i)
    {
        pRemoteTarget = objPendingRemoteTargets.GetAt(i);

        if (pRemoteTarget == IMS_NULL)
        {
            continue;
        }

        if (strKey.Equals(pRemoteTarget->strKey))
        {
            nMatchedIndex = i;
            break;
        }

        pRemoteTarget = IMS_NULL;
    }

    if ((pRemoteTarget != IMS_NULL) && (pRemoteTarget->pstHeader != IMS_NULL))
    {
        SipStack::FreeHeaderEx(pstRemoteTargetURI);
        pstRemoteTargetURI = SipStack::CopyHeader(pRemoteTarget->pstHeader);
    }

    if (pRemoteTarget != IMS_NULL)
    {
        if (nMatchedIndex == (objPendingRemoteTargets.GetSize() - 1))
        {
            RemoveAllPendingRemoteTargets();
        }
        else
        {
            IMS_SINT32 nCount = static_cast<IMS_SINT32>(nMatchedIndex + 1);

            while (nCount > 0)
            {
                pRemoteTarget = objPendingRemoteTargets.GetAt(0);
                objPendingRemoteTargets.RemoveAt(0);

                if (pRemoteTarget != IMS_NULL)
                {
                    delete pRemoteTarget;
                }

                --nCount;
            }
        }

        IMS_TRACE_D("PendingRemoteTarget(update&remove) :: key=%s, index=%d, count=%d",
                SipDebug::GetCharA1(strKey.GetStr(), 8, '@'), nMatchedIndex,
                objPendingRemoteTargets.GetSize());
    }
}

/*

Remarks

*/
PRIVATE
IMS_BOOL SipDialogState::IsFromChangeCapable() const
{
    //---------------------------------------------------------------------------------------------

    return ((nFromChangeOption & FROM_CHANGE_CAPABLE) == FROM_CHANGE_CAPABLE);
}

/*

Remarks

*/
PRIVATE
void SipDialogState::ClearFromChangeOption(IN IMS_SINT32 nOption)
{
    //---------------------------------------------------------------------------------------------

    nFromChangeOption &= (~nOption);
}

/*

Remarks

*/
PRIVATE
void SipDialogState::SetFromChangeOption(IN IMS_SINT32 nOption)
{
    IMS_BOOL bChanged = ((nFromChangeOption & nOption) != nOption);

    //---------------------------------------------------------------------------------------------

    nFromChangeOption |= nOption;

    if (bChanged)
    {
        IMS_TRACE_D("DialogState :: from-change=%02x", nFromChangeOption, 0, 0);
    }
}

/*

Remarks

*/
PRIVATE
void SipDialogState::UpdateFromChangeOption(IN CONST SipMessageInfo& objMInfo)
{
    const SipMethod& objMethod = objMInfo.GetMethod();

    //---------------------------------------------------------------------------------------------

    if (!objMethod.Equals(SipMethod::INVITE))
    {
        // no-op
        return;
    }

    ::SipMessage* pstMessage = objMInfo.GetMessage();

    if ((GetState() == SipDState::STATE_INIT) && SipStack::IsRequestMessage(pstMessage))
    {
        if (SipStack::IsOptionSupported(pstMessage, Sip::STR_FROM_CHANGE))
        {
            SetFromChangeOption(FROM_CHANGE_ON_INVITE_REQUEST);
        }
    }
    else if ((GetState() != SipDState::STATE_CONFIRMED) && !SipStack::IsRequestMessage(pstMessage))
    {
        IMS_SINT32 nStatusCode = SipStack::GetStatusCode(pstMessage);

        if (SipStatusCode::IsProvisional(nStatusCode) || SipStatusCode::IsFinalSuccess(nStatusCode))
        {
            if (SipStack::IsOptionSupported(pstMessage, Sip::STR_FROM_CHANGE))
            {
                SetFromChangeOption(FROM_CHANGE_ON_INVITE_RESPONSE);
            }
        }
    }
}

/*

Remarks

*/
PRIVATE GLOBAL IMS_SINT32 SipDialogState::CompareHeaders(IN SipHeaderBase* pstNewH,
        IN SipHeaderBase* pstExistingH, IN IMS_BOOL bToTagLenient, IN IMS_SINT32 nForkedMessage)
{
    IMS_BOOL bToTagComparison = IMS_FALSE;
    IMS_SINT32 nComparisonResult = NOT_MATCHED;

    //---------------------------------------------------------------------------------------------

    if ((pstNewH == IMS_NULL) || (pstExistingH == IMS_NULL))
        return NOT_MATCHED;

    // Compare the tags which are sufficient for RFC 3261 compliant UAs
    AString strNewTag = SipStack::GetParameter(pstNewH, Sip::STR_TAG);
    AString strExistingTag = SipStack::GetParameter(pstExistingH, Sip::STR_TAG);

    if ((SipStack::GetHeaderType(pstNewH) == ISipHeader::TO) &&
            (SipStack::GetHeaderType(pstExistingH) == ISipHeader::TO))
    {
        bToTagComparison = IMS_TRUE;
    }

    // New-Tag is the tag of the new key.
    // Existing-Tag is the tag of the existing key.
    // If Existing-Tag is NULL, but New-Tag is present, that is fine.
    // But, if New-Tag is NULL and Existing-Tag is not, then the remote end has gobbled the tag.
    // We return NOT_MATCHED in this case.
    if (!strNewTag.IsNULL())
    {
        if (!strExistingTag.IsNULL())
        {
            if (!strNewTag.Equals(strExistingTag))
            {
                if ((bToTagComparison) && (bToTagLenient))
                {
                    if (nForkedMessage == FORKED_INVITE)
                        nComparisonResult = MATCHED_DIFFERENT;
                    else
                        nComparisonResult = MATCHED;
                }
                else
                {
                    nComparisonResult = NOT_MATCHED;
                }
            }
            else
            {
                nComparisonResult = MATCHED;
            }
        }
        else
        {
            // Fix for CSR 1-1316815
            // Leniency should be present only for the To-To pair.
            // For all other pairs (i.e. From-From, From-To, To-From),
            // it should map to NOT_MATCHED.
            if (bToTagComparison)
                nComparisonResult = MATCHED;
            else
                nComparisonResult = NOT_MATCHED;
        }
    }
    else
    {
        if (!strExistingTag.IsNULL())
        {
            // Fix for CSR 1-1316815
            // Leniency should be present only for the To-To pair and that too
            // if the bToTagLenient flag is set (i.e. to provide leniency for special cases).
            if (bToTagComparison && bToTagLenient)
                nComparisonResult = MATCHED;
            else
                nComparisonResult = NOT_MATCHED;
        }
        else
        {
            nComparisonResult = MATCHED;
        }
    }

    // The following code is added to check the two scenarios:
    // 1) Early NOTIFY i.e. a NOTIFY arriving before any other response/SUBSCRIBE.
    // In case of Early NOTIFY, the existing dialog-state would not have a tag
    // whereas the new dialog-state would have one.
    //
    // 2) A NOTIFY arriving because of forking of the SUBSCRIBE. This check has to be
    // prompted by the application and we use the "dIsCheckForkedSubs" boolean for this purpose.
    //
    // NOTE: In case of NOTIFYs from different notifiers(as a result of forked SUBSCRIBE),
    // it is most likely that the type of the remote-address in the dialog-state is the same as
    // the type of the remote-address got from the NOTIFY(i.e. its From header).
    // So, the check for Early NOTIFY will not pass & so we've to explicitly check the boolean,
    // "dIsCheckForkedSubs.
    if ((nComparisonResult == NOT_MATCHED) &&
            ((nForkedMessage == FORKED_SUBSCRIBE) ||
                    ((SipStack::GetHeaderType(pstNewH) == ISipHeader::FROM) &&
                            (SipStack::GetHeaderType(pstExistingH) == ISipHeader::TO))))
    {
        if (!strNewTag.IsNULL())
        {
            if (!strExistingTag.IsNULL() && (nForkedMessage == FORKED_SUBSCRIBE))
            {
                // If To-Tag of the existing dialog-state doesn't match that of the existing
                // dialog-state, then it could be either: NOTIFY received for a forked SUBSCRIBE
                // (OR) NOTIFY outside any subscription. In either case, we will return
                // the partially matched dialog-state and let the application find out the rest.
                nComparisonResult = MATCHED_FORKED_SUBSCRIBE;

                // Check the case,
                //        Existing Header: From (local-tag), New Header: To (local-tag)
                if ((SipStack::GetHeaderType(pstNewH) == ISipHeader::TO) &&
                        (SipStack::GetHeaderType(pstExistingH) == ISipHeader::FROM))
                {
                    if (!strNewTag.Equals(strExistingTag))
                    {
                        nComparisonResult = NOT_MATCHED;
                    }
                }
            }
            else if (strExistingTag.IsNULL())
            {
                // If To-tag is not present in the existing dialog-state,
                // then it could be a case of either:
                // Early NOTIFY (OR) NOTIFY received for a forked SUBSCRIBE.
                // We will treat this as an early NOTIFY.
                // Again, return the partially matching dialog-state,
                // application should figure out the rest.
                nComparisonResult = MATCHED_EARLY_NOTIFY;
            }
        }
    }

    return nComparisonResult;
}

/*

Remarks

*/
PRIVATE GLOBAL IMS_BOOL SipDialogState::IsTargetRefreshMessage(IN ::SipMessage* pstMessage)
{
    SipMethod objMethod = SipStack::GetMethod(pstMessage);

    //---------------------------------------------------------------------------------------------

    if (objMethod.Equals(SipMethod::INVALID))
        return IMS_FALSE;

    // Check if the method is a remote target refreshable one or not
    if (!objMethod.Equals(SipMethod::INVITE) && !objMethod.Equals(SipMethod::UPDATE) &&
            !objMethod.Equals(SipMethod::SUBSCRIBE) && !objMethod.Equals(SipMethod::NOTIFY) &&
            !objMethod.Equals(SipMethod::REFER))
    {
        return IMS_FALSE;
    }

    if (!SipStack::IsRequestMessage(pstMessage))
    {
        IMS_SINT32 nStatusCode = SipStack::GetStatusCode(pstMessage);

        if (!SipStatusCode::IsProvisional(nStatusCode) &&
                !SipStatusCode::IsFinalSuccess(nStatusCode))
        {
            return IMS_FALSE;
        }

        if (SipStatusCode::IsProvisional(nStatusCode))
        {
            // Check if the message is a reliable provisional response or not
            if (SipStack::IsMessageRpr(pstMessage) != IMS_TRUE)
            {
                // Check if the message has a To-Tag or not
                SipHeaderBase* pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::TO);

                // Get To-Tag parameter from To header
                AString strToTag = SipStack::GetParameter(pstHeader, Sip::STR_TAG);
                SipStack::FreeHeaderEx(pstHeader);

                if (strToTag.GetLength() == 0)
                {
                    return IMS_FALSE;
                }
            }
        }
    }

    return IMS_TRUE;
}
