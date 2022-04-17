 /* Platform            : Windows OR Android
 * Author(s)           : sravanthi panditi
 * Modified            : shaikh.azaruddin
 * Creation date       : Feb. 05, 2013

 *****************************************************************************/

#include "msg/SipPAssertedServiceHeader.h"
#include "sip_error.h"
#include "sip_debug.h"
#include "SipTrace.h"
#include "platform/sip_pf_string.h"
#include "SipConfiguration.h"
#include "msg/sip_msgutil.h"


/****************************************************************************
  Class Member Function Implementations
 *****************************************************************************/

SipPAssertedServiceHeader::SipPAssertedServiceHeader()
    : SipHeaderBase(SipHeaderBase::P_ASSERTED_SERVICE)
{
}

SipPAssertedServiceHeader::SipPAssertedServiceHeader(const SipPAssertedServiceHeader &objHeader)
    : SipHeaderBase(objHeader)
{
}

/*destructor*/
SipPAssertedServiceHeader::~SipPAssertedServiceHeader()
{
}

SIP_BOOL SipPAssertedServiceHeader::DecodeHdr
(
 SIP_CHAR    *pucStartPt,
 SIP_UINT32  uiDecLen
 )
{
    /*Case of nothing is present*/
    if (uiDecLen == SIP_ZERO)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipPAssertedServiceHeader::DecodeHdr",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR            *pucEndPt = pucStartPt + uiDecLen - SIP_ONE;
    sipSkipFwLWS(pucStartPt,pucEndPt);

    //validate the service id value
    SIP_CHAR *pszTempString = sipCreateString(pucStartPt,pucStartPt+SIP_NINE);
    if (SipPf_Stricmp("urn:urn-7:",pszTempString) != SIP_ZERO)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipPAssertedServiceHeader::DecodeHdr:value must start by urn:urn-7:",
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

    SIP_CHAR *pucTempCurr = pucStartPt+SIP_TEN;

    //Service id Value Validations   toplebel.subservice id
    //fIND dot and validate topLevel ID and SubService Id
    SIP_UINT16           uiTopLevelIDLen = 0;
    SIP_CHAR            *pucTempPre = SIP_NULL;
    SIP_CHAR            *pucTempNext = SIP_NULL;

    if (sipFindActualPos(pucTempCurr, pucEndPt, &pucTempPre, &pucTempNext, SIP_DOT) == SIP_TRUE)
    {
        uiTopLevelIDLen = pucTempPre-pucTempCurr+1;
    }
    else
    {
        uiTopLevelIDLen = pucEndPt-pucTempCurr+1;
    }

    if ((uiTopLevelIDLen > MAXLETDIG) || (uiTopLevelIDLen <= 0))
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipPAssertedServiceHeader::DecodeHdr:invalid subService id",
                SIP_ZERO,SIP_ZERO);
    }

    if (pucTempNext != SIP_NULL)
    {
        while(pucTempNext <= pucEndPt)
        {
            if ((IS_ALPHANUM(*pucTempNext) == SIP_FALSE) &&
                !IS_HYPHEN(*pucTempNext) &&
                (*pucTempNext != SIP_DOT))
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                        "SipPAssertedServiceHeader::DecodeHdr:invalid subService id",
                        SIP_ZERO,SIP_ZERO);
            }
            if ((*pucTempNext == SIP_DOT) && (pucTempNext == pucEndPt))
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                        "SipPAssertedServiceHeader::DecodeHdr:invalid subService id",
                        SIP_ZERO,SIP_ZERO);
            }
            pucTempNext = pucTempNext+SIP_ONE;
        }
    }

    SIP_UINT16 uiCounter=0;
    //Validate Top Lebel
    while((uiCounter < MAXLETDIG) && (pucTempCurr < pucTempPre))
    {
        if ((IS_ALPHANUM(*pucTempCurr) == SIP_FALSE) && !IS_HYPHEN(*pucTempCurr))
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipPAssertedServiceHeader::DecodeHdr:Top lebel is invalid",
                    SIP_ZERO,SIP_ZERO);
        }
        pucTempCurr=pucTempCurr+SIP_ONE;
        uiCounter++;
    }

    //create and copy the service id
    SIP_CHAR *pValue = sipCreateString(pucStartPt, pucEndPt);
    if (SetValue(pValue) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipPAssertedServiceHeader::DecodeHdr:Memory Allocation Failed",
                SIP_ZERO,SIP_ZERO);
        if (pValue != SIP_NULL) {
            delete[] pValue;
        }
        return SIP_FALSE;
    }
    delete[] pValue;

    return SIP_TRUE;
}

SipHeaderBase* SipPAssertedServiceHeader::GetNewObj(SIP_INT32 /*eHdr*/, SipHeaderBase *pHeader)
{
    if (pHeader != SIP_NULL) {
        return new SipPAssertedServiceHeader(
            *reinterpret_cast<SipPAssertedServiceHeader*>(pHeader));
    }
    return new SipPAssertedServiceHeader();
}
