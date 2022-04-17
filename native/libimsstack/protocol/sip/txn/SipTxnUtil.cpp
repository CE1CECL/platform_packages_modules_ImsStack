/*
   Author
   <table>
   date      author                  description
   --------  --------------          ----------
    20100000  syed.malgimani@        Created
   20170110  vijay.nair@             Modified
   </table>

   Description

 */

#include "SipTrace.h"
#include "sip_error.h"
#include "sip_debug.h"
#include "platform/sip_pf_string.h"
#include "platform/sip_pf_memory.h"

#include "txn/SipTxnKey.h"
#include "txn/SipTxnUtil.h"
#include "msg/sip_msgutil.h"

#include "IMSTypeDef.h"

extern SIP_VOID sip_cbk_displayTxnKey(IN SIP_VOID* pvTxnKey);

SipTxnUtil* SipTxnUtil::m_pSipTxnUtil = SIP_NULL;

/*
 * Description        : This function is constructor for class SipTxnUtil
 */
SipTxnUtil::SipTxnUtil()
        :m_txnKeyList(SipVector<SipTxnKey*>())
{
}

/*
 * Description        : This function gets Class Instance
 */
SipTxnUtil* SipTxnUtil::GetInstance()
{
     if (m_pSipTxnUtil == SIP_NULL)
     {
         m_pSipTxnUtil = new SipTxnUtil();
     }
     return m_pSipTxnUtil;
}

/*
 * Description        : This function searches the list and return the specific TxnKey
 */
SipTxnKey * SipTxnUtil::SearchTxnKey(SipTxnKey *pObjUserTxnkey, SIP_BOOL bCheckRSeq)
{
    if (pObjUserTxnkey == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SipTxnUtil::SearchTxnKey: Txn Object is Null",
                SIP_ZERO,SIP_ZERO);
        return SIP_NULL;
    }

    SIP_UINT32 usSize = m_txnKeyList.GetSize();

    if (usSize <= SIP_ZERO)
    {
        return SIP_NULL;
    }

    SIP_UINT32 nStoredRules = pObjUserTxnkey->GetRules();

    pObjUserTxnkey->SetRules(SipTxnKey::RULE_COMPARE_TO_TAG);

    if (bCheckRSeq == SIP_TRUE)
    {
        pObjUserTxnkey->AddRule(SipTxnKey::RULE_COMPARE_RSEQ);
    }

    SIP_UINT16 uIndex = SIP_ZERO;
    while (uIndex < usSize)
    {
        SipTxnKey*  pObjStoredTxnKey = m_txnKeyList.GetAt(uIndex++);
        if (pObjStoredTxnKey == SIP_NULL)
        {
            pObjUserTxnkey->SetRules(nStoredRules);
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SipTxnUtil::SearchTxnKey: Element Retrieval from List Failed",
                SIP_ZERO,SIP_ZERO);
            return SIP_NULL;
        }

        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
            "SipTxnUtil::SearchTxnKey, CallID: %s, From Tag: %s.",
            pObjStoredTxnKey->GetCallId(),pObjStoredTxnKey->GetFromTag());

        if (pObjStoredTxnKey->CompareKeysForRPR(pObjUserTxnkey) != SIP_MATCHES)
        {
            continue;
        }

        pObjUserTxnkey->SetRules(nStoredRules);

        return pObjStoredTxnKey;
    }

    pObjUserTxnkey->SetRules(nStoredRules);

    SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SipTxnUtil::SearchTxnKey: Element Not Found in the List",
                SIP_ZERO,SIP_ZERO);

    return SIP_NULL;
}

SIP_BOOL SipTxnUtil::IsTxnKeyMatched(SipTxnKey *pObjUserTxnkey, SipTxnKey *pObjStoredTxnKey)
{
    if (pObjStoredTxnKey == SIP_NULL)
    {
        return SIP_FALSE;
    }

    if (pObjStoredTxnKey->CompareKeysForRPR(pObjUserTxnkey) != SIP_MATCHES)
    {
        return SIP_FALSE;
    }

    return SIP_TRUE;
}
/*
 * Description        : This function Adds Tnx Key to the list
 */
SIP_BOOL SipTxnUtil::AddTxnKey(SipTxnKey*  pObj)
{
    if (m_txnKeyList.Add(pObj) < 0)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipTxnUtil::AddTxnKey:Adding in list failed",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    return SIP_TRUE;
}

SIP_BOOL SipTxnUtil::DeleteTxnKey(SipTxnKey * pObjUserTxnkey, SIP_BOOL bCheckToTag)
{
    if (pObjUserTxnkey == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SipTxnUtil::DeleteTxnKey: Txn Object is Null",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_UINT32 usSize = m_txnKeyList.GetSize();
    if (usSize <= SIP_ZERO)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SipTxnUtil::DeleteTxnKey: List Size Zero",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_UINT32 nStoredRules = pObjUserTxnkey->GetRules();

    pObjUserTxnkey->SetRules(SipTxnKey::RULE_NONE);

    if (bCheckToTag == SIP_TRUE)
    {
        pObjUserTxnkey->AddRule(SipTxnKey::RULE_COMPARE_TO_TAG);
    }

    SIP_UINT32 uIndex = SIP_ZERO;
    SipTxnKey*  pObjStoredTxnKey = SIP_NULL;
    while (uIndex < usSize)
    {
        pObjStoredTxnKey = m_txnKeyList.GetAt(uIndex++);
        if (pObjStoredTxnKey == SIP_NULL)
        {
            pObjUserTxnkey->SetRules(nStoredRules);
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SipTxnUtil::DeleteTxnKey: Element Retrieval from List Failed",
                SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }

        if (IsTxnKeyMatched(pObjUserTxnkey, pObjStoredTxnKey) == SIP_TRUE)
        {
            sip_cbk_displayTxnKey((SIP_VOID*)pObjStoredTxnKey);
            pObjStoredTxnKey->SipDelete();
            m_txnKeyList.RemoveAt(uIndex - SIP_ONE);
            //Check again if further elements matches for the same txn key.
            uIndex--;
            usSize--;
        }
    }

    pObjUserTxnkey->SetRules(nStoredRules);

    return SIP_TRUE;
}
