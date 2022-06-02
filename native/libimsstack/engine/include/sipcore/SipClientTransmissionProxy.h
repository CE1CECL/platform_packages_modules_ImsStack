/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20160215  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _SIP_CLIENT_TRANSMISSION_PROXY_H_
#define _SIP_CLIENT_TRANSMISSION_PROXY_H_

// TCP active connection MUST be created at the start time of raw SIP message transmission.
#include "EngineActivity.h"
#include "ISipSocketListener.h"

class ISipClientTransmissionListener;
class SipTimerValues;

class SipClientTransmissionProxy : public EngineActivity, public ISipSocketListener
{
public:
    SipClientTransmissionProxy();
    virtual ~SipClientTransmissionProxy();

private:
    SipClientTransmissionProxy(IN CONST SipClientTransmissionProxy& objRHS);
    SipClientTransmissionProxy& operator=(IN CONST SipClientTransmissionProxy& objRHS);

public:
    // EngineActivity class
    virtual IMS_BOOL DispatchMessage(IN IMSMSG& objMSG);

    void Abort();
    IMS_RESULT Send();
    IMS_RESULT SendWithCredentials();
    void SetListener(IN ISipClientTransmissionListener* piListener);
    void SetTimerValues(IN SipTimerValues* pTV);
    void SetTransactionState(IN SipClientTransactionState* pCTState);

private:
    // ISipSocketListener class
    virtual void Socket_NotifyError(IN SipSocket* pSocket, IN IMS_SINT32 nErrorCode);
    virtual void Socket_SendEnabled(IN SipSocket* pSocket);

    void DestroyStreamSocket();
    IMS_BOOL IsUDPFallbackRequired() const;
    IMS_BOOL IsUDPFallbackSupported() const;
    void NotifyTransportError(IN IMS_SINT32 nErrorCode);
    IMS_RESULT PrepareStreamSocket();
    IMS_RESULT SendMessage();
    void SendPendingMessage();

public:
    enum
    {
        RESULT_NOK = (-1),
        RESULT_OK = 0,
        RESULT_PENDING = 1
    };

private:
    enum
    {
        AMSG_SEND_MESSAGE = AMSG_USER,
        AMSG_NOTIFY_TRANSPORT_ERROR
    };

    SipTimerValues* pTV;
    SipClientTransactionState* pCTState;
    ISipClientTransmissionListener* piListener;

    IMS_BOOL bIsResubmittedRequest;
    // TCP socket
    SipSocket* pSocket;
};

#endif  // _SIP_CLIENT_TRANSMISSION_PROXY_H_
