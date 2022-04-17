/******************************************************************************
 * Project Name   : SIP_RTP
 * Group    : IP-CS [MSG-2]
 * Security   : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename      : SipAcceptResourcePriorityHeader.cpp
 * Purpose     :
 * Platform      : Windows OR Android
 * Author(s)     : Saurabh Srivastava
 * E-mail id.    : saurabh31.srivastava@
 * Creation date   : April . 20, 2011
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
#include "msg/SipAcceptResourcePriorityHeader.h"
#include "sip_error.h"
#include "sip_debug.h"
#include "SipTrace.h"
#include "platform/sip_pf_string.h"
#include "msg/sip_msgutil.h"

/****************************************************************************
  Macro Definitions
 *****************************************************************************/
/****************************************************************************
  Class Member Function Implementations
 *****************************************************************************/

/******************************************************************************
 * Function name  : SipAcceptResourcePriorityHeader::SipAcceptResourcePriorityHeader
 *
 * Description   : Default Constructor for SipAcceptResourcePriorityHeader Class
 *
 * Preconditions  :
 *
 * Side Effects  : none
 *****************************************************************************/
SipAcceptResourcePriorityHeader::SipAcceptResourcePriorityHeader()
    : SipHeaderBase(SipHeaderBase::ACCEPT_RESOURCE_PRIORITY)
    , m_pszNameSpace(SIP_NULL)
    , m_pszRPriority(SIP_NULL)
{
}

/******************************************************************************
 * Function name  : SipAcceptResourcePriorityHeader::SipAcceptResourcePriorityHeader
 *
 * Description   : Copy Constructor for SipAcceptResourcePriorityHeader Class
 *
 * Preconditions  :
 *
 * Side Effects  : none
 *****************************************************************************/
SipAcceptResourcePriorityHeader::SipAcceptResourcePriorityHeader(
        const SipAcceptResourcePriorityHeader &objHeader)
    : SipHeaderBase(objHeader)
    , m_pszNameSpace(SipPf_Strdup(objHeader.m_pszNameSpace))
    , m_pszRPriority(SipPf_Strdup(objHeader.m_pszRPriority))
{
}
/******************************************************************************
 * Function name  : SipAcceptResourcePriorityHeader::~SipAcceptResourcePriorityHeader
 *
 * Description   :
 *
 * Preconditions  :
 *
 * Side Effects  : none
 *****************************************************************************/
SipAcceptResourcePriorityHeader::~SipAcceptResourcePriorityHeader()
{
    if (m_pszNameSpace != SIP_NULL)
    {
        delete[] m_pszNameSpace;
    }

    if (m_pszRPriority != SIP_NULL)
    {
        delete[] m_pszRPriority;
    }
}

/******************************************************************************
 * Function name  : SipAcceptResourcePriorityHeader::EncodeHdr
 *
 * Description   :
 *
 * Preconditions  :
 *
 * Side Effects  : none
 *****************************************************************************/
SIP_BOOL SipAcceptResourcePriorityHeader::EncodeHdr
(
    SIP_CHAR   **ppucCurrPos,
    SIP_BOOL   /*bParams = SIP_TRUE*/
)
{

    if ((m_pszNameSpace == SIP_NULL) && (m_pszRPriority == SIP_NULL))
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Namespace and Priority Missing",
            SIP_ZERO, SIP_ZERO);
        return SIP_TRUE;
    }

    if ((m_pszNameSpace == SIP_NULL) || (m_pszRPriority == SIP_NULL))
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Namespace or Priority Missing",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SipPf_Strcpy(*ppucCurrPos, m_pszNameSpace);
    SipEnc_UpdateCurrPos(ppucCurrPos);

    SIP_ENC_DOT(*ppucCurrPos);

    SipPf_Strcpy(*ppucCurrPos, m_pszRPriority);
    SipEnc_UpdateCurrPos(ppucCurrPos);

    return SIP_TRUE;
}

/******************************************************************************
 * Function name  : SipAcceptResourcePriorityHeader::SetNameSpace
 *
 * Description   :
 *
 * Preconditions  :
 *
 * Side Effects  : none
 *****************************************************************************/
SIP_BOOL SipAcceptResourcePriorityHeader::SetNameSpace
(
 const SIP_CHAR * pkszNameSpace
 )
{
    return SetCharVar(pkszNameSpace,m_pszNameSpace);
}

/******************************************************************************
 * Function name  : SipAcceptResourcePriorityHeader::SetSeq
 *
 * Description   :
 *
 * Preconditions  :
 *
 * Side Effects  : none
 *****************************************************************************/
SIP_BOOL SipAcceptResourcePriorityHeader::SetRPriority
(
 const SIP_CHAR * pkszRPriority
 )

{
    return SetCharVar(pkszRPriority,m_pszRPriority);
}

/******************************************************************************
 * Function name      : SipAcceptResourcePriorityHeader::DecodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipAcceptResourcePriorityHeader::DecodeHdr
(
 SIP_CHAR    *pucStartPt,
 SIP_UINT32  uiDecLen
 )
{
    if (uiDecLen == SIP_ZERO)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Empty buffer",SIP_ZERO, SIP_ZERO);
        return SIP_TRUE;
    }

    SIP_CHAR            *pucEndPt = pucStartPt + uiDecLen - SIP_ONE;
    SIP_CHAR            *pucTempPre  = SIP_NULL;

    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPre, SIP_DOT) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Dot missing", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    m_pszNameSpace = sipCreateString(pucStartPt, pucTempPre);
    if (m_pszNameSpace == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation Fail", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    pucStartPt = pucTempPre + SIP_TWO;

    m_pszRPriority = sipCreateString(pucStartPt, pucEndPt);
    if (m_pszRPriority == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation Fail", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    return SIP_TRUE;
}

SIP_BOOL SipAcceptResourcePriorityHeader::IsValidHeader() const
{
    if (((m_pszNameSpace == SIP_NULL) && (m_pszRPriority == SIP_NULL)) ||
        ((m_pszNameSpace != SIP_NULL) && (m_pszRPriority != SIP_NULL)))
    {
         return SIP_TRUE;
    }
    return SIP_FALSE;
}

SipHeaderBase* SipAcceptResourcePriorityHeader::GetNewObj(SIP_INT32 /*eHdr*/,
            SipHeaderBase *pHeader)
{
    if (pHeader != SIP_NULL) {
        return new SipAcceptResourcePriorityHeader(
            *reinterpret_cast<SipAcceptResourcePriorityHeader*>(pHeader));
    }
    return new SipAcceptResourcePriorityHeader();
}
