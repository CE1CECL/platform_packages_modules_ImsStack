/******************************************************************************
 * Project Name   : SIP_RTP
 * Group    : IP-CS [MSG-2]
 * Security   : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename      : SipContactHeader.cpp
 * Purpose     :
 * Platform      : Windows OR Android
 * Author(s)     : Giridhar
 * E-mail id.    : giridhar.a@
 * Creation date   : Aug. 04, 2010
 *
 * Edit History     Modification         Description(s)
 *
 * Date      Name    Version    Bug-ID    Description
 * ----------    ----------    -------    ------    -------------
 * Month. Date,10    Name       0.0a    Initial creation
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
#include "msg/SipContactHeader.h"
#include "msg/sip_msgutil.h"

/****************************************************************************
  Macro Definitions
 *****************************************************************************/

/****************************************************************************
  Class Member Function Implementations
 *****************************************************************************/

/*constructors*/
SipContactHeader::SipContactHeader()
    : SipNameAddrHeader(SipHeaderBase::CONTACT)
{
}

SipContactHeader::SipContactHeader(const SipContactHeader &objHeader)
    : SipNameAddrHeader(objHeader)
{
}


/*destructor*/
SipContactHeader::~SipContactHeader()
{
}

SIP_BOOL SipContactHeader::DecodeHdr
(
 SIP_CHAR    *pucStartPt,
 SIP_UINT32  uiDecLen
 )
{
    if (uiDecLen == SIP_ZERO)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Empty buffer", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR    *pucEndPt = pucStartPt + uiDecLen - SIP_ONE;
    pucEndPt = sipSkipRwLWS(pucStartPt,pucEndPt);

    if ((pucStartPt == pucEndPt) && (*pucStartPt == ASTERISK))
    {
        SIP_CHAR *pszValue = sipCreateString(pucStartPt, pucEndPt);
        if (SetValue(pszValue) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation Fail", SIP_ZERO, SIP_ZERO);
            if (pszValue != SIP_NULL) {
                delete[] pszValue;
            }
            return SIP_FALSE;
        }
        delete[] pszValue;
    }
    else
    {
        if (SipNameAddrHeader::DecodeHdr(pucStartPt,uiDecLen) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Address param Decoding Failed",
                SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
    }

    return SIP_TRUE;

}

SipHeaderBase* SipContactHeader::GetNewObj(SIP_INT32 /*eHdr*/, SipHeaderBase *pHeader)
{
    if (pHeader != SIP_NULL) {
        return new SipContactHeader(*reinterpret_cast<SipContactHeader*>(pHeader));
    }
    return new SipContactHeader();
}
