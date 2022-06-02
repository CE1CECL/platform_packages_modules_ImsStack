/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100707  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _SIP_FORKED_TRANSACTION_MANAGER_H_
#define _SIP_FORKED_TRANSACTION_MANAGER_H_

#include "SipStackHeaders.h"
#include "RCObject.h"

class SipClientTransactionState;

class SipForkedTransactionManager : public RCObject
{
public:
    SipForkedTransactionManager();
    SipForkedTransactionManager(IN const SipForkedTransactionManager& objRHS);
    virtual ~SipForkedTransactionManager();

private:
    SipForkedTransactionManager& operator=(IN CONST SipForkedTransactionManager& objRHS);

public:
    IMS_BOOL Add(IN SipClientTransactionState* pCTState);
    IMS_BOOL IsEmpty() const;
    IMS_BOOL IsTransactionCompleted() const;
    SipClientTransactionState* Lookup(IN ::SipMessage* pstMessage) const;
    void Remove(IN SipClientTransactionState* pCTState);
    void SetTransactionCompleted(IN IMS_SINT32 nStatusCode);

private:
    // FIX_NO_ACK_RETRANSMISSION :: this will be used for 2xx response received case only
    IMS_SINT32 nStatusCode;
    IMSList<RCPtr<SipClientTransactionState>> objTxnStates;
};

#endif  // _SIP_FORKED_TRANSACTION_MANAGER_H_
