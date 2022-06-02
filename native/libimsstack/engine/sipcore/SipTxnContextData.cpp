/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090725  toastops@                 Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "SipPrivate.h"
#include "SipTxnContextData.h"

__IMS_TRACE_TAG_SIP__;

PUBLIC
SipTxnContextData::SipTxnContextData() :
        pTxnState(IMS_NULL)
{
}

PUBLIC
SipTxnContextData::~SipTxnContextData()
{
    //---------------------------------------------------------------------------------------------

    IMS_TRACE_D("Destructor :: SipTxnContextData (%" PFLS_x ")",
            pTxnState.IsNull() ? 0 : pTxnState.Get(), 0, 0);

    pTxnState = IMS_NULL;
}

PUBLIC
const SipMethod& SipTxnContextData::GetMethod() const
{
    //---------------------------------------------------------------------------------------------

    return objMethod;
}

PUBLIC
SipTransactionState* SipTxnContextData::GetTxnState() const
{
    //---------------------------------------------------------------------------------------------

    return pTxnState.Get();
}

PUBLIC
void SipTxnContextData::SetMethod(IN CONST SipMethod& objMethod)
{
    //---------------------------------------------------------------------------------------------

    this->objMethod = objMethod;
}

PUBLIC
void SipTxnContextData::SetTxnState(IN SipTransactionState* pTxnState)
{
    //---------------------------------------------------------------------------------------------

    this->pTxnState = pTxnState;
}
