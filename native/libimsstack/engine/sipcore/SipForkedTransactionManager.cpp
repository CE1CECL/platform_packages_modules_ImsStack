/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100707  hwangoo.park@             Created
    </table>

    Description

*/

#include "ServiceTrace.h"
#include "ISipHeader.h"
#include "Sip.h"
#include "SipStatusCode.h"
#include "SipDebug.h"
#include "SipClientTransactionState.h"
#include "SipForkedTransactionManager.h"

__IMS_TRACE_TAG_SIP__;

PUBLIC
SipForkedTransactionManager::SipForkedTransactionManager() :
        RCObject(),
        nStatusCode(SipStatusCode::SC_INVALID)
{
}

PUBLIC
SipForkedTransactionManager::SipForkedTransactionManager(
        IN const SipForkedTransactionManager& objRHS) :
        RCObject(objRHS),
        nStatusCode(objRHS.nStatusCode)
{
}

PUBLIC VIRTUAL SipForkedTransactionManager::~SipForkedTransactionManager() {}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipForkedTransactionManager::Add(IN SipClientTransactionState* pCTState)
{
    //---------------------------------------------------------------------------------------------

    for (IMS_UINT32 i = 0; i < objTxnStates.GetSize(); ++i)
    {
        const RCPtr<SipClientTransactionState>& pTmpCTState = objTxnStates.GetAt(i);

        if (pTmpCTState.Get() == pCTState)
        {
            // Already exists
            return IMS_TRUE;
        }
    }

    if (!objTxnStates.Append(pCTState))
    {
        return IMS_FALSE;
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipForkedTransactionManager::IsEmpty() const
{
    //---------------------------------------------------------------------------------------------

    return objTxnStates.IsEmpty();
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipForkedTransactionManager::IsTransactionCompleted() const
{
    //---------------------------------------------------------------------------------------------

    return SipStatusCode::IsFinal(nStatusCode);
}

/*

Remarks

*/
PUBLIC
SipClientTransactionState* SipForkedTransactionManager::Lookup(IN ::SipMessage* pstMessage) const
{
    //---------------------------------------------------------------------------------------------

    if (pstMessage == IMS_NULL)
    {
        return IMS_NULL;
    }

    if (objTxnStates.IsEmpty())
    {
        return IMS_NULL;
    }

    // Not forked case
    if (objTxnStates.GetSize() == 1)
    {
        const RCPtr<SipClientTransactionState>& pCTState = objTxnStates.GetAt(0);

        return pCTState.Get();
    }

    SipHeaderBase* pstHeader;
    AString strNewLocalTag;
    AString strNewRemoteTag;
    AString strCallId;

    // Call Id
    pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::CALL_ID);
    SipStack::EncodeHeaderBody(pstHeader, IMS_FALSE, strCallId);
    SipStack::FreeHeaderEx(pstHeader);

    if (SipStack::IsRequestMessage(pstMessage))
    {
        // Get local tag
        pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::TO);

        strNewLocalTag = SipStack::GetParameter(pstHeader, Sip::STR_TAG);
        SipStack::FreeHeaderEx(pstHeader);

        // Get remote tag
        pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::FROM);

        strNewRemoteTag = SipStack::GetParameter(pstHeader, Sip::STR_TAG);
        SipStack::FreeHeaderEx(pstHeader);
    }
    else
    {
        // Get local tag
        pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::FROM);

        strNewLocalTag = SipStack::GetParameter(pstHeader, Sip::STR_TAG);
        SipStack::FreeHeaderEx(pstHeader);

        // Get remote tag
        pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::TO);

        strNewRemoteTag = SipStack::GetParameter(pstHeader, Sip::STR_TAG);
        SipStack::FreeHeaderEx(pstHeader);
    }

    for (IMS_UINT32 i = 0; i < objTxnStates.GetSize(); ++i)
    {
        const RCPtr<SipClientTransactionState>& pCTState = objTxnStates.GetAt(i);
        SipDialogState* pDState = pCTState->GetDialog()->GetDialogState();

        AString strLocalTag = pDState->GetLocalTag();
        AString strRemoteTag = pDState->GetRemoteTag();

        if (strCallId.Equals(pDState->GetCallId()) && strLocalTag.Equals(strNewLocalTag) &&
                strRemoteTag.Equals(strNewRemoteTag))
        {
            IMS_TRACE_D("ForkedTransactionManager :: Dialog "
                        "(call-id=%s, local-tag=%s, remote-tag=%s)",
                    SipDebug::GetCharA1(strCallId.GetStr(), 8, '@'), strLocalTag.GetStr(),
                    strRemoteTag.GetStr());
            return pCTState.Get();
        }
    }

    const RCPtr<SipClientTransactionState>& pCTState =
            objTxnStates.GetAt(objTxnStates.GetSize() - 1);

    return pCTState.Get();
}

/*

Remarks

*/
PUBLIC
void SipForkedTransactionManager::Remove(IN SipClientTransactionState* pCTState)
{
    //---------------------------------------------------------------------------------------------

    for (IMS_UINT32 i = 0; i < objTxnStates.GetSize(); ++i)
    {
        const RCPtr<SipClientTransactionState>& pTmpCTState = objTxnStates.GetAt(i);

        if (pTmpCTState.Get() == pCTState)
        {
            // Found
            objTxnStates.RemoveAt(i);
            break;
        }
    }
}

/*

Remarks

*/
PUBLIC
void SipForkedTransactionManager::SetTransactionCompleted(IN IMS_SINT32 nStatusCode)
{
    //---------------------------------------------------------------------------------------------

    this->nStatusCode = nStatusCode;
}
