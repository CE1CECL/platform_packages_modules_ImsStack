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
#include "ServiceNetwork.h"
#include "ByteArray.h"
#include "SipPrivate.h"
#include "SipDebug.h"
#include "SipRtConfigUtils.h"
#include "ISipStreamSocketListener.h"
#include "SipStreamSocket.h"
#include "SipStreamSocketNotifier.h"

__IMS_TRACE_TAG_SIP__;

PUBLIC
SipStreamSocketNotifier::SipStreamSocketNotifier(IN IMS_SINT32 nSlotId) :
        SipSocket(nSlotId, SipSocketAddress::SOCKET_TCP),
        piListener(IMS_NULL)
{
}

PUBLIC VIRTUAL SipStreamSocketNotifier::~SipStreamSocketNotifier()
{
    IMS_TRACE_D("StreamSocketNotifier(D) :: (%s, %d)", SipDebug::GetIp(objSA.GetIpAddress()),
            objSA.GetPort(), 0);
}

/*

Remarks

*/
PUBLIC VIRTUAL SipSocket* SipStreamSocketNotifier::Accept()
{
    //---------------------------------------------------------------------------------------------

    if (GetState() != STATE_CONNECTED)
    {
        return IMS_NULL;
    }

    ISocket* piNewSocket = piSocket->Accept();

    if (piNewSocket != IMS_NULL)
    {
        IMS_TRACE_I("StreamSocket will be created by remote end", 0, 0, 0);

        SipStreamSocket* pStreamSocket = new SipStreamSocket(GetSlotId(), piNewSocket);

        if (pStreamSocket == IMS_NULL)
        {
            piNewSocket->SetListener(IMS_NULL);
            piNewSocket->Close();
            NetworkService::GetNetworkService()->DestroySocket(piNewSocket);
            return IMS_NULL;
        }

        ApplyIpSec(piNewSocket);

        // Inherits the server's listener
        pStreamSocket->SetListener(piListener);

        return pStreamSocket;
    }

    return IMS_NULL;
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_BOOL SipStreamSocketNotifier::Create(IN CONST IPAddress& objIPA,
        IN IMS_UINT32 nPort /* = 0 */, IN IMS_BOOL bSecure /* = IMS_FALSE */)
{
    //---------------------------------------------------------------------------------------------

    if (!SipSocket::Create(objIPA, nPort, bSecure))
    {
        return IMS_FALSE;
    }

    if (GetState() != STATE_INITIALIZED)
    {
        return IMS_FALSE;
    }

    if (piSocket->Bind(objIPA, nPort) == ISocket::RESULT_ERROR)
    {
        return IMS_FALSE;
    }

    if (piSocket->Listen() == ISocket::RESULT_ERROR)
    {
        return IMS_FALSE;
    }

    objSA.SetPort(nPort);
    objSA.SetIpAddress(objIPA);

    SetState(STATE_CONNECTED);

    IMS_TRACE_I("StreamSocketNotifier(C) :: (%s, %d)",
            SipRtConfigUtils::IsRoutingInfoHiddenInLog(GetSlotId()) ? "xxx"
                                                                    : SipDebug::GetIp(objIPA),
            nPort, 0);

    ApplyIpSec();

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
void SipStreamSocketNotifier::SetListener(IN ISipStreamSocketListener* piListener)
{
    //---------------------------------------------------------------------------------------------

    this->piListener = piListener;
}

/*

Remarks

*/
PROTECTED VIRTUAL void SipStreamSocketNotifier::Socket_OnConnectionReceived(IN ISocket* piSocket)
{
    //---------------------------------------------------------------------------------------------

    SipSocket::Socket_OnConnectionReceived(piSocket);

    if (piListener != IMS_NULL)
    {
        piListener->StreamSocket_ConnectionReceived(this);

        // Accept & Close : A new connection MUST be extracted from the pending connection queue.
    }
}

/*

Remarks

*/
PROTECTED VIRTUAL void SipStreamSocketNotifier::Socket_OnClosed(
        IN ISocket* piSocket, IN IMS_SINT32 nReason /* = ISocket::CLOSE_REASON_UNKNOWN */)
{
    //---------------------------------------------------------------------------------------------

    SipSocket::Socket_OnClosed(piSocket, nReason);
}
