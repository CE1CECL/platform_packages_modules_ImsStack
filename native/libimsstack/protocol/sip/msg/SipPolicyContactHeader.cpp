/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipPolicyContactHeader.cpp
 * Purpose               :
 * Platform              : Windows OR Android
 * Author(s)           : sravanthi.panditi
 * E-mail id.            : sravanthi.panditi@
 * Creation date       : Feb . 05,2013
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
#include "platform/sip_pf_string.h"
#include "platform/sip_pf_memory.h"
#include "sip_error.h"
#include "sip_debug.h"
#include "SipTrace.h"
#include "msg/SipAddrSpec.h"
#include "msg/SipPolicyContactHeader.h"
#include "msg/sip_msgutil.h"
//#include "msg/sip_enc.h"

/****************************************************************************
  Macro Definitions
 *****************************************************************************/

//extern const SIP_CHAR    gaszSipHdr[][];



/*constructor*/
SipPolicyContactHeader::SipPolicyContactHeader()
    : SipNameAddrHeader(SipHeaderBase::POLICY_CONTACT)
{
}

SipPolicyContactHeader::SipPolicyContactHeader(const SipPolicyContactHeader &objHeader)
    : SipNameAddrHeader(objHeader)
{
}


/*destructor*/
SipPolicyContactHeader::~SipPolicyContactHeader()
{
}

SIP_BOOL SipPolicyContactHeader::EncodeHdr
(
    SIP_CHAR     **ppucCurrPos,
    SIP_BOOL     bParams /*Default = SIP_TRUE*/
)
{
    if (m_pobjNameAddr == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                    "Empty Addr Spec",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    m_pobjNameAddr->SetParameterComponent(this);

    if (m_pobjNameAddr->EncodeNameAddr(ppucCurrPos) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "sipEncodePolicyContactHdr: Addr Spec failed",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    return EncodeHeaderParameters(ppucCurrPos, bParams);
}

/*This function has been added for Policy Contact Header Decoding purpose-- megha.r@*/
/******************************************************************************
 * Function name      : SipPolicyContactHeader::DecodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipPolicyContactHeader::DecodeHdr
(
 SIP_CHAR    *pucStartPt,
 SIP_UINT32  uiDecLen
 )
{
    if ((uiDecLen == SIP_ZERO) || (m_pobjNameAddr == SIP_NULL))
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "Empty buffer or nameAddr null",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    /*Add reference for Percentage Encoding*/
    if (IsPercentEncHdr() == SIP_TRUE)
    {
        m_pobjNameAddr->SetParameterComponent(static_cast<IParameterComponent*>(this));
    }

    //Check whether it is policyContact-info = LAQUOT policyContact-uri RAQUOT, if not Failure case
    SIP_CHAR            *pucEndPt = pucStartPt + uiDecLen - SIP_ONE;
    SIP_CHAR            *pucTempPre= SIP_NULL;
    SIP_INT32            uiLen = SIP_ZERO;
    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPre, RIGHT_ANGLE) == SIP_TRUE)
    {
        if (m_pobjNameAddr->DecodeNameAddr(pucStartPt, pucTempPre) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "%d::DecodeHdr:Name Addr decoding failed",
                    GetHdrType(),SIP_ZERO);
            return SIP_FALSE;
        }
        /*Now find the start of Sip Prm*/
        uiLen = pucTempPre-pucStartPt;
        pucStartPt = pucTempPre + SIP_TWO;
        pucStartPt = sipSkipFwLWS( pucStartPt, pucEndPt);
        pucTempPre = SIP_NULL;
        SIP_CHAR *pucTempNext = SIP_NULL;

        //added the condition ((*pucStartPt)==';')
        if (sipFindActualPos(pucStartPt, pucEndPt,
                &pucTempPre, &pucTempNext, SIP_SEMI) == SIP_TRUE)
        {
            if (DecodeHeaderParameters(pucTempNext, pucEndPt, SIP_SEMI) == SIP_FALSE)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "%d::DecodeHdr: Prm decoding failed",
                    GetHdrType(),SIP_ZERO);
                return SIP_FALSE;
            }
        }
        else
        {
            if (uiLen != (SIP_INT32(uiDecLen-2)))
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                        "%d::DecodeHdr: Invalid Delimiter separating param",
                        GetHdrType(),SIP_ZERO);
                return SIP_FALSE;
            }
        }
    }
    else
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipNameAddr::DecodeNameAddr: Right Angle Not Found",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    return SIP_TRUE;
}


SipHeaderBase* SipPolicyContactHeader::GetNewObj(SIP_INT32 /*eHdr*/, SipHeaderBase *pHeader)
{
    if (pHeader != SIP_NULL) {
        return new SipPolicyContactHeader(*reinterpret_cast<SipPolicyContactHeader*>(pHeader));
    }
    return new SipPolicyContactHeader();
}
