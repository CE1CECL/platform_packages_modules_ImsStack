/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipTimeStampHeader.h
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
#include "msg/SipTimeStampHeader.h"
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
SipTimeStampHeader::SipTimeStampHeader()
    : SipHeaderBase(SipHeaderBase::TIMESTAMP)
    , m_pszTimeVal(SIP_NULL)
    , m_pszDelay(SIP_NULL)
{
}
/*Copy Constructor*/
SipTimeStampHeader::SipTimeStampHeader(const SipTimeStampHeader &objHeader)
    : SipHeaderBase(objHeader)
    , m_pszTimeVal(SipPf_Strdup(objHeader.m_pszTimeVal))
    , m_pszDelay(SipPf_Strdup(objHeader.m_pszDelay))
{
}
/*destructor*/
SipTimeStampHeader::~SipTimeStampHeader()
{
    if (m_pszTimeVal != SIP_NULL)
    {
        delete[] m_pszTimeVal;
        m_pszTimeVal = SIP_NULL;
    }
    if (m_pszDelay != SIP_NULL)
    {
        delete[] m_pszDelay ;
        m_pszDelay = SIP_NULL;
    }
}

/*virtual methods*/
/*Function for encoding of headers*/
/******************************************************************************
 * Function name      : SipTimeStampHeader::EncodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipTimeStampHeader::EncodeHdr
(
    SIP_CHAR     **ppucCurrPos,
    SIP_BOOL     /*bParams = SIP_TRUE*/
 )
{
    /*Encoding of header Value  i.e.
      "Timestamp" HCOLON 1*(DIGIT) [ "." *(DIGIT) ] [ LWS delay ]   */
    /*Encoding of DIGIT*/
    if (m_pszTimeVal == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "EncodeHdr: Missing TimeStamp ",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SipPf_Strcpy(*ppucCurrPos, m_pszTimeVal);
    SipEnc_UpdateCurrPos(ppucCurrPos);

    /*Encoding of Delay*/
    if (m_pszDelay != SIP_NULL)
    {
        SIP_ENC_SP(*ppucCurrPos);

        SipPf_Strcpy(*ppucCurrPos, m_pszDelay);
        SipEnc_UpdateCurrPos(ppucCurrPos);
    }

    return SIP_TRUE;
}

/*Sets */
SIP_BOOL SipTimeStampHeader::SetTimeVal
(
 const SIP_CHAR    *pucTimeVal
 )
{
    return SetCharVar(pucTimeVal, m_pszTimeVal);
}

/*Sets */
SIP_BOOL SipTimeStampHeader::SetDelay
(
 const SIP_CHAR    *pucDelay
 )
{
    return SetCharVar(pucDelay, m_pszDelay);
}


/******************************************************************************
 * Function name      : SipTimeStampHeader::DecodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipTimeStampHeader::DecodeHdr
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

    SIP_CHAR *pucEndPt = pucStartPt + uiDecLen - SIP_ONE;
    SIP_CHAR *pucTempPre  = SIP_NULL;
    /*Find the LWS i.e. End of Transport*/
    if (sipFindLWS(pucStartPt, pucEndPt, &pucTempPre) == SIP_FALSE)
    {
        pucTempPre = pucEndPt;
    }

    m_pszTimeVal = sipCreateString(pucStartPt, pucTempPre);
    if (m_pszTimeVal == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeHdr:Memory Allocation Failed",
                SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    if (pucTempPre != pucEndPt)
    {
        /*point to the start of the LWS*/
        pucTempPre = pucTempPre + SIP_ONE;
        pucStartPt = sipSkipFwLWS(pucTempPre, pucEndPt);
        m_pszDelay = sipCreateString(pucStartPt, pucEndPt);
        if (m_pszDelay == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "DecodeHdr:Memory Allocation Failed",
                    SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }
    }

    return SIP_TRUE;
}

SipHeaderBase* SipTimeStampHeader::GetNewObj(SIP_INT32 /*eHdr*/, SipHeaderBase* pHeader)
{
    if (pHeader != SIP_NULL) {
        return new SipTimeStampHeader(*reinterpret_cast<SipTimeStampHeader*>(pHeader));
    }
    return new SipTimeStampHeader();
}
