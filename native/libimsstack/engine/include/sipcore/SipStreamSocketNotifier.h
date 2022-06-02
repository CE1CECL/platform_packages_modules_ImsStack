/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#ifndef _SIP_STREAM_SOCKET_NOTIFIER_H_
#define _SIP_STREAM_SOCKET_NOTIFIER_H_

#include "private/SipConfig.h"
#include "SipSocket.h"

class ISipStreamSocketListener;

class SipStreamSocketNotifier : public SipSocket
{
public:
    explicit SipStreamSocketNotifier(IN IMS_SINT32 nSlotId);
    virtual ~SipStreamSocketNotifier();

public:
    virtual SipSocket* Accept();
    virtual IMS_BOOL Create(
            IN CONST IPAddress& objIPA, IN IMS_UINT32 nPort = 0, IN IMS_BOOL bSecure = IMS_FALSE);
    void SetListener(IN ISipStreamSocketListener* piListener);

public:
    virtual void Socket_OnConnectionReceived(IN ISocket* piSocket);
    virtual void Socket_OnClosed(
            IN ISocket* piSocket, IN IMS_SINT32 nReason = ISocket::CLOSE_REASON_UNKNOWN);

private:
    ISipStreamSocketListener* piListener;
};

#endif  // _SIP_STREAM_SOCKET_NOTIFIER_H_
