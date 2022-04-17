/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipPrivacyHeader.cpp
 * Purpose               :
 * Platform              : Windows OR Android
 * Author(s)           :
 * E-mail id.            : saurabh31.srivastava@
 * Creation date       : July. 27, 2010
 *
 * Edit History             Modification                         Description(s)
 *
 * Date                Name            Version        Bug-ID        Description
 * ----------        ----------        -------        ------        -------------
 * Month. Date,10        Name                 0.0a            Initial creation
 *****************************************************************************/


/*****************************************************************************
  Header Inclusions
 *****************************************************************************/
#include "msg/SipPrivacyHeader.h"
#include "SipTrace.h"
#include "sip_debug.h"
#include "msg/sip_msgutil.h"
#include "platform/sip_pf_string.h"

/****************************************************************************
  Macro Definitions
 *****************************************************************************/

/****************************************************************************
  Class Member Function Implementations
 *****************************************************************************/
/******************************************************************************
 * Function name      : SipPrivacyHeader::SipPrivacyHeader
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipPrivacyHeader::SipPrivacyHeader()
    : SipHeaderBase(SipHeaderBase::PRIVACY)
    , m_objPrivacyList(SipVector<SIP_CHAR*>())
{
}

/******************************************************************************
 * Function name      : SipPrivacyHeader::SipPrivacyHeader
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipPrivacyHeader::SipPrivacyHeader(const SipPrivacyHeader &objHeader)
    : SipHeaderBase(objHeader)
    , m_objPrivacyList(SipVector<SIP_CHAR*>())
{
    SIP_UINT32 usSize = objHeader.m_objPrivacyList.GetSize();
    for(SIP_UINT32 usCount = SIP_ZERO; usCount < usSize ; usCount++)
    {
        SIP_CHAR *pTempVal = objHeader.m_objPrivacyList.GetAt(usCount);
        if (pTempVal != SIP_NULL)
        {
            SIP_CHAR* pobjVal = SipPf_Strdup(pTempVal);
            m_objPrivacyList.Add(pobjVal);
        }
    }
}

/******************************************************************************
 * Function name      : SipPrivacyHeader::~SipPrivacyHeader
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipPrivacyHeader::~SipPrivacyHeader()
{
    while(m_objPrivacyList.IsEmpty() != SIP_TRUE)
    {
        delete m_objPrivacyList.Top();
        m_objPrivacyList.Pop();
    }
}

/******************************************************************************
 * Function name      : SipPrivacyHeader::EncodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipPrivacyHeader::EncodeHdr
(
    SIP_CHAR   **ppucCurrPos,
    SIP_BOOL   /*bParams = SIP_TRUE*/
 )
{

    SIP_UINT32 usCount = m_objPrivacyList.GetSize();

    if (usCount == 0)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Empty privacy values", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    for(SIP_UINT32 usIndex = SIP_ZERO; usIndex < usCount; usIndex++)
    {
        SIP_CHAR *pucPrivacy = m_objPrivacyList.GetAt(usIndex);
        if (usIndex != SIP_ZERO)
        {
            SIP_ENC_SEMI(*ppucCurrPos);
        }
        SipPf_Strcpy(*ppucCurrPos, pucPrivacy);
        SipEnc_UpdateCurrPos(ppucCurrPos);
    }
    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipPrivacyHeader::DecodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
/*SIP_BOOL SipPrivacyHeader::DecodeHdr
  (
  SIP_CHAR    *pucStartPt,
  SIP_UINT32  uiDecLen
  )
  {
  return SIP_TRUE;
  }*/


/******************************************************************************
 * Function name      : SipPrivacyHeader::SetPrivacy
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipPrivacyHeader::AddPrivacy
(
 const SIP_CHAR  *pszPrivacy
 )
{
    SIP_CHAR* pcPrivacy =  SIP_NULL;
    if (SetCharVar(pszPrivacy,pcPrivacy) == SIP_TRUE)
    {
        return ((m_objPrivacyList.Add(pcPrivacy) >= 0) ? SIP_TRUE : SIP_FALSE);
    }
    return SIP_FALSE;
}

/******************************************************************************
 * Function name      : SipMinSEHeader::DecodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipPrivacyHeader::DecodeHdr
(
 SIP_CHAR    *pucStartPt,
 SIP_UINT32  uiDecLen
 )
{
    /*"Privacy" HCOLON priv-value *(";" priv-value)*/
    if (uiDecLen == SIP_ZERO)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipPrivacyHeader::DecodeHdr:Privacy Value Missing",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR *pucEndPt = pucStartPt + uiDecLen - SIP_ONE;
    while(pucStartPt < pucEndPt)
    {
        SIP_CHAR *pucTempPos= SIP_NULL;

        if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, SIP_SEMI) == SIP_FALSE)
        {
            pucTempPos = pucEndPt;
        }

        SIP_CHAR *pszPrivacy = sipCreateString(pucStartPt, pucTempPos);
        if (pszPrivacy == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipPrivacyHeader::DecodeHdr:Memory Allocation failed",
                    SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }
        /*Put the value into list*/
        if (m_objPrivacyList.Add(pszPrivacy) < SIP_ZERO)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipPrivacyHeader::DecodeHdr:Adding in list failed",
                    SIP_ZERO,SIP_ZERO);
            delete[] pszPrivacy;
            return SIP_FALSE;
        }

        if (pucTempPos == pucEndPt)
        {
            break;
        }
        else
        {
            pucStartPt = pucTempPos + SIP_TWO;
            pucStartPt = sipSkipFwLWS(pucStartPt, pucEndPt);
            if (pucStartPt > pucEndPt)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                        "SipParameterList::DecHdrSipParameterList: No Parameter Present",
                        SIP_ZERO,SIP_ZERO);
                return SIP_FALSE;
            }
        }
    }

    return SIP_TRUE;
}

SipHeaderBase* SipPrivacyHeader::GetNewObj(SIP_INT32 /*eHdr*/, SipHeaderBase* pHeader)
{
    if (pHeader != SIP_NULL) {
        return new SipPrivacyHeader(*reinterpret_cast<SipPrivacyHeader*>(pHeader));
    }
    return new SipPrivacyHeader();
}
