/*
   Author
   <table>
   date      author                 description
   --------  --------------         ----------
   20100000  syed.malgimani@        Created
   20170110  vijay.nair@            Modified
   </table>

   Description

 */

#include "platform/sip_pf_string.h"
#include "platform/sip_pf_memory.h"

#include "SipTrace.h"
#include "sip_debug.h"
#include "sip_error.h"
#include "txn/SipTxnDb.h"
#include "txn/SipTxnKey.h"
#include "txn/SipTxn.h"

#include "IPAddress.h"

#define SIP_MAX_HASH_BUCKETS     100
#define SIP_MAX_HASH_ENTRIES     4000
#define SIP_CALC_HASH_VAL_A      07777777777
#define SIP_CALC_HASH_VAL_B     3
#define SIP_CALC_HASH_VAL_C     28
#define SIP_CALC_HASH_VAL_D     40
#define SIP_CALC_HASH_VAL_E     0140

extern SIP_VOID sip_cbk_displayTxnKey(IN SIP_VOID* pvTxnKey);

/* Global Variable */
static SipTxnDb *gpObjTxnDb = SIP_NULL;

SIP_UINT32 sipTxnCalculateHash(SIP_VOID *pvStrKey)
{
    if (pvStrKey == SIP_NULL)
    {
        SIP_DEBUG_WARNING(EERR_INVALIDPARAM,"sipCalculateHash: Key is Null",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    /* Prepare Key String = CallId */
    SipTxnKey *pobjTxnKey = (SipTxnKey*) (pvStrKey);
    SIP_CHAR* pcCallId = pobjTxnKey->GetCallId();
    SIP_UINT16 usKeyLen = SipPf_Strlen(pcCallId);

    SIP_CHAR* pcStr = pcCallId;
    SIP_CHAR* pcStrEnd = pcStr + usKeyLen;
    SIP_CHAR cVal = SIP_ZERO;
    SIP_UINT32 uiHash = SIP_ZERO;

    while (pcStr != pcStrEnd)
    {
        cVal = *pcStr;
        pcStr = pcStr + 1;
        if (cVal >= SIP_CALC_HASH_VAL_E)
        {
            cVal = cVal - SIP_CALC_HASH_VAL_D;
        }
        uiHash = ((uiHash<<SIP_CALC_HASH_VAL_B) +
                (uiHash>>SIP_CALC_HASH_VAL_C) + cVal);
    }

    return uiHash & SIP_CALC_HASH_VAL_A;
}

SIP_CHAR sipTxnCompareHashKey
(
    SIP_VOID *pvStoredKey,
    SIP_VOID *pvUserKey
)
{
    if ((pvStoredKey == SIP_NULL) || (pvUserKey == SIP_NULL)) {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN, "sipCompareHashKey: pvStoredKey or pvUserKey is Null",
                SIP_ZERO,SIP_ZERO);
        return SIP_NOT_MATCH;
    }

    SipTxnKey *pobjStoredKey = (SipTxnKey *)pvStoredKey;
    SipTxnKey *pobjUserKey   = (SipTxnKey *)pvUserKey;
    if (pobjUserKey->HasRule(SipTxnKey::RULE_COMPARE_VIA_BRANCH) == SIP_TRUE)
    {
        if (SipPf_Strcmp(pobjStoredKey->GetViaBranchParam(),
                pobjUserKey->GetViaBranchParam()) != SIP_EQUALS) {
            SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                    "sipCompareHashKey: Branch Parameter doesn't match",
                    SIP_ZERO,SIP_ZERO);
            return SIP_NOT_MATCH;
        }
    }

    /* Compare Method for Non-ACK only */
    SIP_CHAR *pucUserMethod = pobjUserKey->GetMethod();
    SIP_BOOL bACKFor2xxSent = SIP_FALSE;

    if (SipPf_Strcmp(ACK_METHOD, pucUserMethod) != SIP_EQUALS) {
        if (SipPf_Strcmp(pobjStoredKey->GetMethod(), pucUserMethod) != SIP_EQUALS) {
            SIP_TRACE_NORMAL(ESIPTRACE_MODTXN, "sipCompareHashKey: CseqMethod doesn't match",
                    SIP_ZERO,SIP_ZERO);
            return SIP_NOT_MATCH;
        }
    } else { //487 retransmission issue.
        if (SipPf_Strcmp(pobjStoredKey->GetMethod(), CANCEL_METHOD) == SIP_ZERO) {
            return SIP_NOT_MATCH;
        }

        // Successful response & received ACK request : always new one
        // Test equipment issue: same transaction key - cseq / via-branch / from-tag / to-tag
        if (pobjUserKey->GetTxnType() == SipTxn::INV_SER_TXN
                && pobjStoredKey->GetTxnType() == SipTxn::INV_SER_TXN)
        {
            SIP_UINT16 nStoredStatusCode = pobjStoredKey->GetRespCode();

            if (SIP_SUCCESSFUL_RESP(nStoredStatusCode))
            {
                bACKFor2xxSent = SIP_TRUE;
            }
        }
    }

    /*Host validation*/
    if (pobjUserKey->HasRule(SipTxnKey::RULE_COMPARE_VIA_BRANCH) == SIP_TRUE)
    {
        IPAddress objStoredHost(AString((CONST IMS_CHAR *)pobjStoredKey->GetViaHost()));
        IPAddress objRecevdHost(AString((CONST IMS_CHAR *)pobjUserKey->GetViaHost()));
        if (objStoredHost.Equals(objRecevdHost) == SIP_FALSE) {
            SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                "sipCompareHashKey: VIA Host doesn't match",SIP_ZERO,SIP_ZERO);
            return SIP_NOT_MATCH;
        }
    }

    if (SipPf_Strcmp(pobjStoredKey->GetFromTag(), pobjUserKey->GetFromTag()) != SIP_EQUALS) {
        SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                "sipCompareHashKey: SentByVal doesn't match",SIP_ZERO,SIP_ZERO);
        return SIP_NOT_MATCH;
    }

    SIP_CHAR *pucStoredToTag = pobjStoredKey->GetToTag();
    SIP_CHAR *pucUserToTag = pobjUserKey->GetToTag();
    if ((pucStoredToTag != SIP_NULL) && (pucUserToTag != SIP_NULL)) {
        if (SipPf_Strcmp(pucStoredToTag,pucUserToTag) != SIP_EQUALS)
        {
            SIP_TRACE_NORMAL(ESIPTRACE_MODTXN, "sipCompareHashKey: SentByVal doesn't match",
                    SIP_ZERO,SIP_ZERO);
            return SIP_NOT_MATCH;
        }
    }

    if (bACKFor2xxSent == SIP_TRUE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "txn-comparison: ACK for 2xx sent has same transaction identifiers",
                SIP_ZERO,SIP_ZERO);
        return SIP_NOT_MATCH;
    }

    return SIP_MATCHES;
}

SIP_VOID sipTxnFreeElement(SIP_VOID *pvHashElem)
{
    SipTxn    *pobjTxn = (SipTxn*)pvHashElem;

    if (pobjTxn != SIP_NULL)
    {
        pobjTxn->SipDelete();
    }

    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
            "sipTxnFreeElement: TxnObject is freed ",SIP_ZERO,SIP_ZERO);
}

/* Default free function to free the key in the hash table */
SIP_VOID sipTxnFreeKey(SIP_VOID *pvHashKey)
{
    SipTxnKey* pobjTxnKey = (SipTxnKey*)pvHashKey;

    if (pobjTxnKey != SIP_NULL)
    {
        sip_cbk_displayTxnKey(pobjTxnKey);
        pobjTxnKey->SipDelete();
    }
}

SipTxnDb::SipTxnDb()
{
    SIP_UINT16    usError = SIP_ZERO;

    m_pObjTxnHash = new SipHash (
            sipTxnCalculateHash,
            sipTxnCompareHashKey,
            sipTxnFreeElement,
            sipTxnFreeKey,
            SIP_MAX_HASH_BUCKETS,
            SIP_MAX_HASH_ENTRIES,
            &usError);
}

SipTxnDb::~SipTxnDb()
{
    delete m_pObjTxnHash;
    m_pObjTxnHash = SIP_NULL;
}

SIP_BOOL SipTxnDb::AddElement
(
    SIP_VOID        *pvElement,
    SIP_VOID     *pvKey,
    SIP_UINT16    *pusError
)
{
    return m_pObjTxnHash->Hash_Add(pvElement,pvKey,pusError);
}

SIP_BOOL SipTxnDb::FetchElement
(
    SIP_VOID         *pvKey,
    SIP_VOID     **ppElement,
    SIP_UINT16     *pusError
)
{
    /* Fetch Element increments Reference count */
    *ppElement = m_pObjTxnHash->Hash_Fetch(pvKey, pusError);
    if (*ppElement == SIP_NULL)
    {
        return SIP_FALSE;
    }
    /* Reduce Reference Count */
    m_pObjTxnHash->Hash_Release(pvKey);
    return SIP_TRUE;
}

SIP_BOOL SipTxnDb::FetchElement
(
    SIP_VOID     *pvKey,
    SIP_VOID     **ppElement,
    SIP_VOID     **ppKey,
    SIP_UINT16   *pusError
)
{
    /* Fetch Element increments Reference count */
    *ppElement = m_pObjTxnHash->Hash_Fetch(pvKey, ppKey, pusError);
    if (*ppElement == SIP_NULL)
    {
        return SIP_FALSE;
    }
    /* Reduce Reference Count */
    m_pObjTxnHash->Hash_Release(pvKey);
    return SIP_TRUE;
}

SIP_BOOL SipTxnDb::RemoveElement
(
    SIP_VOID     *pvKey,
    SIP_UINT16    *pusError
)
{
    return m_pObjTxnHash->Hash_Remove(pvKey, pusError);
}

void SipTxnDb_Construct()
{
    SipTxnDb *pObj = gpObjTxnDb;

    if (pObj)
    {
        return;
    }

    pObj = new SipTxnDb();
    gpObjTxnDb = pObj;
}

void SipTxnDb_Destruct()
{
    SipTxnDb *pObj = gpObjTxnDb;

    if (pObj == SIP_NULL)
    {
        return;
    }

    delete pObj;
    gpObjTxnDb = SIP_NULL;
}

SipTxnDb* SipTxnDb_GetInstance()
{
    SipTxnDb *pObj = gpObjTxnDb;
    return pObj;
}
