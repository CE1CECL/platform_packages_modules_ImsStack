/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipTriggerConsentHeader.cpp
 * Purpose               :
 * Platform              : Windows OR Android
 * Author(s)           : Saurabh Srivastava
 * E-mail id.            : saurabh31.srivastava@
 * Creation date       : may. 10, 2011
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
#include "msg/SipTriggerConsentHeader.h"
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

SipTriggerConsentHeader::SipTriggerConsentHeader()
    : SipHeaderBase(SipHeaderBase::TRIGGER_CONSENT)
    , m_pobjSipUri(SIP_NULL)
{
}

SipTriggerConsentHeader::SipTriggerConsentHeader(const SipTriggerConsentHeader &objHeader)
    : SipHeaderBase(objHeader)
    , m_pobjSipUri(SIP_NULL)
{
    if (objHeader.m_pobjSipUri != SIP_NULL) {
        m_pobjSipUri = new SipUri(*(objHeader.m_pobjSipUri));
    }
}

/*destructor*/
SipTriggerConsentHeader::~SipTriggerConsentHeader()
{
    if (m_pobjSipUri != SIP_NULL) {
        m_pobjSipUri->SipDelete();
    }
}

/*virtual methods*/
/*Function for encoding of headers*/
SIP_BOOL     SipTriggerConsentHeader::EncodeHdr
(
    SIP_CHAR     **ppucCurrPos,
    SIP_BOOL     bParams /*Default = SIP_TRUE*/
)
{
    if (m_pobjSipUri == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "EncodeHdr: SIP Uri missing",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    if (m_pobjSipUri->EncodeSipUri(ppucCurrPos) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "EncodeHdr: SIP Uri Encoding Failed",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    return EncodeHeaderParameters(ppucCurrPos, bParams);
}

SipUri* SipTriggerConsentHeader::GetSipUri()
{
    if (m_pobjSipUri != SIP_NULL)
    {
        m_pobjSipUri->increment();
        return m_pobjSipUri;
    }
    return SIP_NULL;
}

SIP_BOOL  SipTriggerConsentHeader::SetSipUri
(
 SipUri * pobjSipUri
 )
{
    if (pobjSipUri == SIP_NULL)
    {
        return SIP_FALSE;
    }
    if ( m_pobjSipUri != SIP_NULL)
    {
        m_pobjSipUri->SipDelete();
    }
    pobjSipUri->increment();
    m_pobjSipUri = pobjSipUri;
    return SIP_TRUE;
}


/******************************************************************************
 * Function name      : SipTriggerConsentHeader::DecodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipTriggerConsentHeader::DecodeHdr
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

    if (*pucStartPt == LEFT_ANGLE)
    {
        pucStartPt++;
    }

    if (*pucEndPt == RIGHT_ANGLE)
    {
        pucEndPt--;
    }

    SIP_CHAR *pucTempPre  = SIP_NULL;
    SIP_CHAR *pucTempNext  = SIP_NULL;

    if (sipFindActualPos(pucStartPt, pucEndPt, &pucTempPre, &pucTempNext, SIP_SEMI) == SIP_TRUE)
    {
        if (DecodeHeaderParameters(pucTempNext, pucEndPt, SIP_SEMI) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "DecodeHdr: Hdr Prm Decoding Failed",
                    SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }
        pucEndPt = pucTempPre;
    }
    /*Update the decode length*/
    uiDecLen = pucEndPt - pucStartPt + SIP_ONE;
    /*Now Decode the SIP URI*/
    m_pobjSipUri = new SipUri();
    if (m_pobjSipUri == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeHdr: Memory Allocation Failed",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    //skip "sip:" from the user name
    SIP_CHAR *pucSIPScheme = SIP_NULL;

    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucSIPScheme, COLON) == SIP_TRUE)
    {
        SIP_CHAR *pucTempScheme = sipCreateString(pucStartPt,pucSIPScheme);
        if (SipPf_Stricmp(pucTempScheme,"sip")== SIP_ZERO)
        {
            pucStartPt = pucSIPScheme + 2;
            uiDecLen -= SipPf_Strlen("sip:");
        }
        if (pucTempScheme != SIP_NULL)
        {
            delete[] pucTempScheme;
        }
    }

    if (m_pobjSipUri->DecodeSipUri(pucStartPt, uiDecLen) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeHdr: SIP URI Decoding Failed",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    return SIP_TRUE;
}

SipHeaderBase* SipTriggerConsentHeader::GetNewObj(SIP_INT32 /*eHdr*/, SipHeaderBase *pHeader)
{
    if (pHeader != SIP_NULL) {
        return new SipTriggerConsentHeader(*reinterpret_cast<SipTriggerConsentHeader*>(pHeader));
    }
    return new SipTriggerConsentHeader();
}
