/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipViaHeader.cpp
 * Purpose               :
 * Platform              : Windows OR Android
 * Author(s)           :
 * E-mail id.            : saurabh31.srivastava@
 * Creation date       : July. 26, 2010
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
#include "IPAddress.h"
#include "msg/SipAddrSpec.h"
#include "platform/sip_pf_string.h"
#include "platform/sip_pf_memory.h"
#include "sip_error.h"
#include "sip_debug.h"
#include "SipTrace.h"
#include "msg/SipViaHeader.h"
#include "SipConfiguration.h"
#include "msg/sip_msgutil.h"

/****************************************************************************
  Macro Definitions
 *****************************************************************************/

/****************************************************************************
  Class Member Function Implementations
 *****************************************************************************/


/******************************************************************************
 * Function name      : SipViaHeader::SipViaHeader
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipViaHeader::SipViaHeader()
    : SipHeaderBase(SipHeaderBase::VIA)
    , m_pszProtocolName(SIP_NULL)
    , m_pszProtocolVer(SIP_NULL)
    , m_pszTransport(SIP_NULL)
    , m_pszHost(SIP_NULL)
    , m_usPort(SIP_ZERO)
    , m_eHostType(SipAddrSpec::HOST_NAME)
{
}
/******************************************************************************
 * Function name      : SipViaHeader(SipViaHeader*)
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipViaHeader::SipViaHeader(const SipViaHeader &objHeader)
    : SipHeaderBase(objHeader)
    , m_pszProtocolName(SipPf_Strdup(objHeader.GetProtocolName()))
    , m_pszProtocolVer(SipPf_Strdup(objHeader.GetProtocolVer()))
    , m_pszTransport(SipPf_Strdup(objHeader.GetTransport()))
    , m_pszHost(SipPf_Strdup(objHeader.GetHost()))
    , m_usPort(objHeader.GetPort())
    , m_eHostType(objHeader.m_eHostType)
{
}

/******************************************************************************
 * Function name      : SipViaHeader::~SipViaHeader
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/

SipViaHeader::~SipViaHeader()
{
    if (m_pszProtocolName != SIP_NULL)
    {
        delete[] m_pszProtocolName;
    }
    if (m_pszProtocolVer != SIP_NULL)
    {
        delete[] m_pszProtocolVer;
    }
    if (m_pszTransport != SIP_NULL)
    {
        delete[] m_pszTransport;
    }
    if (m_pszHost != SIP_NULL)
    {
        delete[] m_pszHost;
    }
}

/******************************************************************************
 * Function name      : SipViaHeader::EncodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/

SIP_BOOL SipViaHeader::EncodeHdr
(
    SIP_CHAR     **ppucCurrPos,
    SIP_BOOL     bParams /*Default = SIP_TRUE*/
)
{
    /* Encoding of header value*/
    /*Encode sent protocol
      sent-protocol = protocol-name SLASH protocol-version SLASH transport */
    /*protocol name*/
    if (m_pszProtocolName == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "Missing Protocol ",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SipPf_Strcpy(*ppucCurrPos, m_pszProtocolName);
    SipEnc_UpdateCurrPos(ppucCurrPos);

    SIP_ENC_SLASH(*ppucCurrPos);

    /*protocol-version*/
    if (m_pszProtocolVer == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "Missing Protocol Version ",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    SipPf_Strcpy(*ppucCurrPos,m_pszProtocolVer);
    SipEnc_UpdateCurrPos(ppucCurrPos);

    SIP_ENC_SLASH(*ppucCurrPos);

    /*transport*/
    if (m_pszTransport == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "Missing Transport ",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    SipPf_Strcpy(*ppucCurrPos,m_pszTransport);
    SipEnc_UpdateCurrPos(ppucCurrPos);

    /*put a space for LWS*/
    SIP_ENC_SP(*ppucCurrPos);

    /*Encode sent by*/
    /*sent-by = host [ COLON port ] */
    if (m_pszHost == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "Missing Host ",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    if ((m_eHostType == SipAddrSpec::HOST_IPV4) || (m_eHostType == SipAddrSpec::HOST_IPV6))
    {
        if (m_eHostType == SipAddrSpec::HOST_IPV6)
        {
            //Left Square and Right Square Bracket already included in Host
            SipPf_Strcpy(*ppucCurrPos, m_pszHost);
            SipEnc_UpdateCurrPos(ppucCurrPos);
        }
        /*Case of IPV4*/
        else
        {
            SipPf_Strcpy(*ppucCurrPos, m_pszHost);
            SipEnc_UpdateCurrPos(ppucCurrPos);
        }
    }
    /*Case of Host name*/
    else
    {
        SipPf_Strcpy(*ppucCurrPos, m_pszHost);
        SipEnc_UpdateCurrPos(ppucCurrPos);
    }
#define MAX_PORT_LEN 20
    if (m_usPort != SIP_ZERO)
    {
        SIP_CHAR ucTmp[MAX_PORT_LEN];
        SipPf_Sprintf(ucTmp, (SIP_CHAR*)"%u", m_usPort);

        SIP_ENC_COLON(*ppucCurrPos);

        SipPf_Strcpy(*ppucCurrPos, ucTmp);
        SipEnc_UpdateCurrPos(ppucCurrPos);
    }

    return EncodeHeaderParameters(ppucCurrPos, bParams);
}


/*set methods*/
/******************************************************************************
 * Function name      : SipViaHeader::SetProtocolName
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipViaHeader::SetProtocolName
(
 const SIP_CHAR    *pkszProtocolNm
 )
{
    return SetCharVar(pkszProtocolNm,m_pszProtocolName);
}

/******************************************************************************
 * Function name      : SipViaHeader::SetProtocolVer
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipViaHeader::SetProtocolVer
(
 const SIP_CHAR    *pkszProtocolVer
 )
{
    return SetCharVar(pkszProtocolVer,m_pszProtocolVer);
}

/******************************************************************************
 * Function name      : SipViaHeader::SetTransport
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipViaHeader::SetTransport
(
 const SIP_CHAR    *pkszTransport
 )
{
    return SetCharVar(pkszTransport,m_pszTransport);
}

/******************************************************************************
 * Function name      : SipViaHeader::SetHost
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipViaHeader::SetHost
(
 const SIP_CHAR    *pkszHost
 )
{
    return SetCharVar(pkszHost,m_pszHost);
}
/******************************************************************************
 * Function name      : SipViaHeader::SetHost
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/

SIP_BOOL SipViaHeader::SetPortNum
(
 SIP_UINT16        usPort
 )
{
    m_usPort = usPort;
    return SIP_TRUE;
}



/*Get methods*/
/******************************************************************************
 * Function name      : SipViaHeader::GetBranch
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
const SIP_CHAR* SipViaHeader::GetBranch() const
{
    SipParameters *pParameters = GetParameters();

    if (pParameters == SIP_NULL)
    {
        return SIP_NULL;
    }
    SipParameterList *pParameterList = pParameters->GetParameterList();

    return pParameterList->GetParamValue("branch");
}
/******************************************************************************
 * Function name      : SipViaHeader::SetRecvdParam
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/

SIP_BOOL SipViaHeader::SetRecvdParam
(
 const SIP_CHAR    *pkszRecvd
 )
{
    SipParameters *pParameters = GetParameters();

    if (pParameters == SIP_NULL)
    {
        InitParameters(SIP_NULL);
        pParameters = GetParameters();
    }

    if (pParameters == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation Fail", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    const SIP_CHAR *pucRcvd = "received";
    if (pParameters->AddParam(pucRcvd, pkszRecvd) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "Set Received failed",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    return SIP_TRUE;

}

/******************************************************************************
 * Function name      : SipViaHeader::SetRecvdParam
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipViaHeader::SetBranchParam
(
 const SIP_CHAR    *pkszBranch
 )
{
    SipParameters *pParameters = GetParameters();

    if (pParameters == SIP_NULL)
    {
        InitParameters(SIP_NULL);
        pParameters = GetParameters();
    }

    if (pParameters == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "Memory Allocation Failed",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    if (pParameters->AddParam("branch", pkszBranch) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "Set Branch failed",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    return SIP_TRUE;
}

/*****************************************************************************
 * Function name      : SipViaHeader::DecHostPort
 *
 * Description        :
 *
 * Preconditions      :
 *
 * Side Effects          : none
 *****************************************************************************/
SIP_BOOL SipViaHeader::DecHostPort
(
 SIP_CHAR        *pucStartPt,
 SIP_CHAR        *pucEndPt
 )
{
    /*hostport = host [ COLON port ]
      host = hostname   /   IPv4address   /   IPv6reference
      hostname = *( domainlabel   "." )   toplabel   [ "." ]
      domainlabel = alphanum   /   alphanum   *( alphanum   /   "-" )   alphanum
      toplabel = ALPHA   /   ALPHA   *( alphanum   /   "-" )   alphanum
      IPv4address = 1*3DIGIT   "."   1*3DIGIT   "."   1*3DIGIT   "."   1*3DIGIT
      IPv6reference = "["   IPv6address   "]"
      IPv6address = hexpart   [ ":"   IPv4address ]
      hexpart = hexseq   /   hexseq   "::"   [ hexseq ]   /   "::"   [ hexseq ]
      hexseq = hex4   *( ":"   hex4 )
      hex4 = 1*4HEXDIG
      port = 1*DIGIT */

    SIP_CHAR *pucTempPre= SIP_NULL;
    /*check for IPV6 address*/
    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPre, LEFT_SQUARE) == SIP_TRUE)
    {
        m_eHostType = SipAddrSpec::HOST_IPV6;
        pucStartPt = pucTempPre + SIP_ONE;
        pucTempPre = SIP_NULL;
        if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPre, RIGHT_SQUARE) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Invalid Host[IPV6]", SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }
        pucTempPre = pucTempPre + SIP_ONE;
    }
    else if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPre, COLON) == SIP_FALSE)
    {
        pucTempPre = pucEndPt;
    }

    m_pszHost = sipCreateString(pucStartPt, pucTempPre);
    if (m_pszHost == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation Fail", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    if (m_eHostType != SipAddrSpec::HOST_IPV6)
    {
        IPAddress objIpAddr;
        if (objIpAddr.Parse(AString(m_pszHost)) == SIP_TRUE)
        {
            m_eHostType = SipAddrSpec::HOST_IPV4;
        }
    }

    pucStartPt = pucTempPre;
    pucTempPre = SIP_NULL;

    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPre, COLON) == SIP_TRUE)
    {
        pucTempPre = pucTempPre + SIP_TWO;
        SIP_CHAR *pszPort = sipCreateString(pucTempPre, pucEndPt);
        if (pszPort == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation Fail", SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
        m_usPort = SipPf_Atoi(pszPort);
        delete[] pszPort;
    }

    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipViaHeader::DecodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipViaHeader::DecodeHdr
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
    SIP_CHAR *pucTempPre= SIP_NULL;
    SIP_CHAR *pucTempNext= SIP_NULL;

    /*Search for the Protocol Name End*/
    /*sent-protocol = protocol-name SLASH protocol-version SLASH transport */
    /*Find First SLASH with Skipped LWS from both side*/
    if (sipFindActualPos(pucStartPt, pucEndPt, &pucTempPre, &pucTempNext, SLASH) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeHdr: Protocol Name Missing",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    m_pszProtocolName = sipCreateString(pucStartPt, pucTempPre);
    if (m_pszProtocolName == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeHdr: Memory Allocation Failed",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    /*Update the Start Point to the Start of protocol version
      i.e. to the right of "/" (LWS Skipped) */
    pucStartPt = pucTempNext;
    pucTempPre = SIP_NULL;
    pucTempNext = SIP_NULL;

    /*Find Next SLASH with Skipped LWS from both side*/
    if (sipFindActualPos(pucStartPt, pucEndPt, &pucTempPre, &pucTempNext, SLASH) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeHdr: Protocol Version Missing",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    m_pszProtocolVer = sipCreateString(pucStartPt, pucTempPre);
    if (m_pszProtocolVer == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeHdr: Memory Allocation Failed",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    /*Update the Start Point to the Start of Transport*/
    pucStartPt = pucTempNext;
    pucTempPre = SIP_NULL;
    pucTempNext = SIP_NULL;

    /*Find the LWS i.e. End of Transport*/
    if (sipFindLWS(pucStartPt, pucEndPt, &pucTempPre) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeHdr: LWS missing in Via",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    m_pszTransport = sipCreateString(pucStartPt, pucTempPre);
    if (m_pszTransport == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipViaHeader::DecodeHdr: Memory Allocation Failed",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    /*Skip Fw LWS And Get the Start of Sent by
      i.e. sent-by = host [ COLON port ]  */
    pucTempPre = pucTempPre + SIP_ONE;
    pucStartPt = sipSkipFwLWS(pucTempPre, pucEndPt);
    pucTempPre = SIP_NULL;
    pucTempNext = SIP_NULL;

    /*Now check for the Via Prm*/
    if (sipFindActualPos(pucStartPt, pucEndPt, &pucTempPre, &pucTempNext, SIP_SEMI) == SIP_FALSE)
    {
        pucTempPre = pucEndPt;
    }

    if (DecHostPort(pucStartPt, pucTempPre) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeHdr: Host Port Decoding Fail in via",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    if (pucTempNext != SIP_NULL)
    {
        return DecodeHeaderParameters(pucTempNext, pucEndPt, SIP_SEMI);
    }

    return SIP_TRUE;
}

SIP_BOOL SipViaHeader::IsValidHeader() const
{
    if ((m_pszProtocolName == SIP_NULL) || (m_pszProtocolVer == SIP_NULL)
        || (m_pszTransport == SIP_NULL) || (m_pszHost == SIP_NULL))
    {
         return SIP_FALSE;
    }

    return SIP_TRUE;
}

SipHeaderBase* SipViaHeader::GetNewObj(SIP_INT32 /*eHdr*/, SipHeaderBase *pHeader)
{
    if (pHeader != SIP_NULL)
    {
        return new SipViaHeader(*reinterpret_cast<SipViaHeader*>(pHeader));
    }
    return new SipViaHeader();
}
