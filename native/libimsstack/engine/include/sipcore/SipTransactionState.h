/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#ifndef _SIP_TRANSACTION_STATE_H_
#define _SIP_TRANSACTION_STATE_H_

#include "IPAddress.h"
#include "RCObject.h"
#include "SipDialogEx.h"
#include "SipTimerValues.h"
#include "SipProfile.h"

class INetworkConnection;
class ISipTransactionStateListener;
class ISipTransportErrorListener;
class SipTransport;

class SipTransactionState : public RCObject
{
public:
    SipTransactionState();
    explicit SipTransactionState(IN SipDialogEx* pDialogEx_);
    SipTransactionState(IN const SipTransactionState& objRHS);
    virtual ~SipTransactionState();

private:
    SipTransactionState& operator=(IN const SipTransactionState& objRHS);

public:
    virtual void Abort();
    virtual void Terminate();
    virtual IMS_SINT32 CheckMessageValidity();
    virtual IMS_BOOL FormMessage();
    virtual IMS_BOOL InitTxnDetails(IN CONST SipTransactionState* pTState);
    virtual void NotifyTimerExpired();
    virtual void PostProcessMessageSentByStack(
            IN ::SipMessage* pstSipMsg, IN const ByteArray& objBuffer);
    virtual void PreProcessMessageSentByStack(IN ::SipMessage* pstSipMsg);
    virtual IMS_BOOL Send(IN SipTimerValues* pTV = IMS_NULL);
    virtual IMS_RESULT RetransmitMessage();
    virtual IMS_BOOL UpdateTransportDetails();

    inline SipDialogEx* GetDialog() const { return pDialogEx.Get(); }
    inline ::SipMessage* GetMessage() const { return pstMessage; }
    inline ::SipMessage* GetLastMessage() const { return pstLastMessage; }
    // MULTI_REG_SIP_PROFILE
    inline SipProfile* GetSipProfile() const { return pSIPProfile.Get(); }
    inline SipTransport* GetSipTransport() const { return pTransport; }
    inline ::SipTxnKey* GetTxnKey() const { return pstTxnKey; }
    IMS_SINT32 GetSlotId() const;
    IMS_BOOL IsIpSecRequired() const;
    // MULTI_REG_SIP_PROFILE
    void SetSipProfile(IN SipProfile* pProfile);
    void SetTransactionListener(IN ISipTransactionStateListener* piListener);
    void SetTransportListener(IN ISipTransportErrorListener* piListener);
    void SetTransportTuple(IN CONST IPAddress& objIPA, IN IMS_SINT32 nPortS, IN IMS_SINT32 nPortC,
            IN IMS_SINT32 nPortFC = 0xFFFF, IN IMS_SINT32 nTransportExt = 0 /* ANY */);

    IMS_BOOL SendToNetwork(IN CONST IMS_BYTE* pBuffer, IN IMS_SINT32 nBuffLen);
    void UpdateMessage(IN ::SipMessage* pstMessage);

protected:
    virtual SipTransactionState* Clone();

    IMS_BOOL Send(IN ::SipMessage* pstMessage, IN SipTimerValues* pTV);
    void SetTimerValues(IN SipTimerValues* pTV, IN_OUT SipTxnContext*& pstTxnContext);
    void SetFlowControlOption(IN CONST SipMethod& objMethod);

public:
    enum
    {
        TYPE_CLIENT = 1,
        TYPE_SERVER
    };

    enum
    {
        CLASS_NONE,
        CLASS_INVITE,
        CLASS_REGULAR,
        CLASS_OVERLAP,
        CLASS_MAX
    };

protected:
    IMS_SINT32 nType;   // CLIENT or SERVER
    IMS_SINT32 nClass;  // Class of transaction (INVITE/non-INVITE/Overlap)
    IMS_UINT32 nCSeqNumber;
    // MULTI_REG_SIP_PROFILE
    RCPtr<SipProfile> pSIPProfile;
    ISipTransactionStateListener* piListener;

    RCPtr<SipDialogEx> pDialogEx;
    SipTransport* pTransport;

    ::SipMessage* pstMessage;
    ::SipMessage* pstLastMessage;
    ::SipTxnKey* pstTxnKey;
    ::SipTxnKey* pstRPRTxnKey;
};

#endif  // _SIP_TRANSACTION_STATE_H_
