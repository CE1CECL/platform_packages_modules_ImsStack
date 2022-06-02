/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#ifndef _SIP_MANAGER_H_
#define _SIP_MANAGER_H_

#include "SipDialogState.h"
#include "SipConnectionNotifier.h"

class SipManager
{
private:
    SipManager();
    SipManager(IN CONST SipManager& objRHS);

public:
    ~SipManager();

public:
    IMS_BOOL AttachDialogState(IN SipDialogState* pDState);
    void DetachDialogState(IN SipDialogState* pDState);
    RCPtr<SipDialogState> LookupDialogState(IN SipDialogState* pDState, IN ::SipMessage* pstMessage,
            IN IMS_BOOL bCheckForked = IMS_FALSE, OUT IMS_BOOL* pbIsForked = IMS_NULL);

    IMS_BOOL AttachConnectionNotifier(IN SipConnectionNotifier* pSCN);
    void DetachConnectionNotifier(IN SipConnectionNotifier* pSCN);
    SipConnectionNotifier* LookupConnectionNotifier(IN CONST SipTransportAddress& objTA,
            IN CONST AString& strFilter = AString::ConstNull());

    static SipManager* GetInstance();

private:
    IMS_BOOL StartUp();
    void CleanUp();

private:
    friend class StaticSip;

    enum
    {
        STATE_INACTIVE,
        STATE_ACTIVE,
        STATE_PENDING
    };

    IMS_SINT32 nState;
    IMSList<SipDialogState*> objDialogStates;
    IMSList<SipConnectionNotifier*> objSCNs;

    // IMSList<SIPSLSubscription*> objSubscriptions;
    // IMSList<SipConnection*> objTransactions;
    // IMSList<SipDialogImpl*> objDialogs;
    // IMSList<SIPRefresher*> objRefreshers;
};

#endif  // _SIP_MANAGER_H_
