/*
    Author
    <table>
    date      author                description
    --------  --------------        ----------
    01/01/2015  vijay.nair@               Created
    </table>

    Description

*/

/*****************************************************************************
  Header Inclusions
 *****************************************************************************/
#include "msg/sip_msgutil.h"
#include "sip_pf_datatypes.h"
#include "platform/sip_pf_string.h"
#include "platform/sip_pf_memory.h"

#include "sip_debug.h"
#include "SipTrace.h"
#include "sip_error.h"

#include "msg/SipMessage.h"
#include "msg/SipMsgBody.h"


SIP_INT32 sipGetMethodType(const SIP_CHAR *pszMethod)
{
    SIP_CHAR SipMethodArray[][SIP_MAX_HDR_LEN] = {
        "INVITE",
        "ACK",
        "OPTIONS",
        "BYE",
        "CANCEL",
        "REGISTER",
        "INFO",
        "PRACK",
        "SUBSCRIBE",
        "NOTIFY",
        "UPDATE",
        "MESSAGE",
        "REFER",
        "PUBLISH",
    };

    if ((pszMethod == SIP_NULL) || (SipPf_Strlen(pszMethod) > SIP_MAX_HDR_LEN))
    {
        return SipMessage::METHOD_INVALID;
    }

    for (SIP_INT16 iIndex = SIP_ZERO; iIndex < SipMessage::METHOD_END; iIndex++)
    {
        if (SipPf_Stricmp(pszMethod, SipMethodArray[iIndex]) == SIP_ZERO) {
            return iIndex;
        }
    }
    return SipMessage::METHOD_INVALID;
}



/****************************************************************************
  Class Member Function Implementations
 *****************************************************************************/



/******************************************************************************
 * Function name      : SipMessage::SipMessage
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipMessage::SipMessage()
    : m_eSipMsgType(SipMessage::TYPE_INVALID)
    , m_pObjReqLine(SIP_NULL)
    , m_pObjStatusLine(SIP_NULL)
    , m_objHdrs(SIP_NULL)
    , m_pObjMsgBodyList(SIP_NULL)
#ifdef SIP_BADMESSAGE_PARSING
    , mbitMask(MANDATORY_HDR_NONE)
    , m_pobjBadHdrList(SIP_NULL)
#endif
{
    m_objHdrs = new SipHeaders();
}

/******************************************************************************
 * Function name      : SipMessage::SipMessage
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipMessage::SipMessage(const SipMessage &objSipMsg)
    : m_eSipMsgType(objSipMsg.m_eSipMsgType)
    , m_pObjReqLine(SIP_NULL)
    , m_pObjStatusLine(SIP_NULL)
    , m_objHdrs(SIP_NULL)
    , m_pObjMsgBodyList(SIP_NULL)
#ifdef SIP_BADMESSAGE_PARSING
    , mbitMask(objSipMsg.mbitMask)
    , m_pobjBadHdrList(SIP_NULL)
#endif
{
    m_objHdrs = new SipHeaders();

    m_objHdrs->CopyHdrs(objSipMsg.m_objHdrs);

    if (objSipMsg.m_pObjReqLine != SIP_NULL)
    {
        m_pObjReqLine = new SipRequestLine(*(objSipMsg.m_pObjReqLine));
    }

    if (objSipMsg.m_pObjStatusLine != SIP_NULL)
    {
        m_pObjStatusLine = new SipStatusLine(*(objSipMsg.m_pObjStatusLine));
    }

    if (objSipMsg.m_pObjMsgBodyList != SIP_NULL)
    {
        m_pObjMsgBodyList = new SipMsgBodyList(*(objSipMsg.m_pObjMsgBodyList));
    }

#ifdef SIP_BADMESSAGE_PARSING
    if (objSipMsg.m_pobjBadHdrList != SIP_NULL)
    {
        m_pobjBadHdrList = new SipHeaderList(*(objSipMsg.m_pobjBadHdrList));
    }
#endif
}

/******************************************************************************
 * Function name      : SipMessage::~SipMessage
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipMessage::~SipMessage()
{
    if (m_pObjReqLine != SIP_NULL)
    {
        m_pObjReqLine->SipDelete();
    }
    if (m_pObjStatusLine != SIP_NULL)
    {
        m_pObjStatusLine->SipDelete();
    }
    if (m_pObjMsgBodyList != SIP_NULL)
    {
        m_pObjMsgBodyList->SipDelete();
    }
    if (m_objHdrs != SIP_NULL)
    {
        delete m_objHdrs;
    }
#ifdef SIP_BADMESSAGE_PARSING
    if (m_pobjBadHdrList != SIP_NULL)
    {
        m_pobjBadHdrList->SipDelete();
    }
#endif
}

/******************************************************************************
 * Function name      : SipMessage::SipMessage
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipMessage::SipMessage(SIP_INT32 eSipMsgType)
    : m_eSipMsgType(eSipMsgType)
    , m_pObjReqLine(SIP_NULL)
    , m_pObjStatusLine(SIP_NULL)
    , m_objHdrs(SIP_NULL)
    , m_pObjMsgBodyList(SIP_NULL)
#ifdef SIP_BADMESSAGE_PARSING
    , mbitMask(MANDATORY_HDR_NONE)
    , m_pobjBadHdrList(SIP_NULL)
#endif
{
    m_objHdrs = new SipHeaders();
}

/******************************************************************************
 * Function name      : SipMessage::SetRequestline
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMessage::SetRequestline
(
 SipRequestLine* pobjReqLine
 )
{
    if (m_pObjReqLine != SIP_NULL)
    {
        m_pObjReqLine->SipDelete();
    }

    m_pObjReqLine = pobjReqLine;
    return SIP_TRUE;
}
/******************************************************************************
 * Function name      : SipMessage::RemoveHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMessage::RemoveHdr(SIP_INT32 eHdrType)
{
    return m_objHdrs->RemoveHdr(eHdrType);
}
/******************************************************************************
 * Function name      : SipMessage::SetHeader
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMessage::SetHeader
(
 SipHeaderBase* pobjHdr
 )
{
    SIP_BOOL bStatus = SIP_FALSE;
    SIP_INT32 nType = pobjHdr->GetHdrType();

    if (nType == SipHeaderBase::UNKNOWN)
    {
        bStatus = m_objHdrs->AppendHdr(pobjHdr);
    }
    else
    {
        bStatus = m_objHdrs->SetHdr(pobjHdr);
    }

    if (bStatus == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODACCESSOR, "set header type %d Fail", nType, SIP_ZERO);
        return SIP_FALSE;
    }

    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipMessage::SetMessageBody
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMessage::SetMessageBody
(
 SipMsgBody* pobjMsgBody
 )
{
    if (m_pObjMsgBodyList == SIP_NULL)
    {
        m_pObjMsgBodyList = new SipMsgBodyList();
    }
    return m_pObjMsgBodyList->AddBody(pobjMsgBody);
}

/******************************************************************************
 * Function name      : SipMessage::SetMessageType
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMessage::SetMessageType(SIP_INT32 eMsgType)
{
    m_eSipMsgType = eMsgType;
    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipMessage::SetStatusLine
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL    SipMessage::SetStatusLine
(
 SipStatusLine *pobjStatusLine
 )
{
    if (m_pObjStatusLine != SIP_NULL)
    {
        m_pObjStatusLine->SipDelete();
    }

    m_pObjStatusLine = pobjStatusLine;
    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipMessage::AppendHeader
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMessage::AppendHeader
(
 SipHeaderBase* pobjHdr
 )
{
    if (m_objHdrs->AppendHdr(pobjHdr) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODACCESSOR, "AppendHeader Fail", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    return SIP_TRUE;
}


/******************************************************************************
 * Function name      : SipMessage::InsertHeader
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : noneGLOBAL IMS_BOOL InsertHeader(IN SipHeaderBase* pstHeader,
                                             IN IMS_UINT32 nIndex,IN_OUT SipMessage *&pstMessage)
 *****************************************************************************/
SIP_BOOL SipMessage::InsertHeader
(
 SipHeaderBase* pobjHdr,
 SIP_UINT32 nIndex
 )
{
    if (m_objHdrs->InsertHdr(pobjHdr,nIndex) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODACCESSOR, "AppendHeader Fail", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipMessage::GetMethodType
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/

SIP_INT32 SipMessage:: GetMethodType()
{
    const SIP_CHAR *pszMethod = GetMethod();
    if (pszMethod == SIP_NULL)
    {
        return SipMessage::METHOD_INVALID;
    }
    return sipGetMethodType(pszMethod);
}

/******************************************************************************
 * Function name      : SipMessage::GetMethod
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
const SIP_CHAR* SipMessage::GetMethod()
{
    if (SipMessage::REQ_TYPE == m_eSipMsgType)
    {
        if (m_pObjReqLine == SIP_NULL)
        {
            return SIP_NULL;

        }
        return m_pObjReqLine->GetMethod();
    }

    if (m_pObjStatusLine == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "status line is NULL", SIP_ZERO, SIP_ZERO);
        return SIP_NULL;
    }

    SipCSeqHeader *pobjCseqHdr = (SipCSeqHeader *)GetHdrObj(SipHeaderBase::CSEQ);
    if (pobjCseqHdr == SIP_NULL)
    {

        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "CSEQ header is NULL", SIP_ZERO, SIP_ZERO);
        return SIP_NULL;
    }

    const SIP_CHAR *pszMethod = pobjCseqHdr->GetMethod();
    pobjCseqHdr->SipDelete();

    return pszMethod;
}

/******************************************************************************
 * Function name      : SipMessage::GetHdrObj
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipHeaderBase* SipMessage::GetHdrObj(SIP_INT32 eHdrType)
{
    return m_objHdrs->getHdrObj(eHdrType,SIP_ZERO);
}

/******************************************************************************
 * Function name      : SipMessage::HasHeader
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMessage::HasHeader(SIP_INT32 eHdrType) const
{
    SipHeaderBase *pobjHdrBase = m_objHdrs->getHdrObj(eHdrType);
    if (pobjHdrBase != SIP_NULL)
    {
        pobjHdrBase->SipDelete();
        return SIP_TRUE;
    }
    return SIP_FALSE;
}



/******************************************************************************
 * Function name      : IsReqLineExists
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMessage::IsReqLineExists()
{
    SipRequestLine *pobjReqLine = GetReqLine();
    if (pobjReqLine != SIP_NULL)
    {
        pobjReqLine->SipDelete();
        return SIP_TRUE;
    }
    return SIP_FALSE;
}

/******************************************************************************
 * Function name      : IsStatusLineExists
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMessage::IsStatusLineExists()
{
    SipStatusLine *pobjStatusLine = GetStatusLine();
    if (pobjStatusLine != SIP_NULL)
    {
        pobjStatusLine->SipDelete();
        return SIP_TRUE;
    }
    return SIP_FALSE;
}

/******************************************************************************
 * Function name      : HasMandatoryHdrs
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/

#ifdef SIP_BADMESSAGE_PARSING
SIP_BOOL SipMessage::HasMandatoryHdrs()
{
    SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "MANDATORY_HDR_MASK = %x", mbitMask, SIP_ZERO);

    if (mbitMask != MANDATORY_HDR_MASK)
    {
        return SIP_FALSE;
    }

    if (IsReqLineExists() == SIP_TRUE)
    {
        const SIP_CHAR *pMethod = GetMethod();

        if ((SipPf_Stricmp(pMethod, "PRACK") == 0) &&
            ((HasHeader(SipHeaderBase::RACK) == SIP_FALSE)))
        {
            return SIP_FALSE;
        }

        SipRequestLine *pReqLine = GetReqLine();
        if (pReqLine == SIP_NULL)
        {
            return SIP_FALSE;
        }

        const SIP_CHAR *pSipVer = pReqLine->GetSipVersion();
        SipAddrSpec *pReqUri = pReqLine->GetReqUri();
        if (((pReqUri != SIP_NULL) && (pMethod != SIP_NULL) && (pSipVer != SIP_NULL))
            == SIP_FALSE)
        {
            pReqLine->SipDelete();
            if (pReqUri != SIP_NULL)
            {
                pReqUri->SipDelete();
            }
            return SIP_FALSE;
        }
        pReqLine->SipDelete();
        pReqUri->SipDelete();
   }
   return SIP_TRUE;
}

/******************************************************************************
 * Function name      : GetBadHeaderCount
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/

SIP_INT32 SipMessage::GetBadHeaderCount() const
{
    if (m_pobjBadHdrList != SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Bad header count = %d",
            m_pobjBadHdrList->GetSize(), SIP_ZERO);
        return m_pobjBadHdrList->GetSize();
    }
    return 0;
}

/******************************************************************************
 * Function name      : GetBadHdrs
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipHeaderList* SipMessage::GetBadHdrs()
{
    return m_pobjBadHdrList;
}

/******************************************************************************
 * Function name      : DeleteBadHdrList
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/

SIP_VOID SipMessage::DeleteBadHdrList()
{
    if (m_pobjBadHdrList != SIP_NULL)
    {
        m_pobjBadHdrList->SipDelete();
        m_pobjBadHdrList = SIP_NULL;
    }
}
#endif
/******************************************************************************
 * Function name      : HasMIMEMessageBody
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMessage::HasMIMEMessageBody()
{
    SipContentTypeHeader *pObjHdr =
        SIP_DYNAMIC_CAST(SipContentTypeHeader *,(GetHdrObj(SipHeaderBase::CONTENT_TYPE)));

    if (pObjHdr == SIP_NULL)
    {
        return SIP_FALSE;
    }

    const SIP_CHAR *pszMType = pObjHdr->GetMediaType();
    SIP_INT16 usResult = SipPf_Stricmp(pszMType, MULTIPART);
    pObjHdr->SipDelete();
    return (usResult == SIP_ZERO) ? SIP_TRUE : SIP_FALSE;
}

/******************************************************************************
 * Function name      : HasSDPMessageBody
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMessage::HasSDPMessageBody()
{
    SipContentTypeHeader *pObjHdr =
        SIP_DYNAMIC_CAST(SipContentTypeHeader *,(GetHdrObj(SipHeaderBase::CONTENT_TYPE)));

    if (pObjHdr == SIP_NULL)
    {
        return SIP_FALSE;
    }

    const SIP_CHAR *pszSubMType = pObjHdr->GetSubMediaType();
    SIP_INT16 usResult = SipPf_Stricmp(pszSubMType, SDP);
    pObjHdr->SipDelete();
    return (usResult == SIP_ZERO) ? SIP_TRUE : SIP_FALSE;
}

/******************************************************************************
 * Function name      : SipMessage::GetHdrList
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipHeaderList* SipMessage::GetHdrList(SIP_INT32 eHdrType)
{
    return (SipHeaders::IsListHdr(eHdrType) == SIP_TRUE) ?
        (SipHeaderList*)m_objHdrs->getHdrObj(eHdrType) : SIP_NULL;
}


/******************************************************************************
 * Function name      : SipMessage::SetHdrList
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMessage::SetHdrList
(
 SipHeaderList    *pobjHdrList
 )
{
    SIP_UINT32 uiListSize = pobjHdrList->GetSize();
    for(SIP_UINT32 uiIndex = SIP_ZERO; uiIndex < uiListSize; uiIndex++)
    {
        SipHeaderBase *pobjHdrbase = pobjHdrList->GetObj(uiIndex);
        if (pobjHdrbase != SIP_NULL)
        {
            SIP_BOOL bStatus = m_objHdrs->AppendHdr(pobjHdrbase);
            pobjHdrbase->SipDelete();
            if (bStatus == SIP_FALSE)
            {
                return SIP_FALSE;
            }
        }
    }
    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipMessage::SetMsgBodyList
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMessage::SetMsgBodyList(SipMsgBodyList *pobjMsgBodyList)
{
    (void)pobjMsgBodyList;
    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipMessage::GetMsgBodyCount
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipMsgBody* SipMessage::GetMessageBody(SIP_INT32 nIndex)
{
    return (m_pObjMsgBodyList != SIP_NULL) ? m_pObjMsgBodyList->GetBodyByIndex(nIndex) : SIP_NULL;
}

/******************************************************************************
 * Function name      : SipMessage::AppendMessageBody
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipMessage::AppendMessageBody(SipMsgBody *pObjMsgBody)
{
    return SetMessageBody(pObjMsgBody);
}



/******************************************************************************
 * Function name      : SipMessage::GetMessageBody
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipMsgBody* SipMessage::GetMsgBody(SIP_UINT32 uiPos)
{
    return (m_pObjMsgBodyList != SIP_NULL) ? m_pObjMsgBodyList->GetBodyByIndex(uiPos) : SIP_NULL;
}


/******************************************************************************
 * Function name  : SipMessage::EncodeMsg
 *
 * Description   :
 *
 * Preconditions  :
 *
 * Side Effects  : none
 *****************************************************************************/
SIP_BOOL SipMessage::EncodeMsg
(
 SIP_CHAR    **ppucSipMsgBuffer, /* in-out parameter*/
 SIP_UINT32    *puiSipMsgLength, /* in-out parameter*/
 SIP_UINT32     uiMsgOptions
 )
{
    if ((m_pObjReqLine == SIP_NULL) && (m_pObjStatusLine == SIP_NULL))
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Start Line Missing", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    if ((m_eSipMsgType != SipMessage::REQ_TYPE) && (m_eSipMsgType != SipMessage::RESP_TYPE))
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Invalid sip message type", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR* pucCurrPos = *ppucSipMsgBuffer;

    if (m_eSipMsgType == SipMessage::REQ_TYPE && m_pObjReqLine != SIP_NULL)
    {
        if (m_pObjReqLine->EncodeRequestLine(&pucCurrPos) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Encoding Request line Fail",
                SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
    }
    else if (m_eSipMsgType == SipMessage::RESP_TYPE && m_pObjStatusLine != SIP_NULL)
    {
        if (m_pObjStatusLine->EncodeStatusLine(&pucCurrPos) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Encoding status line Fail",
                SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
    }

    //Put CRLF at the end of Start Line
    SIP_ENC_CRLF(pucCurrPos);

    SIP_TRACE_MESSAGE(ESIPTRACE_MODENCODER, "SipEnc_SipMsg:Encoded Msg(Start line) - %s",
        *ppucSipMsgBuffer, SIP_ZERO);

    //Set content type header if it is not set
    if (this->m_pObjMsgBodyList != SIP_NULL)
    {
        /*Set the content type and content length headers before header encoding*/
        if (UpdateMessage(uiMsgOptions) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "updating content headers fail",
                SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
    }
    else
    {
        SipUnknownHeader* pObjUnknown = GetUnknownHdrObj(SipHeaderBase::CONTENT_LENGTH);
        if (pObjUnknown == SIP_NULL) {
            SetContentLengthHdr(SIP_ZERO, uiMsgOptions);
        }
        else
        {
            //Content-Length header is present in message, reset the value to ZERO as no
            //body in the message.
            SIP_CHAR  cLen[SIP_CONTLEN_LEN] = {0};

            SipPf_Sprintf(cLen, (SIP_CHAR*)"%d", 0);
            pObjUnknown->SetHeaderValue(cLen);

            pObjUnknown->SipDelete();
            AdjustContentLengthHdr();
        }
    }

    //Encoding of headers
    if (m_objHdrs->EncodeHdrs(&pucCurrPos, uiMsgOptions) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Headers Encoding Failed", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    //Put CRLF at the end of List of Headers
    SIP_ENC_CRLF(pucCurrPos);

    //Encode Message body if present
    if (m_pObjMsgBodyList != SIP_NULL)
    {
        SipContentTypeHeader *pobjContentType = SIP_STATIC_CAST
            (SipContentTypeHeader*, GetHdrObj(SipHeaderBase::CONTENT_TYPE));
        if (pobjContentType == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "No ContentType header", SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        const SIP_CHAR  *pszMediaType = pobjContentType->GetMediaType();
        if (pszMediaType == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Content Type Invalid", SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        const SIP_CHAR *pszSubMediaType = pobjContentType->GetSubMediaType();
        if (pszSubMediaType == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Content Type Invalid", SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        if (SipPf_Strstr(pszSubMediaType, "message-summary") != SIP_NULL)
        {
            SipMsgBody *pobjBody = m_pObjMsgBodyList->GetBodyByIndex(SIP_ZERO);
            if (pobjBody == SIP_NULL)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Msg Body NULL", SIP_ZERO, SIP_ZERO);
                return SIP_FALSE;
            }
            if (pobjBody->EncodeMessageSummaryMsgBody(&pucCurrPos) == SIP_FALSE)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Encode message-summary fail",
                    SIP_ZERO, SIP_ZERO);
                return SIP_FALSE;
            }
            pobjBody->SipDelete();
        }
        else
        {
            /*Case of Single Body*/
            if (SipPf_Stricmp(pszMediaType, MULTIPART) != SIP_ZERO)
            {
                SipMsgBody *pobjBody = m_pObjMsgBodyList->GetBodyByIndex(SIP_ZERO);

                if (pobjBody == SIP_NULL)
                {
                    SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Msg Body NULL", SIP_ZERO, SIP_ZERO);
                    return SIP_FALSE;
                }

                if (pobjBody->EncodeSingleMsgBody(&pucCurrPos) == SIP_FALSE)
                {
                    SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Encode single message body fail",
                        SIP_ZERO, SIP_ZERO);
                    return SIP_FALSE;
                }

                pobjBody->SipDelete();
            }
            /*Case of multipart body*/
            else
            {
                SIP_CHAR *pszBoundary = pobjContentType->GetBoundary();
                if (pszBoundary == SIP_NULL)
                {
                    SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Boundary not found",
                        SIP_ZERO, SIP_ZERO);
                    return SIP_FALSE;
                }

                if (m_pObjMsgBodyList->EncodeBody(&pucCurrPos,pszBoundary) == SIP_FALSE)
                {
                    delete[] pszBoundary;
                    SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,"Encode multipart message body fail",
                        SIP_ZERO, SIP_ZERO);
                    return SIP_FALSE;
                }
                delete[] pszBoundary;
            }

        }
        pobjContentType->SipDelete();
    }

    //Calculate the length of the buffer
    SIP_UINT32 uiMsgLength = pucCurrPos - *ppucSipMsgBuffer;

    *puiSipMsgLength = uiMsgLength;

    SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Encoded Message of length(%d)",
        uiMsgLength, SIP_ZERO);

    return SIP_TRUE;
}

SIP_BOOL SipMessage::UpdateMessage(SIP_UINT32 uiMsgOptions)
{
    if (m_pObjMsgBodyList == SIP_NULL) {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Invalid param to UpdateMessage",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    /*check for content type header and set the new one if not present*/
    if (HasHeader(SipHeaderBase::CONTENT_TYPE) == SIP_FALSE)
    {
        SIP_UINT16 usBodyCount = m_pObjMsgBodyList->GetMsgBodyCount();

        /*In case there is a single body then take out the
          content type header from the body and set into the SIP Message*/
        if (usBodyCount == SIP_ONE)
        {
            SipMsgBody *pstMsgbody = m_pObjMsgBodyList->GetBodyByIndex(SIP_ZERO);
            if (pstMsgbody == SIP_NULL)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "No body in message body list",
                    SIP_ZERO, SIP_ZERO);
                return SIP_FALSE;
            }
            /*get the content type from the body*/
            SipContentTypeHeader *pobjTempContentType = pstMsgbody->GetContentType();
            SipContentTypeHeader *pobjContentType = new SipContentTypeHeader(*pobjTempContentType);

            if (pobjTempContentType != SIP_NULL)
            {
                pobjTempContentType->SipDelete();
            }

            /*Set the header into the SIP message*/
            SetHeader(pobjContentType);

            /*Delete After Setting*/
            pobjContentType->SipDelete();

            /*Delete the message body*/
            pstMsgbody->SipDelete();
        }
        /*Case of more than one bodies*/
        else
        {
            SipContentTypeHeader *pobjContentType = new SipContentTypeHeader();

            if (pobjContentType == SIP_NULL)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Memory allocation Fail",
                    SIP_ZERO, SIP_ZERO);
                return SIP_FALSE;
            }
            pobjContentType->SetMediaType(MULTIPART);
            pobjContentType->SetSubMediaType(MIXED);

            // Addition of boundary parameter is TBD

            SetHeader(pobjContentType);
            pobjContentType->SipDelete();
        }
    }

    /*Reset Content-length in message*/
    SIP_INT32 usLen = SIP_ZERO;

    /*Check the content type from message*/
    SipContentTypeHeader *pobjContentType = SIP_STATIC_CAST(SipContentTypeHeader*,
        GetHdrObj(SipHeaderBase::CONTENT_TYPE));

    if (pobjContentType != SIP_NULL)
    {
        /*Get the boundary*/
        SIP_CHAR *pszBoundary = pobjContentType->GetBoundary();
        /*Single Body*/
        if (pszBoundary == SIP_NULL)
        {
            usLen = m_pObjMsgBodyList->GetTotalBodyLen();
        }
        else
        {
            usLen = m_pObjMsgBodyList->GetTotalBodyLen(pszBoundary);
            delete[] pszBoundary;
        }
        pobjContentType->SipDelete();
    }
    else
    {
        usLen = SIP_ZERO;
    }

    /*Is content-length already present*/
    SipUnknownHeader *pObjUnknown = GetUnknownHdrObj(SipHeaderBase::CONTENT_LENGTH);
    if (pObjUnknown == SIP_NULL)
    { /*if not present add new*/
        SetContentLengthHdr(usLen, uiMsgOptions);
    }
    else
    { /*If present validate and update*/
        SIP_INT32 uiCurLen = 0;
        SIP_CHAR cLen[SIP_CONTLEN_LEN] = {0};
        const SIP_CHAR *pszCurLen = pObjUnknown->GetHeaderValue();

        if (pszCurLen != SIP_NULL)
        {
            uiCurLen = SipPf_Atoi(pszCurLen);
        }

        if (usLen != uiCurLen)
        { /*Mismatch in content length & actual body size*/
            SipPf_Sprintf(cLen, (SIP_CHAR*)"%u", usLen);
            pObjUnknown->SetHeaderValue(cLen);
        }

        pObjUnknown->SipDelete();

        AdjustContentLengthHdr();
    }

    return SIP_TRUE;
}


SIP_BOOL SipMessage::DecMultiPartBody
(
     SIP_CHAR      *pucBuffStart,
     SIP_CHAR      *pucBuffEnd,
     SIP_UINT32    uiMsgBuffLen
)
{
    (void)uiMsgBuffLen; //unused as of now

    //If body list already present, free it
    if (m_pObjMsgBodyList != SIP_NULL)
    {
        m_pObjMsgBodyList->SipDelete();
    }

    m_pObjMsgBodyList = new SipMsgBodyList();
    if (m_pObjMsgBodyList == SIP_NULL)
    {
        return SIP_FALSE;
    }

    SipContentTypeHeader* pobjContentType =
                (SipContentTypeHeader*)m_objHdrs->getHdrObj(SipHeaderBase::CONTENT_TYPE);
    if (pobjContentType == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Content Type Not present",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR* pszBoundary = pobjContentType->GetBoundary();
    if (pszBoundary == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "No boundary in Content Type Hdr",
            SIP_ZERO, SIP_ZERO);
        pobjContentType->SipDelete();
        return SIP_FALSE;
    }

    if (m_pObjMsgBodyList->DecodeMIMEBody(pucBuffStart, pucBuffEnd, pszBoundary) != SIP_TRUE)
    {
       SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecodeMIMEBody Fail", SIP_ZERO, SIP_ZERO);
       delete[] pszBoundary;
       pobjContentType->SipDelete();
       return SIP_FALSE;
    }

    delete[] pszBoundary;
    pobjContentType->SipDelete();

    return SIP_TRUE;
}

SIP_BOOL SipMessage::DecCompleteMsg
(
    SIP_CHAR            * pucMsgBuff,
    SIP_UINT32            uiMsgBuffLen
)
{
    if ((pucMsgBuff == SIP_NULL) || (uiMsgBuffLen <= SIP_ZERO))
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecCompleteMsg:Invalid input",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR *pucStartPt = pucMsgBuff;
    SIP_CHAR *pucEndPt = pucMsgBuff + uiMsgBuffLen;

    //Remove additional CRLF if present at the starting of incoming message.
    while (IS_CR(*pucStartPt) && IS_LF(*(pucStartPt + SIP_ONE)))
    {
        pucStartPt = pucStartPt + SIP_TWO;
    }

    /*Get the buffer till first terminating CRLF
      In case of Request  it should be request line
      In case of Response it should be status line*/
    SIP_BOOL bHdrEnd  = SIP_FALSE;
    SIP_CHAR *pucTempPos = SIP_NULL;

    if (sipFindTerminatingCRLF(pucStartPt, pucEndPt, &pucTempPos, &bHdrEnd) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecCompleteMsg:Incomplete Message",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    /*Update the End Point to the End of Start line*/
    pucEndPt = pucTempPos;
    pucTempPos = SIP_NULL;

    /*Now determine for request line or status line*/
    /*Find the first token*/
    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, SPACE) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecCompleteMsg:Invalid Start Line",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_INT32 eMsgType = sipGetMsgType(pucStartPt);
    SIP_UINT32 uiDecLen = pucEndPt - pucStartPt + SIP_ONE;

    if (eMsgType == SipMessage::REQ_TYPE)
    {
        m_pObjReqLine = new SipRequestLine();
        if (m_pObjReqLine == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecCompleteMsg:Memory Allocation fail",
                SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        if (m_pObjReqLine->DecodeRequestLine(pucStartPt, uiDecLen) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecCompleteMsg:Req Line Decoding fail",
                SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
    }
    else if (eMsgType == SipMessage::RESP_TYPE)
    {
        m_pObjStatusLine = new SipStatusLine();
        if (m_pObjStatusLine == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecCompleteMsg:Memory Allocation fail",
                SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        if (m_pObjStatusLine->DecodeStatusLine(pucStartPt, uiDecLen) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecCompleteMsg:Status Line Decoding fail",
                SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
    }
    else
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecCompleteMsg:Message Type Invalid",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    /*set the message type in message object*/
    this->m_eSipMsgType = eMsgType;

    /*Update the start point with header start*/
    pucStartPt = pucEndPt + SIP_THREE;
    pucTempPos = SIP_NULL;

    /*update the end point of the message*/
    pucEndPt =  pucMsgBuff + uiMsgBuffLen;

    /*Header Decoding*/

    //sipfrag message decoding failure as it may not have headers.
    if (pucStartPt >= pucEndPt)
    {
       return SIP_TRUE;
    }

    while ((pucStartPt < pucEndPt) && (bHdrEnd == SIP_FALSE))
    {
        /*find next terminating CRLF*/
        uiDecLen = SIP_ZERO;
        //Fail condition to be added
        if (sipFindTerminatingCRLF(pucStartPt,pucEndPt, &pucTempPos, &bHdrEnd) == SIP_FALSE) {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecCompleteMsg:Incomplete Message",
                SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        uiDecLen = pucTempPos - pucStartPt + SIP_ONE;

        SIP_CHAR *pucHdrName = SIP_NULL;
        SIP_CHAR *pucHdrBody = SIP_NULL;

        if (m_objHdrs->DecodeHdrs(pucStartPt,uiDecLen, (SIP_CHAR**)&pucHdrName,
            (SIP_CHAR**)&pucHdrBody) == SIP_FALSE)
        {
#ifdef SIP_BADMESSAGE_PARSING
            SipBadHeader* pBadHdr = new SipBadHeader();
            if (pBadHdr != SIP_NULL)
            {
                pBadHdr->SetHeaderName(pucHdrName);
                pBadHdr->SetValue(pucHdrBody);
                if (m_pobjBadHdrList == SIP_NULL)
                {
                    m_pobjBadHdrList = new SipHeaderList(SipHeaderBase::TYPE_INVALID);
                }
                m_pobjBadHdrList->AddHeader((SipHeaderBase*)pBadHdr);
                pBadHdr->SipDelete();
            }
#else
            if (pucHdrName != SIP_NULL)
            {
                delete[] pucHdrName;
            }
            if (pucHdrBody != SIP_NULL)
            {
                delete[] pucHdrBody;
            }

            return SIP_FALSE;
#endif
        }
        else
        {
#ifdef SIP_BADMESSAGE_PARSING
            SIP_INT32 eHdrType = sipGetHdrType(pucHdrName);
            switch (eHdrType)
            {
                case SipHeaderBase::FROM:
                {
                    mbitMask = (mbitMask | MANDATORY_HDR_FROM);
                    break;
                }
                case SipHeaderBase::TO:
                {
                    mbitMask = (mbitMask | MANDATORY_HDR_TO);
                    break;
                }
                case SipHeaderBase::CALL_ID:
                {
                    mbitMask = (mbitMask | MANDATORY_HDR_CALL_ID);
                    break;
                }
                case SipHeaderBase::CSEQ:
                {
                    mbitMask = (mbitMask | MANDATORY_HDR_CSEQ);
                    break;
                }
                case SipHeaderBase::VIA:
                {
                    mbitMask = (mbitMask | MANDATORY_HDR_VIA);
                    break;
                }
                default:
                    break;
            }
#endif
        }
        if (pucHdrName != SIP_NULL)
        {
            delete[] pucHdrName;
        }
        if (pucHdrBody != SIP_NULL)
        {
            delete[] pucHdrBody;
        }

        pucStartPt = pucTempPos + SIP_THREE;
        pucTempPos = SIP_NULL;
    }

    /*Check for Header end completion*/
    if (bHdrEnd != SIP_TRUE) {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecCompleteMsg:Incomplete Message",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    /*Check for CSeq Method Match*/
    if (m_pObjReqLine != SIP_NULL)
    {
        SipCSeqHeader *pobjCSeq = (SipCSeqHeader*)m_objHdrs->getHdrObj(SipHeaderBase::CSEQ);
        if (pobjCSeq != SIP_NULL)
        {
            const SIP_CHAR *pszMethod_ReqLine = m_pObjReqLine->GetMethod();
            const SIP_CHAR *pszMethod_CSeq = pobjCSeq->GetMethod();
            /*Compare Both the methods*/
            if ((SipPf_Stricmp(pszMethod_ReqLine,pszMethod_CSeq)))
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecCompleteMsg:Cseq Method Mismatch",
                    SIP_ZERO, SIP_ZERO);

                pobjCSeq->SipDelete();
                return SIP_FALSE;
            }
            pobjCSeq->SipDelete();
        }
    }

    /*Body decoding*/
    /*Update the start point to the start of Message body*/
    pucStartPt = pucStartPt + SIP_TWO;

    SipUnknownHeader *pobjContentLen = GetUnknownHdrObj(SipHeaderBase::CONTENT_LENGTH);
    if (pobjContentLen == SIP_NULL) {
        SIP_TRACE_NORMAL(ESIPTRACE_MODDECODER, "No Body present in message",
            SIP_ZERO, SIP_ZERO);
        return SIP_TRUE;
    }

    SIP_INT32 uiContentLen = 0;
    const SIP_CHAR *pszHdrValue = pobjContentLen->GetHeaderValue();
    if (pszHdrValue != SIP_NULL)
    {
        uiContentLen = SipPf_Atoi(pszHdrValue);
    }

    pobjContentLen->SipDelete();

    if (uiContentLen == SIP_ZERO)
    {
        SIP_TRACE_NORMAL(ESIPTRACE_MODDECODER, "No message body",SIP_ZERO, SIP_ZERO);
        return SIP_TRUE;
    }

    if (uiContentLen > (pucEndPt - pucStartPt))
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecCompleteMsg:Incomplete Message",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    /*Now Check for MIME or Single Body*/
    //If body list already present, free it
    if (m_pObjMsgBodyList != SIP_NULL)
    {
        m_pObjMsgBodyList->SipDelete();
        m_pObjMsgBodyList = SIP_NULL;
    }

    m_pObjMsgBodyList = new SipMsgBodyList();
    if (m_pObjMsgBodyList == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "DecCompleteMsg:Memory Allocation failed",
            SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    //Check Content-Encoding header
    SipHeaderBase *pobjContEnc = m_objHdrs->getHdrObj(SipHeaderBase::CONTENT_ENCODING,
                                                      SIP_ZERO);
    if (pobjContEnc != SIP_NULL)
    {
        const SIP_CHAR *encType = pobjContEnc->GetValue();
        pobjContEnc->SipDelete();
        if (encType != SIP_NULL)
        {
            if (SipPf_Stricmp(encType, "gzip") == SIP_ZERO)
            {
                if (m_pObjMsgBodyList->DecodeSingleBody(pucStartPt,pucEndPt) == SIP_FALSE)
                {
                    SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                        "DecCompleteMsg, GZIP Decoding single body failed",
                        SIP_ZERO, SIP_ZERO);
                    m_pObjMsgBodyList->SipDelete();
                    m_pObjMsgBodyList = SIP_NULL;
                }
                return SIP_FALSE;
            }
        }
    }

    SipContentTypeHeader *pobjContentType =
        (SipContentTypeHeader*)m_objHdrs->getHdrObj(SipHeaderBase::CONTENT_TYPE);
    if (pobjContentType == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecCompleteMsg: No Content-Type header", SIP_ZERO, SIP_ZERO);
        if (m_pObjMsgBodyList->DecodeSingleBody(pucStartPt, pucEndPt) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                "DecCompleteMsg:Decode single body fail",
                SIP_ZERO,SIP_ZERO);
            m_pObjMsgBodyList->SipDelete();
            m_pObjMsgBodyList = SIP_NULL;
            return SIP_FALSE;
        }
    }
    else
    {
        const SIP_CHAR *pszMType = pobjContentType->GetMediaType();
        if (pszMType == SIP_NULL)
        {
            SIP_TRACE_NORMAL(ESIPTRACE_MODDECODER, "Wronge Content Type",
                SIP_ZERO, SIP_ZERO);
            pobjContentType->SipDelete();
            m_pObjMsgBodyList->SipDelete();
            m_pObjMsgBodyList = SIP_NULL;
            return SIP_FALSE;
        }

        const SIP_CHAR *pszSubMType = pobjContentType->GetSubMediaType();
        if (pszSubMType == SIP_NULL)
        {
            SIP_TRACE_NORMAL(ESIPTRACE_MODDECODER, "Wronge Content Type",
                SIP_ZERO, SIP_ZERO);
            pobjContentType->SipDelete();
            m_pObjMsgBodyList->SipDelete();
            m_pObjMsgBodyList = SIP_NULL;
            return SIP_FALSE;
        }

        if (SipPf_Strstr(pszSubMType, "message-summary") != SIP_NULL)
        {
            if (m_pObjMsgBodyList->DecodeMessageSummaryBody(pucStartPt,
                pucEndPt) == SIP_FALSE)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "DecCompleteMsg:Decode message summary fail",
                    SIP_ZERO,SIP_ZERO);
                m_pObjMsgBodyList->SipDelete();
                m_pObjMsgBodyList = SIP_NULL;
                return SIP_FALSE;
            }
        }
        else
        {
            SIP_BOOL bSingleBody = (SIP_BOOL)SipPf_Stricmp(pszMType, MULTIPART);

            /*bSingleBody will be false if it is a MIME body*/
            if (bSingleBody == SIP_FALSE)
            {
                SIP_CHAR *pszBoundary = pobjContentType->GetBoundary();
                if (pszBoundary == SIP_NULL)
                {
                    SIP_TRACE_NORMAL(ESIPTRACE_MODDECODER,
                        "No boundary in Content Type Hdr", SIP_ZERO, SIP_ZERO);

                    pobjContentType->SipDelete();
                    m_pObjMsgBodyList->SipDelete();
                    m_pObjMsgBodyList = SIP_NULL;
                    return SIP_FALSE;
                }
                if (m_pObjMsgBodyList->DecodeMIMEBody(pucStartPt,
                    pucEndPt,pszBoundary) == SIP_FALSE)
                {
                    SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                            "DecCompleteMsg:Hdr Decoding fail", SIP_ZERO, SIP_ZERO);
                    delete[] pszBoundary;
                    pobjContentType->SipDelete();
                    m_pObjMsgBodyList->SipDelete();
                    m_pObjMsgBodyList = SIP_NULL;
                    return SIP_FALSE;
                }
                delete[] pszBoundary;
            }
            else
            {
                if (m_pObjMsgBodyList->DecodeSingleBody(pucStartPt,
                    pucEndPt) == SIP_FALSE)
                {
                    SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                            "DecCompleteMsg:Hdr Decoding fail",
                            SIP_ZERO,SIP_ZERO);
                    m_pObjMsgBodyList->SipDelete();
                    m_pObjMsgBodyList = SIP_NULL;
                    return SIP_FALSE;
                }
            }
        }

        pobjContentType->SipDelete();
    }

    return SIP_TRUE;
}

/*Set Content-length Header*/
SIP_VOID SipMessage::SetContentLengthHdr(SIP_UINT16 usLen, SIP_UINT32 uiMsgOptions)
{
    SipUnknownHeader* pobjUnknown = new SipUnknownHeader();
    if (pobjUnknown != SIP_NULL)
    {
        SIP_CHAR cLen[SIP_CONTLEN_LEN] = {0};
        SipPf_Sprintf(cLen, (SIP_CHAR*)"%u", usLen);

        const SIP_CHAR *pszHdrName =\
                ((uiMsgOptions & ESIPMSGOPT_ENCSHORTFORM) == ESIPMSGOPT_ENCSHORTFORM) ?\
                "l" : "Content-Length";

        pobjUnknown->SetHeaderName(pszHdrName);
        pobjUnknown->SetHeaderValue(cLen);
        SetHeader(pobjUnknown);

        pobjUnknown->SipDelete();
    }
}

/*Get Unknown header using name of the header*/
SipUnknownHeader* SipMessage::GetUnknownHdrObj(const SIP_CHAR* strHdrName)
{
    if (strHdrName == SIP_NULL) {
        return SIP_NULL;
    }

    SipHeaderList* pList = GetHdrList(SipHeaderBase::UNKNOWN);
    if (pList != SIP_NULL) {
        for (SIP_UINT32 i = 0; i < pList->GetSize(); i++) {
            SipUnknownHeader* pUnknown = (SipUnknownHeader*)pList->GetObj(i);
            if (pUnknown != SIP_NULL) {
                const SIP_CHAR* pszUKHdrName = pUnknown->GetHeaderName();
                if ((pszUKHdrName != SIP_NULL) &&
                    (SipPf_Stricmp(pszUKHdrName, strHdrName) == SIP_ZERO)) {
                        pList->SipDelete();
                        return pUnknown;
                }
                pUnknown->SipDelete();
            }
        }
        pList->SipDelete();
    }
    return SIP_NULL;
}

/*Get Unknown header using type of the header, Only Content-Length/l supported as of now*/
SipUnknownHeader* SipMessage::GetUnknownHdrObj(SIP_INT32 eType)
{
    SipHeaderList* pList = GetHdrList(SipHeaderBase::UNKNOWN);
    if (pList != SIP_NULL) {
        for (SIP_UINT32 i = 0; i < pList->GetSize(); i++) {
            SipUnknownHeader* pUnknown = (SipUnknownHeader*)pList->GetObj(i);
            if (pUnknown != SIP_NULL) {
                const SIP_CHAR* pszUKHdrName = pUnknown->GetHeaderName();
                switch (eType) {
                    case SipHeaderBase::CONTENT_LENGTH:
                        {
                            if (pszUKHdrName != SIP_NULL) {
                                if ((!SipPf_Stricmp(pszUKHdrName, "Content-Length")) ||
                                        (!SipPf_Stricmp(pszUKHdrName, "l"))) {
                                    pList->SipDelete();
                                    return pUnknown;
                                }
                            }
                        }
                        break;

                        /*TODO - All required headers to be added on need basis.*/
                        /*other option - Use SipMessage::GetUnknownHdrObj(SIP_CHAR* strHdrName)*/
                    default:
                        break;
                }
                pUnknown->SipDelete();
            }
        }
        pList->SipDelete();
    }
    return SIP_NULL;
}

/* Adjust Content-Length header : move it to the last unknown header's position */
void SipMessage::AdjustContentLengthHdr()
{
    SipHeaderList* pList = GetHdrList(SipHeaderBase::UNKNOWN);

    if (pList == SIP_NULL)
    {
        return;
    }

    SIP_UINT32 nCount = pList->GetSize();

    if (nCount <= 1)
    {
        pList->SipDelete();
        return;
    }

    /* Content-Length header is the last one, then no action */
    nCount -= 1;

    SipUnknownHeader* pContentLength = SIP_NULL;

    for (SIP_UINT32 i = 0; i < nCount; i++)
    {
        SipUnknownHeader* pUnknown = SIP_DYNAMIC_CAST(
                SipUnknownHeader*, pList->GetObj(i));

        if (pUnknown != SIP_NULL)
        {
            const SIP_CHAR* pszName = pUnknown->GetHeaderName();

            if (pszName != SIP_NULL)
            {
                if (SipPf_Stricmp(pszName, "Content-Length") == 0
                        || SipPf_Stricmp(pszName, "l") == 0)
                {
                    pContentLength = pUnknown;

                    pList->RemoveHdr(i);
                    break;
                }
            }

            pUnknown->SipDelete();
        }
    }

    if (pContentLength != SIP_NULL)
    {
        pList->AddHeader(pContentLength);

        pContentLength->SipDelete();
    }

    pList->SipDelete();
}

SIP_UINT16 SipMessage::GetStatusCode() const
{
    if (m_eSipMsgType != SipMessage::RESP_TYPE)
    {
        return SIP_SC_INVALID;
    }

    if (m_pObjStatusLine == SIP_NULL)
    {
        return SIP_SC_INVALID;
    }

    return m_pObjStatusLine->GetStatusCodeAsInt();
}

SipHeaderBase* SipMessage::GetHeader(SIP_INT32 nType, SIP_UINT32 nIndex)
{
    SipHeaderList* pHdrList = GetHdrList(nType);
    SipHeaderBase* pHdr = SIP_NULL;

    if (pHdrList == SIP_NULL)
    {
        pHdr = GetHdrObj(nType);
    }
    else
    {
        pHdr = pHdrList->GetObj(nIndex);
        pHdrList->SipDelete();
    }
    return pHdr;
}

SIP_INT32 SipMessage::GetHeaderCount(SIP_INT32 nType)
{
    SipHeaderBase* pHdr = GetHdrList(nType);
    if (pHdr == SIP_NULL)
    {
        pHdr = GetHdrObj(nType);
        if (pHdr == SIP_NULL)
        {
            return SIP_ZERO;
        }
        pHdr->SipDelete();
        return SIP_ONE;
    }
    SIP_INT32 nHCount = ((SipHeaderList*)pHdr)->GetSize();
    pHdr->SipDelete();
    return nHCount;
}
