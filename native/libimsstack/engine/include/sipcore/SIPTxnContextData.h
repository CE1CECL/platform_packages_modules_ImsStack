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

#include "SIPTransactionState.h"



class SIPTxnContextData
{
public:
    SIPTxnContextData();
    ~SIPTxnContextData();

private:
    SIPTxnContextData(IN CONST SIPTxnContextData &objRHS);
    SIPTxnContextData& operator=(IN CONST SIPTxnContextData &objRHS);

public:
    const SIPMethod& GetMethod() const;
    SIPTransactionState* GetTxnState() const;
    void SetMethod(IN CONST SIPMethod &objMethod);
    void SetTxnState(IN SIPTransactionState *pTxnState);

private:
    SIPMethod objMethod;
    RCPtr<SIPTransactionState> pTxnState;
};

#endif // _SIP_TXN_CONTEXT_DATA_H_
