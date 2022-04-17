/***********************************************************
 * Project Name : SIP_RTP
 * Group        : MSG-2
 * Security     : Confidential
 ***********************************************************/

/**********************************************************
 * Filename          : SipParameterList.cpp
 * Purpose           :
 * Platform          : Windows XP
 * Author(s)         : Saurabh Srivastava
 * E-mail id.        : saurabh31.srivastava@
 * Creation date     : July 26, 2010
 *
 * Modifications:
 * 1. Modified by    : <Name>
 *    Date           : <mmm. dd, yyyy> (E.g. Apr. 21, 2006)
 *    Description    :
 *    Version Number : 0.0a
 *
 * 2. Modified by    : <Name>
 *    Date           : <mmm. dd, yyyy> (E.g. Apr. 21, 2006)
 *    Description    :
 *    Version Number : 0.0b
 **********************************************************/

/*****************************************************************************
  Header Inclusions
 *****************************************************************************/
#include "msg/SipParameterList.h"
#include "msg/SipAddrSpec.h"
#include "msg/SipRequestLine.h"
#include "platform/sip_pf_string.h"
#include "platform/sip_pf_memory.h"
#include "sip_debug.h"
#include "SipTrace.h"
#include "sip_debug.h"
#include "msg/sip_msgutil.h"

SIP_CHAR gaszAuthInfo[SIP_AINFO_CNT][SIP_AINFO_LEN]={"nextnonce","qop","rspauth","cnonce","nc"};

/****************************************************************************
  Macro Definitions
 *****************************************************************************/

/****************************************************************************
  Class Member Function Implementations
 *****************************************************************************/

/******************************************************************************
 * Function name      : SipParameterList::Add
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipParameterList::~SipParameterList()
{
    while(m_objPrmList.IsEmpty() != SIP_TRUE)
    {
        delete m_objPrmList.Top();
        m_objPrmList.Pop();
    }
}

/******************************************************************************
 * Function name      : SipParameterList::Add
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipParameterList::SipParameterList()
    : m_objPrmList(SipVector<SipNameValue*>())
    , m_eHdrType(SipHeaderBase::TYPE_INVALID)
{
}

/******************************************************************************
 * Function name      : SipParameterList::Add
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipParameterList::SipParameterList(SIP_INT32 eHdrType)
    : m_objPrmList(SipVector<SipNameValue*>())
    , m_eHdrType(eHdrType)
{
}

/******************************************************************************
 * Function name      : SipParameterList::Add
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipParameterList::SipParameterList(const SipParameterList &objPrmList)
    : m_objPrmList(SipVector<SipNameValue*>())
    , m_eHdrType(objPrmList.m_eHdrType)
{
    SIP_UINT32 usSize = objPrmList.m_objPrmList.GetSize();

    for (SIP_UINT32 usCount = SIP_ZERO; usCount < usSize; usCount++)
    {
        SipNameValue *pElement = objPrmList.m_objPrmList.GetAt(usCount);

        if (pElement == SIP_NULL)
        {
            continue;
        }

        SipNameValue* pobjNmVl = new SipNameValue(*pElement);

        if (pobjNmVl == SIP_NULL)
        {
            continue;
        }

        m_objPrmList.Add(pobjNmVl);
    }
}


/******************************************************************************
 * Function name      : SipParameterList::Add
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipParameterList::Add
(
 const SIP_CHAR     *pszName
 )
{
    if (pszName == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "Add: NULL param received",SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    SipNameValue *pobjNV = new SipNameValue();
    if (pobjNV == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "SipEnc_SipMsg: Malloc Failed",SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    pobjNV->m_pszName = SipPf_Strdup(pszName);
    m_objPrmList.Add(pobjNV);
    return SIP_TRUE;


}

/******************************************************************************
 * Function name      : SipParameterList::Add
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipParameterList::Add
(
 const SIP_CHAR     *pszName,
 const SIP_CHAR     *pszValue
 )
{
    if (pszName == SIP_NULL || pszValue == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
            "Add: NULL param received", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_UINT32 usSize = m_objPrmList.GetSize();
    SipNameValue* pobjNV = SIP_NULL;
    SIP_BOOL bFound = SIP_FALSE;
    SIP_UINT32 usIndex = SIP_ZERO;

    while ((usIndex < usSize) && (bFound != SIP_TRUE))
    {
        pobjNV = m_objPrmList.GetAt(usIndex);
        if (SipPf_Strcmp(pobjNV->m_pszName, pszName) == SIP_ZERO)
        {
            //found the required node
            bFound = SIP_TRUE;
            break;
        }
        usIndex++;
    }

    if (bFound == SIP_FALSE)
    {
        pobjNV = new SipNameValue();
        if (pobjNV == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "SipEnc_SipMsg: Malloc Failed", SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
        pobjNV->m_pszName = SipPf_Strdup(pszName);
    }

    pobjNV->m_valueList.Add(SipPf_Strdup(pszValue));

    // the name value obj to the list
    if (bFound == SIP_FALSE)
    {
        m_objPrmList.Add(pobjNV);
    }

    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipParameterList::Add
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL  SipParameterList::Add
(
 const SIP_CHAR     *pszName,
 const SIP_CHAR     *pszValue,
 SIP_INT32    ePrmType
)
{
    (void)pszName;
    (void)pszValue;
    (void)ePrmType;

    return SIP_TRUE;
}


/******************************************************************************
 * Function name      : SipParameterList::Remove
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipParameterList::Remove
(
 const SIP_CHAR     *pszName
 )
{
    if (pszName == SIP_NULL )
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
            "Add: NULL param received", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_UINT32 usSize = m_objPrmList.GetSize();

    SIP_BOOL bFound = SIP_FALSE;
    SIP_UINT32 usIndex = SIP_ZERO;
    while (usIndex < usSize && bFound != SIP_TRUE)
    {
        SipNameValue* pobjNV = m_objPrmList.GetAt(usIndex);
        if (SipPf_Strcmp(pobjNV->m_pszName,
                    pszName) == 0)
        {
            //found the required node
            bFound = SIP_TRUE;
            break;
        }
        usIndex++;
    }

    if (bFound == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "SipEnc_SipMsg: Param Not found", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    delete m_objPrmList.GetAt(usIndex);
    m_objPrmList.RemoveAt(usIndex);

    return SIP_TRUE;
}

SIP_BOOL SipParameterList::FindElement(const SIP_CHAR *pszName,SipNameValue *&pobjNmvl,
                                    SIP_UINT32& iPos)
{
    SIP_UINT32 uSize = m_objPrmList.GetSize();

    for(SIP_UINT32 uIndex = 0; uIndex < uSize; uIndex++)
    {
        SipNameValue *pobjNmVl = m_objPrmList.GetAt(uIndex);
        if (SipPf_Stricmp(pszName,pobjNmVl->m_pszName) == 0)
        {
            iPos = uIndex;
            pobjNmvl = pobjNmVl;
            return SIP_TRUE;
        }
    }
    return SIP_FALSE;
}

/******************************************************************************
 * Function name      : SipParameterList::SetParamValue
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipParameterList::SetParamValue
(
    const SIP_CHAR     *pszName,
    const SIP_CHAR     *pszValue,
    SIP_UINT32     usPos
)
{
    if (pszName == SIP_NULL)
    {
        return SIP_FALSE;
    }

    SipNameValue *pobjNmVl = SIP_NULL;
    SIP_UINT32 iPos = SIP_ZERO;

    /*Check whether parameter already exists or not*/
    SIP_BOOL bStatus = FindElement(pszName,pobjNmVl,iPos);

    /*If parameter not found add new entry*/
    if ((bStatus == SIP_FALSE) || (pobjNmVl == SIP_NULL))
    {
        if ((pszValue != SIP_NULL) && (SipPf_Strlen(pszValue) != SIP_ZERO))
        {
            return Add(pszName, pszValue);
        }
        return Add(pszName);
    }

    /*If parameter already exists, update value*/
    if (pobjNmVl->m_valueList.IsEmpty() != SIP_TRUE)
    {

        if (pobjNmVl->m_valueList.GetSize() < usPos)
        {
            return SIP_FALSE;
        }

        /*If the new value is NULL, remove the existing value*/
        if ((pszValue == SIP_NULL) || (SipPf_Strlen(pszValue) == SIP_ZERO))
        {
            delete pobjNmVl->m_valueList.GetAt(usPos);
            pobjNmVl->m_valueList.RemoveAt(usPos);
        }
        else
        {
            SIP_CHAR *pszVal = SipPf_Strdup(pszValue);
            delete pobjNmVl->m_valueList.GetAt(usPos);
            pobjNmVl->m_valueList.RemoveAt(usPos);
            pobjNmVl->m_valueList.InsertAt(pszVal,usPos);
        }
    }
    else
    {
        /*If parameter already not exists, add value*/
        if ((pszValue != SIP_NULL) && (SipPf_Strlen(pszValue) != SIP_ZERO))
        {
            SIP_CHAR *pszVal = SipPf_Strdup(pszValue);
            pobjNmVl->m_valueList.InsertAt(pszVal,SIP_ZERO);
        }
    }

    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipParameterList::GetParamValue
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_CHAR* SipParameterList::GetParamValue
(
 const SIP_CHAR     *pszName,
 SIP_UINT32     usPos /*default value is zero*/
 )
{
    if (pszName == SIP_NULL)
    {
        return SIP_NULL;
    }

    SipNameValue *pobjNmVl = SIP_NULL;
    SIP_UINT32     usPos1 = SIP_ZERO;

    SIP_BOOL bStatus = FindElement(pszName,pobjNmVl,usPos1);
    if ((bStatus == SIP_FALSE) || (pobjNmVl == SIP_NULL))
    {
        return SIP_NULL;
    }

    if ((pobjNmVl->m_valueList.IsEmpty() == SIP_TRUE) ||
        (pobjNmVl->m_valueList.GetSize() < usPos))
    {
        return SIP_NULL;
    }

    SIP_CHAR  *pElement = pobjNmVl->m_valueList.GetAt(usPos);

    return (pElement != SIP_NULL) ? SipPf_Strdup(pElement) : SIP_NULL;
}

/******************************************************************************
 * Function name      : SipParameterList::GetParamNode
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipParameterList::IsParamExists
(
 const SIP_CHAR     *pszName,
 SIP_UINT32 *pusPos
 )
{
    if (pszName == SIP_NULL)
    {
        return SIP_FALSE;
    }

    SipNameValue *pobjNmVl = SIP_NULL;

    return FindElement(pszName, pobjNmVl, *pusPos);
}

/******************************************************************************
 * Function name      : SipParameterList::GetParamNode
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipNameValue* SipParameterList::GetParamNode
(
 const SIP_CHAR     *pszName,
 SIP_UINT32    *pusPos
 )
{
    if (pszName == SIP_NULL)
    {
        return SIP_NULL;
    }

    SipNameValue *pobjNmVl = SIP_NULL;
    SIP_BOOL bStatus = FindElement(pszName, pobjNmVl, *pusPos);
    if ((bStatus == SIP_FALSE) || (pobjNmVl == SIP_NULL))
    {
        return SIP_NULL;
    }

    return pobjNmVl;
}

SIP_BOOL SipParameterList::EncodeList
(
 SIP_CHAR   **ppucCurrPos,
 SIP_CHAR  ucDelimiter
 )
{
    SIP_UINT32  sCount = m_objPrmList.GetSize();

    SIP_UINT32  iIndex = SIP_ZERO;
    //  enc for *( ";" uri-parameter )
    while (iIndex < sCount)
    {
        SipNameValue* pstParamNamValue = m_objPrmList.GetAt(iIndex);

        *(*ppucCurrPos) = ucDelimiter;
        (*ppucCurrPos)++;
        pstParamNamValue->EncodeFromList(ppucCurrPos);
        iIndex++;
    }
    return SIP_TRUE;
}

SIP_BOOL SipParameterList::EncodeUriParamList
(
 SIP_CHAR   **ppucCurrPos,
 SIP_CHAR  ucDelimiter,
 IParameterComponent   *m_pobjParameterComponent
 )
{
    SIP_UINT32 sCount = m_objPrmList.GetSize();

    SIP_UINT32  iIndex = SIP_ZERO;
    //  enc for *( ";" uri-parameter )
    while (iIndex < sCount)
    {
        SipNameValue* pstParamNamValue = m_objPrmList.GetAt(iIndex);

        *(*ppucCurrPos) = ucDelimiter;
        (*ppucCurrPos)++;
        pstParamNamValue->EncodeFromUriList(ppucCurrPos,m_pobjParameterComponent);
        iIndex++;
    }
    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipNameValue::SipNameValue
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipNameValue::SipNameValue()
    : m_pszName(SIP_NULL)
    , m_valueList(SipVector<SIP_CHAR*>())
    , m_ePrmType(SipParameters::GENERIC)
    , m_ucSep (',')
    , m_eHdrType(SipHeaderBase::TYPE_INVALID)
{

}

/******************************************************************************
 * Function name      : SipNameValue::SipNameValue
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipNameValue::SipNameValue(SIP_INT32 eHdrType)
    : m_pszName(SIP_NULL)
    , m_valueList(SipVector<SIP_CHAR*>())
    , m_ePrmType(SipParameters::GENERIC)
    , m_ucSep (',')
    , m_eHdrType(eHdrType)
{

}

/******************************************************************************
 * Function name      : SipNameValue::SipNameValue
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipNameValue::SipNameValue(const SipNameValue &objNmVl)
    : m_pszName(SipPf_Strdup(objNmVl.m_pszName))
    , m_valueList(SipVector<SIP_CHAR*>())
    , m_ePrmType(objNmVl.m_ePrmType)
    , m_ucSep (objNmVl.m_ucSep)
    , m_eHdrType(SipHeaderBase::TYPE_INVALID)
{
    SIP_UINT32 usSize = objNmVl.m_valueList.GetSize();

    for (SIP_UINT32 usCount = SIP_ZERO; usCount < usSize; usCount++)
    {
        SIP_CHAR* pElement = objNmVl.m_valueList.GetAt(usCount);

        if (pElement == SIP_NULL)
        {
            continue;
        }

        SIP_CHAR* pucValue = SipPf_Strdup(pElement);
        m_valueList.Add(pucValue);
    }
}

/******************************************************************************
 * Function name      : SipNameValue::~SipNameValue
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipNameValue::~SipNameValue()
{
    if (m_pszName != SIP_NULL)
    {
        delete[] m_pszName;
    }
    while(m_valueList.IsEmpty() != SIP_TRUE)
    {
        delete m_valueList.Top();
        m_valueList.Pop();
    }
}


/******************************************************************************
 * Function name      : SipNameValue::SetSeparator
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipNameValue::SetSeparator
(
 SIP_CHAR     ucSeparator
 )
{
    m_ucSep = ucSeparator;
    return SIP_TRUE;
}


SIP_BOOL SipNameValue::EncodeFromList(
        SIP_CHAR   **ppucCurrPos)
{
    if (m_pszName == SIP_NULL)
    {
        return SIP_FALSE;
    }

    SipPf_Strcpy(*ppucCurrPos, m_pszName);
    SipEnc_UpdateCurrPos(ppucCurrPos);

    if (m_valueList.IsEmpty() != SIP_TRUE)
    {
        SIP_UINT32 sCount = m_valueList.GetSize();

        **ppucCurrPos = EQUAL;
        (*ppucCurrPos)++;

        if (m_ePrmType == SipParameters::FEATURE)
        {
            **ppucCurrPos = DQUOTE;
            (*ppucCurrPos)++;
        }

        SIP_UINT32 sLocalCount = SIP_ZERO;

        while (sLocalCount < sCount)
        {
            SIP_CHAR *pucVal = m_valueList.GetAt(sLocalCount);
            if (pucVal == SIP_NULL)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                        "EncodeFromList: null value", SIP_ZERO, SIP_ZERO);
                return SIP_FALSE;
            }

            SipPf_Strcpy( *ppucCurrPos, pucVal);
            SipEnc_UpdateCurrPos(ppucCurrPos);

            /*Condition to prevent last put of separator*/
            if (sLocalCount < (sCount - SIP_ONE))
            {
                **ppucCurrPos = m_ucSep;
                (*ppucCurrPos)++;
            }
            sLocalCount++;
        }

        if (m_ePrmType == SipParameters::FEATURE)
        {
            **ppucCurrPos = DQUOTE;
            (*ppucCurrPos)++;
        }
    }
    else
    {
        if (m_ePrmType == SipParameters::FEATURE)
        {
            **ppucCurrPos = EQUAL;
            (*ppucCurrPos)++;

            **ppucCurrPos = DQUOTE;
            (*ppucCurrPos)++;

            **ppucCurrPos = DQUOTE;
            (*ppucCurrPos)++;
        }
    }

    return SIP_TRUE;
}


SIP_BOOL SipNameValue::EncodeFromUriList(
        SIP_CHAR   **ppucCurrPos,
        IParameterComponent   *pobjParameterComponent)
{
    if (m_pszName == SIP_NULL)
    {
        return SIP_FALSE;
    }

    SipPf_Strcpy(*ppucCurrPos, m_pszName);
    SipEnc_UpdateCurrPos(ppucCurrPos);

    if (m_valueList.IsEmpty() != SIP_TRUE)
    {
        //encode as name =  val
        SIP_CHAR* pucVal = m_valueList.GetAt(SIP_ZERO);
        if (pucVal == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                    "SipNameValue::EncodeFromList: List err", SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        **ppucCurrPos = EQUAL;
        (*ppucCurrPos)++;

        /*Do percentage Encoding of Value if Required*/
        if ((pobjParameterComponent != SIP_NULL) &&
            (pobjParameterComponent->IsValidComponent(m_pszName) == SIP_TRUE))
        {
            SIP_CHAR *pucValTemp = SipPercentEncoding::DoPerEnc_Param(m_pszName, pucVal);
            SipPf_Strcpy(*ppucCurrPos, pucValTemp);
            delete[] pucValTemp;
        }
        else
        {
            SipPf_Strcpy(*ppucCurrPos, pucVal);
        }
        SipEnc_UpdateCurrPos(ppucCurrPos);
    }

    return SIP_TRUE;
}


SIP_BOOL SipNameValue::EncodeFromUriHdrList(
        SIP_CHAR   **ppucCurrPos,
        IParameterComponent   *pobjParameterComponent)
{
    if (m_pszName == SIP_NULL)
    {
        return SIP_FALSE;
    }

    SipPf_Strcpy(*ppucCurrPos, m_pszName);
    SipEnc_UpdateCurrPos(ppucCurrPos);

    if (m_valueList.IsEmpty() != SIP_TRUE)
    {
        //encode as name =  val
        SIP_CHAR* pucVal = m_valueList.GetAt(SIP_ZERO);
        if (pucVal == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                    "SipNameValue::EncodeFromList: List err", SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        **ppucCurrPos = EQUAL;
        (*ppucCurrPos)++;

        /*Do percentage Encoding of Value if Required*/
        if ((pobjParameterComponent != SIP_NULL) &&
            (pobjParameterComponent->IsValidComponent(SIP_HEADERS) == SIP_TRUE))
        {
            SIP_CHAR *pucValTemp =
                SipPercentEncoding::DoPerEnc_Param((SIP_CHAR *)SIP_HEADERS, pucVal);
            SipPf_Strcpy(*ppucCurrPos, pucValTemp);
            delete[] pucValTemp;
        }
        else
        {
            SipPf_Strcpy(*ppucCurrPos, pucVal);
        }
        SipEnc_UpdateCurrPos(ppucCurrPos);
    }

    return SIP_TRUE;
}


/******************************************************************************
 * Function name      : SipNameValue::DecUriNameVal
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipNameValue::DecUriNameVal
(
 SIP_CHAR        *pucStartPt,
 SIP_CHAR        *pucEndPt,
 IParameterComponent     *pobjParameterComponent
 )
{
    SIP_CHAR *pucNameEnd = SIP_NULL;

    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucNameEnd, EQUAL) == SIP_FALSE)
    {
        pucNameEnd = pucEndPt;
    }

    m_pszName = sipCreateString(pucStartPt, pucNameEnd);
    if (m_pszName == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
            "SipNameValue::DecUriNameVal: Memory Allocation Failed",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    if (pucNameEnd != pucEndPt)
    {
        /*Update the pucNameEnd to the start of Value List*/
        SIP_CHAR *pucValuePtr = pucNameEnd + SIP_TWO;
        SIP_CHAR *pszValue = sipCreateString(pucValuePtr, pucEndPt);
        if (pszValue == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipNameValue::DecUriNameVal: Memory Allocation Failed",
                    SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        SIP_CHAR *pszTempValue = SIP_NULL;
        /*Do percent Decoding of value if applicable*/
        if ((pobjParameterComponent != SIP_NULL) &&
            (pobjParameterComponent->IsValidComponent(m_pszName) == SIP_TRUE))
        {
            pszTempValue = SipPercentEncoding::DoPercentDecoding(pszValue);
        }
        else
        {
            pszTempValue = pszValue;
        }
        /*put the value in the value list*/
        if (m_valueList.Add(pszTempValue) < SIP_ZERO)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipNameValue::DecUriNameVal:Adding in list failed",
                    SIP_ZERO, SIP_ZERO);
            delete[] pszTempValue;
            return SIP_FALSE;
        }
    }

    return SIP_TRUE;
}


/******************************************************************************
 * Function name      : SipNameValue::DecUriHdrNameVal
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipNameValue::DecUriHdrNameVal
(
 SIP_CHAR        *pucStartPt,
 SIP_CHAR        *pucEndPt,
 IParameterComponent     *pobjParameterComponent
 )
{
    SIP_CHAR *pucNameEnd = SIP_NULL;

    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucNameEnd, EQUAL) == SIP_FALSE)
    {
        pucNameEnd = pucEndPt;
    }

    m_pszName = sipCreateString(pucStartPt,  pucNameEnd);
    if (m_pszName == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
            "DecUriHdrNameVal: Memory Allocation Failed",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    if (pucNameEnd != pucEndPt)
    {
        /*Update the pucNameEnd to the start of Value List*/
        SIP_CHAR *pucValuePtr = pucNameEnd + SIP_TWO;
        SIP_CHAR *pszValue = sipCreateString(pucValuePtr, pucEndPt);
        if (pszValue == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "DecUriHdrNameVal: Memory Allocation Failed",
                    SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        SIP_CHAR *pszTempValue = SIP_NULL;
        /*Do percent Decoding of value if applicable*/
        if ((pobjParameterComponent != SIP_NULL) &&
            (pobjParameterComponent->IsValidComponent(SIP_HEADERS) == SIP_TRUE))
        {
            pszTempValue = SipPercentEncoding::DoPercentDecoding(pszValue);
        }
        else
        {
            pszTempValue = pszValue;
        }
        /*put the value in the value list*/
        if (m_valueList.Add(pszTempValue) < SIP_ZERO)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "DecUriHdrNameVal:Adding in list failed",
                    SIP_ZERO, SIP_ZERO);
            delete[] pszTempValue;
            return SIP_FALSE;
        }
    }

    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipNameValue::DecHdrNameVal
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipNameValue::DecHdrNameVal
(
    SIP_CHAR        *pucStartPt,
    SIP_CHAR        *pucEndPt
)
{
    if (pucStartPt > pucEndPt)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipNameValue::DecHdrNameVal: No Parameter Present",
                SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR *pucTempNext = SIP_NULL;
    SIP_CHAR *pucNameEnd = SIP_NULL;

    /*Case of name and Value Both*/
    if (sipFindActualPos(pucStartPt, pucEndPt, &pucNameEnd, &pucTempNext, EQUAL) == SIP_FALSE)
    {
        /*in case No equal present => only name present*/
        m_pszName = sipCreateString(pucStartPt,  pucEndPt);
        if (m_pszName == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipNameValue::DecHdrNameVal: Memory Allocation Failed",
                    SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
        return SIP_TRUE;
    }

    /*Check For Double Equal Condition*/
    if (*pucTempNext == EQUAL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipNameValue::DecHdrNameVal: Double Equal Present",
                SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;

    }

    /*Update the value pointer*/
    SIP_CHAR *pucValuePtr =  pucTempNext;

    /*Case of no value*/
    if (pucValuePtr > pucEndPt)
    {
        m_pszName = sipCreateString(pucStartPt,  pucNameEnd);
        if (m_pszName == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipNameValue::DecHdrNameVal: Name Allocation Failed",
                SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }

        return SIP_TRUE;
    }

    m_pszName = sipCreateString(pucStartPt,  pucNameEnd);
    if (m_pszName == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipNameValue::DecHdrNameVal: Name Allocation Failed",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    /*Case of multiple and single values*/
    /*TempPos will point to the end of current value */
    while (pucValuePtr <= pucEndPt)
    {
        SIP_CHAR *pszValue = SIP_NULL;
        pucTempNext = SIP_NULL;

        if (sipFindPreDelimiter(pucValuePtr, pucEndPt, &pucTempNext, COMMA) == SIP_FALSE)
        {
            pucTempNext = pucEndPt;
        }

        pszValue = sipCreateString(pucValuePtr, pucTempNext);
        if (pszValue == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipNameValue::DecHdrNameVal: Memory Allocation Failed",
                    SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
        /*put the value in the value list*/
        if (m_valueList.Add(pszValue) < SIP_ZERO)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipNameValue::DecHdrNameVal:Adding in list failed",
                    SIP_ZERO, SIP_ZERO);
            delete[] pszValue;
            return SIP_FALSE;
        }

        pucValuePtr = pucTempNext + SIP_TWO;

        if (pucTempNext == pucEndPt)
        {
            return SIP_TRUE;
        }
    }
    return SIP_TRUE;
}


/******************************************************************************
 * Function name      : SipParameterList::DecUriSipParameterList
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipParameterList::DecUriSipParameterList
(
 SIP_CHAR        *pucStartPt,
 SIP_CHAR        *pucEndPt,
 SIP_CHAR        ucDelimiter,
 IParameterComponent     *pobjParameterComponent
 )
{
    if (pucStartPt > pucEndPt)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecUriSipParameterList: No Value Present",
                SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    while (pucStartPt <= pucEndPt)
    {
        SIP_CHAR *pucTempPos = SIP_NULL;

        if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos,ucDelimiter) == SIP_FALSE)
        {
            pucTempPos = pucEndPt;
        }

        SipNameValue *pobjNmVl = new SipNameValue();
        if (pobjNmVl == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "DecUriSipParameterList: Memory Allocation Failed",
                    SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        if (pobjNmVl->DecUriNameVal(pucStartPt,pucTempPos,pobjParameterComponent) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "DecUriSipParameterList: Name Val Decode fail",
                    SIP_ZERO, SIP_ZERO);
            delete pobjNmVl;
            return SIP_FALSE;
        }

        if (m_objPrmList.Add(pobjNmVl) < SIP_ZERO)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "DecUriSipParameterList: Append in list Failed",
                    SIP_ZERO, SIP_ZERO);
            delete pobjNmVl;
            return SIP_FALSE;
        }

        if (pucTempPos == pucEndPt)
        {
            return SIP_TRUE;
        }

        pucStartPt = pucTempPos + SIP_TWO;

        if (pucStartPt > pucEndPt)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "DecUriSipParameterList: No Value Present",
                    SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }
    }
    return SIP_TRUE;
}



/******************************************************************************
 * Function name      : SipParameterList::DecUriHdrSipParameterList
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipParameterList::DecUriHdrSipParameterList
(
 SIP_CHAR        *pucStartPt,
 SIP_CHAR        *pucEndPt,
 SIP_CHAR        ucDelimiter,
 IParameterComponent     *pobjParameterComponent
 )
{
    if (pucStartPt > pucEndPt)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "SipParameterList::DecUriHdrSipParameterList: No Value Present",
                SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    while (pucStartPt <= pucEndPt)
    {
        SIP_CHAR *pucTempPos = SIP_NULL;

        if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos,ucDelimiter) == SIP_FALSE)
        {
            pucTempPos = pucEndPt;
        }

        SipNameValue *pobjNmVl = new SipNameValue();
        if (pobjNmVl == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipParameterList::DecUriSipParameterList: Memory Allocation Failed",
                    SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        if (pobjNmVl->DecUriHdrNameVal(pucStartPt,pucTempPos,pobjParameterComponent) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipParameterList::DecUriSipParameterList: Name Val Decode fail",
                    SIP_ZERO, SIP_ZERO);
            delete pobjNmVl;
            return SIP_FALSE;
        }

        if (m_objPrmList.Add(pobjNmVl) < 0)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipParameterList::DecUriSipParameterList: Append in list Failed",
                    SIP_ZERO, SIP_ZERO);
            delete pobjNmVl;
            return SIP_FALSE;
        }

        if (pucTempPos == pucEndPt)
        {
            return SIP_TRUE;
        }

        pucStartPt = pucTempPos + SIP_TWO;

        if (pucStartPt > pucEndPt)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipParameterList::DecUriSipParameterList: No Value Present",
                    SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }
    }

    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipParameterList::DecHdrSipPrm
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipParameterList::DecHdrSipParameterList
(
 SIP_CHAR        *pucStartPt,
 SIP_CHAR        *pucEndPt,
 SIP_CHAR        ucDelimiter
 )
{
    if (pucStartPt > pucEndPt)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecHdrSipParameterList: No Value Present",
                SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    while (pucStartPt <= pucEndPt)
    {
        SIP_CHAR *pucTempPre = SIP_NULL;
        SIP_CHAR *pucTempNext = SIP_NULL;

        if (sipFindActualPos(pucStartPt, pucEndPt,
                &pucTempPre, &pucTempNext, ucDelimiter) == SIP_FALSE)
        {
            pucTempPre = pucEndPt;
            pucTempNext = pucEndPt;
        }

        SipNameValue *pobjNmVl = new SipNameValue(m_eHdrType);
        if (pobjNmVl == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "DecHdrSipParameterList: Memory Allocation Failed",
                    SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }

        SIP_CHAR * pucTempPt = SIP_NULL;

        if (sipFindPreDelimiter(pucStartPt, pucTempPre, &pucTempPt, EQUAL) == SIP_TRUE)
        {
            SIP_CHAR *pucTempPrm = sipCreateString(pucStartPt, pucTempPt);

            if (SipPf_Stricmp(pucTempPrm, "method")== SIP_ZERO)
            {
                SipNameValue  *pobjNmVlMethod = SIP_NULL;
                pobjNmVlMethod = new SipNameValue(m_eHdrType);
                if (pobjNmVlMethod == SIP_NULL)
                {
                    SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                        "DecHdrSipParameterList: Memory Allocation Failed",
                        SIP_ZERO,SIP_ZERO);
                    delete[] pucTempPrm;
                    delete pobjNmVl;
                    return SIP_FALSE;
                }

                SIP_CHAR *pucQtnMark = SIP_NULL;
                if (sipFindPreDelimiter(pucTempPt+2, pucTempPre, &pucQtnMark, QMARK) == SIP_FALSE)
                {
                    SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                            "DecHdrSipParameterList: Question mark not Found",
                            SIP_ZERO,SIP_ZERO);
                    delete pobjNmVlMethod;
                    delete pobjNmVl;
                    delete[] pucTempPrm;
                    return SIP_FALSE;
                }
                if (pobjNmVlMethod->DecHdrNameVal(pucStartPt, pucQtnMark) == SIP_FALSE)
                {
                    SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                            "DecHdrSipParameterList:Name Value decode Failed",
                            SIP_ZERO,SIP_ZERO);
                    delete pobjNmVlMethod;
                    delete pobjNmVl;
                    delete[] pucTempPrm;
                    return SIP_FALSE;
                }
                if (m_objPrmList.Add(pobjNmVlMethod) < 0)
                {
                    SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                            "DecHdrSipParameterList: Append in list Failed",
                            SIP_ZERO,SIP_ZERO);
                    delete pobjNmVlMethod;
                    delete pobjNmVl;
                    delete[] pucTempPrm;
                    return SIP_FALSE;
                }
                pucStartPt = pucQtnMark +2;
            }
            delete[] pucTempPrm;
        }

        if (pobjNmVl->DecHdrNameVal(pucStartPt,pucTempPre) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "DecHdrSipParameterList:Name Value decode Failed",
                    SIP_ZERO,SIP_ZERO);
            delete pobjNmVl;
            return SIP_FALSE;
        }
        if (m_objPrmList.Add(pobjNmVl) < SIP_ZERO)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "DecHdrSipParameterList: Append in list Failed",
                    SIP_ZERO, SIP_ZERO);
            delete pobjNmVl;
            return SIP_FALSE;
        }

        if (pucTempNext == pucEndPt)
        {
            return SIP_TRUE;
        }
        /*Update the Start point to the start of next Name Value Pair*/
        pucStartPt = pucTempNext;
        if (pucStartPt > pucEndPt)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "DecHdrSipParameterList: No Parameter Present",
                    SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
    }

    return SIP_TRUE;

}

/* To Print all header parameters & their values*/
/* Enable SIP_TRACE_ENABLE to log */
SIP_VOID SipParameterList::PrintParams()
{
    //first search the list if the name exists
    SIP_UINT32 usSize = m_objPrmList.GetSize();
    if (usSize <= SIP_ZERO) {
        SIP_TRACE_NORMAL(ESIPTRACE_MODDECODER,
                "PrintParams: Param List Empty",SIP_ZERO,SIP_ZERO);
        return;
    }

    SIP_UINT32 usIndex = SIP_ZERO;
    while (usIndex < usSize)
    {
        SipNameValue* pobjNV = m_objPrmList.GetAt(usIndex);

        if (pobjNV != SIP_NULL)
        {
            SIP_TRACE_NORMAL(ESIPTRACE_MODDECODER, "PrintParams: Name of Param : %s",
                            pobjNV->m_pszName,SIP_ZERO);

            {
                SIP_UINT32 usValSize = pobjNV->m_valueList.GetSize();
                for (SIP_UINT32 usCount = SIP_ZERO; usCount < usValSize; usCount++)
                {
                    SIP_CHAR *pElement = pobjNV->m_valueList.GetAt(usCount);
                    if (pElement == SIP_NULL) {
                        break;
                    }

                    SIP_TRACE_NORMAL(ESIPTRACE_MODDECODER, "PrintParams: Value of param : %s",
                            pElement, SIP_ZERO);
                }
             }
        }
        usIndex++;
    }
}

/*****************************************************************************
  SipParameters - Class Member Function Implementations
*****************************************************************************/
SipParameters::SipParameters() : pParameterList(new SipParameterList())
{
}

SipParameters::SipParameters(const SipParameters &objParameters)
{
    if (objParameters.pParameterList != SIP_NULL)
    {
        pParameterList = new SipParameterList(*(objParameters.pParameterList));
    }
    else
    {
        pParameterList = new SipParameterList();
    }
}

SipParameters::SipParameters(SipParameterList* pParameterList_)
{
    if (pParameterList_ != SIP_NULL)
    {
        pParameterList = new SipParameterList(*pParameterList_);
    }
    else
    {
        pParameterList = new SipParameterList();
    }
}

SipParameters::~SipParameters()
{
    if (pParameterList != SIP_NULL) {
        pParameterList->SipDelete();
    }
}

SIP_BOOL SipParameters::AddParam(const SIP_CHAR *pszName)
{
    if (pParameterList == SIP_NULL) {
        pParameterList = new SipParameterList();
    }

    pParameterList->Add(pszName);

    return SIP_TRUE;
}

SIP_BOOL SipParameters::AddParam(const SIP_CHAR *pszName, const SIP_CHAR *pszValue)
{
    if (pParameterList == SIP_NULL)
    {
        pParameterList = new SipParameterList();
    }

    return pParameterList->Add(pszName, pszValue);
}

SIP_BOOL SipParameters::AddParam
(
    const SIP_CHAR           *pszName,
    const SIP_CHAR           *pszValue,
    SIP_INT32    ePrmType
)
{
    if (pParameterList == SIP_NULL) {
        pParameterList = new SipParameterList();
    }

    return pParameterList->Add(pszName, pszValue, ePrmType);
}

SIP_BOOL SipParameters::RemoveParam
(
    const SIP_CHAR   *pszName
)
{
    if (pParameterList == SIP_NULL) {
        return SIP_TRUE;
    }

    return pParameterList->Remove(pszName);
}

SIP_VOID SipParameters::SetParameterList
(
    SipParameterList    *pobjSipPrm
)
{
    if (pobjSipPrm == SIP_NULL)
    {
        return;
    }

    if (pParameterList != SIP_NULL) {
        pParameterList->SipDelete();
    }

    pParameterList = pobjSipPrm;
}

SIP_BOOL    SipParameters::SetParamValue
(
    const SIP_CHAR     *pszName,
    const SIP_CHAR     *pszValue,
    SIP_UINT32   usPos
)
{
    if (pParameterList == SIP_NULL) {
        pParameterList = new SipParameterList();
    }

    return pParameterList->SetParamValue(pszName, pszValue, usPos);
}

SIP_BOOL SipParameters::IsParamExists(const SIP_CHAR *pszName, SIP_UINT32 * pusPos)
{
    return (pParameterList != SIP_NULL) ?
        pParameterList->IsParamExists(pszName, pusPos) : SIP_FALSE;
}

SipParameterList* SipParameters::GetParameterList()
{
    return pParameterList;
}

SIP_CHAR* SipParameters::GetParamValue
(
    const SIP_CHAR     *pszName
)
{
    if (pParameterList != SIP_NULL)
    {
        return pParameterList->GetParamValue(pszName);
    }
    return SIP_NULL;
}

SipNameValue* SipParameters::GetParamNode
(
    const SIP_CHAR     *pszName,
    SIP_UINT32    *pusPos
)
{
    if (pParameterList != SIP_NULL) {
        return pParameterList->GetParamNode(pszName, pusPos);
    }

    return SIP_NULL;
}
