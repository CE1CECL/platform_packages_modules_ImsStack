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
#include "private/ConfigurationManager.h"
#include "private/SipConfig.h"
#include "SipPrivate.h"
#include "SipFeatures.h"
#include "SipConfigProxy.h"
#include "SipDebug.h"
#include "SipServerTransactionState.h"
#include "SipServerConnection.h"

__IMS_TRACE_TAG_SIP__;

PUBLIC
SipServerConnection::SipServerConnection(IN SipServerTransactionState* pSTState_) :
        SipConnection(),
        nState(STATE_CREATED),
        pSTState(pSTState_)
{
    pSTState->SetTransactionListener(this);
    pSTState->SetTransportListener(this);
}

PUBLIC VIRTUAL SipServerConnection::~SipServerConnection()
{
    if ((nState == STATE_REQUEST_RECEIVED) || (nState == STATE_INITIALIZED) ||
            (nState == STATE_PROVISIONAL_RESPONDED))
    {
        pSTState->Abort();
    }

    pSTState->SetTransactionListener(IMS_NULL);
    pSTState->SetTransportListener(IMS_NULL);
}

/*

Remarks

*/
PUBLIC VIRTUAL void SipServerConnection::Close()
{
    //---------------------------------------------------------------------------------------------

    if ((nState == STATE_REQUEST_RECEIVED) || (nState == STATE_INITIALIZED) ||
            (nState == STATE_PROVISIONAL_RESPONDED))
    {
        pSTState->Abort();
    }
    else if ((nState == STATE_COMPLETED) && pMessage != IMS_NULL &&
            pMessage->GetMethod().Equals(SipMethod::INVITE) &&
            SipStatusCode::IsFinalSuccess(pMessage->GetStatusCode()))
    {
        // Terminate INVITE server transaction promptly
        pSTState->Terminate();
    }

    SetState(STATE_TERMINATED);

    SipConnection::Close();
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_RESULT SipServerConnection::AddHeader(
        IN CONST AString& strName, IN CONST AString& strValue)
{
    //---------------------------------------------------------------------------------------------

    if (nState != STATE_INITIALIZED)
    {
        SipPrivate::SetLastError(SipError::INVALID_STATE);
        return IMS_FAILURE;
    }

    return SipConnection::AddHeader(strName, strValue);
}

/*

Remarks

*/
PUBLIC VIRTUAL AString SipServerConnection::GetHeader(
        IN CONST AString& strName, IN IMS_SINT32 nIndex /* = 0 */)
{
    //---------------------------------------------------------------------------------------------

    // Message is not initialized or the connection is closed
    if (nState == STATE_TERMINATED)
    {
        SipPrivate::SetLastError(SipError::NO_ERROR);
        return AString::ConstNull();
    }

    return SipConnection::GetHeader(strName, nIndex);
}

/*

Remarks

*/
PUBLIC VIRTUAL IMSList<AString> SipServerConnection::GetHeaders(IN CONST AString& strName)
{
    //---------------------------------------------------------------------------------------------

    // Message is not initialized or the connection is closed
    if (nState == STATE_TERMINATED)
    {
        SipPrivate::SetLastError(SipError::NO_ERROR);
        return IMSList<AString>();
    }

    return SipConnection::GetHeaders(strName);
}

/*

Remarks

*/
PUBLIC VIRTUAL const SipMethod& SipServerConnection::GetMethod() const
{
    //---------------------------------------------------------------------------------------------

    // Message is not initialized or the connection is closed
    if (nState == STATE_TERMINATED)
    {
        return SipMethod::INVALID_METHOD;
    }

    return SipConnection::GetMethod();
}

/*

Remarks

*/
PUBLIC VIRTUAL const AString& SipServerConnection::GetReasonPhrase() const
{
    //---------------------------------------------------------------------------------------------

    // Status code is available if the state is in PROCEEDING, UNAUTHORIZED, and COMPLETED
    // Sync. problem : add the state checking (PROVISIONAL_RESPONDED, COMPLETED)
    if ((nState != STATE_INITIALIZED) && (nState != STATE_PROVISIONAL_RESPONDED) &&
            (nState != STATE_COMPLETED))
    {
        return AString::ConstNull();
    }

    return SipConnection::GetReasonPhrase();
}

/*

Remarks

*/
PUBLIC VIRTUAL const AString& SipServerConnection::GetRequestUri() const
{
    //---------------------------------------------------------------------------------------------

    // Message is not initialized or the connection is closed
    if (nState == STATE_TERMINATED)
    {
        return AString::ConstNull();
    }

    return SipConnection::GetRequestUri();
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_SINT32 SipServerConnection::GetStatusCode() const
{
    //---------------------------------------------------------------------------------------------

    // Status code is available if the state is in PROCEEDING, UNAUTHORIZED, and COMPLETED
    // Sync. problem : add the state checking (PROVISIONAL_RESPONDED, COMPLETED)
    if ((nState != STATE_INITIALIZED) && (nState != STATE_PROVISIONAL_RESPONDED) &&
            (nState != STATE_COMPLETED))
    {
        return 0;
    }

    return SipConnection::GetStatusCode();
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_RESULT SipServerConnection::RemoveHeader(IN CONST AString& strName)
{
    //---------------------------------------------------------------------------------------------

    if (nState != STATE_INITIALIZED)
    {
        SipPrivate::SetLastError(SipError::INVALID_STATE);
        return IMS_FAILURE;
    }

    return SipConnection::RemoveHeader(strName);
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_RESULT SipServerConnection::Send()
{
    //---------------------------------------------------------------------------------------------

    if ((nState != STATE_INITIALIZED) && (nState != STATE_PROVISIONAL_RESPONDED) &&
            (nState != STATE_COMPLETED))
    {
        SipPrivate::SetLastError(SipError::INVALID_STATE);
        return IMS_FAILURE;
    }

    IMS_SINT32 nStatusCode = SipConnection::GetStatusCode();

    if ((nState != STATE_INITIALIZED) && (SipConnection::GetMethod().Equals(SipMethod::INVITE)))
    {
        if (!SipStatusCode::IsFinalSuccess(nStatusCode) &&
                !(SipStatusCode::IsProvisional(nStatusCode) && pMessage->IsMessageRpr()))
        {
            SipPrivate::SetLastError(SipError::INVALID_OPERATION);
            return IMS_FAILURE;
        }

        IMS_TRACE_I("Retransmission of %d to INVITE request is requested .....", nStatusCode, 0, 0);

        if (!pMessage->FormMessageOnRetransmission())
        {
            SipPrivate::SetLastError(SipError::INVALID_MESSAGE);
            IMS_TRACE_E(0, "Forming SIP message for retransmission failed", 0, 0, 0);
            return IMS_FAILURE;
        }

        // Retransmission of a provisional (reliable) or final response to INVITE request
        return pSTState->RetransmitMessage();
    }
    else if (nState == STATE_COMPLETED)
    {
        IMS_TRACE_E(0, "Sending a SIP response in STATE_COMPLETED is not allowed", 0, 0, 0);
        return IMS_FAILURE;
    }

    // Throw exception - INVALID_MESSAGE if the message format was invalid
    if (!pMessage->FormMessage())
    {
        SipPrivate::SetLastError(SipError::INVALID_MESSAGE);
        return IMS_FAILURE;
    }

    if (nStatusCode == SipStatusCode::SC_100)
    {
        // Remove to-tag parameter
    }

    if (!pSTState->FormMessage())
    {
        IMS_TRACE_E(0, "FormMessage() failed", 0, 0, 0);
        return IMS_FAILURE;
    }

    if (SipFeatures::IsStandard2XXRetransmissionIntervalRequired(GetSlotId()))
    {
        AdjustTimerHFor2XX();
    }

    if (!pSTState->Send(GetTransactionTimerValues()))
    {
        IMS_TRACE_E(0, "Send() failed", 0, 0, 0);

        if (SipStatusCode::IsProvisional(nStatusCode))
        {
            // To send a final response, transit the state.
            SetState(STATE_PROVISIONAL_RESPONDED);
        }

        return IMS_FAILURE;
    }

    // Update the state
    if (SipStatusCode::IsProvisional(nStatusCode))
    {
        SetState(STATE_PROVISIONAL_RESPONDED);
    }
    else
    {
        SetState(STATE_COMPLETED);
    }

    SipPrivate::SetLastError(SipError::NO_ERROR);

    //// DEBUG
    SipDebug::Send(GetSlotId(), SipDebug::MSG_RSP, SipDebug::DIR_OUT,
            SipConnection::GetMethod().ToInt(), nStatusCode);

    return IMS_SUCCESS;
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_RESULT SipServerConnection::SetHeader(
        IN CONST AString& strName, IN CONST AString& strValue)
{
    //---------------------------------------------------------------------------------------------

    if (nState != STATE_INITIALIZED)
    {
        SipPrivate::SetLastError(SipError::INVALID_STATE);
        return IMS_FAILURE;
    }

    return SipConnection::SetHeader(strName, strValue);
}

/*

Remarks

*/
PUBLIC VIRTUAL const ByteArray& SipServerConnection::GetContent() const
{
    //---------------------------------------------------------------------------------------------

    if (nState != STATE_REQUEST_RECEIVED)
    {
        SipPrivate::SetLastError(SipError::INVALID_STATE);
        return ByteArray::ConstNull();
    }

    return SipConnection::GetContent();
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_RESULT SipServerConnection::SetContent(IN CONST ByteArray& objContent)
{
    //---------------------------------------------------------------------------------------------

    if (nState != STATE_INITIALIZED)
    {
        SipPrivate::SetLastError(SipError::INVALID_STATE);
        return IMS_FAILURE;
    }

    return SipConnection::SetContent(objContent);
}

// IMS extensions
/*

Remarks

*/
PUBLIC VIRTUAL IMS_SINT32 SipServerConnection::GetHeaderCount(IN CONST AString& strName) const
{
    //---------------------------------------------------------------------------------------------

    // Message is not initialized or the connection is closed
    if (nState == STATE_TERMINATED)
    {
        return 0;
    }

    return SipConnection::GetHeaderCount(strName);
}

/*

Remarks
 MULTI_REG_SIP_PROFILE
*/
PUBLIC VIRTUAL void SipServerConnection::SetSipProfile(IN SipProfile* pProfile)
{
    //---------------------------------------------------------------------------------------------

    if (!pSTState.IsNull())
    {
        pSTState->SetSipProfile(pProfile);
    }
}

/*

Remarks

*/
PUBLIC
IMS_RESULT SipServerConnection::InitResponse(IN IMS_SINT32 nStatusCode)
{
    //---------------------------------------------------------------------------------------------

    if ((nState != STATE_REQUEST_RECEIVED) && (nState != STATE_PROVISIONAL_RESPONDED))
    {
        SipPrivate::SetLastError(SipError::INVALID_STATE);
        return IMS_FAILURE;
    }

    if ((nStatusCode < SipStatusCode::SC_100) || (nStatusCode > SipStatusCode::SC_699))
    {
        SipPrivate::SetLastError(SipError::ILLEGAL_ARGUMENT);
        return IMS_FAILURE;
    }

    SipMethod objMethod = SipConnection::GetMethod();

    // If MESSAGE & 200 OK is sent by the SIP core, then throw ALREADY_RESPONDED

    InitMessage(IMS_NULL, sipcore::SipMessage::TYPE_RESPONSE);

    pMessage->SetMethod(objMethod);
    pMessage->SetStatusCode(nStatusCode);
    pMessage->SetReasonPhrase(SipStatusCode::GetReasonPhrase(nStatusCode));

    pSTState->UpdateMessage(pMessage->GetMessage());

    if (!pSTState->InitResponse(nStatusCode))
    {
        SipPrivate::SetLastError(SipError::GENERAL_ERROR);
        return IMS_FAILURE;
    }

    SetState(STATE_INITIALIZED);

    SipPrivate::SetLastError(SipError::NO_ERROR);

    return IMS_SUCCESS;
}

/*

Remarks

*/
PUBLIC
IMS_RESULT SipServerConnection::SetReasonPhrase(IN CONST AString& strReasonPhrase)
{
    //---------------------------------------------------------------------------------------------

    if (nState != STATE_INITIALIZED)
    {
        SipPrivate::SetLastError(SipError::INVALID_STATE);
        return IMS_FAILURE;
    }

    if (strReasonPhrase.GetLength() == 0)
        pMessage->SetReasonPhrase(AString::ConstEmpty());
    else
        pMessage->SetReasonPhrase(strReasonPhrase);

    return IMS_SUCCESS;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipServerConnection::IsSameTransaction(IN CONST SipServerConnection* pOngoingSSC) const
{
    //---------------------------------------------------------------------------------------------

    if (nState == STATE_TERMINATED)
    {
        SipPrivate::SetLastError(SipError::INVALID_STATE);
        return IMS_FALSE;
    }

    const SipMethod& objMethod = SipConnection::GetMethod();

    // Check if the method is CANCEL
    if (!objMethod.Equals(SipMethod::CANCEL))
    {
        SipPrivate::SetLastError(SipError::INVALID_OPERATION);
        return IMS_FALSE;
    }

    if (pOngoingSSC == IMS_NULL)
    {
        SipPrivate::SetLastError(SipError::ILLEGAL_ARGUMENT);
        return IMS_FALSE;
    }

    if ((pOngoingSSC->nState == STATE_COMPLETED) || (pOngoingSSC->nState == STATE_TERMINATED))
    {
        // Ignore the CANCEL request because the ongoing transaction is already completed or
        // terminated.
        IMS_TRACE_D("Server transaction is in %s state",
                (pOngoingSSC->nState == STATE_COMPLETED) ? "COMPLETED" : "TERMINATED", 0, 0);

        // CANCEL_HANDLING_AFTER_200_OK_TO_INVITE
        // When INVITE_TXN_HANDLING_CORRECTION is disabled,
        // returns IMS_FALSE with SipError#NO_ERROR.
    }

    return pSTState->IsSameTransaction(pOngoingSSC->pSTState.Get());
}

/*

Remarks

*/
PUBLIC
IMS_RESULT SipServerConnection::InitRequest()
{
    //---------------------------------------------------------------------------------------------

    pMessage = new sipcore::SipMessage(pSTState->GetMessage());

    if (pMessage == IMS_NULL)
    {
        SipPrivate::SetLastError(SipError::NO_MEMORY);
        return IMS_FAILURE;
    }

    SipDialogEx* pDialogEx = pSTState->GetDialog();
    IMS_SINT32 nState = pDialogEx->GetState();

    // Case 1) If the request is received inside of an existing dialog (it is not in INIT state)
    // Case 2) If the dialog is in INIT state & the request can create a dialog
    // Case 3) If the dialog (created by SUBSCRIBE)  is in INIT state & NOTIFY request is received
    //       or NOTIFY request to forked SUBSCRIBE request
    if ((nState != SipDState::STATE_INIT) ||
            ((nState == SipDState::STATE_INIT) &&
                    (SipDialogBase::IsDialogCreatable(pMessage->GetMethod()) ||
                            pMessage->GetMethod().Equals(SipMethod::NOTIFY))))
    {
        pDialog = new SipDialog(pDialogEx);

        if (pDialog == IMS_NULL)
        {
            SipPrivate::SetLastError(SipError::NO_MEMORY);
            return IMS_FAILURE;
        }
    }

    if (SipConnection::GetMethod().Equals(SipMethod::ACK))
    {
        SetState(STATE_COMPLETED);
    }
    else
    {
        SetState(STATE_REQUEST_RECEIVED);
    }

    SipPrivate::SetLastError(SipError::NO_ERROR);

    //// DEBUG
    SipDebug::Send(
            GetSlotId(), SipDebug::MSG_REQ, SipDebug::DIR_IN, SipConnection::GetMethod().ToInt());

    return IMS_SUCCESS;
}

/*

Remarks

*/
PRIVATE
void SipServerConnection::AdjustTimerHFor2XX()
{
    const SipMethod& objMethod = SipConnection::GetMethod();
    IMS_SINT32 nStatusCode = SipConnection::GetStatusCode();

    if (objMethod.Equals(SipMethod::INVITE) && SipStatusCode::IsFinalSuccess(nStatusCode))
    {
        SipTimerValues* pTV = GetTransactionTimerValues();

        if (pTV != IMS_NULL)
        {
            IMS_SINT32 nT1 = pTV->GetValue(SipTimerValues::TIMER_T1);

            if (nT1 == 0)
            {
                const SipConfig* pSipConfig =
                        ConfigurationManager::GetInstance()->GetSipConfig(GetSlotId());

                nT1 = SipConfigProxy::GetTimerValueT1(
                        GetSlotId(), pSTState->GetSipProfile(), pSipConfig->GetSipConfigV());

                if (nT1 == 0)
                {
                    // 2s
                    nT1 = 2000;
                }
            }

            IMS_TRACE_D("TimerH(2XX) is adjusted - %d >> %d",
                    pTV->GetValue(SipTimerValues::TIMER_H), (nT1 * 64), 0);

            pTV->SetValue(SipTimerValues::TIMER_H, (nT1 * 64));
        }
    }
}

/*

Remarks

*/
PRIVATE
void SipServerConnection::SetState(IN IMS_SINT32 nState)
{
    //---------------------------------------------------------------------------------------------

    IMS_TRACE_I("SSC :: %s to %s", StateToString(this->nState), StateToString(nState), 0);

    this->nState = nState;
}

/*

Remarks

*/
PRIVATE GLOBAL const IMS_CHAR* SipServerConnection::StateToString(IN IMS_SINT32 nState)
{
    //---------------------------------------------------------------------------------------------

    switch (nState)
    {
        case STATE_CREATED:
            return "STATE_CREATED";
        case STATE_REQUEST_RECEIVED:
            return "STATE_REQUEST_RECEIVED";
        case STATE_PROVISIONAL_RESPONDED:
            return "STATE_PROVISIONAL_RESPONDED";
        case STATE_INITIALIZED:
            return "STATE_INITIALIZED";
        case STATE_COMPLETED:
            return "STATE_COMPLETED";
        case STATE_TERMINATED:
            return "STATE_TERMINATED";
        default:
            return "__INVALID__";
    }
}
