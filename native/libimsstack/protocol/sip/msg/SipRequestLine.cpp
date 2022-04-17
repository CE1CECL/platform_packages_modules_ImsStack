/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipRequestLine.cpp
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
#include "sip_pf_datatypes.h"
#include "SipTrace.h"
#include "platform/sip_pf_memory.h"
#include "sip_debug.h"
#include "sip_error.h"
#include "platform/sip_pf_string.h"
#include "msg/SipAddrSpec.h"
#include "msg/SipRequestLine.h"
#include "msg/SipMessage.h"
#include "msg/sip_msgutil.h"

/****************************************************************************
  Macro Definitions
 *****************************************************************************/

/****************************************************************************
  Class Member Function Implementations
 *****************************************************************************/

/******************************************************************************
 * Function name      : SipRequestLine::SipRequestLine
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipRequestLine::SipRequestLine()
    : m_pszMethod(SIP_NULL)
    , m_pobjReqUri(SIP_NULL)
    , m_pszSipVersion(SIP_NULL)
{
}

/******************************************************************************
 * Function name      : SipRequestLine::SipRequestLine
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipRequestLine::SipRequestLine(SIP_CHAR *pszMethod, SipAddrSpec *pobjReqUri,
        const SIP_CHAR * /*pszSipVersion*/)
    : m_pszMethod(SipPf_Strdup(pszMethod))
    , m_pobjReqUri(pobjReqUri)
    , m_pszSipVersion(SipPf_Strdup(SIP_SIPVER))
{
}

/******************************************************************************
 * Function name      : SipRequestLine::SipRequestLine
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipRequestLine::SipRequestLine(const SipRequestLine &objHeader)
    : m_pszMethod(SipPf_Strdup(objHeader.m_pszMethod))
    , m_pobjReqUri(SIP_NULL)
    , m_pszSipVersion(SipPf_Strdup(objHeader.m_pszSipVersion))
{
    if (objHeader.m_pobjReqUri != SIP_NULL)
    {
        m_pobjReqUri = new SipAddrSpec(*(objHeader.m_pobjReqUri));
        m_pobjReqUri->SetParameterComponent(this);
    }
}

/******************************************************************************
 * Function name      : SipRequestLine::~SipRequestLine
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipRequestLine::~SipRequestLine()
{
    if (m_pszMethod != SIP_NULL)
    {
        delete[] m_pszMethod;
    }
    if (m_pobjReqUri != SIP_NULL)
    {
        m_pobjReqUri->SetParameterComponent(SIP_NULL);
        m_pobjReqUri->SipDelete();
    }
    if (m_pszSipVersion != SIP_NULL)
    {
        delete[] m_pszSipVersion;
    }
}

/******************************************************************************
 * Function name      : SipRequestLine::EncodeRequestLine
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipRequestLine::EncodeRequestLine
(
 SIP_CHAR     **ppucCurrPos
 )
{
    /*check for existence of Method, request uri and version */
    if (m_pszMethod == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "SipEnc_RequestLine: Method missing",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    if (m_pobjReqUri == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "SipEnc_RequestLine: Request Uri missing",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    if (m_pszSipVersion == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "SipEnc_RequestLine: Sip Version missing",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    /*Encode Method*/
    SipPf_Strcpy( *ppucCurrPos,m_pszMethod);
    /*Update the Msg Buffer's current position*/
    SipEnc_UpdateCurrPos(ppucCurrPos);

    /* Put a space */
    SIP_ENC_SP(*ppucCurrPos);

    /*Set Startline for Percent Encoding*/
    m_pobjReqUri->SetParameterComponent(this);

    /* Encode Request Uri*/
    m_pobjReqUri->EncodeAddrSpec(ppucCurrPos);

    SIP_ENC_SP(*ppucCurrPos);
    SipPf_Strcpy(*ppucCurrPos,m_pszSipVersion);

    /*Update the Msg Buffer's current position*/
    SipEnc_UpdateCurrPos(ppucCurrPos);

    return SIP_TRUE;
}


/******************************************************************************
 * Function name      : SipRequestLine::SetMethod
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipRequestLine::SetMethod
(
 const SIP_CHAR    *pkszMethod
 )
{
    return SetCharVar(pkszMethod,m_pszMethod);
}

/******************************************************************************
 * Function name      : SipRequestLine::SetSipVersion
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipRequestLine::SetSipVersion
(
 const SIP_CHAR    *pkszVer
 )
{
    return SetCharVar(pkszVer,m_pszSipVersion);
}

/******************************************************************************
 * Function name      : SipRequestLine::SetReqUri
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipRequestLine::SetReqUri
(
 SipAddrSpec    *pobjAddrSpec
 )
{
    if (m_pobjReqUri != SIP_NULL)
    {
        m_pobjReqUri->SipDelete();
    }

    if (pobjAddrSpec)
    {
        m_pobjReqUri = pobjAddrSpec;
        m_pobjReqUri->SetParameterComponent(this);
        m_pobjReqUri->increment();
    }
    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipRequestLine::GetReqUri
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipAddrSpec* SipRequestLine::GetReqUri()
{
    if (m_pobjReqUri != SIP_NULL)
    {
        m_pobjReqUri->increment();
    }

    return m_pobjReqUri;
}

/******************************************************************************
 * Function name      : SipRequestLine::IsValidComponent
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipRequestLine::IsValidComponent(const SIP_CHAR *pucComponent) const
{

    if (SipPf_Stricmp(pucComponent, SIP_USER) == 0)
    {
        return SIP_TRUE;
    }
    else if (SipPf_Stricmp(pucComponent, SIP_PASSWORD) == 0)
    {
        return SIP_TRUE;
    }
    else if (SipPf_Stricmp(pucComponent, SIP_HOST) == 0)
    {
        return SIP_TRUE;
    }
    else if (SipPf_Stricmp(pucComponent, SIP_PORT) == 0)
    {
        return SIP_TRUE;
    }
    else if (SipPf_Stricmp(pucComponent, SIP_USER_PRM) == 0)
    {
        return SIP_TRUE;
    }
    else if (SipPf_Stricmp(pucComponent, SIP_METHOD) == 0)
    {
        return SIP_FALSE;
    }
    else if (SipPf_Stricmp(pucComponent, SIP_MADDR_PRM) == 0)
    {
        return SIP_TRUE;
    }
    else if (SipPf_Stricmp(pucComponent, SIP_TTL_PRM) == 0)
    {
        return SIP_TRUE;
    }
    else if (SipPf_Stricmp(pucComponent, SIP_TRNSPORT_PRM) == 0)
    {
        return SIP_TRUE;
    }
    else if (SipPf_Stricmp(pucComponent, SIP_LR_PRM) == 0)
    {
        return SIP_TRUE;
    }
    else if (SipPf_Stricmp(pucComponent, SIP_OTHER_PRM) == 0)
    {
        return SIP_TRUE;
    }
    else if (SipPf_Stricmp(pucComponent, SIP_HEADERS) == 0)
    {
        return SIP_FALSE;
    }
    return SIP_FALSE;
}



/******************************************************************************
 * Function name      : SipRequestLine::DecodeRequestLine
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipRequestLine::DecodeRequestLine
(
 SIP_CHAR    *pucStartPt,
 SIP_UINT32 uiDecLen
 )
{
    SIP_CHAR            *pucTempLoc = SIP_NULL;
    SIP_CHAR            *pucEndPt = pucStartPt +uiDecLen  - SIP_ONE;

    /*find first space i.e. end of Method*/
    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempLoc, SPACE) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipRequestLine::DecodeRequestLine: Space Not Found",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    /*Create a NULL terminated String of Method*/
    m_pszMethod = sipCreateString(pucStartPt, pucTempLoc);
    if (m_pszMethod == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipRequestLine::DecodeRequestLine:Memory Allocation failed",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    /*Set the method Req line member*/
    /*update the start point */
    pucStartPt = pucTempLoc + SIP_TWO;
    pucTempLoc = SIP_NULL;
    /*find Second space i.e. end of Req URI*/
    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempLoc, SPACE) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipRequestLine::DecodeRequestLine: Space Not Found",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    /*Decode the request URI*/
    SIP_UINT32 uiTempLen = pucTempLoc - pucStartPt + SIP_ONE;
    m_pobjReqUri = new SipAddrSpec();
    if (m_pobjReqUri == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipRequestLine::DecodeRequestLine:Memory Allocation failed",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    /*Set the Requestline For Percent Encoding*/
    m_pobjReqUri->SetParameterComponent(static_cast<IParameterComponent*>(this));

    /*Check for validity of Address Spec of Req URI */
#ifdef SIP_STRICT_PARSING
    if (IsValidAddress(pucStartPt,  uiTempLen) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipRequestLine::DecodeRequestLine: Address Spec is Invalid",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

#endif

    if (m_pobjReqUri->DecodeAddrSpec(pucStartPt, uiTempLen) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipRequestLine::DecodeRequestLine:Addr Spec decode failed",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    /*Take the ptr to the start of  Sip Version*/
    pucStartPt = pucTempLoc + SIP_TWO;
    pucTempLoc = SIP_NULL;
    m_pszSipVersion = sipCreateString(pucStartPt, pucEndPt);
    if (m_pszSipVersion == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipRequestLine::DecodeRequestLine:Memory Allocation failed",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipStatusLine::DecodeStatusLine
 *
 * Description     :
 *
 * Preconditions          SIP_CHAR    *pucStartPt,
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipStatusLine::DecodeStatusLine
(
 SIP_CHAR    *pucStartPt,
 SIP_UINT32 uiDecLen
 )
{
    SIP_CHAR            *pucEndPt = pucStartPt +uiDecLen  - SIP_ONE;
    SIP_CHAR            *pucTempLoc = SIP_NULL;

    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempLoc, SPACE) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipStatusLine::DecodeStatusLine: Space Not Found",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    m_pszSipVersion = sipCreateString(pucStartPt, pucTempLoc);
    if (m_pszSipVersion == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipStatusLine::DecodeStatusLine: Memory Allocation Failed",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    /*update the start point to the start of Status Code*/
    pucStartPt = pucTempLoc + SIP_TWO;
    pucTempLoc = SIP_NULL;
    /*Find the endpoint of status code*/
    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempLoc, SPACE) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipStatusLine::DecodeStatusLine: Space Not Found",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    m_pszStatusCode = sipCreateString(pucStartPt, pucTempLoc);
    if (m_pszStatusCode == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipStatusLine::DecodeStatusLine: Memory Allocation Failed",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    /*Update the start point to the start of reason phrase*/
    pucStartPt =  pucTempLoc + SIP_TWO;
    m_pszRsnPhrase = sipCreateString(pucStartPt, pucEndPt);
    if (m_pszRsnPhrase == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipStatusLine::DecodeStatusLine: No Reason phrase present in response line",
                SIP_ZERO,SIP_ZERO);
    }

    return SIP_TRUE;
}
