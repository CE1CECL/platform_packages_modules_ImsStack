/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100304  hwangoo.park@             Created
    </table>

    Description
     This class is a wrapper class for SipTxnKey & SipTxnBuffer struct data type.
*/

#ifndef _SIP_STACK_TRANSACTION_H_
#define _SIP_STACK_TRANSACTION_H_

#include "IMSTypeDef.h"



class SIPStackTransaction
{
public:
    SIPStackTransaction();
    SIPStackTransaction(IN SipTxnKey *pKey_, IN SipTxnBuffer *pBuffer_);
    SIPStackTransaction(IN CONST SIPStackTransaction &objRHS);
    virtual ~SIPStackTransaction();

public:
    SIPStackTransaction& operator=(IN CONST SIPStackTransaction &objRHS);

public:
    IMS_BOOL CompareKey(IN SipTxnKey *pstKey);
    SipTxnBuffer* GetBuffer() const;
    SipTxnKey* GetKey() const;

private:
    SipTxnKey *pstKey;
    SipTxnBuffer *pstBuffer;
};

#endif // _SIP_STACK_TRANSACTION_H_
