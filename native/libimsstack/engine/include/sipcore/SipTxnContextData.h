/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090725  toastops@                 Created
    </table>

    Description

*/

#ifndef _SIP_TXN_CONTEXT_DATA_H_
#define _SIP_TXN_CONTEXT_DATA_H_

#include "SipTransactionState.h"

class SipTxnContextData
{
public:
    SipTxnContextData();
    ~SipTxnContextData();

private:
    SipTxnContextData(IN CONST SipTxnContextData& objRHS);
    SipTxnContextData& operator=(IN CONST SipTxnContextData& objRHS);

public:
    const SipMethod& GetMethod() const;
    SipTransactionState* GetTxnState() const;
    void SetMethod(IN CONST SipMethod& objMethod);
    void SetTxnState(IN SipTransactionState* pTxnState);

private:
    SipMethod objMethod;
    RCPtr<SipTransactionState> pTxnState;
};

#endif  // _SIP_TXN_CONTEXT_DATA_H_
