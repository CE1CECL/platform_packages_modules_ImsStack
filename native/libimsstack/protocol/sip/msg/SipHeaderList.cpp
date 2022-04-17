/***********************************************************
 * Project Name : SIP_RTP
 * Group        : MSG-2
 * Security     : Confidential
 ***********************************************************/

/**********************************************************
 * Filename          : SipHeaderList.cpp
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
#include "sip_pf_datatypes.h"
#include "msg/SipHeaderList.h"
#include "msg/SipMessage.h"
#include "msg/sip_msgutil.h"
#include "sip_error.h"
#include "sip_debug.h"
#include "SipTrace.h"
#include "platform/sip_pf_string.h"


/****************************************************************************
  Macro Definitions
 *****************************************************************************/
extern SipHeaderBase * (*gaFactoryArray[SipHeaderBase::TYPE_END+SIP_ONE])(SIP_INT32,
                                                                           SipHeaderBase *);

extern SIP_BOOL gHeaderAttributes[SipHeaderBase::TYPE_END][SipHeaderBase::HEADER_ATTRIBUTE_END];
/****************************************************************************
  Enum Declaration
 *****************************************************************************/

/******************************************************************************
 * Function name      : SipHeaderList::SipHeaderList
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipHeaderList::SipHeaderList(SIP_INT32 eHdrType)
    : SipHeaderBase(eHdrType)
    , m_headerList(SipVector<SipHeaderBase*>())
{
}


/******************************************************************************
 * Function name      : SipHeaderList::SipHeaderList
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipHeaderList::SipHeaderList(const SipHeaderList &objHeaderList)
    : SipHeaderBase(objHeaderList)
{

    SIP_UINT32 usSize = objHeaderList.m_headerList.GetSize();
    for (SIP_UINT32 usCount = SIP_ZERO; usCount < usSize ; usCount++)
    {
        SipHeaderBase *pobjOldHdrBase = objHeaderList.m_headerList.GetAt(usCount);
        if (pobjOldHdrBase != SIP_NULL)
        {
            SipHeaderBase* pobjNewHdrBase = GetListObj(pobjOldHdrBase);
            if (pobjNewHdrBase != SIP_NULL)
            {
                if (m_headerList.Add(pobjNewHdrBase) < SIP_ZERO)
                {
                    SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Add to list fail", SIP_ZERO, SIP_ZERO);
                    pobjNewHdrBase->SipDelete();
                }
            }
            else
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "GetListObj Fail", SIP_ZERO, SIP_ZERO);
            }
        }
    }
}


/******************************************************************************
 * Function name      : SipHeaderList::~SipHeaderList
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipHeaderList::~SipHeaderList()
{
    while(m_headerList.IsEmpty() != SIP_TRUE)
    {
        SipHeaderBase *pHeaderBase = m_headerList.Top();
        pHeaderBase->SipDelete();
        m_headerList.Pop();
    }
}

/******************************************************************************
 * Function name      : SipHeaderList::EncodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
 SIP_BOOL SipHeaderList::EncodeHdr
(
    SIP_CHAR     **ppucCurrPos,
    SIP_BOOL     bParams/*= SIP_TRUE*/
 )
{
    return EncodeHdr(ppucCurrPos, bParams, ESIPMSGOPT_NONE);
}

/******************************************************************************
 * Function name      : SipHeaderList::EncodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipHeaderList::EncodeHdr
(
SIP_CHAR    **ppucCurrPos,
SIP_BOOL      bParams,
SIP_UINT32    uiMsgOptions
)
{
    if (m_headerList.IsEmpty() == SIP_TRUE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "List is Empty ", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SipHeaderBase* pstHdrObj = m_headerList.GetAt(SIP_ZERO);

    if (pstHdrObj == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "pstHdrObj is NULL",SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    if (pstHdrObj->EncodeHdr(ppucCurrPos, bParams) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Encode failed", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_UINT32 usSize = m_headerList.GetSize();

    /*Encoding of Next header elements in LIst*/
    for (SIP_UINT32 usCount = SIP_ONE; usCount < usSize; usCount++)
    {
        pstHdrObj = m_headerList.GetAt(usCount);
        if (pstHdrObj == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "pstHdrObj is NULL", SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        if (pstHdrObj->GetHdrType() == SipHeaderBase::UNKNOWN)
        {
            SIP_ENC_CRLF(*ppucCurrPos);
        }
        /*case of Multiple line encoding*/
        else if ((uiMsgOptions & ESIPMSGOPT_ENCMULTILINE) == ESIPMSGOPT_ENCMULTILINE)
        {
            SIP_ENC_CRLF(*ppucCurrPos);
            if ((uiMsgOptions & ESIPMSGOPT_ENCSHORTFORM) == ESIPMSGOPT_ENCSHORTFORM)
            {
                SIP_TRACE_NORMAL(ESIPTRACE_MODENCODER, "ShortForm Hdr", SIP_ZERO, SIP_ZERO);
                sipEncodeShortHdrName(pstHdrObj->GetHdrType(), ppucCurrPos);
            }
            else
            {
                SIP_TRACE_NORMAL(ESIPTRACE_MODENCODER, "Fullform Hdr",SIP_ZERO,SIP_ZERO);
                sipEncodeHdrName(pstHdrObj->GetHdrType(), ppucCurrPos, uiMsgOptions);
            }
        }
        else if (usCount < usSize)
        {
            SIP_ENC_COMMA(*ppucCurrPos);
        }

        if (pstHdrObj->EncodeHdr(ppucCurrPos, bParams) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Encode failed", SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
    }
    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipHeaderList::GetListObj
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipHeaderBase* SipHeaderList::GetListObj()
{
    SIP_INT32 eHdrType = GetHdrType();
    return gaFactoryArray[eHdrType](eHdrType,SIP_NULL);
}
/******************************************************************************
 * Function name      : SipHeaderList::GetListObj
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipHeaderBase* SipHeaderList::GetListObj(SipHeaderBase*pobjHdr)
{
    SIP_INT32 eHdrType = GetHdrType();
    return gaFactoryArray[eHdrType](eHdrType,pobjHdr);
}

/******************************************************************************
 * Function name      : SipHeaderList::AddHeader
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipHeaderList::AddHeader(SipHeaderBase* pObj)
{
    if (m_headerList.Add(pObj) < SIP_ZERO)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Adding in list failed", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    pObj->increment();
    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipHeaderList::InsertHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipHeaderList::InsertHdrAtPos(SipHeaderBase* pObj,SIP_UINT32 nIndex)
{
    if (nIndex > m_headerList.GetSize())
    {
        return SIP_FALSE;
    }

    m_headerList.InsertAt(pObj,nIndex);
    pObj->increment();
    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipHeaderList::RemoveHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
void SipHeaderList::RemoveHdr(SIP_UINT32 nIndex)
{
    if (nIndex < m_headerList.GetSize())
    {
        SipHeaderBase *pHeaderBase = m_headerList.GetAt(nIndex);
        pHeaderBase->SipDelete();
        m_headerList.RemoveAt(nIndex);
    }
}

/******************************************************************************
 * Function name      : SipHeaderList::Getobj
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipHeaderBase* SipHeaderList::GetObj(SIP_UINT32 usIndex)
{
    if (m_headerList.GetSize() < usIndex)
    {
        return SIP_NULL;
    }

    SipHeaderBase *pobjHdr = m_headerList.GetAt(usIndex);
    if (pobjHdr == SIP_NULL)
    {
        return SIP_NULL;
    }
    pobjHdr->increment();
    return pobjHdr;
}

/******************************************************************************
 * Function name      : SipHeaderList::DecodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipHeaderList::DecodeHdr
(
 SIP_CHAR        *pucStartPt,
 SIP_UINT32     uiDecLen
 )
{
    if (uiDecLen == SIP_ZERO )
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Empty buffer", SIP_ZERO, SIP_ZERO);
        return gHeaderAttributes[GetHdrType()][HEADER_EMPTY_BODY_ALLOWED];
    }

    SIP_CHAR *pucEndPt = pucStartPt + uiDecLen - SIP_ONE;
    while (pucStartPt <= pucEndPt)
    {
        SIP_CHAR *pucTempPre= SIP_NULL;
        SIP_CHAR *pucTempNext= SIP_NULL;

        if (sipFindActualPos(pucStartPt, pucEndPt, &pucTempPre, &pucTempNext, COMMA) == SIP_FALSE)
        {
            pucTempPre = pucEndPt;
            pucTempNext = pucEndPt;
        }

        SipHeaderBase *pobjHdrBase = GetListObj();
        if (pobjHdrBase == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory allocation Fail", SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        SIP_UINT32 uiLen = pucTempPre - pucStartPt + SIP_ONE;
        if (pobjHdrBase->DecodeHdr(pucStartPt,uiLen) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Hdr Decoding Fail", SIP_ZERO, SIP_ZERO);
            pobjHdrBase->SipDelete();
            return SIP_FALSE;
        }

        if (AddHeader(pobjHdrBase) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Add to list Failed", SIP_ZERO, SIP_ZERO);
            pobjHdrBase->SipDelete();//giri 25dec
            return SIP_FALSE;
        }
        pobjHdrBase->SipDelete();
        pucStartPt = (pucTempNext == pucEndPt) ? (pucTempNext + SIP_ONE) : pucTempNext;
    }

    return SIP_TRUE;
}
/******************************************************************************
 * Function name      : SipHeaderList::SipHeaderList
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/

SipHeaderBase* SipHeaderList:: GetNewListObj(SIP_INT32 eHdr, SipHeaderBase *pHeaderList)
{
    if (pHeaderList != SIP_NULL)
    {
        return new SipHeaderList(*((SipHeaderList*)pHeaderList));
    }
    return new SipHeaderList(eHdr);
}
