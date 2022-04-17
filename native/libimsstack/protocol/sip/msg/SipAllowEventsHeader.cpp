/******************************************************************************
 * Project Name   : SIP_RTP
 * Group    : IP-CS [MSG-2]
 * Security   : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename      : SipAllowEventsHeader.cpp
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
#include "msg/SipAllowEventsHeader.h"
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
 * Function name  : SipAllowEventsHeader::SipAllowEventsHeader
 *
 * Description   :
 *
 * Preconditions  :
 *
 * Side Effects  : none
 *****************************************************************************/
SipAllowEventsHeader::SipAllowEventsHeader()
    : SipHeaderBase(SipHeaderBase::ALLOW_EVENTS)
    , m_pobjEventTemplateList(SIP_NULL)
{
}


/******************************************************************************
 * Function name  : SipAllowEventsHeader::SipAllowEventsHeader
 *
 * Description   :
 *
 * Preconditions  :
 *
 * Side Effects  : none
 *****************************************************************************/
SipAllowEventsHeader::SipAllowEventsHeader(const SipAllowEventsHeader &objHeader)
    : SipHeaderBase(objHeader)
    , m_pobjEventTemplateList(SIP_NULL)
{
    if (objHeader.m_pobjEventTemplateList != SIP_NULL) {
        m_pobjEventTemplateList = new SipParameterList(*(objHeader.m_pobjEventTemplateList));
    }
}


/******************************************************************************
 * Function name  : SipAllowEventsHeader::~SipAllowEventsHeader
 *
 * Description   :
 *
 * Preconditions  :
 *
 * Side Effects  : none
 *****************************************************************************/
SipAllowEventsHeader::~SipAllowEventsHeader()
{
    if (m_pobjEventTemplateList != SIP_NULL)
    {
        m_pobjEventTemplateList->SipDelete();
    }
}

/******************************************************************************
 * Function name  : SipAllowEventsHeader::EncodeHdr
 *
 * Description   :
 *
 * Preconditions  :
 *
 * Side Effects  : none
 *****************************************************************************/
SIP_BOOL SipAllowEventsHeader::EncodeHdr
(
    SIP_CHAR   **ppucCurrPos,
    SIP_BOOL   /*bParams = SIP_TRUE*/
)
{
    const SIP_CHAR *pValue = GetValue();
    if (pValue == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Missing Event package", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SipPf_Strcpy(*ppucCurrPos, pValue);
    SipEnc_UpdateCurrPos(ppucCurrPos);

    return (m_pobjEventTemplateList != SIP_NULL) ?
        m_pobjEventTemplateList->EncodeList(ppucCurrPos,SIP_DOT) : SIP_TRUE;
}


SIP_BOOL SipAllowEventsHeader::AddEvtTemplate(const SIP_CHAR  *pkszEvntTmpl)
{
    if (m_pobjEventTemplateList == SIP_NULL)
    {
        m_pobjEventTemplateList = new SipParameterList();
    }

    if (m_pobjEventTemplateList == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Memory allocation fail", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR* pcEvTmpl =  SIP_NULL;
    if (SetCharVar(pkszEvntTmpl,pcEvTmpl) == SIP_TRUE)
    {
        SIP_BOOL bResult = m_pobjEventTemplateList->Add(pcEvTmpl);
        delete[] pcEvTmpl;
        return bResult;
    }

    return SIP_FALSE;

}

/******************************************************************************
 * Function name      : SipAllowEventsHeader::DecodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipAllowEventsHeader::DecodeHdr
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

    SIP_CHAR            *pucEndPt = pucStartPt + uiDecLen - SIP_ONE;
    SIP_CHAR            *pucTempPos  = SIP_NULL;

    /*Case of having event template*/
    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, SIP_DOT) == SIP_FALSE)
    {
        pucTempPos = pucEndPt;
    }

    SIP_CHAR *pValue = sipCreateString(pucStartPt, pucTempPos);
    if (SetValue(pValue) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation Fail", SIP_ZERO, SIP_ZERO);
        if (pValue != SIP_NULL) {
            delete[] pValue;
        }
        return SIP_FALSE;
    }
    delete[] pValue;

    if (pucTempPos != pucEndPt)
    {
        m_pobjEventTemplateList = new SipParameterList(GetHdrType());
        if (m_pobjEventTemplateList == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation Fail", SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
        /*Update the tempPos to the start of eventTamplate*/
        pucTempPos = pucTempPos + SIP_TWO;
        if (m_pobjEventTemplateList->DecUriSipParameterList(pucTempPos,
            pucEndPt, SIP_DOT) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Hdr Prm Decoding Fail", SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
    }
    return SIP_TRUE;
}

SipHeaderBase* SipAllowEventsHeader::GetNewObj(SIP_INT32 /*eHdr*/, SipHeaderBase *pHeader)
{
    if (pHeader != SIP_NULL)
    {
        return new SipAllowEventsHeader(*reinterpret_cast<SipAllowEventsHeader*>(pHeader));
    }
    return new SipAllowEventsHeader();
}
