/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#ifndef _SIP_CLIENT_TRANSACTION_STATE_H_
#define _SIP_CLIENT_TRANSACTION_STATE_H_

#include "SipTransactionState.h"
#include "SipForkedTransactionManager.h"
#include "SipTransportAddress.h"

class ISipClientTransactionStateListener;

class SipClientTransactionState : public SipTransactionState
{
public:
    explicit SipClientTransactionState(IN IMS_SINT32 nSlotId);
    virtual ~SipClientTransactionState();

private:
    SipClientTransactionState& operator=(IN CONST SipClientTransactionState& objRHS);

public:
    virtual void Abort();
    virtual IMS_SINT32 CheckMessageValidity();
    virtual IMS_BOOL FormMessage();
    virtual IMS_BOOL FormMessageForResubmissionRequest();
    virtual IMS_BOOL InitTxnDetails(IN CONST SipTransactionState* pTState);
    virtual IMS_BOOL Send(IN SipTimerValues* pTV = IMS_NULL);
    virtual IMS_BOOL UpdateTransportDetails();

    IMS_BOOL AdjustTransportProtocolAsUdp();
    ::SipMessage* CreateAckRequest(IN ::SipMessage* pstRespMessage);
    inline IMS_BOOL IsTargetUpdated() const { return (nRoutingType == TARGET_SR); }
    IMS_BOOL InitCancel(IN CONST SipClientTransactionState* pInviteTState);
    IMS_BOOL InitRequest(IN CONST SipMethod& objMethod);
    IMS_BOOL InitRequest(IN CONST SipMethod& objMethod, IN SipDialogEx* pDialogEx);
    // FORKED_RESPONSE
    IMS_SINT32 RemoveForkedTransaction();
    IMS_BOOL SendWithCredentials(IN SipTimerValues* pTV = IMS_NULL);
    void SetExtensionTokenForViaBranch(IN CONST AString& strToken);
    // IMPLICIT_ROUTE
    void SetImplicitRouteHeader(IN CONST AString& strRouteHeader);
    inline void SetListener(IN ISipClientTransactionStateListener* piListener)
    {
        this->piListener = piListener;
    }
    IMS_BOOL UpdateRouteDetails(IN CONST SipMethod& objMethod);
    IMS_SINT32 HandleResponse(IN ::SipMessage* pstMessage);
    static IMS_SINT32 MatchTransaction(IN ::SipMessage* pstMessage,
            IN CONST SipTransportAddress& objFarEnd,
            OUT RCPtr<SipClientTransactionState>& pCTState);

private:
    virtual SipTransactionState* Clone();

    IMS_BOOL CorrectRouteHeader(IN_OUT ::SipMessage*& pstMessage);
    void CheckNSendAck();
    IMS_BOOL HandleForkedResponse(IN CONST SipMessageInfo& objMInfo);
    IMS_BOOL InitAck(IN_OUT ::SipMessage*& pstAckMessage, IN ::SipMessage* pstRespMessage);
    IMS_BOOL SetDialogRelatedHeaders(IN CONST SipMethod& objMethod);
    IMS_BOOL SetMandatoryHeaders(IN CONST SipMethod& objMethod);
    void SetPANIHeader(IN CONST SipMethod& objMethod, IN_OUT ::SipMessage*& pstMessage);
    IMS_BOOL UpdateTxnDetails(IN CONST SipMethod& objMethod);

private:
    // Flag which will be used to determine the destination transport information
    // to send a request.
    // If the flag is TARGET_SR,
    //    the Request-URI will be selected.
    // Otherwise,
    //    if the Route entry is present and the flag is TARGET_LR,
    //    the first Route entry will be selected.
    //    if the flag is TARGET_NO_ROUTE, the Request-URI will be selected.
    enum
    {
        TARGET_LR = 0,
        TARGET_SR,
        TARGET_NO_ROUTE
    };

    IMS_SINT32 nRoutingType;
    // IMPLICIT_ROUTE
    SipAddrSpec* pstImplicitRoute;
    ISipClientTransactionStateListener* piListener;

    // FORKED_RESPONSE
    RCPtr<SipForkedTransactionManager> pForkedTxnMngr;
    // FORKED_RESPONSE_TO_SUPPORT_EARLY_DIALOG_TERMINATION
    RCPtr<SipForkedTransactionManager> pPersistentForkedTxnMngr;
};

#endif  // _SIP_CLIENT_TRANSACTION_STATE_H_
