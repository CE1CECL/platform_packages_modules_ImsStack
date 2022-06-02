/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#ifndef _SIP_CLIENT_CONNECTION_H_
#define _SIP_CLIENT_CONNECTION_H_

#include "Credential.h"
#include "SipConnection.h"
#include "ISipClientTransactionStateListener.h"
#include "ISipClientTransmissionListener.h"
#include "SipClientTransmissionProxy.h"
#include "SipClientTransactionState.h"

class IOnSipClientConnectionListener;
class ISipGenericChallenge;
class ISipAckPackage;
class SipConnectionNotifier;
class SipAuHelper;

class SipClientConnection :
        public SipConnection,
        public ISipClientTransactionStateListener,
        public ISipClientTransmissionListener
{
public:
    SipClientConnection();
    explicit SipClientConnection(IN CONST AString& strTargetURI_);
    explicit SipClientConnection(IN SipClientTransactionState* pCTState_);
    virtual ~SipClientConnection();

private:
    SipClientConnection(IN CONST SipClientConnection& objRHS);
    SipClientConnection& operator=(IN CONST SipClientConnection& objRHS);

public:
    // IConnection interface
    virtual void Close();

    // ISipConnection interface
    virtual IMS_RESULT AddHeader(IN CONST AString& strName, IN CONST AString& strValue);
    virtual AString GetHeader(IN CONST AString& strName, IN IMS_SINT32 nIndex = 0);
    virtual IMSList<AString> GetHeaders(IN CONST AString& strName);
    virtual const SipMethod& GetMethod() const;
    virtual const AString& GetReasonPhrase() const;
    virtual const AString& GetRequestUri() const;
    virtual IMS_SINT32 GetStatusCode() const;
    virtual IMS_RESULT RemoveHeader(IN CONST AString& strName);
    virtual IMS_RESULT Send();
    virtual IMS_RESULT SetHeader(IN CONST AString& strName, IN CONST AString& strValue);
    virtual const ByteArray& GetContent() const;
    virtual IMS_RESULT SetContent(IN CONST ByteArray& objContent);
    // IMS extensions
    virtual IMS_SINT32 GetHeaderCount(IN CONST AString& strName) const;
    // MULTI_REG_SIP_PROFILE
    virtual void SetSipProfile(IN SipProfile* pProfile);

    // ISipClientConnection interface
    IMS_RESULT InitAck();
    SipClientConnection* InitCancel();
    IMS_RESULT InitRequest(IN CONST AString& strMethod, IN SipConnectionNotifier* pSCN);
    IMS_RESULT Receive(IN IMS_SLONG nTimeout = 0);
    IMS_RESULT SetCredentials(IN IMSList<Credential>& objCredentials);
    IMS_RESULT SetCredentials(IN CONST Credential& objCredential);
    void SetListener(IN IOnSipClientConnectionListener* piListener);
    IMS_RESULT SetRequestUri(IN CONST AString& strURI);
    ISipGenericChallenge* GetAuthenticationChallenge(IN IMS_SINT32 nIndex = 0) const;
    ISipAckPackage* GrabAck();
    IMS_RESULT InitResubmissionRequest();
    void RemoveAllChallenges();
    void RemoveAllCredentials();
    IMS_RESULT SetAuthenticationChallenge(IN ISipGenericChallenge* piChallenge);
    void SetExtensionTokenForViaBranch(IN CONST AString& strToken);
    void SetImplicitRouteHeader(IN CONST AString& strRouteHeader);
    void SetTransportTuple(IN CONST IPAddress& objIPA, IN IMS_SINT32 nPortS, IN IMS_SINT32 nPortC,
            IN IMS_SINT32 nPortFC = 0xFFFF, IN IMS_SINT32 nTransportExt = 0 /* ANY */);

    // Extension methods
    IMS_RESULT InitDialogRequest(IN CONST SipMethod& objMethod, IN SipDialogEx* pDialogEx);
    IMS_RESULT SendWithCredentials();

protected:
    IMS_BOOL DispatchMessage(IN ImsMessage& objMsg) override;

private:
    // ISipClientTransactionStateListener interface
    virtual void ClientTransactionState_ForkedResponseReceived(
            IN SipClientTransactionState* pCTState);
    virtual void ClientTransactionState_ResponseReceived(IN ::SipMessage* pstMessage);

    // SIP_TRANSPORT_ERROR_REPORT_ON_TXN
    virtual IMS_BOOL IsTransportErrorReportRequired(
            IN IMS_SINT32 nCode, IN CONST AString& strMessage) const;

    // ISipClientTransmissionListener class
    virtual void ClientTransmission_NotifyError(IN IMS_SINT32 nCode, IN CONST AString& strMessage);
    virtual void ClientTransmission_TransmissionCompleted();

    void SetState(IN IMS_SINT32 nState);

    static const IMS_CHAR* StateToString(IN IMS_SINT32 nState);

public:
    enum
    {
        STATE_CREATED = 0,
        STATE_INITIALIZED,
        // STATE_STREAM_OPEN,
        STATE_PROCEEDING,
        STATE_COMPLETED,
        STATE_UNAUTHORIZED,
        STATE_TERMINATED
    };

private:
    enum
    {
        AMSG_SIP_RESPONSE_RECEIVED = AMSG_USER,
        AMSG_SIP_FORKED_RESPONSE_RECEIVED
    };

    static const AString ANONYMOUS_URI;

    IMS_SINT32 nState;
    IMS_BOOL bACKSent;
    IMS_BOOL bResubmissionRequestInitialized;

    // Request-URI should be equal to the INVITE request
    //    : CANCEL & ACK to non-2xx response
    AString strTargetURI;
    RCPtr<SipClientTransactionState> pCTState;

    // Require & Proxy-Require headers to the INVITE request
    // IMSList<SipHeader*> *pInviteRequires;    // TODO:: move to Session class (J281)

    IMSList<sipcore::SipMessage*> objResponseMessages;
    SipAuHelper* pAuHelper;

    IOnSipClientConnectionListener* piListener;

    // UDP_FALLBACK
    SipClientTransmissionProxy* pTransmissionProxy;
};

#endif  // _SIP_CLIENT_CONNECTION_H_
