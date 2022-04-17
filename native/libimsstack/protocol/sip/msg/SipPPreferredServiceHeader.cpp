 /* Platform            : Windows OR Android
 * Author(s)           : sravanthi panditi
 * Modified            : shaikh.azaruddin
 * Creation date       : Feb. 05, 2013

 *****************************************************************************/

#include "msg/SipPPreferredServiceHeader.h"
#include "sip_error.h"
#include "sip_debug.h"
#include "SipTrace.h"
#include "platform/sip_pf_string.h"
#include "SipConfiguration.h"
#include "msg/sip_msgutil.h"


/****************************************************************************
  Class Member Function Implementations
 *****************************************************************************/

SipPPreferredServiceHeader::SipPPreferredServiceHeader()
    : SipHeaderBase(SipHeaderBase::P_PREFERRED_SERVICE)
{
}

SipPPreferredServiceHeader::SipPPreferredServiceHeader(
    const SipPPreferredServiceHeader &objHeader)
    : SipHeaderBase(objHeader)

{
}

SipPPreferredServiceHeader::~SipPPreferredServiceHeader()
{
}

SIP_BOOL SipPPreferredServiceHeader::DecodeHdr
(
 SIP_CHAR    *pucStartPt,
 SIP_UINT32  uiDecLen
 )
{
    if (uiDecLen == SIP_ZERO)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "Empty buffer", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR            *pucEndPt = pucStartPt + uiDecLen - SIP_ONE;
    sipSkipFwLWS(pucStartPt,pucEndPt);

    //validate urn:urn-7 mandatory prefix
    SIP_CHAR *pszTempString = sipCreateString(pucStartPt,pucStartPt+SIP_NINE);
    if (SipPf_Stricmp("urn:urn-7:",pszTempString) != SIP_ZERO)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeHdr:value must start by urn:urn-7:",
                SIP_ZERO,SIP_ZERO);
        if (pszTempString != SIP_NULL)
        {
            delete[] pszTempString;
        }
        #ifdef SIP_STRICT_PARSING
          return SIP_FALSE;
        #endif
    }
    else if (pszTempString != SIP_NULL)
    {
        delete[] pszTempString;
    }

    SIP_CHAR            *pucTempCurr =pucStartPt+SIP_TEN;;
    SIP_CHAR            *pucTempPre = SIP_NULL;
    SIP_CHAR            *pucTempNext = SIP_NULL;
    //Find First dot and validate SubService Id
    if (sipFindActualPos(pucTempCurr,pucEndPt,&pucTempPre,&pucTempNext,SIP_DOT) == SIP_TRUE)
    {
        while(pucTempNext <= pucEndPt)
        {
            if ((IS_ALPHANUM(*pucTempNext) == SIP_FALSE) &&
                !IS_HYPHEN(*pucTempNext) &&
                (*pucTempNext != SIP_DOT))
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                        "DecodeHdr:invalid subService id",
                        SIP_ZERO,SIP_ZERO);
            }
            if ((*pucTempNext == SIP_DOT) && (pucTempNext == pucEndPt))
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                        "DecodeHdr:invalid subService id",
                        SIP_ZERO,SIP_ZERO);
            }
            pucTempNext = pucTempNext + SIP_ONE;
        }
    }
    SIP_UINT16 uiCounter = SIP_ZERO;
    //Validate Top Lebel
    while((uiCounter < MAXLETDIG) && (pucTempCurr < pucTempPre))
    {
        if ((IS_ALPHANUM(*pucTempCurr) == SIP_FALSE) && !IS_HYPHEN(*pucTempCurr))
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "DecodeHdr:Top lebel is invalid",
                    SIP_ZERO,SIP_ZERO);
        }
        pucTempCurr = pucTempCurr + SIP_ONE;
        uiCounter++;
    }

    //create the service and copy
    SIP_CHAR *pValue = sipCreateString(pucStartPt, pucEndPt);
    if (SetValue(pValue) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecodeHdr:Memory Allocation Failed",
                SIP_ZERO,SIP_ZERO);
        if (pValue != SIP_NULL) {
            delete[] pValue;
        }
        return SIP_FALSE;
    }
    delete[] pValue;
    return SIP_TRUE;

}


SipHeaderBase* SipPPreferredServiceHeader::GetNewObj(SIP_INT32 /*eHdr*/, SipHeaderBase *pHeader)
{
    if (pHeader != SIP_NULL) {
        return new SipPPreferredServiceHeader(
            *reinterpret_cast<SipPPreferredServiceHeader*>(pHeader));
    }
    return new SipPPreferredServiceHeader();
}
