/******************************************************************************
 * Project Name   : SIP_RTP
 * Group    : IP-CS [MSG-2]
 * Security   : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename      : SipAuthBase.h
 * Purpose     :
 * Platform      : Windows OR Android
 * Author(s)     : Saurabh Srivastava
 * E-mail id.    : saurabh31.srivastava
 * Creation date   : may. 09 ,2011
 *
 * Edit HisAlertry   Modification description(s)
 * Date      Name    Version    Bug-ID    Description
 * ----------    ----------    -------    ------    -------------
 * Month. Date,10    Saurabh      0.0a    Initial creation
 *****************************************************************************/

/*****************************************************************************
  Header Inclusions
 *****************************************************************************/
#include "msg/SipHeaderBase.h"
#include "msg/SipAuthBase.h"
#include "msg/SipParameterList.h"
#include "sip_error.h"
#include "sip_debug.h"
#include "SipTrace.h"
#include "SipConfiguration.h"
#include "msg/sip_msgutil.h"
#include "platform/sip_pf_string.h"
/****************************************************************************
  Macro Definitions
 *****************************************************************************/


/****************************************************************************
  Enum Declaration
 *****************************************************************************/

/****************************************************************************
  Class Declaration Starts
 *****************************************************************************/
/*constructor*/
SipAuthBase::SipAuthBase(SIP_INT32 eHdrType)
    : SipHeaderBase(eHdrType)
    , m_objAuthList(SipVector<SipNameValue*>())
{
}

SipAuthBase::SipAuthBase(const SipAuthBase &objHeader)
    : SipHeaderBase(objHeader)
    , m_objAuthList(SipVector<SipNameValue*>())
{
    SIP_UINT32 usSize = objHeader.m_objAuthList.GetSize();

    for (SIP_UINT32 usCount = SIP_ZERO; usCount < usSize ; usCount++)
    {
        SipNameValue* pTempNmVl = objHeader.m_objAuthList.GetAt(usCount);

        if (pTempNmVl != SIP_NULL)
        {
            SipNameValue* pobjNmVl = new SipNameValue(*pTempNmVl);
            m_objAuthList.Add(pobjNmVl);
        }
    }
}


/*destructor*/
SipAuthBase::~SipAuthBase()
{
    while(m_objAuthList.IsEmpty() != SIP_TRUE)
    {
        delete m_objAuthList.Top();
        m_objAuthList.Pop();
    }
}

SIP_BOOL SipAuthBase::EncodeAuthList(
        SIP_CHAR   **ppucCurrPos,
        SIP_CHAR  ucDelimiter)
{
    SIP_UINT32  iIndex = SIP_ZERO;
    SIP_UINT32 sCount = m_objAuthList.GetSize();

    //  enc for *( ";" uri-parameter )
    while(iIndex < sCount)
    {
        SipNameValue* pstParamNamValue = m_objAuthList.GetAt(iIndex);

        if (iIndex > 0)
        {
            *(*ppucCurrPos) = ucDelimiter;
            (*ppucCurrPos)++;
        }

        if (pstParamNamValue->EncodeFromList(ppucCurrPos) == SIP_FALSE)
        {
            return SIP_FALSE;
        }

        iIndex++;
    }
    return SIP_TRUE;
}

/*virtual methods*/
/*Function for encoding of headers*/
SIP_BOOL SipAuthBase::EncodeHdr
(
    SIP_CHAR   **ppucCurrPos,
    SIP_BOOL   /*bParams = SIP_TRUE*/
)
{
    const SIP_CHAR *pValue = GetValue();
    if (pValue == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Missing Scheme",SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SipPf_Strcpy(*ppucCurrPos, pValue);
    SipEnc_UpdateCurrPos(ppucCurrPos);

    /*Encode space*/
    **ppucCurrPos = SPACE;
    (*ppucCurrPos)++;
    EncodeAuthList(ppucCurrPos,COMMA);

    return SIP_TRUE;
}


SIP_BOOL SipAuthBase::SetParams(const SIP_CHAR* pszName, const SIP_CHAR* pszVal,
        SIP_BOOL bIsFeatureParam)
{
    SipNameValue *pobjNV = new SipNameValue();

    if (pobjNV == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Memory allocation Fail", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    if (bIsFeatureParam == SIP_TRUE) {
        pobjNV->m_ePrmType = SipParameters::FEATURE;
    }

    pobjNV->m_pszName = SipPf_Strdup(pszName);
    if (pobjNV->m_pszName == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Memory allocation Fail", SIP_ZERO, SIP_ZERO);
        delete pobjNV;
        return SIP_FALSE;
    }

    SIP_CHAR *pucVal = SipPf_Strdup(pszVal);
    if (pucVal == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Memory allocation Fail", SIP_ZERO, SIP_ZERO);
        delete pobjNV;
        return SIP_FALSE;
    }

    pobjNV->m_valueList.Add(pucVal);
    m_objAuthList.Add(pobjNV);

    return SIP_TRUE;
}

SIP_BOOL SipAuthBase::FindElement(const SIP_CHAR *pszName, SipNameValue *&pobjNmvl,
                                    SIP_UINT32& iPos)
{
    SIP_UINT32 uSize = m_objAuthList.GetSize();

    for(SIP_UINT32 uIndex = 0; uIndex < uSize; uIndex++)
    {
        SipNameValue *pobjNmVl = m_objAuthList.GetAt(uIndex);
        if (SipPf_Stricmp(pszName,pobjNmVl->m_pszName) == 0)
        {
            iPos = uIndex;
            pobjNmvl = pobjNmVl;
            return SIP_TRUE;
        }
    }
    return SIP_FALSE;
}

SIP_CHAR* SipAuthBase::GetAuthValue(const SIP_CHAR* pszName)
{
    if (pszName == SIP_NULL)
    {
        return SIP_NULL;
    }

    SipNameValue *pobjNmVl = SIP_NULL;
    SIP_UINT32 usPos = SIP_ZERO;
    SIP_BOOL bStatus = FindElement(pszName,pobjNmVl,usPos);
    if ((bStatus == SIP_FALSE) || (pobjNmVl == SIP_NULL))
    {
        return SIP_NULL;
    }

    SipVector<SIP_CHAR*>& valueList = pobjNmVl->m_valueList;
    if (valueList.IsEmpty() == SIP_TRUE)
    {
        return SIP_NULL;
    }

    SIP_CHAR   *pszVal = SIP_NULL;
    SIP_CHAR   *pElement = valueList.GetAt(SIP_ZERO);
    if (pobjNmVl->m_ePrmType == SipParameters::FEATURE)
    {
        int iLen = SIP_ZERO;
        if (pElement != SIP_NULL)
        {
            iLen = SipPf_Strlen(pElement);
        }
        pszVal = new SIP_CHAR[iLen+SIP_THREE];
        pszVal[0] = DQUOTE;
        SipPf_Strcpy(pszVal+SIP_ONE,pElement);
        pszVal[iLen+SIP_ONE] = DQUOTE;
        pszVal[iLen+SIP_TWO] = SIP_NULL;
    }
    else
    {
        pszVal = SipPf_Strdup(pElement);
    }
    return pszVal;
}


/******************************************************************************
 * Function name      : SipAuthBase::DecodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipAuthBase ::DecodeHdr
(
 SIP_CHAR    *pucStartPt,
 SIP_UINT32  uiDecLen
 )
{
    SIP_CHAR            *pucEndPt = pucStartPt + uiDecLen - SIP_ONE;
    SIP_CHAR            *pucTempPre = SIP_NULL;

    if (sipFindLWS(pucStartPt, pucEndPt,&pucTempPre) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "LWS not found", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR *pszScheme = sipCreateString(pucStartPt, pucTempPre);
    if (SetValue(pszScheme) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation failed", SIP_ZERO, SIP_ZERO);
        if (pszScheme != SIP_NULL) {
            delete[] pszScheme;
        }
        return SIP_FALSE;
    }
    delete[] pszScheme;

    /*Update the temp to start of LWS*/
    pucTempPre = pucTempPre + SIP_ONE;
    /*Skip the LWS*/
    pucStartPt = sipSkipFwLWS(pucTempPre,pucEndPt);
    pucTempPre = SIP_NULL;

    while(pucStartPt < pucEndPt)
    {
        SIP_CHAR *pucTempNext = SIP_NULL;

        if (sipFindActualPos(pucStartPt, pucEndPt, &pucTempPre, &pucTempNext, COMMA) == SIP_FALSE)
        {
            pucTempPre = pucEndPt;
            pucTempNext = pucEndPt;
        }

        SipNameValue *pobjNmVl = new SipNameValue(GetHdrType());
        if (pobjNmVl == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation Fail", SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        if (pobjNmVl->DecHdrNameVal(pucStartPt,pucTempPre) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Name Value decode Fail", SIP_ZERO, SIP_ZERO);
            delete pobjNmVl;
            return SIP_FALSE;
        }
        if (m_objAuthList.Add(pobjNmVl) < 0)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Adding in list fail", SIP_ZERO, SIP_ZERO);
            delete pobjNmVl;
            return SIP_FALSE;
        }
        /*Update the Start point to the start of next Name Value Pair*/
        pucStartPt = pucTempNext;
        pucTempPre = SIP_NULL;
    }

    return SIP_TRUE;
}

SipHeaderBase* SipAuthBase::GetNewObj(SIP_INT32 eHdr, SipHeaderBase *pHeader)
{
    if (pHeader != SIP_NULL) {
        return new SipAuthBase(*reinterpret_cast<SipAuthBase*>(pHeader));
    }
    return new SipAuthBase(eHdr);
}
