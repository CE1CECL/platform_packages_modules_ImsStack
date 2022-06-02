/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090302  toastops@                 Created
    </table>

    Description
    This file contains constants representing SIP response codes as defined in RFC 3261
    and extensions.
*/

#include "ServiceMemory.h"
#include "ServiceMutex.h"
#include "SipPrivate.h"
#include "private/SipConfigV.h"
#include "SipConfigProxy.h"
#include "SipDebug.h"
#include "SipTxnContextData.h"
#include "SipStackTransaction.h"
#include "SipStackState.h"

__IMS_TRACE_TAG_SIP__;

PRIVATE
SipStackState::SipStackState() :
        piLock(IMS_NULL),
        objTxnAggregate(IMSMap<IMS_UINT32, IMSList<SipStackTransaction*>>())
{
    piLock = MutexService::GetMutexService()->CreateMutex();
}

PUBLIC
SipStackState::~SipStackState()
{
    CleanUp();
    MutexService::GetMutexService()->DestroyMutex(piLock);
}

/*

Remarks

*/
PUBLIC
void SipStackState::CleanUp()
{
    LockGuard objLock(piLock);

    IMS_TRACE_D("StackState - Number Of Transaction : (%d)", objTxnAggregate.GetSize(), 0, 0);

    if (!objTxnAggregate.IsEmpty())
    {
        for (IMS_UINT32 i = 0; i < objTxnAggregate.GetSize(); ++i)
        {
            IMSList<SipStackTransaction*>& objTransactions = objTxnAggregate.GetValueAt(i);

            for (IMS_UINT32 j = 0; j < objTransactions.GetSize(); ++j)
            {
                SipStackTransaction* pTransaction = objTransactions.GetAt(j);

                if (pTransaction == IMS_NULL)
                {
                    continue;
                }

                ::SipTxnKey* pKey = pTransaction->GetKey();

                SipStack::TerminateTransaction(pKey);

                IMS_TRACE_D("StackState (Transaction) - Call ID (%s), Via Branch (%s)",
                        SipDebug::GetCharA1(SipStack::TxnKey_GetCallId(pKey), 8, '@'),
                        SipStack::TxnKey_GetViaBranch(pKey), 0);

                delete pTransaction;
            }
        }

        objTxnAggregate.Clear();
    }
}

/*

Remarks

*/
PUBLIC
void SipStackState::StartUp()
{
    //---------------------------------------------------------------------------------------------

    // For transaction layer handling
    // Initialize the retransmission initial timer values, timeout timer values,
    // and wait timer values.
    SipStack::Initialize();
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipStackState::AbortTransaction(IN ::SipTxnKey* pKey, IN SipTransactionState* pTxnState)
{
    if (pKey == IMS_NULL || pTxnState == IMS_NULL)
    {
        return IMS_FALSE;
    }

    SipTxnContext* pTxnContext = SipStack::CreateTxnContext();

    if (pTxnContext != IMS_NULL)
    {
        SipTxnContextData* pTxnContextData = new SipTxnContextData();

        if (pTxnContextData != IMS_NULL)
        {
            pTxnContextData->SetTxnState(pTxnState);
        }

        pTxnContext->pTxnContextData = reinterpret_cast<SIP_VOID*>(pTxnContextData);
    }

    // Release the stack transaction structure & stop retransmissions.
    if (!SipStack::AbortTransaction(pKey, pTxnContext))
    {
        SipStack::DestroyTxnContext(pTxnContext);
        return IMS_FALSE;
    }

    SipStack::DestroyTxnContext(pTxnContext);

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipStackState::FetchTransaction(
        IN ::SipTxnKey* pKey, IN IMS_SINT32 nOption, OUT ::SipTxnKey*& pOutKey, OUT SipTxn*& pTxn)
{
    LockGuard objLock(piLock);
    SipStackTransaction* pTransaction = FindTransaction(pKey);

    if (pTransaction == IMS_NULL)
    {
        if (nOption == TXN_CREATE)
        {
            return AddTransaction(pKey, pTxn);
        }
        else
        {
            pOutKey = IMS_NULL;
            pTxn = IMS_NULL;
            return IMS_FALSE;
        }
    }

    pOutKey = pTransaction->GetKey();
    pTxn = pTransaction->GetTxn();

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipStackState::ReleaseTransaction(
        IN ::SipTxnKey* pKey, IN IMS_SINT32 nOption, OUT ::SipTxnKey*& pOutKey, OUT SipTxn*& pTxn)
{
    LockGuard objLock(piLock);
    SipStackTransaction* pTransaction = RemoveTransaction(pKey, nOption);

    if (pTransaction == IMS_NULL)
    {
        pOutKey = IMS_NULL;
        pTxn = IMS_NULL;

        return IMS_FALSE;
    }

    if (nOption == TXN_REMOVE)
    {
        pOutKey = pTransaction->GetKey();
        pTxn = pTransaction->GetTxn();

        delete pTransaction;
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
void SipStackState::SetTransactionTimerValues(
        IN IMS_SINT32 nSlotId, IN CONST SipProfile* pSIPProfile)
{
    const SipConfigV* pSipConfigV =
            DYNAMIC_CAST(const SipConfigV*, SipConfigProxy::GetSipConfigV(nSlotId));

    //---------------------------------------------------------------------------------------------

    if ((pSIPProfile == IMS_NULL) && (pSipConfigV != IMS_NULL) &&
            !pSipConfigV->IsTimerValueConfiguredOnRuntime())
    {
        // Do not update the transaction timer values in runtime...
        IMS_TRACE_D("SIP timer values are not configured on runtime ...", 0, 0, 0);
        return;
    }

    // For transaction layer handling
    // Initialize the retransmission initial timer values, timeout timer values,
    // and wait timer values.
    SipStack::SetTransactionTimerValues(nSlotId, pSIPProfile, pSipConfigV);
}

/*

Remarks

*/
PUBLIC GLOBAL SipStackState* SipStackState::GetInstance()
{
    static SipStackState* pStackState = IMS_NULL;

    //---------------------------------------------------------------------------------------------

    if (pStackState == IMS_NULL)
    {
        pStackState = new SipStackState();
    }

    return pStackState;
}

/*

Remarks

*/
PRIVATE
IMS_BOOL SipStackState::AddTransaction(IN ::SipTxnKey* pKey, IN SipTxn* pTxn)
{
    SipStackTransaction* pTransaction = new SipStackTransaction(pKey, pTxn);

    if (pTransaction == IMS_NULL)
    {
        return IMS_FALSE;
    }

    IMS_UINT32 nKey = AString::GetHashCode(SipStack::TxnKey_GetCallId(pKey));
    IMS_SLONG nKeyIndex = objTxnAggregate.GetIndexOfKey(nKey);

    if (nKeyIndex >= 0)
    {
        IMSList<SipStackTransaction*>& objTransactions = objTxnAggregate.GetValueAt(nKeyIndex);

        if (!objTransactions.Append(pTransaction))
        {
            delete pTransaction;
            return IMS_FALSE;
        }

        return IMS_TRUE;
    }

    IMSList<SipStackTransaction*> objTransactions;

    if (!objTransactions.Append(pTransaction))
    {
        delete pTransaction;
        return IMS_FALSE;
    }

    if (!objTxnAggregate.Add(nKey, objTransactions))
    {
        delete pTransaction;
        return IMS_FALSE;
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PRIVATE
SipStackTransaction* SipStackState::FindTransaction(IN ::SipTxnKey* pKey)
{
    IMS_UINT32 nKey = AString::GetHashCode(SipStack::TxnKey_GetCallId(pKey));
    IMS_SLONG nKeyIndex = objTxnAggregate.GetIndexOfKey(nKey);

    if (nKeyIndex < 0)
    {
        // Transaction not found
        return IMS_NULL;
    }

    IMSList<SipStackTransaction*>& objTransactions = objTxnAggregate.GetValueAt(nKeyIndex);

    for (IMS_UINT32 i = 0; i < objTransactions.GetSize(); ++i)
    {
        SipStackTransaction* pTransaction = objTransactions.GetAt(i);

        // Check if the transaction is matched or not
        if (pTransaction->CompareKey(pKey))
        {
            // Matched transaction found ...
            return pTransaction;
        }
    }

    return IMS_NULL;
}

/*

Remarks

*/
PRIVATE
SipStackTransaction* SipStackState::RemoveTransaction(IN ::SipTxnKey* pKey, IN IMS_SINT32 nOption)
{
    IMS_UINT32 nKey = AString::GetHashCode(SipStack::TxnKey_GetCallId(pKey));
    IMS_SLONG nKeyIndex = objTxnAggregate.GetIndexOfKey(nKey);

    if (nKeyIndex < 0)
    {
        // Transaction not found
        IMS_TRACE_D("REMOVE :: TXN (%s) NOT FOUND, BUT OK!!!", SipStack::TxnKey_GetViaBranch(pKey),
                0, 0);
        return IMS_NULL;
    }

    IMSList<SipStackTransaction*>& objTransactions = objTxnAggregate.GetValueAt(nKeyIndex);

    for (IMS_UINT32 i = 0; i < objTransactions.GetSize(); ++i)
    {
        SipStackTransaction* pTransaction = objTransactions.GetAt(i);

        // Check if the transaction is matched or not
        if (pTransaction->CompareKey(pKey))
        {
            // Matched transaction found ...
            if (nOption == TXN_REMOVE)
            {
                IMS_TRACE_D("REMOVE TRANSACTION - S (%d)\r\n", GetTransactionCount(), 0, 0);

                objTransactions.RemoveAt(i);

                if (objTransactions.IsEmpty())
                {
                    // No element
                    objTxnAggregate.RemoveAt(nKeyIndex);
                }

                // DEBUG ...
                SipStack::DisplayTxnKey(pTransaction->GetKey());

                IMS_TRACE_D("REMOVE TRANSACTION - E (%d)\r\n", GetTransactionCount(), 0, 0);
            }

            return pTransaction;
        }
    }

    return IMS_NULL;
}

/*

Remarks

*/
PRIVATE
IMS_UINT32 SipStackState::GetTransactionCount() const
{
    IMS_UINT32 nCount = 0;

    //---------------------------------------------------------------------------------------------

    for (IMS_UINT32 i = 0; i < objTxnAggregate.GetSize(); ++i)
    {
        const IMSList<SipStackTransaction*>& objTransactions = objTxnAggregate.GetValueAt(i);

        nCount += objTransactions.GetSize();
    }

    return nCount;
}
