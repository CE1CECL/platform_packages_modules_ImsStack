/******************************************************************************
 * Project Name   : SIP_RTP
 * Group    : IP-CS [MSG-2]
 * Security   : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename      : SipAcceptHeader.cpp
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
#include "msg/SipAcceptHeader.h"
#include "sip_error.h"
#include "sip_debug.h"
#include "SipTrace.h"
#include "platform/sip_pf_string.h"
#include "SipConfiguration.h"
#include "msg/sip_msgutil.h"

/****************************************************************************
  Macro Definitions
 *****************************************************************************/

/****************************************************************************
  Class Member Function Implementations
 *****************************************************************************/

/******************************************************************************
 * Function name  : SipAcceptHeader::SipAcceptHeader
 *
 * Description   :
 *
 * Preconditions  :
 *
 * Side Effects  : none
 *****************************************************************************/
SipAcceptHeader::SipAcceptHeader()
    : SipHeaderBase(SipHeaderBase::ACCEPT)
    , m_pszMType(SIP_NULL)
    , m_pszMSubType(SIP_NULL)
{
}

/******************************************************************************
 * Function name  : SipAcceptHeader::SipAcceptHeader
 *
 * Description   :
 *
 * Preconditions  :
 *
 * Side Effects  : none
 *****************************************************************************/
SipAcceptHeader::SipAcceptHeader(const SipAcceptHeader &objHeader)
    : SipHeaderBase(objHeader)
    , m_pszMType(SipPf_Strdup(objHeader.m_pszMType))
    , m_pszMSubType(SipPf_Strdup(objHeader.m_pszMSubType))
{
}

/******************************************************************************
 * Function name  : SipAcceptHeader::~SipAcceptHeader
 *
 * Description   :
 *
 * Preconditions  :
 *
 * Side Effects  : none
 *****************************************************************************/
SipAcceptHeader::~SipAcceptHeader()
{
    if (m_pszMType != SIP_NULL)
    {
        delete[] m_pszMType;
    }

    if (m_pszMSubType != SIP_NULL)
    {
        delete[] m_pszMSubType;
    }
}

/******************************************************************************
 * Function name  : SipAcceptHeader::EncodeHdr
 *
 * Description   :
 *
 * Preconditions  :
 *
 * Side Effects  : none
 *****************************************************************************/
SIP_BOOL SipAcceptHeader::EncodeHdr
(
    SIP_CHAR   **ppucCurrPos,
    SIP_BOOL   bParams /*Default = SIP_TRUE*/
)
{
    if ((m_pszMType == SIP_NULL) && (m_pszMSubType == SIP_NULL))
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "No MType and MSubType", SIP_ZERO, SIP_ZERO);
        return SIP_TRUE;
    }

    if ((m_pszMType == SIP_NULL) || (m_pszMSubType == SIP_NULL))
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Missing MType or MSubType", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SipPf_Strcpy(*ppucCurrPos,m_pszMType);
    SipEnc_UpdateCurrPos(ppucCurrPos);

    SIP_ENC_SLASH(*ppucCurrPos);

    SipPf_Strcpy(*ppucCurrPos,m_pszMSubType);
    SipEnc_UpdateCurrPos(ppucCurrPos);

    return EncodeHeaderParameters(ppucCurrPos,bParams);
}


/******************************************************************************
 * Function name  : SipAcceptHeader::SetMediaType
 *
 * Description   :
 *
 * Preconditions  :
 *
 * Side Effects  : none
 *****************************************************************************/
SIP_BOOL SipAcceptHeader::SetMediaType
(
 const SIP_CHAR  *pkszMType
 )
{
    return SetCharVar(pkszMType,m_pszMType);
}

/******************************************************************************
 * Function name  : SipAcceptHeader::SetMediaSubType
 *
 * Description   :
 *
 * Preconditions  :
 *
 * Side Effects  : none
 *****************************************************************************/
SIP_BOOL SipAcceptHeader::SetMediaSubType
(
 const SIP_CHAR  *pkszMSubType
 )
{
    return SetCharVar(pkszMSubType,m_pszMSubType);
}

/******************************************************************************
 * Function name      : SipAcceptHeader::DecodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipAcceptHeader::DecodeHdr
(
 SIP_CHAR    *pucStartPt,
 SIP_UINT32  uiDecLen
 )
{
   if (uiDecLen == SIP_ZERO)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Empty buffer", SIP_ZERO, SIP_ZERO);
        return SIP_TRUE;
    }

    SIP_CHAR *pucEndPt = pucStartPt + uiDecLen - SIP_ONE;
    SIP_CHAR *pucTempPre = SIP_NULL;
    SIP_CHAR *pucTempNext = SIP_NULL;
    /*Find the SLASH*/
    if (sipFindActualPos(pucStartPt, pucEndPt, &pucTempPre, &pucTempNext, SLASH) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "SLASH missing in Accept", SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    m_pszMType = sipCreateString(pucStartPt, pucTempPre);
    if (m_pszMType == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation Fail", SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    pucStartPt = pucTempNext;
    pucTempNext  = SIP_NULL;
    pucTempPre  = SIP_NULL;

    if (sipFindActualPos(pucStartPt, pucEndPt, &pucTempPre, &pucTempNext, SIP_SEMI) == SIP_FALSE)
    {
        pucTempPre = pucEndPt;
    }

    m_pszMSubType = sipCreateString(pucStartPt, pucTempPre);
    if (m_pszMSubType == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation Failed", SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    if (pucTempNext != SIP_NULL)
    {
        return DecodeHeaderParameters(pucTempNext, pucEndPt, SIP_SEMI);
    }
    return SIP_TRUE;
}

SIP_BOOL SipAcceptHeader::IsValidHeader() const
{
    if (((m_pszMType == SIP_NULL) && (m_pszMSubType == SIP_NULL)) ||
        ((m_pszMType != SIP_NULL) && (m_pszMSubType != SIP_NULL)))
    {
         return SIP_TRUE;
    }
    return SIP_FALSE;
}

SipHeaderBase* SipAcceptHeader::GetNewObj(SIP_INT32 /*eHdr*/, SipHeaderBase *pHeader)
{
    if (pHeader != SIP_NULL) {
        return new SipAcceptHeader(*reinterpret_cast<SipAcceptHeader*>(pHeader));
    }
    return new SipAcceptHeader();
}
