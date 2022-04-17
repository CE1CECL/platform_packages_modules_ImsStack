/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipRAcKHeader.cpp
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

#include "msg/SipAddrSpec.h"
#include "platform/sip_pf_string.h"
#include "platform/sip_pf_memory.h"
#include "sip_error.h"
#include "sip_debug.h"
#include "SipTrace.h"
#include "msg/SipRAcKHeader.h"
#include "SipConfiguration.h"
#include "msg/sip_msgutil.h"

/****************************************************************************
  Macro Definitions
 *****************************************************************************/

/****************************************************************************
  Class Member Function Implementations
 *****************************************************************************/


/******************************************************************************
 * Function name      : SipRAcKHeader::SipRAcKHeader
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/

SipRAcKHeader::SipRAcKHeader()
    : SipHeaderBase(SipHeaderBase::RACK)
    , m_uiResponseNum(SIP_ZERO)
    , m_uiCSeqNum(SIP_ZERO)
    , m_pszMethod(SIP_NULL)
{
}
/******************************************************************************
 * Function name      : SipRAcKHeader(SipRAcKHeader*)
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/

SipRAcKHeader::SipRAcKHeader(const SipRAcKHeader &objHeader)
    : SipHeaderBase(objHeader)
    , m_uiResponseNum(objHeader.m_uiResponseNum)
    , m_uiCSeqNum(objHeader.m_uiCSeqNum)
    , m_pszMethod(SipPf_Strdup(objHeader.m_pszMethod))
{
}

/******************************************************************************
 * Function name      : SipRAcKHeader::~SipRAcKHeader
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/

SipRAcKHeader::~SipRAcKHeader()
{
    if (m_pszMethod != SIP_NULL)
    {
        delete[] m_pszMethod;
    }

}

/******************************************************************************
 * Function name      : SipRAcKHeader::EncodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/

SIP_BOOL SipRAcKHeader::EncodeHdr
(
    SIP_CHAR     **ppucCurrPos,
    SIP_BOOL     /*bParams = SIP_TRUE*/
)
{
    if (m_pszMethod == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "Method missing",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SipPf_Sprintf(*ppucCurrPos, (SIP_CHAR*)"%u %u %s", m_uiResponseNum,
            m_uiCSeqNum, m_pszMethod);
    SipEnc_UpdateCurrPos(ppucCurrPos);

    return SIP_TRUE;
}

/*set methods*/
/******************************************************************************
 * Function name      : SipRAcKHeader::SetProtocolName
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipRAcKHeader::SetMethod
(
 const SIP_CHAR    *pkszMethod
 )
{
    return SetCharVar(pkszMethod, m_pszMethod);
}

/*Get methods*/

/******************************************************************************
 * Function name      : SipRAcKHeader::DecodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipRAcKHeader::DecodeHdr
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

    SIP_CHAR            *pucEndPt = pucStartPt + uiDecLen - SIP_ONE;
    SIP_CHAR            *pucTempPre= SIP_NULL;

    if (sipFindLWS(pucStartPt, pucEndPt, &pucTempPre) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeHdr: LWS missing in RAcK",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR *pszResponseNum = sipCreateString(pucStartPt, pucTempPre);
    if (pszResponseNum == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeHdr: Memory Allocation Failed",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    m_uiResponseNum = SipPf_Atoi(pszResponseNum);
    delete[] pszResponseNum;

    /*Skip Fw LWS And Get the Start of CSeq Num
      i.e. sent-by = host [ COLON port ]  */
    pucTempPre = pucTempPre + SIP_ONE; // point to the start of LWS
    pucStartPt = sipSkipFwLWS(pucTempPre, pucEndPt);
    pucTempPre = SIP_NULL;

    /*Now find the end of CSeq Num*/
    if (sipFindLWS(pucStartPt,  pucEndPt, &pucTempPre) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeHdr: LWS missing in RAcK",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR *pszCSeqNum = sipCreateString(pucStartPt, pucTempPre);
    if (pszCSeqNum == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeHdr: Memory Allocation Failed",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    m_uiCSeqNum = SipPf_Atoi(pszCSeqNum);
    delete[] pszCSeqNum;

    /*Update the start point*/
    pucTempPre = pucTempPre + SIP_ONE; // point to the start of LWS
    pucStartPt = sipSkipFwLWS(pucTempPre, pucEndPt);
    pucTempPre = SIP_NULL;

    m_pszMethod = sipCreateString(pucStartPt, pucEndPt);
    if (m_pszMethod == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeHdr: Memory Allocation Failed",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    return SIP_TRUE;
}

SipHeaderBase* SipRAcKHeader::GetNewObj(SIP_INT32 /*eHdr*/, SipHeaderBase* pHeader)
{
    if (pHeader != SIP_NULL) {
        return new SipRAcKHeader(*reinterpret_cast<SipRAcKHeader*>(pHeader));
    }
    return new SipRAcKHeader();
}
