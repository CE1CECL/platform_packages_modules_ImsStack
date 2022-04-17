/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipUserAgentHeader.h
 * Purpose               :
 * Platform              : Windows OR Android
 * Author(s)           :
 * E-mail id.            : giridhar.a@
 * Creation date       : July. 27,2010
 *
 * Edit HisAlertry         Modification description(s)
 * Date                Name            Version        Bug-ID        Description
 * ----------        ----------        -------        ------        -------------
 * Month. Date,10        Giridhar               0.0a            Initial creation
 *****************************************************************************/

/*****************************************************************************
  Header Inclusions
 *****************************************************************************/
#include "msg/SipUserAgentHeader.h"
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
  Enum Declaration
 *****************************************************************************/

/****************************************************************************
  Class Declaration Starts
 *****************************************************************************/

/*constructor*/
SipUserAgentHeader::SipUserAgentHeader(SIP_INT32 eHdrType)
    : SipHeaderBase(eHdrType)
    , m_objProductList(SipVector<SIP_CHAR*>())
{
}


/*constructor*/
SipUserAgentHeader::SipUserAgentHeader(const SipUserAgentHeader &objHeader)
    : SipHeaderBase(objHeader)
    , m_objProductList(SipVector<SIP_CHAR*>())
{
    SIP_UINT32 usSize = objHeader.m_objProductList.GetSize();
    for (SIP_UINT32 usCount = SIP_ZERO; usCount < usSize; usCount++)
    {
        SIP_CHAR *pTempVal = objHeader.m_objProductList.GetAt(usCount);
        if (pTempVal != SIP_NULL)
        {
            SIP_CHAR* pobjVal = SipPf_Strdup(pTempVal);
            m_objProductList.Add(pobjVal);
        }
    }
}


/*destructor*/
SipUserAgentHeader::~SipUserAgentHeader()
{
    while(m_objProductList.IsEmpty() != SIP_TRUE)
    {
        delete m_objProductList.Top();
        m_objProductList.Pop();
    }
}


/*virtual methods*/
/*Function for encoding of headers*/
SIP_BOOL SipUserAgentHeader::EncodeHdr
(
    SIP_CHAR     **ppucCurrPos,
    SIP_BOOL     /*bParams = SIP_TRUE*/
 )
{
    if (m_objProductList.IsEmpty() == SIP_TRUE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "No header body", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_UINT32 usSize = m_objProductList.GetSize();
    for (SIP_UINT32 uiIndex = SIP_ZERO; uiIndex <  usSize;uiIndex++)
    {
        if (uiIndex != SIP_ZERO)
        {
            SIP_ENC_SP(*ppucCurrPos);
        }
        SIP_CHAR *pcVal = m_objProductList.GetAt(uiIndex);
        SipPf_Strcpy(*ppucCurrPos,pcVal);
        SipEnc_UpdateCurrPos(ppucCurrPos);
    }

    return SIP_TRUE;
}


/*Sets */
SIP_BOOL SipUserAgentHeader::AddProductNameVer
(
 const SIP_CHAR    *pucProduct
 )
{
    if (pucProduct == SIP_NULL)
    {
        return SIP_FALSE;
    }

    SIP_CHAR *pcProduct =  SipPf_Strdup(pucProduct);
    if (pcProduct == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "Malloc Failed",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    m_objProductList.Add(pcProduct);

    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipUserAgentHeader::DecodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipUserAgentHeader::DecodeHdr
(
 SIP_CHAR    *pucStartPt,
 SIP_UINT32  uiDecLen
 )
{
    if (uiDecLen == SIP_ZERO)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "Empty buffer",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR *pucTempPos= SIP_NULL;
    SIP_CHAR *pucCommentStart= SIP_NULL;
    SIP_CHAR *pucCommentEnd= SIP_NULL;
    SIP_CHAR *pucEndPt = pucStartPt + uiDecLen - SIP_ONE;
    /*"UserAgent" HCOLON UserAgent-val *(LWS UserAgent-val) */
    while (pucStartPt < pucEndPt)
    {
        SIP_BOOL bStatus = FindComment(pucStartPt, pucEndPt, pucCommentStart, pucCommentEnd);

        if (sipFindLWS(pucStartPt, pucEndPt,&pucTempPos) == SIP_FALSE)
        {
            pucTempPos = pucEndPt;
        }

        //check LWS is between comment
        if (bStatus == SIP_TRUE &&
            (pucCommentStart < (pucTempPos) && (pucTempPos) < pucCommentEnd) == SIP_TRUE)
        {
            pucStartPt = pucCommentStart;
            pucTempPos = pucCommentEnd;
        }

        SIP_CHAR *pszUserAgent = sipCreateString(pucStartPt,  pucTempPos);
        if (pszUserAgent == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "DecodeHdr:Memory Allocation failed",
                    SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }
        /*Put the value into list*/
        if (m_objProductList.Add(pszUserAgent) < SIP_ZERO)
        {
            delete[] pszUserAgent;
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "DecodeHdr:Adding in list failed",
                    SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }

        if (pucTempPos == pucEndPt)
        {
            pucStartPt =pucEndPt;
        }
        else
        {
            pucTempPos = pucTempPos + SIP_ONE;
            pucStartPt = sipSkipFwLWS(pucTempPos,pucEndPt);
            pucTempPos = SIP_NULL;
        }
    }

    return SIP_TRUE;
}

SipHeaderBase* SipUserAgentHeader::GetNewObj(SIP_INT32 eHdr, SipHeaderBase* pHeader)
{
    if (pHeader != SIP_NULL)
    {
        return new SipUserAgentHeader(*reinterpret_cast<SipUserAgentHeader*>(pHeader));
    }
    return new SipUserAgentHeader(eHdr);
}
