/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipWarningHeader.h
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
#include "msg/SipWarningHeader.h"
#include "sip_error.h"
#include "sip_debug.h"
#include "SipTrace.h"
#include "platform/sip_pf_string.h"
#include "SipConfiguration.h"
#include "msg/sip_msgutil.h"


/****************************************************************************
  Macro Definitions
 *****************************************************************************/
#define MAX_WARN_LEN 12

/****************************************************************************
  Enum Declaration
 *****************************************************************************/

/****************************************************************************
  Class Declaration Starts
 *****************************************************************************/

/*constructor*/
SipWarningHeader::SipWarningHeader ()
    : SipHeaderBase(SipHeaderBase::WARNING)
    , m_uiWarnCode(SIP_ZERO)
    , m_pszWarnAgent(SIP_NULL)
    , m_pszWarnText(SIP_NULL)
{
}

SipWarningHeader::SipWarningHeader(const SipWarningHeader &objHeader)
    : SipHeaderBase(objHeader)
    , m_uiWarnCode(objHeader.m_uiWarnCode)
    , m_pszWarnAgent(SipPf_Strdup(objHeader.m_pszWarnAgent))
    , m_pszWarnText(SipPf_Strdup(objHeader.m_pszWarnText))
{
}

/*destructor*/
SipWarningHeader::~SipWarningHeader ()
{
    if (m_pszWarnAgent != SIP_NULL)
    {
        delete[] m_pszWarnAgent;
        m_pszWarnAgent = SIP_NULL;
    }
    if (m_pszWarnText != SIP_NULL)
    {
        delete[] m_pszWarnText;
        m_pszWarnText = SIP_NULL;
    }
}


/*virtual methods*/
/*Function for encoding of headers*/
SIP_BOOL SipWarningHeader::EncodeHdr
(
    SIP_CHAR     **ppucCurrPos,
    SIP_BOOL     /*bParams = SIP_TRUE*/
 )
{
    if ((m_pszWarnAgent == SIP_NULL) || (m_pszWarnText == SIP_NULL))
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "Missing Warn Agent or Warn Text", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR ucLen[MAX_WARN_LEN];
    SipPf_Sprintf(ucLen, (SIP_CHAR*)"%u", m_uiWarnCode);

    SipPf_Strcpy(*ppucCurrPos,ucLen);
    SipEnc_UpdateCurrPos(ppucCurrPos);

    SIP_ENC_SP(*ppucCurrPos);

    SipPf_Strcpy(*ppucCurrPos,m_pszWarnAgent);
    SipEnc_UpdateCurrPos(ppucCurrPos);

    SIP_ENC_SP(*ppucCurrPos);

    if (HasSpace(m_pszWarnText))
    {
        SIP_ENC_LDQUOT(*ppucCurrPos);

        SipPf_Strcpy(*ppucCurrPos,m_pszWarnText);
        SipEnc_UpdateCurrPos(ppucCurrPos);

        SIP_ENC_RDQUOT(*ppucCurrPos);
    }
    else
    {
        SipPf_Strcpy(*ppucCurrPos,m_pszWarnText);
        SipEnc_UpdateCurrPos(ppucCurrPos);
    }

    return SIP_TRUE;
}

/*Sets the visited nw name*/
SIP_BOOL SipWarningHeader::SetWarnAgent
(
 const SIP_CHAR    *pucWarnAgent
 )
{
    return SetCharVar(pucWarnAgent,m_pszWarnAgent);
}

/*Sets the visited nw name*/
SIP_BOOL SipWarningHeader::SetWarnText
(
 const SIP_CHAR    *pucWarnText
 )
{
    return SetCharVar(pucWarnText,m_pszWarnText);
}


/******************************************************************************
 * Function name      : SipWarningHeader::DecodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipWarningHeader::DecodeHdr
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
    SIP_CHAR *pucTempLoc= SIP_NULL;

    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempLoc, SPACE) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeHdr: Space Not Found",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR *pucWarnCode = sipCreateString(pucStartPt, pucTempLoc);
    if (pucWarnCode == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeHdr: Memory Allocation Failed",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    m_uiWarnCode = SipPf_Atoi(pucWarnCode);
    delete[] pucWarnCode;
    if (m_uiWarnCode > MAX_WARNCODE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeHdr:Warn code Value is not valid",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    /*update the start point to the start of Warn Agent*/
    pucStartPt = pucTempLoc + SIP_TWO;
    pucTempLoc = SIP_NULL;
    /*Find the endpoint of Warn Agent*/
    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempLoc, SPACE) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeHdr: Space Not Found",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    m_pszWarnAgent = sipCreateString(pucStartPt, pucTempLoc);
    if (m_pszWarnAgent == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeHdr: Memory Allocation Failed",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    /*Update the start point to the start of Warn text*/
    pucStartPt =  pucTempLoc + SIP_THREE;
    m_pszWarnText = sipCreateString(pucStartPt, pucEndPt);
    if (m_pszWarnText == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeHdr: Memory Allocation Failed",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    return SIP_TRUE;
}

SIP_BOOL SipWarningHeader::IsValidHeader() const
{
    if ((m_pszWarnAgent == SIP_NULL) || (m_pszWarnText == SIP_NULL))
    {
         return SIP_FALSE;
    }

    return SIP_TRUE;
}

SipHeaderBase* SipWarningHeader::GetNewObj(SIP_INT32 /*eHdr*/, SipHeaderBase *pHeader)
{
    if (pHeader != SIP_NULL) {
        return new SipWarningHeader(*reinterpret_cast<SipWarningHeader*>(pHeader));
    }
    return new SipWarningHeader();
}
