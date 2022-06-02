/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#ifndef _SIP_CONNECTION_NOTIFIER_H_
#define _SIP_CONNECTION_NOTIFIER_H_

#include "Connection.h"
#include "ISipServerTransactionStateListener.h"
#include "ISipSocketListener.h"
#include "SipProfile.h"
#include "SipTransportAddress.h"

class SipTransportHelper;
class SipParameter;
class SipSocket;
class SipSocketAddress;
class SipServerTransactionState;
class ISipDialog;
class ISipServerConnection;
class IOnSipServerConnectionListener;
class IOnSipConnectionNotifierErrorListener;

class SipConnectionNotifier :
        public Connection,
        public ISipServerTransactionStateListener,
        public ISipSocketListener
{
public:
    SipConnectionNotifier(IN IMS_SINT32 nScheme_, IN IMS_SINT32 nPort_,
            IN CONST AString& strParams_, IN IMS_BOOL bSharedMode_ = IMS_FALSE);
    virtual ~SipConnectionNotifier();

private:
    SipConnectionNotifier(IN CONST SipConnectionNotifier& objRHS);
    SipConnectionNotifier& operator=(IN CONST SipConnectionNotifier& objRHS);

public:
    // IConnection interface
    virtual void Close();

    // ISipConnectionNotifier interface
    ISipServerConnection* AcceptAndOpen();
    const IPAddress& GetLocalAddress() const;
    IMS_SINT32 GetLocalPort() const;
    void SetListener(IN IOnSipServerConnectionListener* piListener);

    //// IMS extensions
    ISipServerConnection* AcceptAndOpen(OUT ISipDialog*& piOrigDialog);
    AString GetContactAddress() const;
    // MULTI_REG_SIP_PROFILE
    SipProfile* GetSipProfile() const;
    IMS_BOOL IsTransportResourceReserved(IN IMS_SINT32 nType = TRANSPORT_ALL) const;
    IMS_RESULT ReserveTransportResource(IN CONST IPAddress& objIPA, IN IMS_SINT32 nPortS,
            IN IMS_SINT32 nPortC, IN IMS_SINT32 nPortFlowControl);
    IMS_RESULT RestoreTransportResource(
            IN IMS_SINT32 nType, IN CONST IPAddress& objPeerIP, IN IMS_SINT32 nPeerPort);
    void SetErrorListener(IN IOnSipConnectionNotifierErrorListener* piListener);
    void SetFilter(IN CONST AString& strFilter);
    void SetFromAndContact(IN CONST AString& strFrom, IN CONST AString& strDisplayName,
            IN CONST AString& strUserInfo);
    // MULTI_REG_SIP_PROFILE
    void SetSipProfile(IN SipProfile* pProfile);
    void UpdatePortFlowControl(IN IMS_SINT32 nPort);
    void UpdatePortUc(IN IMS_SINT32 nPort);

    // Extension methods
    AString GetUserIdentity() const;
    IMS_BOOL IsSameConnectionNotifier(IN CONST SipTransportAddress& objTA) const;
    inline IMS_BOOL IsSharedMode() const { return (nMode == SHARED) ? IMS_TRUE : IMS_FALSE; }

protected:
    // ISipServerTransactionStateListener interface
    virtual void ServerTransactionState_ForkedRequestReceived(
            IN SipServerTransactionState* pSTState, IN SipDialogEx* pOrigDialogEx);
    virtual void ServerTransactionState_RequestCreated(IN SipServerTransactionState* pSTState);
    virtual void ServerTransactionState_RequestReceived(IN SipServerTransactionState* pSTState);

    // ISipSocketListener interface
    virtual void Socket_NotifyError(IN SipSocket* pSocket, IN IMS_SINT32 nErrorCode);
    virtual void Socket_SendEnabled(IN SipSocket* pSocket);

private:
    void ClearTransportResource();
    // FIX_MESSAGE_ORDER_ON_MIXED_TRANSPORT_USE
    void ControlUDPClientReference(IN IMS_SINT32 nControl);
    IMS_BOOL CreateClientInitiatedConnection(IN IMS_SINT32 nPort, IN SipSocketAddress* pFarEnd);
    IMS_BOOL ConnectClientInitiatedConnection();
    void DestroyClientInitiatedConnection(IN IMS_SINT32 nPort);
    void ExtractProperties(IN CONST AString& strParams);
    SipTransportHelper* GetTransportHelper() const;
    IMS_BOOL IsClientInitiatedConnectionRequired() const;
    IMS_BOOL IsIpSecRequired() const;
    IMS_BOOL IsTcpConnectionOnlyRequired() const;

    IMS_RESULT RestoreTransportResourceForClientInitiatedConnection(
            IN CONST IPAddress& objPeerIP, IN IMS_SINT32 nPeerPort);
    IMS_RESULT RestoreTransportResourceForServerConnection();

private:
    enum
    {
        SHARED = 0,
        DEDICATED
    };

    // FIX_MESSAGE_ORDER_ON_MIXED_TRANSPORT_USE
    enum
    {
        CTRL_CREATE = 1,
        CTRL_DESTROY = 2
    };

    // Error codes
    enum
    {
        TRANSPORT_ERROR_TCP_CLIENT = 1,
        TRANSPORT_ERROR_TCP_SERVER = 2,
        TRANSPORT_ERROR_UDP_SERVER = 3
    };

    // Types of transport resource
    enum
    {
        TRANSPORT_CLIENT_INITIATED_CONNECTION = 0x01,
        TRANSPORT_SERVER_CONNECTION = 0x02,
        TRANSPORT_ALL = (TRANSPORT_CLIENT_INITIATED_CONNECTION | TRANSPORT_SERVER_CONNECTION)
    };

#ifdef __JSR180_ONLY__
    struct UserProfile
    {
        // Active SIP user identity : From header
        AString strFrom;

        // Terminal SIP settings : Contact header
        AString strDisplayName;
        AString strUserInfo;
    };
#endif

    class ForkedTxnState
    {
    public:
        ForkedTxnState(IN SipDialogEx* pDialogEx_, IN SipServerTransactionState* pSTState_) :
                pDialogEx(pDialogEx_),
                pSTState(pSTState_)
        {
        }

        ~ForkedTxnState() {}

    public:
        RCPtr<SipDialogEx> pDialogEx;
        RCPtr<SipServerTransactionState> pSTState;
    };

    static IMS_SINT32* pGlobalSystemPort;

    IMS_SINT32 nMode;  // Shared mode or Dedicated mode

    // Connetor.open() info.
    IMS_SINT32 nScheme;
    IMS_SINT32 nPort;
    IMS_SINT32 nTransportProtocol;  // UDP only / TCP only / Both (udp/tcp)
    // MULTI_REG_TRANSPORT
    IMS_SINT32 nTransportExt;
    AString strType;
    AString strFilter;  // For PushRegistry
    IMSList<SipParameter*> objParameters;

#ifdef __JSR180_ONLY__
    UserProfile stUserProfile;
#endif

    // Identifier for IP connectivity
    IPAddress objIPA;
    IMS_SINT32 nPortC;
    SipSocket* pSocket_UDP;
    SipSocket* pSocket_TCP;
    // RFC5626_FLOW_CONTROL
    IMS_SINT32 nPortFlowControl;

    // FIX_MESSAGE_ORDER_ON_MIXED_TRANSPORT_USE
    // The patch grabs the reference of the datagram socket (UDP client socket)
    // to prevent misordering SIP messages when REG or re-REG is done.
    // It's only applied if IPSec is turned on and this assumes that
    // UDP transport protocol may be used in the near future.
    SipSocket* pSocket_UDPClient;
    // For client initiated connection (TCP_ONLY / FLOW_CONTROL)
    SipSocket* pSocket_TCPClient;
    SipSocketAddress* pSA_FarEnd;

    // MULTI_REG_SIP_PROFILE
    RCPtr<SipProfile> pSIPProfile;
    // Queue for incoming request messages
    IMSList<RCPtr<SipServerTransactionState>> objTxnStates;
    IMSList<ForkedTxnState*> objForkedTxnStates;
    IOnSipServerConnectionListener* piListener;
    IOnSipConnectionNotifierErrorListener* piErrorListener;
};

#endif  // _SIP_CONNECTION_NOTIFIER_H_
