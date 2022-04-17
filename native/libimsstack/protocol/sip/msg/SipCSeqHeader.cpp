/******************************************************************************
 * Project Name   : SIP_RTP
 * Group    : IP-CS [MSG-2]
 * Security   : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename      : SipCSeqHeader.cpp
 * Purpose     :
 * Platform      : Windows OR Android
 * Author(s)     :
 * E-mail id.    : saurabh31.srivastava@
 * Creation date   : July. 27, 2010
 *
 * Edit History     Modification         Description(s)
 *
 * Date      Name    Version    Bug-ID    Description
 * ----------    ----------    -------    ------    -------------
 * Month. Date,10    Name       0.0a    Initial creation
 *****************************************************************************/


/*****************************************************************************
  Header Inclusions
 *****************************************************************************/
#include "msg/SipCSeqHeader.h"
#include "sip_error.h"
#include "sip_debug.h"
#include "SipTrace.h"
#include "platform/sip_pf_string.h"
#include "msg/sip_msgutil.h"

/****************************************************************************
  Macro Definitions
 *****************************************************************************/
#define MAX_CSEQ_LEN 12
/****************************************************************************
  Class Member Function Implementations
 *****************************************************************************/

/******************************************************************************
 * Function name  : SipCSeqHeader::SipCSeqHeader
 *
 * Description   :
 *
 * Preconditions  :
 *
 * Side Effects  : none
 *****************************************************************************/
SipCSeqHeader::SipCSeqHeader()
    : SipHeaderBase(SipHeaderBase::CSEQ)
    , m_pszMethod(SIP_NULL)
    , m_uiSeq(SIP_ZERO)
{
}

/******************************************************************************
 * Function name  : SipCSeqHeader::SipCSeqHeader
 *
 * Description   :
 *
 * Preconditions  :
 *
 * Side Effects  : none
 *****************************************************************************/
SipCSeqHeader::SipCSeqHeader(const SipCSeqHeader &objHeader)
    : SipHeaderBase(objHeader)
    , m_pszMethod(SipPf_Strdup(objHeader.m_pszMethod))
    , m_uiSeq(objHeader.m_uiSeq)
{
}
/******************************************************************************
 * Function name  : SipCSeqHeader::~SipCSeqHeader
 *
 * Description   :
 *
 * Preconditions  :
 *
 * Side Effects  : none
 *****************************************************************************/
SipCSeqHeader::~SipCSeqHeader()
{
    if (m_pszMethod != SIP_NULL)
    {
        delete[] m_pszMethod;
    }
}

/******************************************************************************
 * Function name  : SipCSeqHeader::EncodeHdr
 *
 * Description   :
 *
 * Preconditions  :
 *
 * Side Effects  : none
 *****************************************************************************/
SIP_BOOL SipCSeqHeader::EncodeHdr
(
    SIP_CHAR   **ppucCurrPos,
    SIP_BOOL   /*bParams = SIP_TRUE*/
)
{
    if (m_pszMethod == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Missing CSeq Method", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR ucBuf[MAX_CSEQ_LEN];
    SipPf_Sprintf(ucBuf, (SIP_CHAR*)"%u", m_uiSeq);

    SipPf_Strcpy(*ppucCurrPos, ucBuf);
    SipEnc_UpdateCurrPos(ppucCurrPos);

    SIP_ENC_SP(*ppucCurrPos);

    SipPf_Strcpy(*ppucCurrPos, m_pszMethod);
    SipEnc_UpdateCurrPos(ppucCurrPos);

    return SIP_TRUE;
}

/******************************************************************************
 * Function name  : SipCSeqHeader::SetMethod
 *
 * Description   :
 *
 * Preconditions  :
 *
 * Side Effects  : none
 *****************************************************************************/
SIP_BOOL SipCSeqHeader::SetMethod
(
 const SIP_CHAR  *pkszMethod
 )
{
    return SetCharVar(pkszMethod,m_pszMethod);
}

/******************************************************************************
 * Function name      : SipCSeqHeader::DecodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipCSeqHeader::DecodeHdr
(
 SIP_CHAR    *pucStartPt,
 SIP_UINT32  uiDecLen
 )
{
    if (uiDecLen == SIP_ZERO)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Empty buffer", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR *pucEndPt = pucStartPt + uiDecLen - SIP_ONE;
    SIP_CHAR *pucTempPre  = SIP_NULL;

    if (sipFindLWS(pucStartPt,  pucEndPt, &pucTempPre) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "LWS missing in Cseq", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR *pszSeq = sipCreateString(pucStartPt, pucTempPre);
    if (pszSeq == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation Fail", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    if (SipPf_Atoi_IsZero(pszSeq) == SIP_FALSE)
    {
        m_uiSeq = SipPf_Atoi_Unsigned(pszSeq);
        if ((m_uiSeq > MAX_CSEQ) || (m_uiSeq == SIP_ZERO))
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Invalid CSeq Value", SIP_ZERO, SIP_ZERO);
            delete[] pszSeq;
            return SIP_FALSE;
        }
    }
    else
    {
        m_uiSeq = SIP_ZERO;
    }
    delete[] pszSeq;

    pucTempPre = pucTempPre + SIP_ONE;
    pucStartPt = sipSkipFwLWS(pucTempPre, pucEndPt);

    m_pszMethod = sipCreateString(pucStartPt, pucEndPt);
    if (m_pszMethod == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation Fail", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    return SIP_TRUE;
}

SIP_BOOL SipCSeqHeader::IsValidHeader() const
{
    if ((m_pszMethod == SIP_NULL) ||
        ((m_uiSeq > MAX_CSEQ) || (m_uiSeq == SIP_ZERO)))
    {
        return SIP_FALSE;
    }
    return SIP_TRUE;
}

SipHeaderBase* SipCSeqHeader::GetNewObj(SIP_INT32 /*eHdr*/, SipHeaderBase* pHeader)
{
    if (pHeader != SIP_NULL) {
        return new SipCSeqHeader(*reinterpret_cast<SipCSeqHeader*>(pHeader));
    }
    return new SipCSeqHeader();
}
