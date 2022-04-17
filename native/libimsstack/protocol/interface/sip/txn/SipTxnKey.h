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

#ifndef __SIP_TXN_KEY_H__
#define __SIP_TXN_KEY_H__

#include "sip_pf_datatypes.h"
#include "msg/sip_comdef.h"
#include "msg/SipMessage.h"

/* Transaction Key Used to Match Response to Request vice versa */
class SipTxnKey :virtual public SipRefBase
{
private:
    SIP_UINT32 m_uiRSeqNum;

public:
    enum
    {
        RULE_NONE = 0x0000,
        RULE_COMPARE_TO_TAG = 0x0001,
        RULE_COMPARE_VIA_BRANCH = 0x0002,
        RULE_COMPARE_RSEQ = 0x0004
    };

    SIP_INT32 m_eMsgType;

    /* Key Fetched from via */
    SIP_CHAR *m_pszViaBranchParam;
    SIP_CHAR *m_pszViaHost;
    SIP_UINT16 m_usViaHostPort;

    /*
       For Req : Method <== Request Line
       For Resp:Method <==  CSeq
     */
    SIP_CHAR *m_pszMethod;
    SIP_UINT32 m_uiCseqNum;

    /* For INVITE at Receiving Side */
    SipAddrSpec    *m_pobjRequestUri;

    SIP_CHAR *m_pszToTag;
    SIP_CHAR *m_pszFromTag;
    SIP_CHAR *m_pszCallId;
    /*Status code store*/
    SIP_UINT16 m_usRespCode;
    SIP_INT32 m_eTxnType;
    SIP_UINT32 m_nRules;

public:
    SipTxnKey();
    SipTxnKey(SipTxnKey *pobjTxnKey, SIP_UINT16 *pusError);
    SipTxnKey(SipMessage *pobjSipMsg, SIP_UINT16 *pusError);
    virtual ~SipTxnKey();

    SIP_VOID Init(SipTxnKey *pobjTxnKey, SIP_UINT16 *pusError);

    inline void AddRule(SIP_UINT32 nRule)
    { m_nRules |= nRule; }
    inline void RemoveRule(SIP_UINT32 nRule)
    { m_nRules &= (~nRule); }
    inline SIP_BOOL HasRule(SIP_UINT32 nRule) const
    { return ((m_nRules & nRule) != 0) ? SIP_TRUE : SIP_FALSE; }
    inline SIP_UINT32 GetRules() const
    { return m_nRules; }
    inline SIP_VOID SetRules(SIP_UINT32 nRules)
    { m_nRules = nRules; }

    inline SIP_CHAR* GetCallId() const
    { return m_pszCallId; }
    inline SIP_CHAR* GetFromTag() const
    { return m_pszFromTag; }
    inline SIP_CHAR* GetMethod() const
    { return m_pszMethod; }
    inline SipAddrSpec* GetRequestUri() const
    { return m_pobjRequestUri; }
    inline SIP_CHAR* GetToTag() const
    { return m_pszToTag; }
    inline SIP_CHAR* GetViaBranchParam() const
    { return m_pszViaBranchParam; }
    inline SIP_CHAR* GetViaHost() const
    { return m_pszViaHost; }
    inline SIP_UINT32 GetCSeqNum() const
    { return m_uiCseqNum; }
    inline SIP_INT32 GetMsgType() const
    { return m_eMsgType; }
    inline SIP_UINT16 GetViaHostPort() const
    { return m_usViaHostPort; }
    inline SIP_UINT32 GetRSeq() const
    { return m_uiRSeqNum; }
    inline SIP_UINT16 GetRespCode() const
    { return m_usRespCode; }
    inline SIP_INT32 GetTxnType() const
    { return m_eTxnType; }

    SIP_VOID SetCallId(const SIP_CHAR *pszCallId);
    SIP_VOID SetFromTag(const SIP_CHAR *pszFromTag);
    SIP_VOID SetMethod(const SIP_CHAR *pszMethod);
    SIP_VOID SetRequestUri(SipAddrSpec *pobjRequestUri);
    SIP_VOID SetToTag(const SIP_CHAR *pszToTag);
    SIP_VOID SetViaBranchParam(const SIP_CHAR *pszViaBranchParam);
    SIP_VOID SetViaHost(const SIP_CHAR *pszViaHost);
    inline SIP_VOID SetCseqNum(SIP_UINT32 uiCseqNum)
    { m_uiCseqNum = uiCseqNum; }
    inline SIP_VOID SetMsgType(SIP_INT32 eMsgType)
    { m_eMsgType = eMsgType; }
    inline SIP_VOID SetViaHostPort(SIP_UINT16 usViaHostPort)
    { m_usViaHostPort = usViaHostPort; }
    inline SIP_VOID SetRSeq(SIP_UINT32 uiRseqNum)
    { m_uiRSeqNum = uiRseqNum; }
    inline SIP_VOID SetRespCode(SIP_UINT16 uiStatusCode)
    { m_usRespCode = uiStatusCode; }
    inline SIP_VOID SetTxnType(SIP_INT32 eTxnType)
    { m_eTxnType = eTxnType; }

    SIP_INT32 CompareKeys(SipTxnKey* pGeneratedKey);
    SIP_INT32 CompareKeysForRPR(SipTxnKey* pGeneratedKey);

private:
    SIP_VOID Clear();
};

#endif //__SIP_TXN_KEY_H__
