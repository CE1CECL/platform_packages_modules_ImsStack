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
#include "ByteArray.h"
#include "ISipDatagramSocketListener.h"
#include "SipPrivate.h"
#include "SipDebug.h"
#include "SipRtConfigUtils.h"
#include "SipMessageBuffer.h"
#include "SipDatagramSocket.h"

__IMS_TRACE_TAG_SIP__;

PUBLIC
SipDatagramSocket::SipDatagramSocket(IN IMS_SINT32 nSlotId) :
        SipSocket(nSlotId),
        piListener(IMS_NULL)
{
}

PUBLIC VIRTUAL SipDatagramSocket::~SipDatagramSocket()
{
    IMS_TRACE_D("DatagramSocket(D) :: (%s, %d)", SipDebug::GetIp(objSA.GetIpAddress()),
            objSA.GetPort(), 0);
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_BOOL SipDatagramSocket::Create(IN CONST IPAddress& objIPA,
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

    objSA.SetPort(nPort);
    objSA.SetIpAddress(objIPA);

    SetState(STATE_CONNECTING);

    IMS_TRACE_I("DatagramSocket(C) ::(%s , %d)",
            SipRtConfigUtils::IsRoutingInfoHiddenInLog(GetSlotId()) ? "xxx"
                                                                    : SipDebug::GetIp(objIPA),
            nPort, 0);

    ApplyIpSec();

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_SINT32 SipDatagramSocket::Send(IN CONST IMS_BYTE* pBuffer,
        IN IMS_SINT32 nBuffLen, IN IMS_UINT32 nPort /* = 0 */,
        IN CONST IPAddress& objIPA /* = IPAddress::NONE */)
{
    //---------------------------------------------------------------------------------------------

    if ((GetState() != STATE_CONNECTED) && (GetState() != STATE_CONNECTING))
    {
        return ISocket::RESULT_ERROR;
    }

    return piSocket->SendTo(pBuffer, nBuffLen, objIPA, nPort);
}

/*

Remarks

*/
PUBLIC
void SipDatagramSocket::SetListener(IN ISipDatagramSocketListener* piListener)
{
    //---------------------------------------------------------------------------------------------

    this->piListener = piListener;
}

/*

Remarks

*/
PROTECTED VIRTUAL void SipDatagramSocket::Socket_OnDataReceived(IN ISocket* piSocket)
{
    IMS_SINT32 nReadBytes;
    IMS_UINT32 nPort = 0;
    IPAddress objIPA;

    RCPtr<SipMessageBuffer> pMessageBuffer = SipMessageBuffer::GetInstance();
    IMS_BYTE* pRecvBuffer = pMessageBuffer->GetBuffer(GetSlotId());

    //---------------------------------------------------------------------------------------------

    IMS_MEM_Memset(pRecvBuffer, 0x00, pMessageBuffer->GetLength());

    nReadBytes = piSocket->ReceiveFrom(pRecvBuffer, pMessageBuffer->GetLength(), objIPA, nPort);

    if (nReadBytes > 0)
    {
        // Ignore CR LF
        if ((nReadBytes == 2) && (pRecvBuffer[0] == 0x0D) && (pRecvBuffer[1] == 0x0A))
        {
            IMS_TRACE_I("CRLF is ignored by SIP transport layer", 0, 0, 0);
            return;
        }

        pRecvBuffer[nReadBytes] = '\0';

        ByteArray objRecvBuffer(pRecvBuffer, nReadBytes);

        if (piListener != IMS_NULL)
        {
            piListener->DatagramSocket_DataReceived(this, objRecvBuffer, objIPA, nPort);
        }
    }

    SipSocket::Socket_OnDataReceived(piSocket);
}

/*

Remarks

*/
PROTECTED VIRTUAL void SipDatagramSocket::Socket_OnSendEnabled(IN ISocket* piSocket)
{
    //---------------------------------------------------------------------------------------------

    SetState(STATE_CONNECTED);

    SipSocket::Socket_OnSendEnabled(piSocket);
}

/*

Remarks

*/
PROTECTED VIRTUAL void SipDatagramSocket::Socket_OnClosed(
        IN ISocket* piSocket, IN IMS_SINT32 nReason /* = ISocket::CLOSE_REASON_UNKNOWN */)
{
    //---------------------------------------------------------------------------------------------

    SipSocket::Socket_OnClosed(piSocket, nReason);
}
