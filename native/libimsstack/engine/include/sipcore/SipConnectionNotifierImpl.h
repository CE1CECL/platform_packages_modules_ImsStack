/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#ifndef _SIP_CONNECTION_NOTIFIER_IMPL_H_
#define _SIP_CONNECTION_NOTIFIER_IMPL_H_

#include "ISipConnectionNotifier.h"
#include "IOnSipServerConnectionListener.h"
#include "IOnSipConnectionNotifierErrorListener.h"

class ISipServerConnectionListener;
class SipConnectionNotifier;

class SipConnectionNotifierImpl :
        public ISipConnectionNotifier,
        public IOnSipServerConnectionListener,
        public IOnSipConnectionNotifierErrorListener
{
public:
    explicit SipConnectionNotifierImpl(IN SipConnectionNotifier* pSCN_);
    virtual ~SipConnectionNotifierImpl();

private:
    SipConnectionNotifierImpl(IN CONST SipConnectionNotifierImpl& objRHS);
    SipConnectionNotifierImpl& operator=(IN CONST SipConnectionNotifierImpl& objRHS);

public:
    SipConnectionNotifier* GetConnectionNotifier() const;

private:
    // IConnection interface
    virtual void Close();

    // ISipConnectionNotifier interface
    virtual ISipServerConnection* AcceptAndOpen();
    virtual const IPAddress& GetLocalAddress() const;
    virtual IMS_SINT32 GetLocalPort() const;
    virtual void SetListener(IN ISipServerConnectionListener* piListener);
    //// IMS extensions
    virtual ISipServerConnection* AcceptAndOpen(OUT ISipDialog*& piOrigDialog);
    virtual AString GetContactAddress() const;
    // MULTI_REG_SIP_PROFILE
    virtual SipProfile* GetSipProfile() const;
    virtual IMS_SINT32 GetSlotId() const;
    virtual IMS_BOOL IsTransportResourceReserved(IN IMS_SINT32 nType = TRANSPORT_ALL) const;
    virtual IMS_RESULT ReserveTransportResource(IN CONST IPAddress& objIPA, IN IMS_SINT32 nPortS,
            IN IMS_SINT32 nPortC, IN IMS_SINT32 nPortFlowControl);
    virtual IMS_RESULT RestoreTransportResource(
            IN IMS_SINT32 nType, IN CONST IPAddress& objPeerIP, IN IMS_SINT32 nPeerPort);
    virtual void SetFilter(IN CONST AString& strFilter);
    virtual void SetFromAndContact(IN CONST AString& strFrom, IN CONST AString& strDisplayName,
            IN CONST AString& strUserInfo);
    // MULTI_REG_SIP_PROFILE
    virtual void SetSipProfile(IN SipProfile* pProfile);
    virtual void UpdatePortFlowControl(IN IMS_SINT32 nPort);
    virtual void UpdatePortUc(IN IMS_SINT32 nPort);
    virtual void AddErrorListener(IN ISipConnectionNotifierErrorListener* piListener);
    virtual void RemoveErrorListener(IN ISipConnectionNotifierErrorListener* piListener);

    // IOnSipServerConnectionListener interface
    virtual void OnServerConnection_NotifyRequest(IN SipConnectionNotifier* pSCN);
    virtual void OnServerConnection_NotifyForkedRequest(IN SipConnectionNotifier* pSCN);

    // IOnSipConnectionNotifierErrorListener interface
    virtual void OnConnectionNotifierError_NotifyError(
            IN SipConnectionNotifier* pSCN, IN IMS_SINT32 nCode, IN CONST AString& strMessage);

private:
    ISipServerConnectionListener* piListener;
    IMSList<ISipConnectionNotifierErrorListener*> objErrorListeners;

    SipConnectionNotifier* pSCN;
};

#endif  // _SIP_CONNECTION_NOTIFIER_IMPL_H_
