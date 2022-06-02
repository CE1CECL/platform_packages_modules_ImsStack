/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#ifndef _SIP_DATAGRAM_SOCKET_H_
#define _SIP_DATAGRAM_SOCKET_H_

#include "SipSocket.h"

class ISipDatagramSocketListener;

class SipDatagramSocket : public SipSocket
{
public:
    explicit SipDatagramSocket(IN IMS_SINT32 nSlotId);
    virtual ~SipDatagramSocket();

public:
    virtual IMS_BOOL Create(
            IN CONST IPAddress& objIPA, IN IMS_UINT32 nPort = 0, IN IMS_BOOL bSecure = IMS_FALSE);
    virtual IMS_SINT32 Send(IN CONST IMS_BYTE* pBuffer, IN IMS_SINT32 nBuffLen,
            IN IMS_UINT32 nPort = 0, IN CONST IPAddress& objIPA = IPAddress::NONE);
    void SetListener(IN ISipDatagramSocketListener* piListener);

protected:
    // ISocketListener interface
    virtual void Socket_OnDataReceived(IN ISocket* piSocket);
    virtual void Socket_OnSendEnabled(IN ISocket* piSocket);
    virtual void Socket_OnClosed(
            IN ISocket* piSocket, IN IMS_SINT32 nReason = ISocket::CLOSE_REASON_UNKNOWN);

private:
    ISipDatagramSocketListener* piListener;
};

#endif  // _SIP_DATAGRAM_SOCKET_H_
