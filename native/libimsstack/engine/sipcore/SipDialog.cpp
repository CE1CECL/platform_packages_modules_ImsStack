/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100526  hwangoo.park@             Created
    </table>

    Description
     This class represents one SIP dialog. The SipDialog can be retrieved from a SipConnection
    object, when it is available (at earliest after provisional 101~199 response).
    Three SIP requests can open a dialog: INVITE, SUBSCRIBE/NOTIFY and REFER/NOTIFY.
    An implementation compliant to this specification must support all of the following ways
    of creating dialogs:
    - INVITE-1xx-2xx-ACK will open a dialog. Subsequent SipClientConnection in the same dialog
    can be obtained by calling GetNewClientConnection(...) method. The dialog is terminated
    when the transaction BYE-200 OK is completed.
    - SUBSCRIBE-200 OK (or matching NOTIFY) will open a dialog. Subsequent SipClientConnection
    in the same dialog can be obtained by calling GetNewClientConnection(...) method.
    The dialog is terminated when a notifier sends a NOTIFY request with a "Subscription-State"
    of "terminated" and there are no other subscriptions alive in this dialog.
    - REFER-matching NOTIFY will open a dialog. Subsequent SipClientConnection in the same dialog
    can be obtained by calling GetNewClientConnection(...) method. The dialog is terminated
    when a notifier sends a NOTIFY request with a "Subscription-State" of "terminated"
    and there are no other subscriptions alive in this dialog.
*/

#include "ServiceMemory.h"
#include "SipPrivate.h"
#include "SipDebug.h"
#include "SipClientConnection.h"
#include "SipDialog.h"

__IMS_TRACE_TAG_SIP__;

PUBLIC
SipDialog::SipDialog(IN SipDialogEx* pDialogEx_) :
        pDialogEx(pDialogEx_)
{
}

PUBLIC
SipDialog::SipDialog(IN CONST SipDialog& objRHS) :
        pDialogEx(objRHS.pDialogEx)
{
}

PUBLIC
SipDialog::~SipDialog()
{
    IMS_TRACE_D("Destructor :: SipDialog (%s)",
            SipDebug::GetCharA1(pDialogEx->GetDialogState()->GetCallId().GetStr(), 8, '@'), 0, 0);
}

PUBLIC
SipDialog& SipDialog::operator=(IN CONST SipDialog& objRHS)
{
    //---------------------------------------------------------------------------------------------

    if (this != &objRHS)
    {
        pDialogEx = objRHS.pDialogEx;
    }

    return (*this);
}

/*
 Returns a new SipClientConnection in this dialog. The returned SipClientConnection will be
in STATE_INITIALIZED state. The object is initialized with the given method and default headers.

Remarks
 The following headers will be set by the method:
    To
    From
    CSeq
    Call-ID
    Via
    Route
    Contact
    Max-Forwards
*/
PUBLIC
SipClientConnection* SipDialog::CreateClientConnection(IN CONST AString& strMethod)
{
    IMS_SINT32 nState = GetState();

    //---------------------------------------------------------------------------------------------

    if ((nState != STATE_EARLY) && (nState != STATE_CONFIRMED))
    {
        // BYE_REQUEST_ON_DIALOG_TERMINATED
        if (strMethod.Equals(SipMethod::ToName(SipMethod::BYE)))
        {
            IMS_TRACE_D("BYE ignores the dialog state(TERMINATED)", 0, 0, 0);
        }
        else
        {
            SipPrivate::SetLastError(SipError::INVALID_STATE);
            return IMS_NULL;
        }
    }

    if (strMethod.IsNULL())
    {
        SipPrivate::SetLastError(SipError::ILLEGAL_ARGUMENT);
        return IMS_NULL;
    }

    if (strMethod.IsEmpty())
    {
        SipPrivate::SetLastError(SipError::ILLEGAL_ARGUMENT);
        return IMS_NULL;
    }

    SipMethod objMethod(strMethod);

    // Check the method validity for the current dialog
    if (!CheckMethodValidity(objMethod))
    {
        SipPrivate::SetLastError(SipError::INVALID_OPERATION);
        return IMS_NULL;
    }

    // Get or create a proper dialog
    RCPtr<SipDialogEx> pNewDialogEx = GetOptimumDialog(objMethod);

    if (pNewDialogEx.IsNull())
    {
        SipPrivate::SetLastError(SipError::NO_MEMORY);
        return IMS_NULL;
    }

    SipClientConnection* pSCC = new SipClientConnection();

    if (pSCC == IMS_NULL)
    {
        SipPrivate::SetLastError(SipError::NO_MEMORY);
        return IMS_NULL;
    }

    if (pSCC->InitDialogRequest(objMethod, pNewDialogEx.Get()) != IMS_SUCCESS)
    {
        delete pSCC;

        SipPrivate::SetLastError(SipError::TRANSACTION_UNAVAILABLE);
        return IMS_NULL;
    }

    return pSCC;
}

/*
 Returns the Call-ID component of this dialog.

Remarks

*/
PUBLIC
const AString& SipDialog::GetCallId() const
{
    //---------------------------------------------------------------------------------------------

    return pDialogEx->GetDialogState()->GetCallId();
}

/*
 Returns the local contact address of this dialog.

Remarks

*/
PUBLIC
const ISipHeader* SipDialog::GetContactHeader() const
{
    //---------------------------------------------------------------------------------------------

    return pDialogEx->GetDialogState()->GetContactHeader();
}

/*
 Returns the local-tag component of this dialog.

Remarks

*/
PUBLIC
AString SipDialog::GetLocalTag() const
{
    //---------------------------------------------------------------------------------------------

    return pDialogEx->GetDialogState()->GetLocalTag();
}

/*
 Returns the remote-tag component of this dialog.

Remarks

*/
PUBLIC
AString SipDialog::GetRemoteTag() const
{
    //---------------------------------------------------------------------------------------------

    return pDialogEx->GetDialogState()->GetRemoteTag();
}

/*
 Returns the state of the SIP dialog.

Remarks
- STATE_INITIALIZED
    Internal state where the dialog has been created.
    This state is not visible to the user, since the dialog can be fetched earliest
    in the STATE_EARLY state.
 - STATE_EARLY
    Provisional 101 ~ 199 response received or sent (with to-tag).
 - STATE_CONFIRMED
    Final 2xx response received (or sent) for the original request.
    NOTIFY confirming the subscription received (or sent).
 - STATE_TERMINATED
    No response or error response (3xx ~ 6xx) received (or sent).
    If the dialog is terminated with BYE or un-SUBSCRIBE, GetNewClientConnection(...) method
    can't be called in this state.

*/
PUBLIC
IMS_SINT32 SipDialog::GetState() const
{
    //---------------------------------------------------------------------------------------------

    // Check if the dialog usage is already in TERMINATED state
    if (pDialogEx->IsDialogTerminated())
        return STATE_TERMINATED;

    switch (pDialogEx->GetState())
    {
        case SipDState::STATE_TERMINATED:
            return STATE_TERMINATED;

        case SipDState::STATE_EARLY:
            return STATE_EARLY;

        case SipDState::STATE_CONFIRMED:
            return STATE_CONFIRMED;

        default:
            return STATE_INIT;
    }
}

/*
 Compares if the given SipDialog equals or not.

Remarks

*/
PUBLIC
IMS_BOOL SipDialog::IsSameDialog(IN CONST SipDialog* pDialog)
{
    //---------------------------------------------------------------------------------------------

    if (pDialog == IMS_NULL)
        return IMS_FALSE;

    SipDialogState* pDState = pDialogEx->GetDialogState();
    SipDialogState* pOtherDState = pDialog->pDialogEx->GetDialogState();

    if (!pDState->Equals(pOtherDState))
    {
        return IMS_FALSE;
    }

    // Only check a dialog usage in this time ...
    return pDialogEx->Equals(pDialog->pDialogEx.Get());
}

/*

Remarks
 CONTACT_HEADER_PARAMETER_CONTROL_FOR_MID_DIALOG_REQUEST

*/
PUBLIC
IMS_RESULT SipDialog::SetContactParameter(
        IN CONST AString& strParameter, IN IMS_SINT32 nOperation /* = 0 (0: ADD, 1: REMOVE) */)
{
    //---------------------------------------------------------------------------------------------

    return pDialogEx->GetDialogState()->SetContactParameter(strParameter, nOperation);
}

/*

Remarks

*/
PUBLIC
void SipDialog::TerminateDialogUsage()
{
    pDialogEx->TerminateDialogUsage();
}

/*

Remarks

*/
PRIVATE
void SipDialog::UpdateDialog(IN SipDialogEx* pDialogEx)
{
    //---------------------------------------------------------------------------------------------

    this->pDialogEx = pDialogEx;
}

/*

Remarks

*/
PRIVATE
IMS_BOOL SipDialog::CheckMethodValidity(IN CONST SipMethod& objMethod) const
{
    //---------------------------------------------------------------------------------------------

    // Case 1)
    //    Condition - subscribe usage & method except for INVITE/SUBSCRIBE/REFER/NOTIFY
    if (!(pDialogEx->IsInviteUsage()) && !objMethod.Equals(SipMethod::INVITE) &&
            !objMethod.Equals(SipMethod::SUBSCRIBE) && !objMethod.Equals(SipMethod::REFER) &&
            !objMethod.Equals(SipMethod::NOTIFY))
    {
        IMS_TRACE_D(
                "%s is not allowed in the subscribe usage", objMethod.ToString().GetStr(), 0, 0);
        return IMS_FALSE;
    }
    // Case 2)
    //    Condition - invite usage & NOTIFY method
    else if (pDialogEx->IsInviteUsage() && objMethod.Equals(SipMethod::NOTIFY))
    {
        IMS_TRACE_D("%s is not allowed in the invite usage", objMethod.ToString().GetStr(), 0, 0);
        return IMS_FALSE;
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PRIVATE
SipDialogEx* SipDialog::GetOptimumDialog(IN CONST SipMethod& objMethod) const
{
    //---------------------------------------------------------------------------------------------

    // Case 1)
    //    Condition - invite usage & method except for REFER/SUBSCRIBE
    //    Action - Use an invite usage as it is
    if (pDialogEx->IsInviteUsage() && !objMethod.Equals(SipMethod::SUBSCRIBE) &&
            !objMethod.Equals(SipMethod::REFER))
    {
        return pDialogEx.Get();
    }
    // Case 2)
    //    Condition - subscribe usage & NOTIFY method
    //    Action - Use an subscribe usage as it is
    else if (!(pDialogEx->IsInviteUsage()) && objMethod.Equals(SipMethod::NOTIFY))
    {
        return pDialogEx.Get();
    }
    // Case 3)
    //    Condition - subscribe usage & INVITE method / invite usage & REFER/SUBSCRIBE method
    //    Action - Create an invite/subscribe usage & use it
    //
    // Other Cases
    //
    else
    {
        // After the request is sent, it will be updated by the client transaction
        return SipDialogEx::CreateDialog(pDialogEx->GetDialogState(), objMethod);
    }
}
