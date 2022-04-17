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

#include "txn/SipTxn.h"
#include "txn/SipTxnKey.h"
#include "SipTrace.h"
#include "sip_error.h"
#include "sip_debug.h"
#include "platform/sip_pf_string.h"
#include "platform/sip_pf_memory.h"

SipTxnKey::SipTxnKey()
    :m_uiRSeqNum (SIP_ZERO), m_eMsgType(SipMessage::TYPE_INVALID),
    m_pszViaBranchParam(SIP_NULL), m_pszViaHost (SIP_NULL), m_usViaHostPort (SIP_ZERO),
    m_pszMethod (SIP_NULL), m_uiCseqNum (SIP_ZERO), m_pobjRequestUri(SIP_NULL),
    m_pszToTag(SIP_NULL), m_pszFromTag(SIP_NULL), m_pszCallId (SIP_NULL),
    m_usRespCode (SIP_ZERO), m_eTxnType(SipTxn::INVALID_TXN),
    m_nRules(RULE_COMPARE_TO_TAG | RULE_COMPARE_VIA_BRANCH)
{
}

SipTxnKey::SipTxnKey(SipTxnKey *pobjTxnKey, SIP_UINT16 *pusError)
    :m_uiRSeqNum (SIP_ZERO), m_eMsgType(SipMessage::TYPE_INVALID),
    m_pszViaBranchParam(SIP_NULL), m_pszViaHost (SIP_NULL), m_usViaHostPort (SIP_ZERO),
    m_pszMethod (SIP_NULL), m_uiCseqNum (SIP_ZERO), m_pobjRequestUri(SIP_NULL),
    m_pszToTag(SIP_NULL), m_pszFromTag(SIP_NULL), m_pszCallId (SIP_NULL),
    m_usRespCode (SIP_ZERO), m_eTxnType(SipTxn::INVALID_TXN),
    m_nRules(RULE_COMPARE_TO_TAG | RULE_COMPARE_VIA_BRANCH)
{
    Init(pobjTxnKey, pusError);
}

SipTxnKey::SipTxnKey(SipMessage *pobjSipMsg, SIP_UINT16 *pusError)
    :m_uiRSeqNum(SIP_ZERO), m_eMsgType(SipMessage::TYPE_INVALID),
    m_pszViaBranchParam(SIP_NULL), m_pszViaHost (SIP_NULL), m_usViaHostPort (SIP_ZERO),
    m_pszMethod (SIP_NULL), m_uiCseqNum (SIP_ZERO), m_pobjRequestUri(SIP_NULL),
    m_pszToTag(SIP_NULL), m_pszFromTag(SIP_NULL), m_pszCallId (SIP_NULL),
    m_usRespCode (SIP_ZERO), m_eTxnType(SipTxn::INVALID_TXN),
    m_nRules(RULE_COMPARE_TO_TAG | RULE_COMPARE_VIA_BRANCH)
{
    m_eMsgType = pobjSipMsg->GetMsgType();

    if (m_eMsgType == SipMessage::TYPE_INVALID)
    {
        *pusError = EMSGERR_INVALIDMSGTYPE;
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SipTxnKey Constructor: Invalid Txn\n",SIP_ZERO,SIP_ZERO);
        return;
    }

    /* Fetch Branch : Branch can be null */
    SipViaHeader *pobjViaHdr = (SipViaHeader*)pobjSipMsg->GetHdrObj(SipHeaderBase::VIA);
    if (pobjViaHdr == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SipTxnKey Constructor: ViaHdr is NULL Txn\n",SIP_ZERO,SIP_ZERO);
        return;
    }
    m_pszViaBranchParam = const_cast<SIP_CHAR*>(pobjViaHdr->GetBranch());

    /* Fetch Host : Host cannot be null */
    m_pszViaHost = SipPf_Strdup(pobjViaHdr->GetHost());

    if (m_pszViaHost == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SipTxnKey Constructor: No m_pszViaHost \n",SIP_ZERO,SIP_ZERO);
        return;
    }

    /* Fetch Port : Port cannot be 0 */
    m_usViaHostPort = pobjViaHdr->GetPort();
    pobjViaHdr->SipDelete();

    /* Fetch based on Message Type */
    if (m_eMsgType == SipMessage::REQ_TYPE)
    {
        SipAddrSpec *pobjAddrSpec = SIP_NULL;
        /* Fetch Request URI */
        SipRequestLine *pobjReqLine = pobjSipMsg->GetReqLine();
        if (pobjReqLine == SIP_NULL)
        {
            *pusError = EMSGERR_REQLINEMISSING;
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "SipTxnKey::SipTxnKey(pobjTxnKey): EMSGERR_REQLINEMISSING",
                    SIP_ZERO,SIP_ZERO);
            return;
        }
        pobjAddrSpec = pobjReqLine->GetReqUri();
        pobjReqLine->SipDelete();

        m_pobjRequestUri = new SipAddrSpec(*pobjAddrSpec);
        pobjAddrSpec->SipDelete();

        if (m_pobjRequestUri == SIP_NULL)
        {
            *pusError = E_ERR_PF_MALLOCFAILED;
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "SipTxnKey::SipTxnKey(pobjTxnKey): Memory Allocation Failed",
                    SIP_ZERO,SIP_ZERO);
            return;
        }

        /* Fetch Method */
        m_pszMethod = SipPf_Strdup(pobjReqLine->GetMethod());

        /* Fetch CSeq Num */
        SipCSeqHeader *pobjCseqHdr = (SipCSeqHeader *)pobjSipMsg->GetHdrObj(SipHeaderBase::CSEQ);
        if (pobjCseqHdr != SIP_NULL)
        {
            m_uiCseqNum = pobjCseqHdr->GetCSeq();
            pobjCseqHdr->SipDelete();
        }
    }
    else
    {
        /* Fetch Method */
        SipCSeqHeader *pobjCseqHdr = (SipCSeqHeader *)pobjSipMsg->GetHdrObj(SipHeaderBase::CSEQ);
        if (pobjCseqHdr != SIP_NULL)
        {
            m_pszMethod = SipPf_Strdup(pobjCseqHdr->GetMethod());

            /* Fetch CSeq Num */
            m_uiCseqNum = pobjCseqHdr->GetCSeq();
            pobjCseqHdr->SipDelete();
        }
        m_usRespCode = pobjSipMsg->GetStatusCode();
    }

    /* Fetch To-Tag */
    SipToHeader *pobjToHdr = (SipToHeader *)pobjSipMsg->GetHdrObj(SipHeaderBase::TO);
    if (pobjToHdr == SIP_NULL)
    {
        *pusError = EMSGERR_TOMISSING;
        return;
    }
    m_pszToTag = pobjToHdr->GetTag();

    pobjToHdr->SipDelete();

    /* Fetch From-Tag */
    SipFromHeader *pobjFromHdr = (SipFromHeader*)pobjSipMsg->GetHdrObj(SipHeaderBase::FROM);
    if (pobjFromHdr == SIP_NULL)
    {
        *pusError = EMSGERR_FROMMISSING;
        return;
    }
    m_pszFromTag = pobjFromHdr->GetTag();

    pobjFromHdr->SipDelete();

    /* Fetch Call-ID */
    SipHeaderBase *pobjCallHdr = pobjSipMsg->GetHdrObj(SipHeaderBase::CALL_ID);
    if (pobjCallHdr != SIP_NULL)
    {
        m_pszCallId = SipPf_Strdup(pobjCallHdr->GetValue());
        pobjCallHdr->SipDelete();
    }

    SipRSeqHeader *pobjRSeqhdr = (SipRSeqHeader *)pobjSipMsg->GetHdrObj(SipHeaderBase::RSEQ);
    if (pobjRSeqhdr != SIP_NULL)
    {
        SetRSeq(pobjRSeqhdr->GetRSeqValue());
        pobjRSeqhdr->SipDelete();
    }
}

SipTxnKey::~SipTxnKey()
{
    Clear();
}

SIP_VOID SipTxnKey::Init(SipTxnKey *pobjTxnKey, SIP_UINT16 *pusError)
{
    if (pobjTxnKey == SIP_NULL)
    {
        return;
    }

    Clear();

    m_uiRSeqNum = pobjTxnKey->m_uiRSeqNum;
    m_eMsgType = pobjTxnKey->m_eMsgType;
    m_usRespCode = pobjTxnKey->m_usRespCode;
    m_eTxnType = pobjTxnKey->m_eTxnType;
    m_nRules = pobjTxnKey->m_nRules;

    m_pszViaBranchParam = (SIP_CHAR *)SipPf_Strdup(
            (SIP_CHAR *)pobjTxnKey->m_pszViaBranchParam);
    if (m_pszViaBranchParam == SIP_NULL)
    {
        *pusError = E_ERR_PF_MALLOCFAILED;
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SipTxnKey::Init: Memory Allocation Failed",
                SIP_ZERO,SIP_ZERO);
        return;
    }
    m_pszViaHost = (SIP_CHAR *)SipPf_Strdup(
            (SIP_CHAR *)pobjTxnKey->m_pszViaHost);
    if (m_pszViaHost == SIP_NULL)
    {
        *pusError = E_ERR_PF_MALLOCFAILED;
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SipTxnKey::Init: Memory Allocation Failed",
                SIP_ZERO,SIP_ZERO);
        delete[] m_pszViaBranchParam;
        m_pszViaBranchParam = SIP_NULL;
        return;
    }
    m_usViaHostPort = pobjTxnKey->m_usViaHostPort;
    m_pszMethod = (SIP_CHAR *)SipPf_Strdup(
            (SIP_CHAR *)pobjTxnKey->m_pszMethod);
    if (m_pszMethod == SIP_NULL)
    {
        *pusError = E_ERR_PF_MALLOCFAILED;
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SipTxnKey::Init: Memory Allocation Failed",
                SIP_ZERO,SIP_ZERO);
        delete[] m_pszViaHost;
        m_pszViaHost = SIP_NULL;
        delete[] m_pszViaBranchParam;
        m_pszViaBranchParam = SIP_NULL;
        return;
    }

    m_uiCseqNum = pobjTxnKey->m_uiCseqNum;
    m_pobjRequestUri = new SipAddrSpec(*(pobjTxnKey->m_pobjRequestUri));
    if (m_pobjRequestUri == SIP_NULL)
    {
        *pusError = E_ERR_PF_MALLOCFAILED;
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SipTxnKey::Init: Memory Allocation Failed",
                SIP_ZERO,SIP_ZERO);
        delete[] m_pszMethod;
        m_pszMethod = SIP_NULL;
        delete[] m_pszViaHost;
        m_pszViaHost = SIP_NULL;
        delete[] m_pszViaBranchParam;
        m_pszViaBranchParam = SIP_NULL;
        return;
    }

    if (pobjTxnKey->m_pszToTag != SIP_NULL)
    {
        m_pszToTag = (SIP_CHAR *)SipPf_Strdup((SIP_CHAR *)pobjTxnKey->m_pszToTag);
        if (m_pszToTag == SIP_NULL)
        {
            *pusError = E_ERR_PF_MALLOCFAILED;
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "SipTxnKey::Init: Memory Allocation Failed",
                    SIP_ZERO,SIP_ZERO);
            delete[] m_pszMethod;
            m_pszMethod = SIP_NULL;
            delete[] m_pszViaHost;
            m_pszViaHost = SIP_NULL;
            delete[] m_pszViaBranchParam;
            m_pszViaBranchParam = SIP_NULL;
            delete m_pobjRequestUri;
            m_pobjRequestUri = SIP_NULL;
            return;
        }
    }

    m_pszFromTag = (SIP_CHAR *)SipPf_Strdup(
            (SIP_CHAR *)pobjTxnKey->m_pszFromTag);
    if (m_pszFromTag == SIP_NULL)
    {
        *pusError = E_ERR_PF_MALLOCFAILED;
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SipTxnKey::Init: Memory Allocation Failed",
                SIP_ZERO,SIP_ZERO);
        delete[] m_pszToTag;
        m_pszToTag = SIP_NULL;
        delete[] m_pszMethod;
        m_pszMethod = SIP_NULL;
        delete[] m_pszViaHost;
        m_pszViaHost = SIP_NULL;
        delete[] m_pszViaBranchParam;
        m_pszViaBranchParam = SIP_NULL;
        delete m_pobjRequestUri;
        m_pobjRequestUri = SIP_NULL;
        return;
    }
    m_pszCallId = (SIP_CHAR *)SipPf_Strdup(
            (SIP_CHAR *)pobjTxnKey->m_pszCallId);
    if (m_pszCallId == SIP_NULL)
    {
        *pusError = E_ERR_PF_MALLOCFAILED;
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SipTxnKey::Init: Memory Allocation Failed",
                SIP_ZERO,SIP_ZERO);
        delete[] m_pszFromTag;
        m_pszFromTag = SIP_NULL;
        delete[] m_pszToTag;
        m_pszToTag = SIP_NULL;
        delete[] m_pszMethod;
        m_pszMethod = SIP_NULL;
        delete[] m_pszViaHost;
        m_pszViaHost = SIP_NULL;
        delete[] m_pszViaBranchParam;
        m_pszViaBranchParam = SIP_NULL;
        delete m_pobjRequestUri;
        m_pobjRequestUri = SIP_NULL;
        return;
    }
}

SIP_VOID SipTxnKey::SetCallId(const SIP_CHAR *pszCallId)
{
    if (m_pszCallId != SIP_NULL)
    {
        delete[] m_pszCallId;
    }

    m_pszCallId = (SIP_CHAR *)SipPf_Strdup(pszCallId);
}

SIP_VOID SipTxnKey::SetFromTag(const SIP_CHAR *pszFromTag)
{
    if (m_pszFromTag != SIP_NULL)
    {
        delete[] m_pszFromTag;
    }

    m_pszFromTag = (SIP_CHAR *)SipPf_Strdup(pszFromTag);
}

SIP_VOID SipTxnKey::SetMethod(const SIP_CHAR *pszMethod)
{
    if (m_pszMethod != SIP_NULL)
    {
        delete[] m_pszMethod;
    }

    m_pszMethod = (SIP_CHAR *)SipPf_Strdup(pszMethod);
}

SIP_VOID SipTxnKey::SetRequestUri(SipAddrSpec *pobjRequestUri)
{
    if (m_pobjRequestUri != SIP_NULL)
    {
        m_pobjRequestUri->SipDelete();
    }

    m_pobjRequestUri = pobjRequestUri;
}

SIP_VOID SipTxnKey::SetToTag(const SIP_CHAR *pszToTag)
{
    if (m_pszToTag != SIP_NULL)
    {
        delete[] m_pszToTag;
    }

    m_pszToTag = (SIP_CHAR *)SipPf_Strdup(pszToTag);
}

SIP_VOID SipTxnKey::SetViaBranchParam(const SIP_CHAR *pszViaBranchParam)
{
    if (m_pszViaBranchParam != SIP_NULL)
    {
        delete[] m_pszViaBranchParam;
    }

    m_pszViaBranchParam = (SIP_CHAR *)SipPf_Strdup(pszViaBranchParam);
}

SIP_VOID SipTxnKey::SetViaHost(const SIP_CHAR *pszViaHost)
{
    if (m_pszViaHost != SIP_NULL)
    {
        delete[] m_pszViaHost;
    }

    m_pszViaHost = (SIP_CHAR *)SipPf_Strdup(pszViaHost);
}

SIP_INT32 SipTxnKey::CompareKeys(SipTxnKey* pGeneratedKey)
{
    if (pGeneratedKey == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "txn-comparison: GeneratedKey is null", SIP_ZERO, SIP_ZERO);
        return SIP_NOT_MATCH;
    }

    if (pGeneratedKey->HasRule(RULE_COMPARE_VIA_BRANCH) == SIP_TRUE)
    {
        if (SipPf_Stricmp(m_pszViaBranchParam,
                pGeneratedKey->m_pszViaBranchParam) != SIP_EQUALS)
        {
            SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                    "txn-comparison: not-match >> Via branch", SIP_ZERO, SIP_ZERO);
            return SIP_NOT_MATCH;
        }
    }

    if (m_uiCseqNum != pGeneratedKey->m_uiCseqNum)
    {
        SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                "txn-comparison: not-match >> CSeq (s:%d, g:%d)",
                m_uiCseqNum, pGeneratedKey->m_uiCseqNum);
        return SIP_NOT_MATCH;
    }

    SIP_BOOL bACKFor2xxSent = SIP_FALSE;

    if (SipPf_Strcmp(ACK_METHOD, pGeneratedKey->m_pszMethod) != SIP_EQUALS)
    {
        if (SipPf_Strcmp(m_pszMethod, pGeneratedKey->m_pszMethod) != SIP_EQUALS)
        {
            SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                    "txn-comparison: not-match >> method", SIP_ZERO, SIP_ZERO);
            return SIP_NOT_MATCH;
        }
    }
    else
    {
        // 487 retransmission case
        if (SipPf_Strcmp(m_pszMethod, CANCEL_METHOD) == SIP_EQUALS)
        {
            return SIP_NOT_MATCH;
        }

        // Successful response & received ACK request : always new one
        // Test equipment issue: same transaction key - cseq / via-branch / from-tag / to-tag
        if (pGeneratedKey->m_eTxnType == SipTxn::INV_SER_TXN
                && m_eTxnType == SipTxn::INV_SER_TXN)
        {
            if (SIP_SUCCESSFUL_RESP(m_usRespCode))
            {
                bACKFor2xxSent = SIP_TRUE;
            }
        }
    }

    if (SipPf_Strcmp(m_pszCallId, pGeneratedKey->m_pszCallId) != SIP_EQUALS)
    {
        SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                "txn-comparison: not-match >> call-id", SIP_ZERO, SIP_ZERO);
        return SIP_NOT_MATCH;
    }

    if (SipPf_Stricmp(m_pszFromTag, pGeneratedKey->m_pszFromTag) != SIP_EQUALS)
    {
        SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                "txn-comparison: not-match >> from-tag", SIP_ZERO, SIP_ZERO);
        return SIP_NOT_MATCH;
    }

    if (HasRule(RULE_COMPARE_TO_TAG) == SIP_TRUE
            && pGeneratedKey->HasRule(RULE_COMPARE_TO_TAG) == SIP_TRUE)
    {
        if (m_pszToTag != SIP_NULL && pGeneratedKey->m_pszToTag != SIP_NULL)
        {
            if (SipPf_Stricmp(m_pszToTag, pGeneratedKey->m_pszToTag) != SIP_EQUALS)
            {
                SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                        "txn-comparison: not-match >> to-tag", SIP_ZERO, SIP_ZERO);
                return SIP_NOT_MATCH;
            }
        }
        else
        {
        }
    }

    if (pGeneratedKey->HasRule(RULE_COMPARE_RSEQ) == SIP_TRUE)
    {
        if (m_uiRSeqNum != SIP_ZERO && pGeneratedKey->m_uiRSeqNum != SIP_ZERO)
        {
            if (m_uiRSeqNum != pGeneratedKey->m_uiRSeqNum)
            {
                SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                        "txn-comparison: not-match >> RSeq (s:%d, g:%d)",
                        m_uiRSeqNum, pGeneratedKey->m_uiRSeqNum);
                return SIP_NOT_MATCH;
            }
        }
    }

    if (bACKFor2xxSent == SIP_TRUE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "txn-comparison: ACK for 2xx sent has same transaction identifiers",
                SIP_ZERO, SIP_ZERO);
        return SIP_NOT_MATCH;
    }

    return SIP_MATCHES;
}

SIP_INT32 SipTxnKey::CompareKeysForRPR(SipTxnKey* pGeneratedKey)
{
    if (pGeneratedKey == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "rpr-txn-comparison: GeneratedKey is null", SIP_ZERO, SIP_ZERO);
        return SIP_NOT_MATCH;
    }

    if (SipPf_Strcmp(m_pszMethod, INVITE_METHOD) != SIP_EQUALS)
    {
        // Check if this condition is required or not...
        return SIP_NOT_MATCH;
    }

    if (SipPf_Strcmp(m_pszCallId, pGeneratedKey->m_pszCallId) != SIP_EQUALS)
    {
        SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                "rpr-txn-comparison: not-match >> call-id", SIP_ZERO, SIP_ZERO);
        return SIP_NOT_MATCH;
    }

    if (SipPf_Stricmp(m_pszFromTag, pGeneratedKey->m_pszFromTag) != SIP_EQUALS)
    {
        SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                "rpr-txn-comparison: not-match >> from-tag", SIP_ZERO, SIP_ZERO);
        return SIP_NOT_MATCH;
    }

    if (pGeneratedKey->HasRule(RULE_COMPARE_TO_TAG) == SIP_TRUE)
    {
        if (m_pszToTag != SIP_NULL && pGeneratedKey->m_pszToTag != SIP_NULL)
        {
            if (SipPf_Stricmp(m_pszToTag, pGeneratedKey->m_pszToTag) != SIP_EQUALS)
            {
                SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                        "rpr-txn-comparison: not-match >> to-tag", SIP_ZERO, SIP_ZERO);
                return SIP_NOT_MATCH;
            }
        }
    }

    if (pGeneratedKey->HasRule(RULE_COMPARE_RSEQ) == SIP_TRUE)
    {
        if (m_uiRSeqNum != SIP_ZERO && pGeneratedKey->m_uiRSeqNum != SIP_ZERO)
        {
            if (m_uiRSeqNum != pGeneratedKey->m_uiRSeqNum)
            {
                SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                        "rpr-txn-comparison: not-match >> RSeq (s:%d, g:%d)",
                        m_uiRSeqNum, pGeneratedKey->m_uiRSeqNum);
                return SIP_NOT_MATCH;
            }
        }
    }

    return SIP_MATCHES;
}

SIP_VOID SipTxnKey::Clear()
{
    if (m_pszViaBranchParam != SIP_NULL)
    {
        delete[] m_pszViaBranchParam;
        m_pszViaBranchParam = SIP_NULL;
    }
    if (m_pszViaHost != SIP_NULL)
    {
        delete[] m_pszViaHost;
        m_pszViaHost = SIP_NULL;
    }
    if (m_pszMethod != SIP_NULL)
    {
        delete[] m_pszMethod;
        m_pszMethod = SIP_NULL;
    }
    if (m_pobjRequestUri != SIP_NULL)
    {
        m_pobjRequestUri->SipDelete();
        m_pobjRequestUri = SIP_NULL;
    }
    if (m_pszToTag != SIP_NULL)
    {
        delete[] m_pszToTag;
        m_pszToTag = SIP_NULL;
    }
    if (m_pszFromTag != SIP_NULL)
    {
        delete[] m_pszFromTag;
        m_pszFromTag = SIP_NULL;
    }
    if (m_pszCallId != SIP_NULL)
    {
        delete[] m_pszCallId;
        m_pszCallId = SIP_NULL;
    }

    m_uiRSeqNum = SIP_ZERO;
    m_eMsgType = SipMessage::TYPE_INVALID;
    m_usViaHostPort = SIP_ZERO;
    m_uiCseqNum = SIP_ZERO;
    m_usRespCode = SIP_ZERO;
    m_eTxnType = SipTxn::INVALID_TXN;
    m_nRules = RULE_COMPARE_TO_TAG | RULE_COMPARE_VIA_BRANCH;
}
