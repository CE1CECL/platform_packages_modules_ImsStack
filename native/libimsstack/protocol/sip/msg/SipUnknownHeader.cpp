/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipUnknownHeader.cpp
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
#include "msg/SipUnknownHeader.h"
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
 * Function name      : SipUnknownHeader::SipUnknownHeader
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipUnknownHeader::SipUnknownHeader()
    : SipHeaderBase(SipHeaderBase::UNKNOWN)
    , m_pszHdrName(SIP_NULL)
    , m_pszHdrValue(SIP_NULL)
{
}

/******************************************************************************
 * Function name      : SipUnknownHeader::SipUnknownHeader
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipUnknownHeader::SipUnknownHeader(const SipUnknownHeader &objHeader)
    : SipHeaderBase(SipHeaderBase::UNKNOWN)
    , m_pszHdrName(SipPf_Strdup(objHeader.m_pszHdrName))
    , m_pszHdrValue(SipPf_Strdup(objHeader.m_pszHdrValue))
{
}

/******************************************************************************
 * Function name      : SipUnknownHeader::~SipUnknownHeader
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipUnknownHeader::~SipUnknownHeader()
{
    if (m_pszHdrName != SIP_NULL)
    {
        delete[] m_pszHdrName;
    }
    if (m_pszHdrValue != SIP_NULL)
    {
        delete[] m_pszHdrValue;
    }
}

/******************************************************************************
 * Function name      : SipUnknownHeader::EncodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipUnknownHeader::EncodeHdr
(
    SIP_CHAR     **ppucCurrPos,
    SIP_BOOL     /*bParams = SIP_TRUE*/
)
{
    if (m_pszHdrName == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Header name not found",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SipPf_Strcpy(*ppucCurrPos,m_pszHdrName);
    SipEnc_UpdateCurrPos(ppucCurrPos);

    SIP_ENC_COLON(*ppucCurrPos);

    SIP_ENC_SP(*ppucCurrPos);

    SipPf_Strcpy(*ppucCurrPos,m_pszHdrValue);
    SipEnc_UpdateCurrPos(ppucCurrPos);

    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipUnknownHeader::DecodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipUnknownHeader::DecodeHdr
(
 SIP_CHAR    *pucStartPt,
 SIP_UINT32  uiDecLen
 )
{
    (void)pucStartPt;
    (void)uiDecLen;

    return SIP_TRUE;
}

SIP_BOOL SipUnknownHeader::SetHeaderName(const SIP_CHAR *pkszHdrName)
{
    return SetCharVar(pkszHdrName,m_pszHdrName);
}

SIP_BOOL SipUnknownHeader::SetHeaderValue(const SIP_CHAR *pkszHdrValue)
{
    return SetCharVar(pkszHdrValue,m_pszHdrValue);
}

SipHeaderBase* SipUnknownHeader::GetNewObj(SIP_INT32 /*eHdr*/, SipHeaderBase *pHeader)
{
    if (pHeader != SIP_NULL) {
        return new SipUnknownHeader(*reinterpret_cast<SipUnknownHeader*>(pHeader));
    }
    return new SipUnknownHeader();
}
