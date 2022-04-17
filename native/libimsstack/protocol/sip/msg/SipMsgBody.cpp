/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipMsgBody.cpp
 * Purpose               :
 * Platform              : Windows OR Android
 * Author(s)           : Saurabh Srivastava
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

#include "msg/sip_msgutil.h"
#include "msg/SipMsgBody.h"
#include "msg/SipMessage.h"
#include "platform/sip_pf_memory.h"
#include "sip_debug.h"
#include "SipTrace.h"
#include "platform/sip_pf_string.h"
//#include "msg/sip_dec.h"

#define MAX_BODY_SIZE 1500
#define NAME_CONTENT_TRANSFER_ENCODING "Content-Transfer-Encoding"

extern  SIP_CHAR    gaszSipHdr[][SIP_MAX_HDR_LEN];

/****************************************************************************
  Macro Definitions
 *****************************************************************************/
/******************************************************************************
 * Function name      : sipFindLWS
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL sipFindLWSP_Char
(
 SIP_CHAR        *pucStartPt,
 SIP_CHAR        *pucEndPt,
 SIP_CHAR        **pucTempLoc
 )
{
    while(pucStartPt <= pucEndPt)
    {
        if (IS_WSP(*pucStartPt))
        {
            *pucTempLoc = pucStartPt - SIP_ONE;
            return SIP_TRUE;
        }
        pucStartPt++;
    }
    return SIP_FALSE;
}


/******************************************************************************
 * Function name      : sipFindCRLF
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL sipFindCRLF
(
 SIP_CHAR        *pucStartPt,
 SIP_CHAR        *pucEndPt,
 SIP_CHAR        **pucTempLoc
 )
{
    SIP_CHAR        *pucNext1Pt = pucStartPt + SIP_ONE;
    while(pucNext1Pt <= pucEndPt)
    {
        if (IS_CRLF(*pucStartPt, *pucNext1Pt))
        {
            *pucTempLoc = pucStartPt - SIP_ONE;
            return SIP_TRUE;
        }
        pucStartPt++;
        pucNext1Pt++;
    }
    return SIP_FALSE;
}



/******************************************************************************
 * Function name      : sipFindBodyEnd
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_CHAR* sipFindBodyEnd
(
 SIP_CHAR        *pucStartPt,
 SIP_CHAR        *pucEndPt,
 SIP_CHAR        *pszBoundary,
 SIP_BOOL        *bBodyEnd
 )
{
    if (pucStartPt == SIP_NULL)
    {
        return SIP_NULL;
    }

    SIP_UINT16 uiBoundaryLen = SipPf_Strlen(pszBoundary);
    SIP_CHAR *pucNextPt = pucStartPt + SIP_ONE;
    SIP_CHAR *pucTempEndPt = pucStartPt + uiBoundaryLen + SIP_TWO;
    SIP_CHAR *pucEndNext = pucTempEndPt + SIP_ONE;

    while(pucEndNext <= pucEndPt)
    {
        if (IS_HYPHEN(*pucStartPt) && IS_HYPHEN(*pucNextPt))
        {
            SIP_CHAR* pucTempStartPt = pucStartPt + SIP_TWO;
            if (SipPf_Strncmp(pucTempStartPt, pszBoundary, uiBoundaryLen) == SIP_ZERO)
            {
                if (IS_HYPHEN(*pucTempEndPt) && IS_HYPHEN(*pucEndNext))
                {
                    *bBodyEnd = SIP_TRUE;
                }

                // Remove preceding CRLF: CRLF--boundary
                // Start pointer: first '-'
                return (pucStartPt - SIP_TWO);
            }
        }
        pucStartPt++;
        pucNextPt = pucStartPt+ SIP_ONE;
        pucTempEndPt = pucStartPt + uiBoundaryLen + SIP_TWO;
        pucEndNext = pucTempEndPt + SIP_ONE;
    }
    return SIP_NULL;
}


/*****************************************************************************
 * Function name      : sipGetMIMEHdrType
 *
 * Description        :
 *
 * Preconditions      :
 *
 * Side Effects          : none
 *****************************************************************************/
SIP_INT32 sipGetMIMEHdrType
(
 SIP_CHAR            *pszHdrName
 )
{
    /* Validates Header Name, Skip SWS */

    switch(pszHdrName[0])
    {
        case 'c':
        case 'C':
            if (SipPf_Stricmp(pszHdrName, "c") == 0)
            {
                return SipHeaderBase::CONTENT_TYPE;
            }
            else if (SipPf_Stricmp(pszHdrName,
                        gaszSipHdr[SipHeaderBase::CONTENT_TYPE]) == 0)
            {
                return SipHeaderBase::CONTENT_TYPE;
            }
            else if (SipPf_Stricmp(pszHdrName,
                        gaszSipHdr[SipHeaderBase::CONTENT_LENGTH]) == 0)
            {
                return SipHeaderBase::CONTENT_LENGTH;
            }
            else if (SipPf_Stricmp(pszHdrName,
                        gaszSipHdr[SipHeaderBase::CONTENT_DISPOSITION]) == 0)
            {
                return SipHeaderBase::CONTENT_DISPOSITION;
            }
            else if (SipPf_Stricmp(pszHdrName,
                        gaszSipHdr[SipHeaderBase::CONTENT_ENCODING]) == 0)
            {
                return SipHeaderBase::UNKNOWN;
            }
            else if (SipPf_Stricmp(pszHdrName,
                        gaszSipHdr[SipHeaderBase::CONTENT_LANGUAGE]) == 0)
            {
                return SipHeaderBase::CONTENT_LANGUAGE;
            }
            else if (SipPf_Stricmp(pszHdrName,
                    NAME_CONTENT_TRANSFER_ENCODING) == 0)
            {
                return SipHeaderBase::CONTENT_ENCODING;
            }
            break;

            /*Apply same for all other headers*/

        default:
            return SipHeaderBase::UNKNOWN;

            /*treat as unknown header*/
    }
    return SipHeaderBase::UNKNOWN;
    /* go for unknown header check*/
}


/****************************************************************************
  Class Member Function Implementations
 *****************************************************************************/

/******************************************************************************
 * Function name      : SipMIMEHdrs::SipMIMEHdrs()
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipMIMEHdrs::SipMIMEHdrs():m_pobjContentType(SIP_NULL),m_pobjContentEncoding(SIP_NULL),
    m_pobjContentDisposition(SIP_NULL),m_pobjUnKnownHdrList(SIP_NULL)
{}


/******************************************************************************
 * Function name      : SipMIMEHdrs::SipMIMEHdrs()
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipMIMEHdrs::SipMIMEHdrs(const SipMIMEHdrs &objMimeHdr)
    : m_pobjContentType(SIP_NULL)
    , m_pobjContentEncoding(SIP_NULL)
    , m_pobjContentDisposition(SIP_NULL)
    , m_pobjUnKnownHdrList(SIP_NULL)
{
    if (objMimeHdr.m_pobjContentType != SIP_NULL)
    {
        m_pobjContentType = new SipContentTypeHeader(*(objMimeHdr.m_pobjContentType));
    }

    if (objMimeHdr.m_pobjContentDisposition != SIP_NULL)
    {
        m_pobjContentDisposition = new SipHeaderBase(*(objMimeHdr.m_pobjContentDisposition));
    }

    if (objMimeHdr.m_pobjContentEncoding != SIP_NULL)
    {
        m_pobjContentEncoding = new SipHeaderBase(*(objMimeHdr.m_pobjContentEncoding));
    }

    if (objMimeHdr.m_pobjUnKnownHdrList != SIP_NULL)
    {
        m_pobjUnKnownHdrList = new SipHeaderList(*(objMimeHdr.m_pobjUnKnownHdrList));
    }

}



/******************************************************************************
 * Function name      : SipMIMEHdrs::~SipMIMEHdrs
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipMIMEHdrs::~SipMIMEHdrs()
{
    if (m_pobjContentType != SIP_NULL)
    {
        m_pobjContentType->SipDelete();
    }
    if (m_pobjContentEncoding != SIP_NULL)
    {
        m_pobjContentEncoding->SipDelete();
    }
    if (m_pobjContentDisposition != SIP_NULL)
    {
        m_pobjContentDisposition->SipDelete();
    }
    if (m_pobjUnKnownHdrList != SIP_NULL)
    {
        m_pobjUnKnownHdrList->SipDelete();
    }

}


SIP_BOOL SipMIMEHdrs::SetMimeHdrs
(
 SipHeaderBase        *pobjHdr
 )
{
    if (pobjHdr == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODACCESSOR, "SetHdrs Failed, pobjHdr is NULL",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_INT32 eHdrType = pobjHdr->GetHdrType();
    switch(eHdrType)
    {
        case SipHeaderBase::CONTENT_TYPE:
            if (m_pobjContentType != SIP_NULL)
            {
                m_pobjContentType->SipDelete();
            }
            m_pobjContentType = (SipContentTypeHeader *)pobjHdr;
            pobjHdr->increment();
            break;

        case SipHeaderBase::CONTENT_DISPOSITION:
            if (m_pobjContentDisposition != SIP_NULL)
            {
                m_pobjContentDisposition->SipDelete();
            }
            m_pobjContentDisposition = pobjHdr;
            pobjHdr->increment();
            break;

        case SipHeaderBase::CONTENT_ENCODING:
            if (m_pobjContentEncoding != SIP_NULL)
            {
                m_pobjContentEncoding->SipDelete();
            }
            m_pobjContentEncoding = pobjHdr;
            pobjHdr->increment();
            break;
        case SipHeaderBase::UNKNOWN:
            {
                if (m_pobjUnKnownHdrList == SIP_NULL)
                {
                    m_pobjUnKnownHdrList = new SipHeaderList(SipHeaderBase::UNKNOWN);
                }
                m_pobjUnKnownHdrList->AddHeader(pobjHdr);
            }
            break;
        default:
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODACCESSOR, "SetMimeHdr Failed, Type = %d",
                    eHdrType, SIP_ZERO);
                return SIP_FALSE;
            }
    }
    return SIP_TRUE;
}

SipHeaderBase* SipMIMEHdrs::GetContentType()
{
    if (m_pobjContentType != SIP_NULL)
    {
        m_pobjContentType->increment();
        return m_pobjContentType;
    }
    return SIP_NULL;
}

SipHeaderBase* SipMIMEHdrs::GetContentEncoding()
{
    if (m_pobjContentEncoding != SIP_NULL)
    {
        m_pobjContentEncoding->increment();
        return m_pobjContentEncoding;
    }
    return SIP_NULL;
}

SipHeaderBase* SipMIMEHdrs::GetContentDisposition()
{
    if (m_pobjContentDisposition != SIP_NULL)
    {
        m_pobjContentDisposition->increment();
        return m_pobjContentDisposition;
    }
    return SIP_NULL;
}

SipHeaderBase* SipMIMEHdrs::GetUnknownHdr(SIP_UINT32 uiIndex)
{
    return (m_pobjUnKnownHdrList != SIP_NULL) ?
        m_pobjUnKnownHdrList->GetObj(uiIndex) : SIP_NULL;
}

/******************************************************************************
 * Function name      : SipMIMEHdrs::getNewMIMEHdrObj
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipHeaderBase* SipMIMEHdrs::getMimeHdrObj(SIP_INT32 eIndex)
{
    switch (eIndex)
    {
        case CONTENT_TYPE:
            if (m_pobjContentType != SIP_NULL)
            {
                m_pobjContentType->increment();
            }
            return m_pobjContentType;

        case CONTENT_ENCODING:
            if (m_pobjContentEncoding != SIP_NULL)
            {
                m_pobjContentEncoding->increment();
            }
            return m_pobjContentEncoding;

        case CONTENT_DISPOSITION:
            if (m_pobjContentDisposition != SIP_NULL)
            {
                m_pobjContentDisposition->increment();
            }
            return m_pobjContentDisposition;

        case UNKNOWN:
            if (m_pobjUnKnownHdrList != SIP_NULL)
            {
                m_pobjUnKnownHdrList->increment();
            }
            return m_pobjUnKnownHdrList;

        default:
            return SIP_NULL;
    }
}

/******************************************************************************
 * Function name      : SipMIMEHdrs::getNewMIMEHdrObj
 *
 * Description     : To be used only in decoder for creating MIME headers
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipHeaderBase* SipMIMEHdrs::getNewMIMEHdrObj
(
 SIP_INT32    eHdrType
 )
{
    switch(eHdrType)
    {
        case SipHeaderBase::CONTENT_TYPE:
            if (m_pobjContentType == SIP_NULL)
            {
                m_pobjContentType = new SipContentTypeHeader();
                if (m_pobjContentType == SIP_NULL)
                {
                    return SIP_NULL;
                }
                return m_pobjContentType;
            }
            else
            {
                return SIP_NULL;
            }
            break;

        case SipHeaderBase::CONTENT_ENCODING:
            if (m_pobjContentEncoding == SIP_NULL)
            {
                m_pobjContentEncoding = new SipHeaderBase(SipHeaderBase::CONTENT_ENCODING);
                if (m_pobjContentEncoding == SIP_NULL)
                {
                    return SIP_NULL;
                }
                return m_pobjContentEncoding;
            }
            else
            {
                return SIP_NULL;
            }
            break;

        case SipHeaderBase::CONTENT_DISPOSITION:
            if (m_pobjContentDisposition == SIP_NULL)
            {
                m_pobjContentDisposition = new SipHeaderBase(SipHeaderBase::CONTENT_DISPOSITION);
                if (m_pobjContentDisposition == SIP_NULL)
                {
                    return SIP_NULL;
                }
                return m_pobjContentDisposition;
            }
            else
            {
                return SIP_NULL;
            }
            break;

        default:
            if (m_pobjUnKnownHdrList == SIP_NULL)
            {
                m_pobjUnKnownHdrList = new SipHeaderList(SipHeaderBase::UNKNOWN);
                if (m_pobjUnKnownHdrList == SIP_NULL)
                {
                    return SIP_NULL;
                }
                return m_pobjUnKnownHdrList;
            }
            else
            {
                return m_pobjUnKnownHdrList;
            }
            return SIP_NULL;
    }
}
/******************************************************************************
 * Function name      : SipMIMEHdrs::DecodeMIMEHdrs
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMIMEHdrs::DecodeMIMEHdrs
(
 SIP_CHAR    *pucStartPt,
 SIP_UINT32     uiDecLen
 )

{
    SIP_CHAR    *pucTempPos= SIP_NULL;
    SIP_CHAR    *pucEndPt = pucStartPt + uiDecLen - SIP_ONE;

    /*Get the position previous to ":"*/
    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, COLON) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecodeMIMEHdrs: colon not found",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR *pucTempNext = pucTempPos + SIP_TWO;
    pucTempNext = sipSkipFwLWS( pucTempNext,pucEndPt);

    /*skip the WSP form back*/
    pucTempPos = sipSkipRwWSP( pucStartPt,pucTempPos);

    /*Create  the header name*/
    SIP_CHAR *pszHdrName = sipCreateString( pucStartPt,  pucTempPos);
    if (pszHdrName == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecodeMIMEHdrs: Memory Allocation failed",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    /*this will return the type of header on passing name*/
    SIP_INT32 eHdrType = sipGetMIMEHdrType(pszHdrName);

    /*Free the header name*/
    /*Get the header object*/
    SipHeaderBase *pobjHdrBase = getNewMIMEHdrObj(eHdrType);
    if (pobjHdrBase == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecodeMIMEHdrs: getNewMIMEHdrObj failed",
            SIP_ZERO, SIP_ZERO);
        delete[] pszHdrName;
        return SIP_FALSE;
    }

    if (pobjHdrBase->GetHdrType() == SipHeaderBase::UNKNOWN)
    {
        SipUnknownHeader* pobjUnknown = new SipUnknownHeader();
        if (pobjUnknown == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecodeMIMEHdrs: Memory Allocation failed",
                SIP_ZERO, SIP_ZERO);
            delete[] pszHdrName;
            return SIP_FALSE;
        }
        SIP_CHAR* pszHdrValue = SIP_NULL;
        if (pobjUnknown->SetHeaderName(pszHdrName) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecodeMIMEHdrs: Add to list Failed",
                SIP_ZERO, SIP_ZERO);
            pobjUnknown->SipDelete();
            delete[] pszHdrName;
            return SIP_FALSE;
        }
        delete[] pszHdrName;

        pszHdrValue = sipCreateString( pucTempNext,  pucEndPt);
        if (pszHdrValue == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecodeMIMEHdrs: Memory Allocation failed",
                SIP_ZERO, SIP_ZERO);
            pobjUnknown->SipDelete();
            return SIP_FALSE;
        }
        if (pobjUnknown->SetHeaderValue(pszHdrValue) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecodeMIMEHdrs: Add to list Failed",
                SIP_ZERO, SIP_ZERO);
            pobjUnknown->SipDelete();
            delete[] pszHdrValue;
            return SIP_FALSE;
        }
        delete[] pszHdrValue;

        /*Add the header into the unknown list*/
        if (m_pobjUnKnownHdrList->AddHeader(pobjUnknown) == SIP_FALSE)
        {
            pobjUnknown->SipDelete();
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecodeMIMEHdrs: Add to list Failed",
                SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
        pobjUnknown->SipDelete();
    }

    else
    {
        delete[] pszHdrName;
        /*Update the Start Point to the start of hdr value*/
        pucStartPt = pucTempNext;
        /*Update the length for decoding*/
        uiDecLen = pucEndPt - pucStartPt + SIP_ONE;
        /*Call the Decoder function*/
        if (pobjHdrBase->DecodeHdr(pucStartPt, uiDecLen) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecodeHdrs: Hdr Decoding Failed",
                SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
    }

    return SIP_TRUE;
}
/******************************************************************************
 * Function name      : SipMsgBody::SipMsgBody
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipMsgBody::SipMsgBody()
    :m_eBodyType(SINGLE_BODY), m_pobjMIMEHdrs(SIP_NULL),
    m_pobjMessageSummary(SIP_NULL), m_pucBuffer(SIP_NULL),
    m_uiBufLen(SIP_ZERO), m_pobjBodyList(SIP_NULL), m_bEncodeMime(SIP_TRUE)
{

    //m_pobjMIMEHdrs = new SipMIMEHdrs();

}

/******************************************************************************
 * Function name      : SipMsgBody::SipMsgBody
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipMsgBody::SipMsgBody(SIP_INT32 eBodyType)
    :m_eBodyType(SINGLE_BODY), m_pobjMIMEHdrs(SIP_NULL),
    m_pobjMessageSummary(SIP_NULL), m_pucBuffer(SIP_NULL),
    m_uiBufLen(SIP_ZERO), m_pobjBodyList(SIP_NULL), m_bEncodeMime(SIP_TRUE)
{
    m_eBodyType = eBodyType;
    m_pobjMIMEHdrs = new SipMIMEHdrs();
    if ((eBodyType == MULTI_PART_MIXED_BODY) ||
            (eBodyType == MULTI_PART_ALT_BODY))
    {
        m_pobjBodyList = new SipMsgBodyList();
    }
    else if (eBodyType == MESSAGE_SUMMARY_BODY)
    {
        m_pobjMessageSummary = new SipMessageSummary();
    }
}

/******************************************************************************
 * Function name      : SipMsgBody::SipUri
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipMsgBody::SipMsgBody(const SipMsgBody &objMsgBody)
    : m_eBodyType(objMsgBody.m_eBodyType), m_pobjMIMEHdrs(SIP_NULL)
    , m_pobjMessageSummary(SIP_NULL), m_pucBuffer(SIP_NULL)
    , m_uiBufLen(SIP_ZERO), m_pobjBodyList(SIP_NULL)
    , m_bEncodeMime(objMsgBody.m_bEncodeMime)
{
    if ((objMsgBody.m_pucBuffer != SIP_NULL) && (objMsgBody.m_uiBufLen  > SIP_ZERO))
    {
        m_pucBuffer = new SIP_CHAR[objMsgBody.m_uiBufLen];
        SipPf_Memcpy(m_pucBuffer, objMsgBody.m_pucBuffer, objMsgBody.m_uiBufLen);
        m_uiBufLen = objMsgBody.m_uiBufLen;
    }

    if (objMsgBody.m_pobjMIMEHdrs != SIP_NULL)
    {
        m_pobjMIMEHdrs = new SipMIMEHdrs(*(objMsgBody.m_pobjMIMEHdrs));
    }

    if (objMsgBody.m_pobjBodyList != SIP_NULL)
    {
        m_pobjBodyList = new SipMsgBodyList(*(objMsgBody.m_pobjBodyList));
    }
    if (objMsgBody.m_pobjMessageSummary != SIP_NULL) {
        m_pobjMessageSummary = new SipMessageSummary(*(objMsgBody.m_pobjMessageSummary));
    }
}



/******************************************************************************
 * Function name      : SipMsgBody::~SipMsgBody
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipMsgBody::~SipMsgBody()
{
    if (m_pobjMIMEHdrs != SIP_NULL)
    {
        m_pobjMIMEHdrs->SipDelete();
        m_pobjMIMEHdrs = SIP_NULL;
    }
    if (m_pucBuffer != SIP_NULL)
    {
        delete[] m_pucBuffer;
        m_pucBuffer = SIP_NULL;
    }
    if (m_pobjBodyList != SIP_NULL)
    {
        m_pobjBodyList->SipDelete();
    }
    if (m_pobjMessageSummary != SIP_NULL)
    {
        m_pobjMessageSummary->SipDelete();
    }
}

/******************************************************************************
 * Function name      :SipMsgBody::EncodeMIMEHdrs
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMsgBody::EncodeMIMEHdrs
(
 SIP_CHAR     **ppucCurrPos
 )
{
    if (m_pobjMIMEHdrs == SIP_NULL)
    {
       return SIP_TRUE;
    }

    SIP_INT32 iHdr = SipMIMEHdrs::CONTENT_TYPE;
    while(iHdr < SipMIMEHdrs::END)
    {
        SipHeaderBase *pobjTemp = m_pobjMIMEHdrs->getMimeHdrObj(iHdr);
        if (pobjTemp != SIP_NULL)
        {
            if ((pobjTemp->GetHdrType()) != SipHeaderBase::UNKNOWN)
            {
                sipEncodeHdrName(pobjTemp->GetHdrType(), ppucCurrPos, ESIPMSGOPT_NONE);
            }
            SIP_BOOL bRetStatus = pobjTemp->EncodeHdr(ppucCurrPos);
            if (bRetStatus == SIP_FALSE)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "EncodeMIMEHdrsFailed",
                    SIP_ZERO, SIP_ZERO);
                pobjTemp->SipDelete();
                return SIP_FALSE;

            }
            pobjTemp->SipDelete();
            SIP_ENC_CRLF(*ppucCurrPos);
        }
        iHdr++;
    }

    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipMsgBody::EncodeMsgBody
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMsgBody::EncodeSingleMsgBody
(
 SIP_CHAR     **ppucCurrPos
 )
{
    if (m_pucBuffer == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "EncodeMsgBody Failed - No body",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    SIP_CHAR* pucBody = *ppucCurrPos;
    SipPf_Memcpy(pucBody,m_pucBuffer,m_uiBufLen);

    /*Update the current position*/
    *ppucCurrPos+=m_uiBufLen;

    return SIP_TRUE;
}



/******************************************************************************
 * Function name      : SipMsgBody::EncodeMIMEMsgBody
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMsgBody::EncodeMIMEMsgBody
(
 SIP_CHAR     **ppucCurrPos
 )
{
    /*Check for message body list*/
    if (m_pobjBodyList == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "EncodeMIMEMsgBody: No body",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    /*Get The content type to get the boundary*/
    SipContentTypeHeader *pobjContentType = SIP_STATIC_CAST
        (SipContentTypeHeader*, GetContentType());
    if (pobjContentType == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Content Type Not Present",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;

    }

    SIP_CHAR *pszBoundary = pobjContentType->GetBoundary();

    if (m_pobjBodyList->EncodeBody(ppucCurrPos,pszBoundary) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Msg Body Enc Failed",
            SIP_ZERO, SIP_ZERO);
        if (pszBoundary != SIP_NULL)
        {
            delete[] pszBoundary;
        }
        return SIP_FALSE;
    }

    if (pszBoundary != SIP_NULL)
    {
        delete[] pszBoundary;
    }
    pobjContentType->SipDelete();

    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipMsgBody::DecodeMessageSummaryMsgBody
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMsgBody::EncodeMessageSummaryMsgBody
(
 SIP_CHAR     **ppucCurrPos
 )
{
    if (m_pobjMessageSummary == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipMsgBody::EncodeMessageSummaryMsgBody:Memory Allocation failed",
                SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    if (m_pobjMessageSummary->EncodeMessageSummary(ppucCurrPos) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
            "EncodeMessageSummaryMsgBody:EncodeMessageSummary failed", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipMsgBody::DecodeSingleMsgBody
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMsgBody::DecodeSingleMsgBody
(
 SIP_CHAR    *pucStartPt,
 SIP_CHAR     *pucEndPt
 )
{
    SIP_UINT16 usSize = pucEndPt - pucStartPt;
    SIP_CHAR *pcData = new SIP_CHAR[usSize + SIP_ONE];

    if (pcData == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
            "DecodeSingleMsgBody:Memory Allocation failed",
            SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SipPf_Memcpy(pcData, pucStartPt, usSize);
    pcData[usSize] = SIP_NULL_CHAR;

    m_pucBuffer = pcData;
    m_uiBufLen = usSize;
    m_eBodyType = SINGLE_BODY;

    return SIP_TRUE;
}


/******************************************************************************
 * Function name      : SipMsgBody::DecodeMIMEMsgBody
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMsgBody::DecodeMIMEMsgBody
(
 SIP_CHAR    *pucStartPt,
 SIP_CHAR     *pucEndPt
 )
{
    if (pucStartPt == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecodeMIMEMsgBody: Illegal argument",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR *pucNext1Pt = pucStartPt + SIP_ONE;
    /*Case when No Header is present before the start of body*/
    if (IS_CRLF(*pucStartPt, *pucNext1Pt))
    {
        pucStartPt = pucStartPt + SIP_TWO;
        SIP_UINT16 usSize = pucEndPt - pucStartPt;
        SIP_CHAR *pcData = new SIP_CHAR[usSize];
        if (pcData == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecodeMIMEMsgBody:Memory Allocation failed",
                SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
        SipPf_Memcpy(pcData, pucStartPt, usSize);
        m_pucBuffer = pcData;
        m_uiBufLen = usSize;
        m_eBodyType = SINGLE_BODY;
        return SIP_TRUE;
    }
    /*Header Decoding*/
    m_pobjMIMEHdrs = new SipMIMEHdrs();
    if (m_pobjMIMEHdrs == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecodeMIMEMsgBody:Memory Allocation failed",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_BOOL bHdrEnd = SIP_FALSE;
    while((pucStartPt <= pucEndPt) && (bHdrEnd == SIP_FALSE))
    {
        /*find next terminating CRLF*/
        SIP_UINT32 uiDecLen = SIP_ZERO;
        SIP_CHAR *pucTempPos = SIP_NULL;
        //Fail condition to be added
        sipFindTerminatingCRLF(pucStartPt,pucEndPt, &pucTempPos, &bHdrEnd);
        uiDecLen = pucTempPos - pucStartPt + SIP_ONE;

        if (m_pobjMIMEHdrs->DecodeMIMEHdrs(pucStartPt,uiDecLen) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecodeMIMEMsgBody:Hdr Decoding failed",
                SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
        pucStartPt = pucTempPos + SIP_THREE;
        pucTempPos = SIP_NULL;
    }
    /*Check for Header end completion*/
    if (bHdrEnd != SIP_TRUE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecodeMIMEMsgBody:Incomplete Message",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    /*Update the start point to the start of Actual Message body Buffer*/
    pucStartPt = pucStartPt + SIP_TWO;
    /*Now Get Type of Body*/
    SipContentTypeHeader *pobjContentType = SIP_STATIC_CAST(SipContentTypeHeader* ,
            m_pobjMIMEHdrs->GetContentType());
    if (pobjContentType == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecodeMIMEMsgBody: Body in not valid",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    const SIP_CHAR *pszMType = pobjContentType->GetMediaType();
    if (pszMType == SIP_NULL)
    {
        SIP_TRACE_NORMAL(ESIPTRACE_MODDECODER,
            "Wronge Content Type", SIP_ZERO, SIP_ZERO);
        pobjContentType->SipDelete();
        return SIP_FALSE;
    }
    SIP_BOOL bSingleBody = (SIP_BOOL)SipPf_Stricmp(pszMType, MULTIPART);
    /*Case of mime body*/
    if (bSingleBody == SIP_FALSE)
    {
        SIP_CHAR *pszBoundary = pobjContentType->GetBoundary();
        if (pszBoundary == SIP_NULL)
        {
            SIP_TRACE_NORMAL(ESIPTRACE_MODDECODER, "No boundary in Content Type Hdr",
                SIP_ZERO, SIP_ZERO);
            pobjContentType->SipDelete();
            return SIP_FALSE;
        }

        m_pobjBodyList = new SipMsgBodyList();
        if (m_pobjBodyList == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeMIMEMsgBody: Memory Allocation Failed",
                SIP_ZERO, SIP_ZERO);
            pobjContentType->SipDelete();
            delete[] pszBoundary;
            return SIP_FALSE;
        }
        m_eBodyType = MULTI_PART_MIXED_BODY;

        if (m_pobjBodyList->DecodeMIMEBody(pucStartPt,pucEndPt, pszBoundary) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecodeMIMEBody: failed", SIP_ZERO, SIP_ZERO);
            pobjContentType->SipDelete();
            delete[] pszBoundary;
            return SIP_FALSE;
        }
        delete[] pszBoundary;
        pszBoundary = SIP_NULL;
    }
    /*Case of Single Body*/
    else
    {
        SIP_UINT16 usSize = pucEndPt - pucStartPt;
        SIP_CHAR *pcData = new SIP_CHAR[usSize];
        if (pcData == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecodeMIMEMsgBody:Memory Allocation failed",
                SIP_ZERO, SIP_ZERO);
            pobjContentType->SipDelete();
            return SIP_FALSE;
        }
        SipPf_Memcpy(pcData, pucStartPt, usSize);
        m_pucBuffer = pcData;
        m_uiBufLen = usSize;
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecodeMIMEMsgBody: size=%d", usSize, SIP_ZERO);
        m_eBodyType = SINGLE_BODY;
    }
    /*Delete the Content type*/
    pobjContentType->SipDelete();
    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipMsgBody::DecodeMessageSummaryMsgBody
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMsgBody::DecodeMessageSummaryMsgBody
(
 SIP_CHAR    *pucStartPt,
 SIP_CHAR     *pucEndPt
 )
{
    m_pobjMessageSummary = new SipMessageSummary();
    if (m_pobjMessageSummary == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
            "DecodeMessageSummaryMsgBody:Memory Allocation failed", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    if (m_pobjMessageSummary->DecodeMessageSummary(pucStartPt, pucEndPt) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
            "DecodeMessageSummaryMsgBody:Body Decoding failed", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    m_eBodyType = MESSAGE_SUMMARY_BODY;
    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipMsgBody::SetContentType
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMsgBody::SetMimeHdr
(
 SipHeaderBase        *pobjHdrBase
 )
{
    if (m_pobjMIMEHdrs == SIP_NULL)
    {
        m_pobjMIMEHdrs = new SipMIMEHdrs();
    }
    if (m_pobjMIMEHdrs->SetMimeHdrs(pobjHdrBase))
    {
        return SIP_TRUE;
    }
    return SIP_FALSE;
}

/******************************************************************************
 * Function name      : SipMsgBody::SetMsgBuffer
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMsgBody::SetMsgBuffer
(
 const SIP_CHAR    *pkszMsgBuffer,
 SIP_UINT32        uiBufLen
 )
{
    if (pkszMsgBuffer != SIP_NULL)
    {
        SIP_CHAR *pcData = new SIP_CHAR[uiBufLen + SIP_ONE];
        if (pcData == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "SetMsgBuffer:Memory Allocation failed",
                SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
        SipPf_Memcpy(pcData, pkszMsgBuffer, uiBufLen);
        pcData[uiBufLen] = SIP_NULL_CHAR;
        m_pucBuffer = pcData;
        m_uiBufLen = uiBufLen;
        return SIP_TRUE;
    }

    return SIP_FALSE;
}

/******************************************************************************
 * Function name      : SipMsgBody::GetContentType
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipContentTypeHeader* SipMsgBody::GetContentType()
{
    if (m_pobjMIMEHdrs != SIP_NULL)
    {
        return SIP_DYNAMIC_CAST(SipContentTypeHeader*, m_pobjMIMEHdrs->GetContentType());
    }

    return SIP_NULL;
}

/******************************************************************************
 * Function name      : SipMsgBody::GetContentEncoding
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipHeaderBase* SipMsgBody::GetContentEncoding()
{
    if (m_pobjMIMEHdrs != SIP_NULL)
    {
        return m_pobjMIMEHdrs->GetContentEncoding();
    }

    return SIP_NULL;
}

/******************************************************************************
 * Function name      : SipMsgBody::GetContentDisposition
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipHeaderBase* SipMsgBody::GetContentDisposition()
{
    if (m_pobjMIMEHdrs != SIP_NULL)
    {
        return m_pobjMIMEHdrs->GetContentDisposition();
    }

    return SIP_NULL;
}


/******************************************************************************
 * Function name      : SipMsgBody::GetMimeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipHeaderBase* SipMsgBody::GetMimeHdr
(
 SIP_INT32     eHdrType ,
 SIP_UINT32     uiIndex
 )
{
    if (m_pobjMIMEHdrs != SIP_NULL)
    {
        switch(eHdrType)
        {
            case SipHeaderBase::CONTENT_TYPE:
                return m_pobjMIMEHdrs->GetContentType();
                break;

            case SipHeaderBase::CONTENT_DISPOSITION:
                return m_pobjMIMEHdrs->GetContentDisposition();
                break;

            case SipHeaderBase::CONTENT_ENCODING:
                return m_pobjMIMEHdrs->GetContentEncoding();
                break;

            case SipHeaderBase::UNKNOWN:
                return m_pobjMIMEHdrs->GetUnknownHdr(uiIndex);
                break;

            default:
                return SIP_NULL;
        }
    }

    return SIP_NULL;
}

/******************************************************************************
 * Function name      : SipMsgBody::GetMsgBuffer
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMsgBody::GetMsgBuffer
(
 SIP_CHAR    **ppszMsgBuffer
 )
{
    if (m_pucBuffer != SIP_NULL)
    {
        SIP_UINT32 usSize = m_uiBufLen;
        SIP_CHAR *pcData = new SIP_CHAR[usSize + SIP_ONE];
        if (pcData == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "GetMsgBuffer:Memory Allocation failed",
                SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
        SipPf_Memcpy(pcData, m_pucBuffer, usSize);
        pcData[usSize] = SIP_NULL_CHAR;
        *ppszMsgBuffer = pcData;
        return SIP_TRUE;
    }
    return SIP_FALSE;
}

/******************************************************************************
 * Function name      : SipMsgBodyList::SipMsgBodyList
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipMsgBodyList::SipMsgBodyList()
                : m_objBodyList(SipVector<SipMsgBody*>())
{
}


SipMsgBodyList::~SipMsgBodyList()
{
    while(m_objBodyList.IsEmpty() != SIP_TRUE)
    {
        SipMsgBody *pMsgBody =  m_objBodyList.Top();
        pMsgBody->SipDelete();
        m_objBodyList.Pop();
    }
}

/******************************************************************************
 * Function name      : SipMsgBodyList::SipMsgBodyList
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipMsgBodyList::SipMsgBodyList(const SipMsgBodyList &objMsgBodyList)
                :  m_objBodyList(SipVector<SipMsgBody*>())
{
    SIP_UINT32  usSize = objMsgBodyList.m_objBodyList.GetSize();

    for(SIP_UINT32 usCount = SIP_ZERO; usCount < usSize ; usCount++)
    {
        SipMsgBody *pTempBody = objMsgBodyList.m_objBodyList.GetAt(usCount);
        if (pTempBody != SIP_NULL)
        {
            SipMsgBody* pobjBody = new SipMsgBody(*pTempBody);
            if (pobjBody == SIP_NULL)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Copy Constructor Malloc failed",
                    SIP_ZERO, SIP_ZERO);
            }
            if ((m_objBodyList.Add(pobjBody)) < 0)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Copy Constructor Append Failed",
                    SIP_ZERO, SIP_ZERO);
            }
        }
    }
}


/******************************************************************************
 * Function name  : SipMsgBodyList::EncodeBody
 *
 * Description   :
 *
 * Preconditions  :
 *
 * Side Effects  : none
 *****************************************************************************/
SIP_BOOL SipMsgBodyList::EncodeBody
(
 SIP_CHAR     **ppucMsgBuffCurrPos,
 SIP_CHAR     *pszBoundary
 )
{
    SIP_UINT32 iNumBodies = m_objBodyList.GetSize();

    SIP_CHAR        *pucMsgCurrPtr = *ppucMsgBuffCurrPos;
    /*Encoding Of Boundary*/
    SIP_ENC_DASH(pucMsgCurrPtr);
    SipPf_Strcpy( pucMsgCurrPtr,pszBoundary);
    /*Update the current position*/
    SipEnc_UpdateCurrPos(&pucMsgCurrPtr);

    /*Get the message bodies and encode them*/
    for(SIP_UINT32 usCount = SIP_ZERO; usCount < iNumBodies ; usCount++)
    {
        SipMsgBody* pstMsgbody = m_objBodyList.GetAt(usCount);
        if (pstMsgbody == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "EncodeBody Failed - No body",
                SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
        /*Encode the  MIME headers*/
        if (pstMsgbody->IsMimeEncoding() == SIP_TRUE)
        {
            SIP_ENC_CRLF(pucMsgCurrPtr);
            SIP_BOOL bRetStatus = pstMsgbody->EncodeMIMEHdrs(&pucMsgCurrPtr);
            if (bRetStatus == SIP_FALSE)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "EncodeBody Failed - No body",
                    SIP_ZERO, SIP_ZERO);
            }
            /*Put the second CRLF*/
            SIP_ENC_CRLF(pucMsgCurrPtr);
        }
        if (pstMsgbody->GetBodyType() == SipMsgBody::SINGLE_BODY )
        {
            SIP_BOOL bRetStatus = pstMsgbody->EncodeSingleMsgBody(&pucMsgCurrPtr);
            if (bRetStatus == SIP_FALSE)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                        "SipMsgBodyList::EncodeBody Failed",
                        SIP_ZERO,SIP_ZERO);
                return SIP_FALSE;
            }
        }
        else
        {
            if (pstMsgbody->EncodeMIMEMsgBody(&pucMsgCurrPtr) == SIP_FALSE)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "EncodeBody Failed", SIP_ZERO, SIP_ZERO);
                return SIP_FALSE;
            }
        }
        pstMsgbody = SIP_NULL;
        /*Put the Closing Boundary*/
        SIP_ENC_CRLF(pucMsgCurrPtr);
        SIP_ENC_DASH(pucMsgCurrPtr);
        SipPf_Strcpy( pucMsgCurrPtr,pszBoundary);
        SipEnc_UpdateCurrPos(&pucMsgCurrPtr);

    }
    /*End boundary*/
    SIP_ENC_DASH(pucMsgCurrPtr);
    SIP_ENC_CRLF(pucMsgCurrPtr);
    *ppucMsgBuffCurrPos = pucMsgCurrPtr;

    return SIP_TRUE;
}

/******************************************************************************
 * Function name      :SipMsgBody::IsMessageBodySDP();
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMsgBody::IsMessageBodySDP()
{
    SipContentTypeHeader *pObjHdr = GetContentType();
    if (pObjHdr == SIP_NULL)
    {
        return SIP_FALSE;
    }
    const SIP_CHAR *pszSubMType = pObjHdr->GetSubMediaType();
    SIP_INT16 usResult = SipPf_Stricmp(pszSubMType, SDP);
    pObjHdr->SipDelete();
    return (usResult == SIP_ZERO) ? SIP_TRUE : SIP_FALSE;
}

/******************************************************************************
 * Function name      : SipMsgBodyList::add
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/

//SIP_BOOL SipMsgBodyList::add(SipMsgBody* pObj);

SIP_UINT32 SipMsgBodyList::GetTotalBodyLen(SIP_CHAR* pszBoundary)
{
    SIP_UINT32 usCount = m_objBodyList.GetSize();
    if (usCount == SIP_ZERO)
    {
        return SIP_ZERO;
    }
    SIP_CHAR *pucLocalBuffer = new SIP_CHAR[SIP_MAX_MSG_SIZE];
    SIP_CHAR *pucBufferStart = pucLocalBuffer;
    if (pucLocalBuffer == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "SipEnc_SipMsg: Malloc Failed",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    SipPf_Memset(pucLocalBuffer, 0x00, SIP_MAX_MSG_SIZE);

    SipMsgBody* pobjBody = SIP_NULL;
    SIP_UINT32 uiLen = SIP_ZERO;
    /*Now check for length calculation with boundary or without boundary*/
    if (pszBoundary == SIP_NULL)
    {
        pobjBody = m_objBodyList.GetAt(SIP_ZERO);
        pobjBody->GetMsgBuffLen(&uiLen);
    }
    else
    {
        EncodeBody( &pucLocalBuffer , pszBoundary);
        uiLen = SipPf_Strlen(pucBufferStart);
    }
    delete[] pucBufferStart;
    return uiLen;
}

SIP_BOOL SipMsgBodyList::AddBody(SipMsgBody* pObjBody)
{
    if ((m_objBodyList.Add(pObjBody)) < 0)
    {
        return SIP_FALSE;
    }
    pObjBody->increment();
    return SIP_TRUE;
}


SipMsgBody* SipMsgBodyList::GetBodyByIndex(SIP_UINT32 index)
{
    if (index < m_objBodyList.GetSize())
    {
        SipMsgBody* pbjBody = m_objBodyList.GetAt(index);
        pbjBody->increment();
        return pbjBody;
    }
    return SIP_NULL;
}



/******************************************************************************
 * Function name      : SipMsgBodyList::DecodeSingleBody
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMsgBodyList::DecodeSingleBody
(
 SIP_CHAR            *pucStartPt,
 SIP_CHAR             *pucEndPt
 )
{
    /*single body support*/

    SipMsgBody *pobjMsgBody = new SipMsgBody();
    if (pobjMsgBody == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
            "SipMsgBodyList::DecodeSingleBody:Memory Allocation failed",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    if (pobjMsgBody->DecodeSingleMsgBody( pucStartPt, pucEndPt) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
            "SipMsgBodyList::DecodeSingleBody:Body Decoding failed",
            SIP_ZERO, SIP_ZERO);
        pobjMsgBody->SipDelete();
        return SIP_FALSE;
    }
    if (m_objBodyList.Add(pobjMsgBody) < 0)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
            "SipMsgBodyList::DecodeSingleBody:Body Decoding failed",
            SIP_ZERO, SIP_ZERO);
        pobjMsgBody->SipDelete();
        return SIP_FALSE;
    }

    return SIP_TRUE;
}


/******************************************************************************
 * Function name      : SipMsgBodyList::DecodeMIMEBody
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMsgBodyList::DecodeMIMEBody
(
 SIP_CHAR            *pucStartPt,
 SIP_CHAR             *pucEndPt,
 SIP_CHAR            *pszBoundary
 )
{
    /*Boundary check*/
    if (pszBoundary == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
            "SipMsgBodyList::DecodeMIMEBody: Boundary unavailable",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    /*Remove CRLF before boundary*/
    pucStartPt = SkipConsecutiveCRLFs(pucStartPt);

    /*Get the boundrary (Find start of Transport -padding)*/
    /*Update start pt aftr  "--" */
    /*Get the boundrary (In case of No Transport -padding)*/
    /*Update the start point to the sart of boundary*/
    pucStartPt = pucStartPt + SIP_TWO;
    SIP_CHAR *pszTempBoundary= SIP_NULL;
    SIP_CHAR *pucTempPos = SIP_NULL;
    if (sipFindCRLF(pucStartPt, pucEndPt,&pucTempPos) == SIP_TRUE)
    {
        pszTempBoundary = sipCreateString(pucStartPt, pucTempPos);
        if (pszTempBoundary == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipMsgBodyList::DecodeMIMEBody:Memory Allocation failed",
                SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
    }
    else
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
            "SipMsgBodyList::DecodeMIMEBody:Boundary Invalid",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    /*Get */
    SIP_UINT32 uiBoundaryLen = pucTempPos - pucStartPt;
    if (SipPf_Stricmp(pszBoundary, pszTempBoundary) != SIP_ZERO)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
            "SipMsgBodyList::DecodeMIMEBody:Boundary Not Matching",
            SIP_ZERO,SIP_ZERO);
        delete[] pszTempBoundary;
        return SIP_FALSE;
    }
    delete[] pszTempBoundary;
    pszTempBoundary = SIP_NULL;
    /*Update the start point till the start of boundary*/
    pucStartPt = pucStartPt + uiBoundaryLen + SIP_ONE;
    /*Update the Start point to the Start Of headers*/
    pucStartPt = sipSkipFwLWS(pucStartPt,pucEndPt);

    SIP_BOOL bBodyEnd = SIP_FALSE;
    while((pucStartPt <= pucEndPt) && (bBodyEnd == SIP_FALSE))
    {
        SipMsgBody *pobjMsgBody = new SipMsgBody();
        if (pobjMsgBody == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipMsgBodyList::DecodeMIMEBody:Memory Allocation failed",
                SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }
        SIP_CHAR *pcTempEnd = sipFindBodyEnd(pucStartPt, pucEndPt,pszBoundary, &bBodyEnd);
        SIP_BOOL bStatus = pobjMsgBody->DecodeMIMEMsgBody( pucStartPt, pcTempEnd);
        if (bStatus == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipMsgBodyList::DecodeMIMEBody:Body Decoding failed",
                SIP_ZERO,SIP_ZERO);
            pobjMsgBody->SipDelete();
            return bStatus;
        }
        if (m_objBodyList.Add(pobjMsgBody) < 0)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipMsgBodyList::DecodeMIMEBody:Body Decoding failed",
                SIP_ZERO,SIP_ZERO);
            pobjMsgBody->SipDelete();
            return SIP_FALSE;
        }
        pobjMsgBody = SIP_NULL;
        /*Update the start point to the start of next body*/
        /* 2 for -- 1 for len and 2 for CRLF*/
        // pcTempEnd doesn't include CRLF before boundary information
        pucStartPt = pcTempEnd + SIP_TWO + uiBoundaryLen + SIP_FIVE;
    }
    return SIP_TRUE;
}

SIP_BOOL SipMsgBodyList:: DecodeMessageSummaryBody
(
 SIP_CHAR            *pucStartPt,
 SIP_CHAR             *pucEndPt
 )
{
    SipMsgBody *pobjMsgBody = new SipMsgBody();
    if (pobjMsgBody == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
            "SipMsgBodyList::DecodeMessageSummaryBody:Memory Allocation failed",
            SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    SIP_BOOL bStatus = pobjMsgBody->DecodeMessageSummaryMsgBody( pucStartPt, pucEndPt);
    if (bStatus == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
            "SipMsgBodyList::DecodeMessageSummaryBody:Body Decoding failed",
            SIP_ZERO,SIP_ZERO);
        pobjMsgBody->SipDelete();
        return SIP_FALSE;
    }
    bStatus = AddBody( pobjMsgBody);
    if (bStatus == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
            "SipMsgBodyList::DecodeMessageSummaryBody:Body Decoding failed",
            SIP_ZERO,SIP_ZERO);
        pobjMsgBody->SipDelete();
        return SIP_FALSE;
    }
    return SIP_TRUE;
}

SipMessageSummary::SipMessageSummary()
    : m_dStatus(SIPMSGWAITINGNO)
    , m_pobjAddrSpec(SIP_NULL)
    , m_objSummaryLineList(SipVector<SipSummaryLine*>())
    , m_objNameValueList(SipVector<SipNameValue*>())
{
}


SipMessageSummary::SipMessageSummary(const SipMessageSummary &objMessageSummary)
    : m_dStatus(objMessageSummary.m_dStatus)
    , m_pobjAddrSpec(SIP_NULL)
    , m_objSummaryLineList(SipVector<SipSummaryLine*>())
    , m_objNameValueList(SipVector<SipNameValue*>())
{
    if (objMessageSummary.m_pobjAddrSpec != SIP_NULL)
    {
        m_pobjAddrSpec = new SipAddrSpec(*(objMessageSummary.m_pobjAddrSpec));
    }

    SIP_UINT32  usSize = objMessageSummary.m_objSummaryLineList.GetSize();
    for(SIP_INT32 usCount = SIP_ZERO; usCount < usSize ; usCount++)
    {
        SipSummaryLine*    pTempLine = objMessageSummary.m_objSummaryLineList.GetAt(usCount);
        if (pTempLine)
        {
            SipSummaryLine* pobjLine = new SipSummaryLine(*pTempLine);
            if (pobjLine == SIP_NULL)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMsgBodyList::SipMsgBodyList:Copy Constructor Malloc failed",
                    SIP_ZERO,SIP_ZERO);
            }
            if ((m_objSummaryLineList.Add(pobjLine)) < 0)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMsgBodyList::SipMsgBodyList:Copy Constructor Append Failed",
                    SIP_ZERO,SIP_ZERO);
            }
            pobjLine  = SIP_NULL;
        }
    }
    usSize = objMessageSummary.m_objNameValueList.GetSize();
    for(SIP_UINT32 usCount = SIP_ZERO; usCount < usSize ; usCount++)
    {
        SipNameValue*  pTempNmVl = objMessageSummary.m_objNameValueList.GetAt(usCount);
        if (pTempNmVl)
        {
            SipNameValue* pobjNmVl = new SipNameValue(*pTempNmVl);
            if (pobjNmVl == SIP_NULL)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMsgBodyList::SipMsgBodyList:Copy Constructor Malloc failed",
                    SIP_ZERO,SIP_ZERO);
            }
            if (m_objNameValueList.Add(pobjNmVl) < 0)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMsgBodyList::SipMsgBodyList:Copy Constructor Append Failed",
                    SIP_ZERO,SIP_ZERO);
            }
            pobjNmVl  = SIP_NULL;
        }
    }
}

SipMessageSummary::~SipMessageSummary()
{
    if (m_pobjAddrSpec != SIP_NULL)
    {
        m_pobjAddrSpec->SipDelete();
    }

    while(m_objSummaryLineList.IsEmpty() != SIP_TRUE)
    {
        SipSummaryLine *pSummaryLine = m_objSummaryLineList.Top();
        pSummaryLine->SipDelete();
        m_objSummaryLineList.Pop();
    }

    while(m_objNameValueList.IsEmpty() != SIP_TRUE)
    {
        SipNameValue *pNameValue = m_objNameValueList.Top();
        pNameValue->SipDelete();
        m_objNameValueList.Pop();
    }
}

/******************************************************************************
 * Function name      : SipMessageSummary::EncodeMessageSummaryMsgBody
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMessageSummary::EncodeMessageSummary
(
 SIP_CHAR     **ppucCurrPos
 )
{
    SIP_CHAR *pucBody = *ppucCurrPos;

    SipPf_Strcpy( pucBody,"Messages-Waiting: ");
    SipEnc_UpdateCurrPos(&pucBody);

    if (m_dStatus == SIPMSGWAITINGNO)
    {
        SipPf_Strcpy( pucBody,"No");
    }
    else if (m_dStatus == SIPMSGWAITINGYES)
    {
        SipPf_Strcpy( pucBody,"Yes");
    }
    else
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
            "SipMessageSummary::EncodeMessageSummaryMsgBody: Missing Message Waiting Status ",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SipEnc_UpdateCurrPos(&pucBody);
    SIP_ENC_CRLF(pucBody);

    if (m_pobjAddrSpec != SIP_NULL)
    {
        SipPf_Strcpy( pucBody,"Messages-Account: ");
        SipEnc_UpdateCurrPos(&pucBody);

        if (m_pobjAddrSpec->EncodeAddrSpec(&pucBody) == SIP_FALSE )
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "SipMessageSummary::EncodeMessageSummaryMsgBody: Addr Spec failed",
                SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

    }

    SIP_UINT32 uSize = m_objSummaryLineList.GetSize();
    SIP_UINT16 uIndex = SIP_ZERO;
    while(uIndex < uSize)
    {
        SipSummaryLine       *pobjSummary = m_objSummaryLineList.GetAt(uIndex);
        SipPf_Strcpy(pucBody, pobjSummary->GetMedia());

        SipEnc_UpdateCurrPos(&pucBody);
        SIP_ENC_COLON(pucBody);

        SIP_CHAR                    newmsg[SIP_MAX_INT];
        SipPf_Sprintf(newmsg, (SIP_CHAR*)"%d", pobjSummary->GetNewMessages());
        SipPf_Strcpy(pucBody, newmsg);
        SipEnc_UpdateCurrPos(&pucBody);
        SIP_ENC_SLASH(pucBody);

        SIP_CHAR                    oldmsg[SIP_MAX_INT];
        SipPf_Sprintf(oldmsg,(SIP_CHAR*)"%d",pobjSummary->GetOldMessages());
        SipPf_Strcpy(pucBody, oldmsg);
        SipEnc_UpdateCurrPos(&pucBody);


        if ((pobjSummary->GetOldUrgentMessages()) || (pobjSummary->GetNewUrgentMessages()))
            /* if there are urgent messages */
        {
            SIP_ENC_LPAREN(pucBody);

            SIP_CHAR                    newurgentmsg[SIP_MAX_INT];
            SipPf_Sprintf(newurgentmsg, (SIP_CHAR*)"%d", pobjSummary->GetNewUrgentMessages());
            SipPf_Strcpy(pucBody, newurgentmsg);
            SipEnc_UpdateCurrPos(&pucBody);
            SIP_ENC_SLASH(pucBody);

            SIP_CHAR                    oldurgentmsg[SIP_MAX_INT];
            SipPf_Sprintf(oldurgentmsg,(SIP_CHAR*)"%d",pobjSummary->GetOldUrgentMessages());
            SipPf_Strcpy(pucBody, oldurgentmsg);
            SipEnc_UpdateCurrPos(&pucBody);

            SIP_ENC_RPAREN(pucBody);
        }

        SIP_ENC_CRLF(pucBody);
        uIndex++;
    }

    if (uSize)
    {
        SIP_ENC_CRLF(pucBody);
    }

    uIndex = 0;
    uSize = m_objNameValueList.GetSize();
    while (uIndex < uSize)
    {
        SipNameValue *pcNmVl = m_objNameValueList.GetAt(uIndex);

        SipPf_Strcpy(pucBody, pcNmVl->m_pszName);//copy name
        SipEnc_UpdateCurrPos(&pucBody);
        SIP_ENC_COLON(pucBody);

        SIP_UINT32  uSizeVl = pcNmVl->m_valueList.GetSize();
        SIP_UINT32 uIndexVl = SIP_ZERO;
        while (uIndexVl < uSizeVl)
        {
            SIP_CHAR *pcval = pcNmVl->m_valueList.GetAt(uIndexVl);
            SipPf_Strcpy(pucBody, pcval);
            SipEnc_UpdateCurrPos(&pucBody);

            if (uIndexVl < (uSizeVl - SIP_ONE))
            {
                SIP_ENC_COMMA(pucBody);
            }
            uIndexVl++;
        }

        SIP_ENC_CRLF(pucBody);
        uIndex++;
    }
    SIP_ENC_CRLF(pucBody);

    *ppucCurrPos = pucBody;

    return SIP_TRUE;

}


/******************************************************************************
 * Function name      : SipMessageSummary::DecodeMessageSummaryMsgBody
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMessageSummary::DecodeMessageSummary
(
 SIP_CHAR    *pucStartPt,
 SIP_CHAR     *pucEndPt
 )
{
    SIP_UINT16 usSize = pucEndPt - pucStartPt + SIP_ONE;

    if (usSize <= SIP_ZERO)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipMessageSummary::DecodeMessageSummaryMsgBody:InvalidInput",
                SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    //Message Waiting = yes/no
    SIP_CHAR           *pucTempPos = SIP_NULL;

    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, COLON) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipMessageSummary::DecodeMessageSummaryMsgBody: COLON Not Found",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR *pucTempNext = pucTempPos + SIP_TWO;
    pucTempNext = sipSkipFwLWS(pucTempNext,pucEndPt);
    /*skip the WSP form back*/
    pucTempPos = sipSkipRwWSP( pucStartPt,pucTempPos);

    SIP_CHAR *TempStr = sipCreateString(pucStartPt,  pucTempPos);
    if (TempStr == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipMessageSummary::DecodeMessageSummaryMsgBody: Memory Allocation failed",
                SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    if (SipPf_Stricmp(TempStr, "Messages-Waiting"))
    {
        delete[] TempStr;
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipMessageSummary::DecodeMessageSummaryMsgBody:InvalidInput",
                SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    delete[] TempStr;
    TempStr = SIP_NULL;
    pucStartPt = pucTempNext;

    if (sipFindCRLF(pucStartPt, pucEndPt, &pucTempPos) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipMessageSummary::DecodeMessageSummaryMsgBody: CRLF Not Found",
                SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    pucTempNext = pucTempPos + SIP_THREE;
    pucTempNext = sipSkipFwLWS(pucTempNext,pucEndPt);
    /*skip the WSP form back*/
    pucTempPos = sipSkipRwWSP( pucStartPt,pucTempPos);

    TempStr = sipCreateString(pucStartPt,  pucTempPos);
    if (TempStr == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipMessageSummary::DecodeMessageSummaryMsgBody: Memory Allocation failed",
                SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    if (SipPf_Stricmp(TempStr, "Yes") == SIP_ZERO)
    {
        m_dStatus = SIPMSGWAITINGYES;
    }
    else if (SipPf_Stricmp(TempStr, "No") == SIP_ZERO)
    {
        m_dStatus = SIPMSGWAITINGNO;
    }
    else
    {
        delete[] TempStr;
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipMessageSummary::DecodeMessageSummaryMsgBody:InvalidInput",
                SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    delete[] TempStr;
    TempStr = SIP_NULL;
    pucStartPt = pucTempNext;

    if (pucStartPt < pucEndPt)
    {
        if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, COLON) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMessageSummary::DecodeMessageSummaryMsgBody: COLON Not Found",
                    SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }

        pucTempNext = pucTempPos + SIP_TWO;
        pucTempNext = sipSkipFwLWS(pucTempNext,pucEndPt);
        /*skip the WSP form back*/
        pucTempPos = sipSkipRwWSP( pucStartPt,pucTempPos);

        TempStr = sipCreateString(pucStartPt,  pucTempPos);
        if (TempStr == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMessageSummary::DecodeMessageSummaryMsgBody: Memory Allocation failed",
                    SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        if (SipPf_Stricmp(TempStr, "Message-Account"))
        {
            delete[] TempStr;
            TempStr = SIP_NULL;
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMessageSummary::DecodeMessageSummaryMsgBody:InvalidInput",
                    SIP_ZERO, SIP_ZERO);
            goto label;
        }
        delete[] TempStr;
        pucStartPt = pucTempNext;

        if (sipFindCRLF(pucStartPt, pucEndPt, &pucTempPos) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMessageSummary::DecodeMessageSummaryMsgBody: CRLF Not Found",
                    SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
        pucTempPos = sipSkipRwWSP( pucStartPt,pucTempPos);
        m_pobjAddrSpec = new SipAddrSpec();
        if (m_pobjAddrSpec == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMessageSummary::DecodeMessageSummaryMsgBody: Memory Allocation failed",
                    SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        SIP_UINT32 uiDecLen = pucTempPos - pucStartPt +SIP_ONE;
        if (m_pobjAddrSpec ->DecodeAddrSpec(pucStartPt, uiDecLen) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMessageSummary::DecodeMessageSummaryMsgBody: URI Wrong",
                    SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
        pucTempNext = pucTempPos + SIP_THREE;
        pucTempNext = sipSkipFwLWS(pucTempNext,pucEndPt);
        pucStartPt = pucTempNext;


    }
label:
    //Summary line

    while(pucStartPt < pucEndPt)
    {
        if (sipFindCRLF(pucStartPt, pucEndPt, &pucTempPos) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMessageSummary::DecodeMessageSummaryMsgBody: CRLF Not Found",
                    SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
        pucTempPos = sipSkipRwWSP( pucStartPt,pucTempPos);
        SipSummaryLine *pobjSummary = new SipSummaryLine();
        if (pobjSummary == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMessageSummary::DecodeMessageSummaryMsgBody: Memory Allocation failed",
                    SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        if (pobjSummary->DecodeSummaryLine(pucStartPt, pucTempPos + SIP_TWO) == SIP_FALSE)
        {
            delete pobjSummary;
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMessageSummary::DecodeMessageSummaryMsgBody:InvalidInput",
                    SIP_ZERO, SIP_ZERO);
            break;
        }

        if (m_objSummaryLineList.Add(pobjSummary) < 0)
        {
            delete pobjSummary;
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMessageSummary::DecodeMessageSummaryMsgBody: Node not added",
                    SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
        pucTempNext = pucTempPos + SIP_THREE;
        pucTempNext = sipSkipFwLWS(pucTempNext,pucEndPt);
        pucStartPt = pucTempNext;
    }

    //Name Value Validation
    while(pucStartPt < pucEndPt)
    {
        if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, COLON) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMessageSummary::DecodeMessageSummaryMsgBody: CRLF Not Found",
                    SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        pucTempPos = sipSkipRwWSP( pucStartPt,pucTempPos);
        pucTempNext = pucTempPos + SIP_TWO;
        pucTempNext = sipSkipFwLWS(pucTempNext,pucEndPt);

        TempStr = sipCreateString(pucStartPt,  pucTempPos);
        if (TempStr == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMessageSummary::DecodeMessageSummaryMsgBody: Memory Allocation failed",
                    SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
        SipNameValue *pucNmVl = new SipNameValue();
        if (pucNmVl == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMessageSummary::DecodeMessageSummaryMsgBody: Memory Allocation failed",
                    SIP_ZERO, SIP_ZERO);
            delete[] TempStr;
            return SIP_FALSE;
        }
        pucNmVl->m_pszName = TempStr;
        pucStartPt = pucTempNext;

        while(sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, COMMA))
        {
            if (sipFindCRLF(pucStartPt, pucTempPos, &pucTempPos))
            {
                break;
            }
            pucTempPos = sipSkipRwWSP( pucStartPt,pucTempPos);
            pucTempNext = pucTempPos + SIP_TWO;
            pucTempNext = sipSkipFwLWS(pucTempNext,pucEndPt);

            TempStr = sipCreateString(pucStartPt,  pucTempPos);
            if (TempStr == SIP_NULL)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                        "SipMessageSummary::DecodeMessageSummaryMsgBody: Memory Allocation failed",
                        SIP_ZERO, SIP_ZERO);
                delete pucNmVl;
                return SIP_FALSE;
            }
            if (pucNmVl->m_valueList.Add(TempStr) < 0)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                        "SipMessageSummary::DecodeMessageSummaryMsgBody: Node not added",
                        SIP_ZERO, SIP_ZERO);
                delete pucNmVl;
                return SIP_FALSE;
            }
            pucStartPt = pucTempNext;
        }

        if (sipFindCRLF(pucStartPt, pucEndPt, &pucTempPos) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMessageSummary::DecodeMessageSummaryMsgBody: CRLF Not Found",
                    SIP_ZERO, SIP_ZERO);
            delete pucNmVl;
            return SIP_FALSE;
        }
        pucTempPos = sipSkipRwWSP( pucStartPt,pucTempPos);
        pucTempNext = pucTempPos + SIP_THREE;
        pucTempNext = sipSkipFwLWS(pucTempNext,pucEndPt);
        TempStr = sipCreateString(pucStartPt, pucTempPos);
        if (TempStr == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMessageSummary::DecodeMessageSummaryMsgBody: Memory Allocation failed",
                    SIP_ZERO, SIP_ZERO);
           delete pucNmVl;
           return SIP_FALSE;
        }
        if (pucNmVl->m_valueList.Add(TempStr) < 0)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMessageSummary::DecodeMessageSummaryMsgBody: Node not added",
                    SIP_ZERO, SIP_ZERO);
            delete[] TempStr;
            delete pucNmVl;
            return SIP_FALSE;
        }
        pucStartPt = pucTempNext;
        if (m_objNameValueList.Add(pucNmVl) < 0)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMessageSummary::DecodeMessageSummaryMsgBody: Node not added",
                    SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
    }

    return SIP_TRUE;
}

SipSummaryLine::SipSummaryLine()
    : pMedia(SIP_NULL), newMessages(SIP_ZERO), oldMessages(SIP_ZERO)
    , newUrgentMessages(SIP_ZERO), oldUrgentMessages(SIP_ZERO)
{

}

SipSummaryLine::SipSummaryLine(const SipSummaryLine &objSummaryLine)
    : pMedia(SipPf_Strdup(objSummaryLine.GetMedia()))
    , newMessages(objSummaryLine.GetNewMessages())
    , oldMessages(objSummaryLine.GetOldMessages())
    , newUrgentMessages(objSummaryLine.GetNewUrgentMessages())
    , oldUrgentMessages(objSummaryLine.GetOldUrgentMessages())
{
}

SipSummaryLine::~SipSummaryLine()
{
    if (pMedia != SIP_NULL)
    {
        delete[] pMedia;
    }
}

SIP_BOOL SipSummaryLine :: DecodeSummaryLine
(
 SIP_CHAR * pucStartPt,
 SIP_CHAR * pucEndPt
 )
{

    if (pucStartPt >=pucEndPt)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipSummaryLine :: DecodeSummaryLine:InvalidInput",
                SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR *pucTempPos = SIP_NULL;
    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, COLON) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipSummaryLine :: DecodeSummaryLine: COLON Not Found",
                SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR *pucTempNext = pucTempPos + SIP_TWO;
    pucTempNext = sipSkipFwLWS(pucTempNext,pucEndPt);
    /*skip the WSP form back*/
    pucTempPos = sipSkipRwWSP( pucStartPt,pucTempPos);

    SIP_CHAR *TempStr = sipCreateString(pucStartPt, pucTempPos);
    if (TempStr == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipMessageSummary::DecodeMessageSummaryMsgBody: Memory Allocation failed",
                SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    pMedia = TempStr;

    pucStartPt = pucTempNext;
    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, SLASH) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipSummaryLine :: DecodeSummaryLine: COLON Not Found",
                SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    pucTempNext = pucTempPos + SIP_TWO;
    pucTempNext = sipSkipFwLWS(pucTempNext,pucEndPt);
    pucTempPos = sipSkipRwWSP( pucStartPt,pucTempPos);

    TempStr = sipCreateString(pucStartPt, pucTempPos);
    if (TempStr == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipMessageSummary::DecodeMessageSummaryMsgBody: Memory Allocation failed",
                SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }


    newMessages = SipPf_Atoi(TempStr);
    delete[] TempStr;

    pucStartPt = pucTempNext;

    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, LPARAN) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipSummaryLine :: DecodeSummaryLine: PARAN Not Found",
                SIP_ZERO, SIP_ZERO);

        if (sipFindCRLF(pucStartPt, pucEndPt, &pucTempPos) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMessageSummary::DecodeMessageSummaryMsgBody: CRLF Not Found",
                    SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
    }
    pucTempNext = pucTempPos + SIP_TWO;
    pucTempPos = sipSkipRwWSP( pucStartPt,pucTempPos);
    pucTempNext = sipSkipFwLWS(pucTempNext,pucEndPt);

    TempStr = sipCreateString(pucStartPt, pucTempPos);
    if (TempStr == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipMessageSummary::DecodeMessageSummaryMsgBody: Memory Allocation failed",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    oldMessages = SipPf_Atoi(TempStr);
    delete[] TempStr;

    pucStartPt = pucTempNext;
    if (pucStartPt <pucEndPt)
    {
        if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, SLASH) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipSummaryLine :: DecodeSummaryLine: COLON Not Found",
                    SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
        pucTempPos = sipSkipRwWSP( pucStartPt,pucTempPos);

        TempStr = sipCreateString(pucStartPt, pucTempPos);
        if (TempStr == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMessageSummary::DecodeMessageSummaryMsgBody: Memory Allocation failed",
                    SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        newUrgentMessages = SipPf_Atoi(TempStr);
        delete[] TempStr;
        pucTempNext = pucTempPos + SIP_TWO;
        pucTempNext = sipSkipFwLWS(pucTempNext,pucEndPt);
        pucStartPt = pucTempNext;

        if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, RPARAN) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipSummaryLine :: DecodeSummaryLine: COLON Not Found",
                    SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
        pucTempPos = sipSkipRwWSP( pucStartPt,pucTempPos);

        TempStr = sipCreateString(pucStartPt, pucTempPos);
        if (TempStr == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipMessageSummary::DecodeMessageSummaryMsgBody: Memory Allocation failed",
                    SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        oldUrgentMessages = SipPf_Atoi(TempStr);
        delete[] TempStr;
        pucTempNext = pucTempPos + SIP_THREE;
        pucTempNext = sipSkipFwLWS(pucTempNext,pucEndPt);
    }
    else
    {
        pucTempNext = pucTempPos + SIP_TWO;
        pucTempNext = sipSkipFwLWS(pucTempNext,pucEndPt);
    }
    if (pucTempNext < pucEndPt)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipSummaryLine :: DecodeSummaryLine:InvalidInput",
                SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    return SIP_TRUE;
}
