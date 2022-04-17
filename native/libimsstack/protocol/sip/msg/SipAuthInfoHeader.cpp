/******************************************************************************
 * Project Name   : SIP_RTP
 * Group    : IP-CS [MSG-2]
 * Security   : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename      : SipAuthInfoHeader.cpp
 * Purpose     :
 * Platform      : Windows OR Android
 * Author(s)     : Saurabh Srivastava
 * E-mail id.    : saurabh31.srivastava@
 * Creation date   : may. 10, 2011
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
#include "msg/SipAuthInfoHeader.h"
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
SipAuthInfoHeader::SipAuthInfoHeader()
    : SipHeaderBase(SipHeaderBase::AUTHENTICATION_INFO)
    , m_pobjAiInfo(SIP_NULL)
{
}
SipAuthInfoHeader::SipAuthInfoHeader(const SipAuthInfoHeader &objHeader)
    : SipHeaderBase(objHeader)
    , m_pobjAiInfo(SIP_NULL)
{
    m_pobjAiInfo = new SipNameValue(*(objHeader.m_pobjAiInfo));
}

SipAuthInfoHeader::~SipAuthInfoHeader()
{
    if (m_pobjAiInfo != SIP_NULL) {
        m_pobjAiInfo->SipDelete();
    }
}

SIP_BOOL   SipAuthInfoHeader::EncodeHdr
(
    SIP_CHAR   **ppucCurrPos,
    SIP_BOOL   /*bParams = SIP_TRUE*/
)
{
    if (m_pobjAiInfo == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Auth info missing", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    SIP_CHAR    *pucMsgCurrPtr  =  *ppucCurrPos;
    SIP_BOOL bStatus = m_pobjAiInfo->EncodeFromList(&pucMsgCurrPtr);
    if (bStatus == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Name Value Encode fail", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    *ppucCurrPos = pucMsgCurrPtr;

    return bStatus;
}

const SIP_CHAR* SipAuthInfoHeader::GetAiInfoVal
(
 SIP_UINT32   usPos /*default value is zero*/
)
{
    if ((m_pobjAiInfo != SIP_NULL) && (usPos < m_pobjAiInfo->m_valueList.GetSize()))
    {
        return m_pobjAiInfo->m_valueList.GetAt(usPos);
    }
    return SIP_NULL;
}

/******************************************************************************
 * Function name      : SipAuthInfoHeader::DecodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipAuthInfoHeader::DecodeHdr
(
 SIP_CHAR    *pucStartPt,
 SIP_UINT32  uiDecLen
 )
{
    m_pobjAiInfo = new SipNameValue(GetHdrType());
    if (m_pobjAiInfo == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation Failed", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR *pucEndPt = pucStartPt + uiDecLen - SIP_ONE;
    if (m_pobjAiInfo->DecHdrNameVal(pucStartPt, pucEndPt) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Name Value Decoding fail", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    return SIP_TRUE;
}

SipHeaderBase* SipAuthInfoHeader::GetNewObj(SIP_INT32 /*eHdr*/, SipHeaderBase *pHeader)
{
    if (pHeader != SIP_NULL) {
        return new SipAuthInfoHeader(*reinterpret_cast<SipAuthInfoHeader*>(pHeader));
    }
    return new SipAuthInfoHeader();
}
