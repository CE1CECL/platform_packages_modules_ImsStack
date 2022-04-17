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

#include "ServiceMemory.h"
#include "SIPStackHeaders.h"
#include "SIPStackTransaction.h"



PUBLIC
SIPStackTransaction::SIPStackTransaction()
    : pstKey(IMS_NULL)
    , pstBuffer(IMS_NULL)
{
}

PUBLIC
SIPStackTransaction::SIPStackTransaction(IN SipTxnKey *pKey_, IN SipTxnBuffer *pBuffer_)
    : pstKey(pKey_)
    , pstBuffer(pBuffer_)
{
    SIPStack::AddReference(pstKey);
    SIPStack::AddReference(pstBuffer);
}

PUBLIC
SIPStackTransaction::SIPStackTransaction(IN CONST SIPStackTransaction &objRHS)
    : pstKey(objRHS.pstKey)
    , pstBuffer(objRHS.pstBuffer)
{
    SIPStack::AddReference(pstKey);
    SIPStack::AddReference(pstBuffer);
}

PUBLIC VIRTUAL
SIPStackTransaction::~SIPStackTransaction()
{
    SIPStack::FreeTxnKey(pstKey);
    SIPStack::FreeTxnBuffer(pstBuffer);
}

PUBLIC
SIPStackTransaction& SIPStackTransaction::operator=(IN CONST SIPStackTransaction &objRHS)
{
    if (this != &objRHS)
    {
        SIPStack::FreeTxnKey(pstKey);
        SIPStack::FreeTxnBuffer(pstBuffer);

        pstKey = objRHS.pstKey;
        pstBuffer = objRHS.pstBuffer;

        SIPStack::AddReference(pstKey);
        SIPStack::AddReference(pstBuffer);
    }

    return (*this);
}

PUBLIC
IMS_BOOL SIPStackTransaction::CompareKey(IN SipTxnKey *pstKey)
{
    return SIPStack::CompareTxnKeys(this->pstKey, pstKey);
}

PUBLIC
SipTxnBuffer* SIPStackTransaction::GetBuffer() const
{
    return pstBuffer;
}

PUBLIC
SipTxnKey* SIPStackTransaction::GetKey() const
{
    return pstKey;
}
