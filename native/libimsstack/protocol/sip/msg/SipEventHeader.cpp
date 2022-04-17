/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipEventHeader.cpp
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
#include "msg/SipEventHeader.h"
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
 * Function name      : SipEventHeader::SipEventHeader
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipEventHeader::SipEventHeader()
    : SipHeaderBase(SipHeaderBase::EVENT)
    , m_pobjEventTemplateList(SIP_NULL)
{
}


/******************************************************************************
 * Function name      : SipEventHeader::SipEventHeader
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipEventHeader::SipEventHeader(const SipEventHeader &objHeader)
    : SipHeaderBase(objHeader)
    , m_pobjEventTemplateList(SIP_NULL)
{
    if (objHeader.m_pobjEventTemplateList != SIP_NULL) {
        m_pobjEventTemplateList = new SipParameterList(*(objHeader.m_pobjEventTemplateList));
    }
}


/******************************************************************************
 * Function name      : SipEventHeader::~SipEventHeader
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipEventHeader::~SipEventHeader()
{
    if (m_pobjEventTemplateList != SIP_NULL)
    {
        m_pobjEventTemplateList->SipDelete();
    }
}

/******************************************************************************
 * Function name      : SipEventHeader::EncodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipEventHeader::EncodeHdr
(
    SIP_CHAR     **ppucCurrPos,
    SIP_BOOL     bParams /*Default = SIP_TRUE*/
)
{
    const SIP_CHAR *pValue = GetValue();
    if (pValue == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "No Evt package", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SipPf_Strcpy(*ppucCurrPos, pValue);
    SipEnc_UpdateCurrPos(ppucCurrPos);
    if (m_pobjEventTemplateList != SIP_NULL)
    {
        SipVector<SipNameValue*> &sipList = m_pobjEventTemplateList->GetList();
        SIP_UINT32 usSize = sipList.GetSize();
        for (SIP_UINT32 iIndex = SIP_ZERO; iIndex < usSize; iIndex++)
        {
            SipNameValue *pobjNmVl = sipList.GetAt(iIndex);
            if (pobjNmVl != SIP_NULL)
            {
                SIP_ENC_DOT(*ppucCurrPos);

                SipPf_Strcpy(*ppucCurrPos,pobjNmVl->m_pszName);
                SipEnc_UpdateCurrPos(ppucCurrPos);
            }
        }
    }

    return EncodeHeaderParameters(ppucCurrPos, bParams);
}

/******************************************************************************
 * Function name      : SipEventHeader::SetEventPkg
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipEventHeader::AddEventTemplate
(
 const SIP_CHAR  *pkszEvntTmpl
 )
{
    if (pkszEvntTmpl == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODACCESSOR, "Empty value", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    if (m_pobjEventTemplateList ==  SIP_NULL)
    {
        m_pobjEventTemplateList = new SipParameterList();
    }

    if (m_pobjEventTemplateList == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODACCESSOR, "Memory Allocation fail", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    return m_pobjEventTemplateList->Add(pkszEvntTmpl);
}


/******************************************************************************
 * Function name      : SipEventHeader::DecodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipEventHeader::DecodeHdr
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
    SIP_CHAR *pucTempNext  = SIP_NULL;

    if (sipFindActualPos(pucStartPt, pucEndPt, &pucTempPre, &pucTempNext, SIP_SEMI) == SIP_TRUE)
    {
        if (DecodeHeaderParameters(pucTempNext, pucEndPt, SIP_SEMI) == SIP_FALSE)
        {
            return SIP_FALSE;
        }
        pucEndPt = pucTempPre;
    }

    SIP_CHAR *pucTempPos  = SIP_NULL;
    if (sipFindPreDelimiter(pucStartPt,pucEndPt,&pucTempPos,SIP_DOT) == SIP_FALSE)
    {
        pucTempPos = pucEndPt;
    }

    SIP_CHAR *pszEvent = sipCreateString(pucStartPt, pucTempPos);
    if (SetValue(pszEvent) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation Fail", SIP_ZERO, SIP_ZERO);
        if (pszEvent != SIP_NULL) {
            delete[] pszEvent;
        }
        return SIP_FALSE;
    }
    delete[] pszEvent;

    if(pucTempPos != pucEndPt)
    {
        m_pobjEventTemplateList = new SipParameterList();
        if (m_pobjEventTemplateList == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation Fail", SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        pucTempPos = pucTempPos + SIP_TWO;

        if (m_pobjEventTemplateList->DecUriSipParameterList(pucTempPos,
            pucEndPt, SIP_DOT) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Event Package Decoding Failed",
                SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
    }
    return SIP_TRUE;
}

SipHeaderBase* SipEventHeader::GetNewObj(SIP_INT32 /*eHdr*/, SipHeaderBase* pHeader)
{
    if (pHeader != SIP_NULL) {
        return new SipEventHeader(*reinterpret_cast<SipEventHeader*>(pHeader));
    }
    return new SipEventHeader();
}
