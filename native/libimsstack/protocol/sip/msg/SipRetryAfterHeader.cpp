/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipRetryAfterHeader.cpp
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
#include "msg/SipRetryAfterHeader.h"
#include "sip_error.h"
#include "sip_debug.h"
#include "SipTrace.h"
#include "platform/sip_pf_string.h"
#include "SipConfiguration.h"
#include "msg/sip_msgutil.h"


/****************************************************************************
  Macro Definitions
 *****************************************************************************/
#define MAX_RETRY_AFTER_LEN 12


/****************************************************************************
  Enum Declaration
 *****************************************************************************/

/****************************************************************************
  Class Declaration Starts
 *****************************************************************************/


/*constructor*/
SipRetryAfterHeader::SipRetryAfterHeader()
    : SipHeaderBase(SipHeaderBase::RETRY_AFTER_SEC)
    , m_uiDeltaSec(SIP_ZERO), m_pszComment(SIP_NULL)
{
}

/*Copy constructor*/
SipRetryAfterHeader::SipRetryAfterHeader(const SipRetryAfterHeader &objHeader)
    : SipHeaderBase(objHeader)
    , m_uiDeltaSec(objHeader.m_uiDeltaSec)
    , m_pszComment(SipPf_Strdup(objHeader.m_pszComment))
{
}

/*destructor*/
SipRetryAfterHeader::~SipRetryAfterHeader()
{
    if (m_pszComment != SIP_NULL)
    {
        delete[] m_pszComment;
    }
}

/*virtual methods*/
/*Function for encoding of headers*/
SIP_BOOL SipRetryAfterHeader::EncodeHdr
(
    SIP_CHAR     **ppucCurrPos,
    SIP_BOOL     bParams /*Default = SIP_TRUE*/
 )
{
    SIP_CHAR ucLen[MAX_RETRY_AFTER_LEN];
    SipPf_Sprintf(ucLen, (SIP_CHAR*)"%u", m_uiDeltaSec);

    SipPf_Strcpy(*ppucCurrPos,ucLen);
    SipEnc_UpdateCurrPos(ppucCurrPos);

    if (m_pszComment != SIP_NULL)
    {
        SIP_ENC_LPAREN(*ppucCurrPos);
        SipPf_Strcpy(*ppucCurrPos,m_pszComment);
        SipEnc_UpdateCurrPos(ppucCurrPos);
        SIP_ENC_RPAREN(*ppucCurrPos);
    }

    return EncodeHeaderParameters(ppucCurrPos, bParams);
}

/*Sets */
SIP_BOOL SipRetryAfterHeader::SetComment
(
 const SIP_CHAR    *pucComment
 )
{
    return SetCharVar(pucComment,m_pszComment);
}

/******************************************************************************
 * Function name      : SipRetryAfterHeader::DecodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipRetryAfterHeader::DecodeHdr
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

    SIP_CHAR            *pucCommentStart = SIP_NULL;
    SIP_CHAR            *pucCommentEnd = SIP_NULL;
    SIP_CHAR            *pucEndPt = pucStartPt + uiDecLen - SIP_ONE;
    SIP_CHAR            *pucTempPre = SIP_NULL;
    SIP_CHAR            *pucTempNext = SIP_NULL;

    SIP_BOOL bStatus = FindComment(pucStartPt, pucEndPt, pucCommentStart, pucCommentEnd);

    if (sipFindActualPos(pucStartPt, pucEndPt, &pucTempPre, &pucTempNext, SIP_SEMI) == SIP_TRUE)
    {
        if (((bStatus == SIP_TRUE) && ((pucTempPre+1) > pucCommentEnd)) || (bStatus == SIP_FALSE))
        {
            if (DecodeHeaderParameters(pucTempNext,pucEndPt, SIP_SEMI) == SIP_FALSE)
            {
                return SIP_FALSE;
            }
            pucEndPt = pucTempPre;
            pucTempPre  = SIP_NULL;
            pucTempNext  = SIP_NULL;
        }

    }
    else
    {
        // if there is some extra string after comment ends
        if ((bStatus == SIP_TRUE) && (pucCommentEnd != pucEndPt))
        {
            return SIP_FALSE;
        }

    }

    //if comment exists
    if (pucCommentStart != SIP_NULL)
    {
        m_pszComment = sipCreateString(pucCommentStart, pucCommentEnd);
        if (m_pszComment == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "DecodeHdr:Memory Allocation failed",
                    SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }
        pucEndPt = pucCommentStart-2;
    }

    /*Now decode the delta sec value*/
    SIP_CHAR *pszValue = sipCreateString(pucStartPt, pucEndPt);
    if (pszValue == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeHdr:Memory Allocation failed",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    if (SipPf_Atoi_IsZero(pszValue) == SIP_FALSE)
    {
        m_uiDeltaSec = SipPf_Atoi_Unsigned(pszValue);
        if ((m_uiDeltaSec > MAX_CSEQ) || (m_uiDeltaSec == SIP_ZERO))
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "DecodeHdr:Retry After value is not valid",
                    SIP_ZERO,SIP_ZERO);
            delete[] pszValue;
            return SIP_FALSE;
        }
    }
    else
    {
        m_uiDeltaSec = SIP_ZERO;
    }
    delete[] pszValue;

    return SIP_TRUE;
}

SipHeaderBase* SipRetryAfterHeader::GetNewObj(SIP_INT32 /*eHdr*/, SipHeaderBase* pHeader)
{
    if (pHeader != SIP_NULL) {
        return new SipRetryAfterHeader(*reinterpret_cast<SipRetryAfterHeader*>(pHeader));
    }
    return new SipRetryAfterHeader();
}
