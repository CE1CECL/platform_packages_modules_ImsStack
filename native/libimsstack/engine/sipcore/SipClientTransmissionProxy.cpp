/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20160215  hwangoo.park@             Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "ServiceTrace.h"
#include "ISipClientTransmissionListener.h"
#include "SipConfigProxy.h"
#include "SipFactoryProxy.h"
#include "SipTransportHelper.h"
#include "SipTransport.h"
#include "SipClientTransactionState.h"
#include "SipClientTransmissionProxy.h"

__IMS_TRACE_TAG_SIP__;

PUBLIC
SipClientTransmissionProxy::SipClientTransmissionProxy() :
        EngineActivity(),
        pTV(IMS_NULL),
        pCTState(IMS_NULL),
        piListener(IMS_NULL),
        bIsResubmittedRequest(IMS_FALSE),
        pSocket(IMS_NULL)
{
}

PUBLIC VIRTUAL SipClientTransmissionProxy::~SipClientTransmissionProxy()
{
    DestroyStreamSocket();
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_BOOL SipClientTransmissionProxy::DispatchMessage(IN IMSMSG& objMSG)
{
    switch (objMSG.GetName())
    {
        case AMSG_SEND_MESSAGE:
            SendPendingMessage();
            DestroyStreamSocket();
            break;

        case AMSG_NOTIFY_TRANSPORT_ERROR:
            NotifyTransportError(LONG_TO_INT(objMSG.nLparam));
            break;

        default:
            break;
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
void SipClientTransmissionProxy::Abort()
{
    //---------------------------------------------------------------------------------------------

    DestroyStreamSocket();
}

/*

Remarks

*/
PUBLIC
IMS_RESULT SipClientTransmissionProxy::Send()
{
    //---------------------------------------------------------------------------------------------

    bIsResubmittedRequest = IMS_FALSE;

    IMS_RESULT nResult = SendMessage();

    if (nResult != RESULT_PENDING)
    {
        DestroyStreamSocket();
    }

    return nResult;
}

/*

Remarks

*/
PUBLIC
IMS_RESULT SipClientTransmissionProxy::SendWithCredentials()
{
    //---------------------------------------------------------------------------------------------

    bIsResubmittedRequest = IMS_TRUE;

    IMS_RESULT nResult = SendMessage();

    if (nResult != RESULT_PENDING)
    {
        DestroyStreamSocket();
    }

    return nResult;
}

/*

Remarks

*/
PUBLIC
void SipClientTransmissionProxy::SetListener(IN ISipClientTransmissionListener* piListener)
{
    //---------------------------------------------------------------------------------------------

    this->piListener = piListener;
}

/*

Remarks

*/
PUBLIC
void SipClientTransmissionProxy::SetTimerValues(IN SipTimerValues* pTV)
{
    //---------------------------------------------------------------------------------------------

    this->pTV = pTV;
}

/*

Remarks

*/
PUBLIC
void SipClientTransmissionProxy::SetTransactionState(IN SipClientTransactionState* pCTState)
{
    //---------------------------------------------------------------------------------------------

    this->pCTState = pCTState;
}

/*

Remarks

*/
PRIVATE VIRTUAL void SipClientTransmissionProxy::Socket_NotifyError(
        IN SipSocket* pSocket, IN IMS_SINT32 nErrorCode)
{
    //---------------------------------------------------------------------------------------------

    if (this->pSocket == IMS_NULL)
    {
        return;
    }

    if (this->pSocket != pSocket)
    {
        return;
    }

    DestroyStreamSocket();

    PostMessage(AMSG_NOTIFY_TRANSPORT_ERROR, 0, nErrorCode);
}

/*

Remarks

*/
PRIVATE VIRTUAL void SipClientTransmissionProxy::Socket_SendEnabled(IN SipSocket* pSocket)
{
    //---------------------------------------------------------------------------------------------

    if (this->pSocket == IMS_NULL)
    {
        return;
    }

    if (this->pSocket != pSocket)
    {
        return;
    }

    PostMessage(AMSG_SEND_MESSAGE, 0, 0);
}

/*

Remarks

*/
PRIVATE
void SipClientTransmissionProxy::DestroyStreamSocket()
{
    //---------------------------------------------------------------------------------------------

    if (pSocket != IMS_NULL)
    {
        SipTransportHelper* pTransportHelper =
                SipFactoryProxy::GetInstance()->GetTransportHelper(GetSlotId());
        pTransportHelper->Destroy(pSocket, this);
        pSocket = IMS_NULL;
    }
}

/*

Remarks

*/
PRIVATE
IMS_BOOL SipClientTransmissionProxy::IsUDPFallbackRequired() const
{
    SipTransport* pTransport = (pCTState != IMS_NULL) ? pCTState->GetSipTransport() : IMS_NULL;

    //---------------------------------------------------------------------------------------------

    if (pTransport == IMS_NULL)
    {
        return IMS_FALSE;
    }

    IMS_SINT32 nProtocol = pTransport->GetProtocol(SipTransport::TA_FAR);

    return (!pTransport->IsTcpConnectionOnlyRequired() &&
            (nProtocol == SipTransportAddress::PROTOCOL_TCP));
}

/*

Remarks

*/
PRIVATE
IMS_BOOL SipClientTransmissionProxy::IsUDPFallbackSupported() const
{
    SipProfile* pProfile = (pCTState != IMS_NULL) ? pCTState->GetSipProfile() : IMS_NULL;

    //---------------------------------------------------------------------------------------------

    return SipConfigProxy::IsUdpFallbackConfigured(GetSlotId(), pProfile);
}

/*

Remarks

*/
PRIVATE
void SipClientTransmissionProxy::NotifyTransportError(IN IMS_SINT32 nErrorCode)
{
    //---------------------------------------------------------------------------------------------

    if ((nErrorCode == SipSocket::ERROR_CONNECTION_TIMEDOUT) ||
            (nErrorCode == SipSocket::ERROR_CONNECT_FAILED))
    {
        // Change transport protocol to UDP
        if (pCTState != IMS_NULL)
        {
            IMS_TRACE_D("TransmissionProxy :: UDP fallback", 0, 0, 0);
            pCTState->AdjustTransportProtocolAsUdp();
            SendPendingMessage();
            return;
        }
    }

    if (piListener != IMS_NULL)
    {
        AString strError =
                SipTransport::CreateSocketErrorMessage(nErrorCode, SipSocketAddress::SOCKET_NONE);

        piListener->ClientTransmission_NotifyError(SipError::TRANSPORT_ERROR, strError);
    }
}

/*

Remarks

*/
PRIVATE
IMS_RESULT SipClientTransmissionProxy::PrepareStreamSocket()
{
    //---------------------------------------------------------------------------------------------

    IMS_TRACE_D("TransmissionProxy :: Preparing a stream socket", 0, 0, 0);

    if (pSocket == IMS_NULL)
    {
        SipTransport* pTransport = (pCTState != IMS_NULL) ? pCTState->GetSipTransport() : IMS_NULL;

        if (pTransport == IMS_NULL)
        {
            return RESULT_NOK;
        }

        pSocket = pTransport->CreateTcpClientSocket();

        if (pSocket == IMS_NULL)
        {
            IMS_TRACE_E(0, "Creating a stream socket failed", 0, 0, 0);
            return RESULT_NOK;
        }

        if (!pSocket->Connect())
        {
            DestroyStreamSocket();

            IMS_TRACE_E(0, "Connecting a stream socket failed", 0, 0, 0);
            return RESULT_NOK;
        }

        pSocket->SetListener(this);
    }

    if (pSocket->GetState() == SipSocket::STATE_CONNECTED)
    {
        return RESULT_OK;
    }

    // Waits for TCP connection establishement.

    return RESULT_PENDING;
}

/*

Remarks

*/
PRIVATE
IMS_RESULT SipClientTransmissionProxy::SendMessage()
{
    //---------------------------------------------------------------------------------------------

    if (pCTState == IMS_NULL)
    {
        IMS_TRACE_E(0, "SipClientTransactionState is null", 0, 0, 0);
        return RESULT_NOK;
    }

    if (IsUDPFallbackSupported() && IsUDPFallbackRequired())
    {
        IMS_RESULT nResult = PrepareStreamSocket();

        if (nResult == RESULT_NOK)
        {
            IMS_TRACE_E(0, "Preparing a stream socket failed", 0, 0, 0);
            return RESULT_NOK;
        }
        else if (nResult == RESULT_PENDING)
        {
            IMS_TRACE_D("TransmissionProxy :: Waits for stream socket", 0, 0, 0);
            return RESULT_PENDING;
        }
    }

    if (bIsResubmittedRequest)
    {
        if (!pCTState->SendWithCredentials(pTV))
        {
            IMS_TRACE_E(0, "Send() failed", 0, 0, 0);
            return RESULT_NOK;
        }
    }
    else
    {
        if (!pCTState->Send(pTV))
        {
            IMS_TRACE_E(0, "Send() failed", 0, 0, 0);
            return RESULT_NOK;
        }
    }

    return RESULT_OK;
}

/*

Remarks

*/
PRIVATE
void SipClientTransmissionProxy::SendPendingMessage()
{
    IMS_RESULT nResult = RESULT_NOK;

    //---------------------------------------------------------------------------------------------

    if (pCTState == IMS_NULL)
    {
        IMS_TRACE_E(0, "SipClientTransactionState is null", 0, 0, 0);
        goto EXIT_SendPendingMessage;
    }

    IMS_TRACE_D("TransmissionProxy :: Sending a pending message", 0, 0, 0);

    if (bIsResubmittedRequest)
    {
        if (!pCTState->SendWithCredentials(pTV))
        {
            IMS_TRACE_E(0, "SendWithCredentials() failed", 0, 0, 0);
            goto EXIT_SendPendingMessage;
        }

        nResult = RESULT_OK;
    }
    else
    {
        if (!pCTState->Send(pTV))
        {
            IMS_TRACE_E(0, "Send() failed", 0, 0, 0);
            goto EXIT_SendPendingMessage;
        }

        nResult = RESULT_OK;
    }

EXIT_SendPendingMessage:

    if (piListener != IMS_NULL)
    {
        if (nResult == RESULT_OK)
        {
            piListener->ClientTransmission_TransmissionCompleted();
        }
        else
        {
            AString strError = "SIP message transmission failed";
            piListener->ClientTransmission_NotifyError(SipError::TRANSPORT_ERROR, strError);
        }
    }
}
