/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipHeaders.cpp
 * Purpose               :
 * Platform              : Windows OR Android
 * Author(s)             : Vijay Nair
 * E-mail id.            : vijay.nair@
 * Creation date         : Jan 7, 2015
 *
 * Edit History             Modification                         Description(s)
 *
 * Date                Name            Version        Bug-ID        Description
 * ----------        ----------        -------        ------        -------------
 * Month,Date,Year     Name                 0.0a            Initial creation
 *****************************************************************************/

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

#include "msg/SipHeaders.h"
#include "msg/SipMsgBody.h"

#define NUM_OF_MANDATORY_HEADERS 5

extern  SIP_CHAR    gaszSipHdr[][SIP_MAX_HDR_LEN];

SipHeaderBase * (*gaFactoryArray[SipHeaderBase::TYPE_END+SIP_ONE])(SIP_INT32,SipHeaderBase *) = {
    SipHeaderBase::GetNewObj,//Allow
    SipAllowEventsHeader::GetNewObj,//AllowEvent
    SipAuthBase::GetNewObj, //Authorization
    SipHeaderBase::GetNewObj, //CallId
    SipContactHeader::GetNewObj,//    SipHeaderBase::CONTACT
    SipContactHeader::GetNewObj,//    SipHeaderBase::CONTACT_WILD
    SipContactHeader::GetNewObj,//    SipHeaderBase::CONTACT_ANY
    SipHeaderBase::GetNewObj, //    SipHeaderBase::CONTENT_DISPOSITION
    SipHeaderBase::GetNewObj,//    SipHeaderBase::CONTENT_ENCODING
    SipIntegerHeader::GetNewObj,//    SipHeaderBase::CONTENT_LENGTH
    SipContentTypeHeader::GetNewObj,//    SipHeaderBase::CONTENT_TYPE //10
    SipCSeqHeader::GetNewObj,//    SipHeaderBase::CSEQ
    SipEventHeader::GetNewObj,//    SipHeaderBase::EVENT
    SipIntegerHeader::GetNewObj,//    SipHeaderBase::EXPIRES_DATE
    SipIntegerHeader::GetNewObj,//    SipHeaderBase::EXPIRES_SEC
    SipIntegerHeader::GetNewObj,//    SipHeaderBase::EXPIRES_ANY
    SipAcceptHeader::GetNewObj,//     SipHeaderBase::ACCEPT
    SipIntegerHeader::GetNewObj,//    SipHeaderBase::MIN_EXPIRES //added
    SipFromHeader::GetNewObj,//    SipHeaderBase::FROM
    SipIntegerHeader::GetNewObj,//    SipHeaderBase::MAX_FORWARDS
    SipHeaderBase::GetNewObj,//    SipHeaderBase::MIME_VERSION,//20
    SipPrivacyHeader::GetNewObj,//    SipHeaderBase::PRIVACY,
    SipNameAddrHeader::GetNewObj,//    SipHeaderBase::P_PREFERRED_IDENTITY,
    SipNameAddrHeader::GetNewObj,//    SipHeaderBase::P_ASSERTED_IDENTITY,
    SipIntegerHeader::GetNewObj,//    SipHeaderBase::MIN_SE,
    SipNameAddrHeader::GetNewObj,//    SipHeaderBase::PATH,
    SipNameAddrHeader::GetNewObj,//    SipHeaderBase::P_ASSOCIATED_URI,
    SipNameAddrHeader::GetNewObj,//    SipHeaderBase::P_CALLED_PARTY_ID,
    SipPVisitedNetworkIdHeader::GetNewObj,//    SipHeaderBase::P_VISITED_NETWORK_ID,
    SipPChargingFunctionAddressesHeader::GetNewObj,//    SipHeaderBase::P_CHRG_FUN_ADDR,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::P_ACCESS_NETWORK_INFO,//30
    SipPChargingVectorHeader::GetNewObj,//    SipHeaderBase::P_CHARGING_VECTOR,
    SipNameAddrHeader::GetNewObj,//    SipHeaderBase::SERVICE_ROUTE,
    SipNameAddrHeader::GetNewObj,//    SipHeaderBase::HISTORY_INFO,
    SipRequestDispositionHeader::GetNewObj,//    SipHeaderBase::REQUEST_DISPOSITION,
    SipAcceptContactHeader::GetNewObj,//    SipHeaderBase::ACCEPT_CONTACT,
    SipRejectContactHeader::GetNewObj,//    SipHeaderBase::REJECT_CONTACT,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::JOIN,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::SIP_IF_MATCH,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::SIP_ETAG,
    SipAuthBase::GetNewObj,//    SipHeaderBase::PROXY_AUTHENTICATE,//40
    SipAuthBase::GetNewObj,//    SipHeaderBase::PROXY_AUTHORIZATION,
    SipRAcKHeader::GetNewObj,//    SipHeaderBase::RACK,
    SipNameAddrHeader::GetNewObj,//    SipHeaderBase::RECORD_ROUTE,
    SipNameAddrHeader::GetNewObj,//    SipHeaderBase::REFERRED_BY,
    SipNameAddrHeader::GetNewObj,//    SipHeaderBase::REFER_TO,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::REPLACES,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::REQUIRE,
    SipNameAddrHeader::GetNewObj,//    SipHeaderBase::ROUTE,
    SipRSeqHeader::GetNewObj,//    SipHeaderBase::RSEQ,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::SECURITY_CLIENT,//50
    SipHeaderBase::GetNewObj,//    SipHeaderBase::SECURITY_VERIFY,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::SECURITY_SERVER,
    SipIntegerHeader::GetNewObj,//    SipHeaderBase::SESSION_EXPIRES,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::SUBSCRIPTION_STATE,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::SUPPORTED,
    SipTimeStampHeader::GetNewObj,//    SipHeaderBase::TIMESTAMP,
    SipToHeader::GetNewObj,//    SipHeaderBase::TO,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::UNSUPPORTED,
    SipViaHeader::GetNewObj,//    SipHeaderBase::VIA,
    SipWarningHeader::GetNewObj,//    SipHeaderBase::WARNING,//60
    SipAuthBase::GetNewObj,//    SipHeaderBase::WWW_AUTHENTICATE,
    SipUnknownHeader::GetNewObj,//    SipHeaderBase::UNKNOWN,
    SipRetryAfterHeader::GetNewObj,//    SipHeaderBase::RETRY_AFTER_DATE,
    SipRetryAfterHeader::GetNewObj,//    SipHeaderBase::RETRY_AFTER_SEC,
    SipRetryAfterHeader::GetNewObj,//    SipHeaderBase::RETRY_AFTER_ANY,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::P_EARLY_MEDIA,
    SipResourcePriorityHeader::GetNewObj,//    SipHeaderBase::RESOURCE_PRIORITY,
    SipAcceptResourcePriorityHeader::GetNewObj,//    SipHeaderBase::ACCEPT_RESOURCE_PRIORITY,
    SipDateHeader::GetNewObj,//    SipHeaderBase::DATE,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::ACCEPT_ENCODING,//70
    SipHeaderBase::GetNewObj,//    SipHeaderBase::ACCEPT_LANGUAGE,
    SipInfoBase::GetNewObj,//    SipHeaderBase::ALERT_INFO,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::ANSWER_MODE,
    SipAuthInfoHeader::GetNewObj,//    SipHeaderBase::AUTHENTICATION_INFO,
    SipInfoBase::GetNewObj,//    SipHeaderBase::CALL_INFO,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::CONTENT_LANGUAGE,
    SipInfoBase::GetNewObj,//    SipHeaderBase::ERROR_INFO,
    SipIntegerHeader::GetNewObj,//    SipHeaderBase::FLOW_TIMER,
    SipIdentityHeader::GetNewObj,//    SipHeaderBase::IDENTITY,
    SipInfoBase::GetNewObj,//    SipHeaderBase::IDENTITY_INFO,//80
    SipHeaderBase::GetNewObj,//    SipHeaderBase::IN_REPLY_TO,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::ORGANIZATION,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::P_ANSWER_STATE,
    SipNameAddrHeader::GetNewObj,//    SipHeaderBase::PERMISSION_MISSING,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::P_MEDIA_AUTHORIZATION,
    SipNameAddrHeader::GetNewObj,//    SipHeaderBase::P_PROFILE_KEY,
    SipNameAddrHeader::GetNewObj,//    SipHeaderBase::P_REFUSED_URI_LIST,
    SipHeaderBase::GetNewObj, //   SipHeaderBase::PRIORITY,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::PRIV_ANSWER_MODE,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::PROXY_REQUIRE,//90
    SipNameAddrHeader::GetNewObj,//    SipHeaderBase::P_SERVED_USER,
    SipInfoBase::GetNewObj,//    SipHeaderBase::P_USER_DATABASE,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::REASON,
    SipReferSubHeader::GetNewObj,//    SipHeaderBase::REFER_SUB,
    SipNameAddrHeader::GetNewObj,//    SipHeaderBase::REPLY_TO,
    SIP_NULL,//    SipHeaderBase::RESPONSE_KEY,
    SipUserAgentHeader::GetNewObj,//    SipHeaderBase::SERVER,
                                  //Server header same as user agent syntax
    SipHeaderBase::GetNewObj,//    SipHeaderBase::SUBJECT,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::SUPPRESS_IF_MATCH,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::TARGET_DIALOG,//100
    SipTriggerConsentHeader::GetNewObj,//    SipHeaderBase::TRIGGER_CONSENT,
    SipUserAgentHeader::GetNewObj,//    SipHeaderBase::USER_AGENT,
    SipFeatureCapsHeader::GetNewObj,//    SipHeaderBase::FEATURE_CAPS,
    SipNameAddrHeader::GetNewObj,//    SipHeaderBase::GEOLOCATION,
    SipGeolocationErrorHeader::GetNewObj,//    SipHeaderBase::GEOLOCATION_ERROR,
    SipGeolocationRoutingHeader::GetNewObj,//    SipHeaderBase::GEOLOCATION_ROUTING,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::INFO_PACKAGE,//110
    SipIntegerHeader::GetNewObj,//    SipHeaderBase::MAX_BREADTH,
    SipPAssertedServiceHeader::GetNewObj,//    SipHeaderBase::P_ASSERTED_SERVICE,
    SipPolicyContactHeader::GetNewObj,//    SipHeaderBase::POLICY_CONTACT,
    SipNameAddrHeader::GetNewObj,//    SipHeaderBase::POLICY_ID,
    SipPPreferredServiceHeader::GetNewObj,//    SipHeaderBase::P_PREFERRED_SERVICE,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::RECV_INFO,
    SipHeaderBase::GetNewObj,//    SipHeaderBase::SESSION_ID
    SIP_NULL//    SipHeaderBase::TYPE_END //120
};

/******************************************************************************
 * Function name      : SipHeaders::SipHeaders
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipHeaders::SipHeaders()
{
    memset(m_HeaderArray,SIP_NULL,(SipHeaderBase::TYPE_END + SIP_ONE)* sizeof(SipHeaderBase*));
}


/******************************************************************************
 * Function name      : SipHeaders::CreateHdrObj
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipHeaderBase* SipHeaders::CreateCoreHdrObj
(
 SIP_INT32    eHdrType
 )
{
    eHdrType = CheckAndGetHdrEnumType(eHdrType);
    if ((eHdrType >= SIP_ZERO) && eHdrType < SipHeaderBase::TYPE_END)
    {
        return gaFactoryArray[eHdrType](eHdrType,SIP_NULL);
    }
    return SIP_NULL;
}

/******************************************************************************
 * Function name      : SipHeaders::SipHeaders
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipHeaders::CopyHdrs(SipHeaders *pobjHdrs)
{
    for (SIP_UINT32 uiCount = 0;uiCount < SipHeaderBase::TYPE_END; uiCount++ )
    {
        if (pobjHdrs->m_HeaderArray[uiCount] != SIP_NULL)
        {
            m_HeaderArray[uiCount] = (IsListHdr(uiCount) == SIP_TRUE) ?
                SipHeaderList::GetNewListObj(uiCount,pobjHdrs->m_HeaderArray[uiCount]) :
                gaFactoryArray[uiCount](uiCount,pobjHdrs->m_HeaderArray[uiCount]);
        }
    }
    return SIP_TRUE;
}

SipHeaderBase* SipHeaders::CloneHdrObj
(
 SipHeaderBase* pobjOld
 )
{
    if (pobjOld == SIP_NULL)
    {
        return SIP_NULL;
    }

    SIP_INT32 eHdrType = pobjOld->GetHdrType();

    return (eHdrType == SipHeaderBase::UNKNOWN) ?
        SipHeaderList::GetNewListObj(eHdrType,pobjOld) :
        gaFactoryArray[eHdrType](eHdrType,pobjOld);
}


SipHeaders::~SipHeaders()
{
    for (SIP_INT32 uiCount = 0; uiCount < SipHeaderBase::TYPE_END; uiCount++)
    {
        if (m_HeaderArray[uiCount] != SIP_NULL)
        {
            m_HeaderArray[uiCount]->SipDelete();
        }
    }
}


/******************************************************************************
 * Function name      : SipHeaders::getHdrObj
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipHeaderBase* SipHeaders::getHdrObj
(
 SIP_INT32 eIndex,
 SIP_UINT16 usPos
 )
{
    if ((eIndex < SIP_ZERO) || eIndex >= SipHeaderBase::TYPE_END)
    {
        return SIP_NULL;
    }

    if (m_HeaderArray[eIndex] != SIP_NULL)
    {
        m_HeaderArray[eIndex]->increment();

        return (IsListHdr(eIndex) == SIP_TRUE) ?
            ((SipHeaderList*)m_HeaderArray[eIndex])->GetObj(usPos) :
            m_HeaderArray[eIndex];
    }

    return SIP_NULL;
}

SIP_VOID SipHeaders::OverWriteHdrObj(IN SipHeaders* pobjSrcHdrs, IN SIP_BOOL bIgnoreUnknownHeader)
{
    for (SIP_INT32 iCount = 0; iCount < SipHeaderBase::TYPE_END; iCount++)
    {

        if ((pobjSrcHdrs->m_HeaderArray[iCount] == SIP_NULL) ||
            ((iCount == SipHeaderBase::UNKNOWN) && (bIgnoreUnknownHeader == SIP_TRUE)))
        {
            continue;
        }

        SipHeaderBase* pobjTemp = (IsListHdr(iCount) == SIP_TRUE) ?
            SipHeaderList::GetNewListObj(iCount, pobjSrcHdrs->m_HeaderArray[iCount]) :
            gaFactoryArray[iCount](iCount, pobjSrcHdrs->m_HeaderArray[iCount]);

        if (m_HeaderArray[iCount] != SIP_NULL)
        {
            m_HeaderArray[iCount]->SipDelete();
        }

        m_HeaderArray[iCount] = pobjTemp;
    }
}

/******************************************************************************
 * Function name      : SipHeaders::getHdrObj
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipHeaderBase* SipHeaders::getHdrObj(SIP_INT32 eIndex)
{
    if ((eIndex < SIP_ZERO) || eIndex >= SipHeaderBase::TYPE_END)
    {
        return SIP_NULL;
    }
    if (m_HeaderArray[eIndex] != SIP_NULL)
    {
        m_HeaderArray[eIndex]->increment();
        return m_HeaderArray[eIndex];
    }
    return SIP_NULL;
}

/******************************************************************************
 * Function name      : SipHeaders::getNewHdrObj
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipHeaderBase* SipHeaders::getNewHdrObj
(
 SIP_INT32    eHdrType
 )
{
    eHdrType = CheckAndGetHdrEnumType(eHdrType);
    if ((eHdrType < SIP_ZERO) || eHdrType >= SipHeaderBase::TYPE_END)
    {
        return SIP_NULL;
    }
    if (m_HeaderArray[eHdrType] != SIP_NULL)
    {
        return m_HeaderArray[eHdrType];
    }

    m_HeaderArray[eHdrType] = (IsListHdr( eHdrType) == SIP_TRUE) ?
        SipHeaderList::GetNewListObj(eHdrType,SIP_NULL) :
        gaFactoryArray[eHdrType](eHdrType,SIP_NULL);

    return m_HeaderArray[eHdrType];
}


SIP_BOOL SipHeaders::RemoveHdr(SIP_INT32 eHdrType)
{
    if ((eHdrType < SIP_ZERO) || eHdrType >= SipHeaderBase::TYPE_END)
    {
        return SIP_FALSE;
    }
    if (m_HeaderArray[eHdrType] != SIP_NULL) {
        (m_HeaderArray[eHdrType])->SipDelete();
    }
    m_HeaderArray[eHdrType] = SIP_NULL;
    return SIP_TRUE;
}


SIP_BOOL SipHeaders::SetHdr
(
 SipHeaderBase* pobjHdr
)
{
    if ((pobjHdr == SIP_NULL) || (pobjHdr->IsValidHeader() == SIP_FALSE))
    {
        return SIP_FALSE;
    }

    SIP_INT32 eHdrType = pobjHdr->GetHdrType();

    if (IsListHdr(eHdrType) == SIP_TRUE)
    {
        if (m_HeaderArray[eHdrType] != SIP_NULL)
        {
            RemoveHdr(eHdrType);
        }
        if (m_HeaderArray[eHdrType] == SIP_NULL)
        {
            m_HeaderArray[eHdrType] = getNewHdrObj(eHdrType);
        }
        ((SipHeaderList*)m_HeaderArray[eHdrType])->AddHeader(pobjHdr);
    }
    else
    {
        if (m_HeaderArray[eHdrType] != SIP_NULL)
        {
            m_HeaderArray[eHdrType]->SipDelete();
        }

        m_HeaderArray[eHdrType] = pobjHdr;
        pobjHdr->increment();
    }
    return SIP_TRUE;
}

SIP_BOOL SipHeaders::AppendHdr
(
 SipHeaderBase* pobjHdr
)
{
    if ((pobjHdr == SIP_NULL) || (pobjHdr->IsValidHeader() == SIP_FALSE))
    {
        return SIP_FALSE;
    }

    SIP_INT32 eHdrType = pobjHdr->GetHdrType();
    if (IsListHdr(eHdrType) == SIP_TRUE)
    {
        if (m_HeaderArray[eHdrType] == SIP_NULL)
         {
             m_HeaderArray[eHdrType] = getNewHdrObj(eHdrType);
         }
         ((SipHeaderList*)m_HeaderArray[eHdrType])->AddHeader(pobjHdr);
    }
    else
    {
        if (m_HeaderArray[eHdrType] != SIP_NULL)
        {
            m_HeaderArray[eHdrType]->SipDelete();
        }
        m_HeaderArray[eHdrType] = pobjHdr;
        pobjHdr->increment();
    }
    return SIP_TRUE;
}

SIP_BOOL SipHeaders::InsertHdr
(
 SipHeaderBase* pobjHdr,
 SIP_UINT32 nIndex
)
{
    if ((pobjHdr == SIP_NULL) || (pobjHdr->IsValidHeader() == SIP_FALSE))
    {
        return SIP_FALSE;
    }

    SIP_INT32 eHdrType = pobjHdr->GetHdrType();
    if (IsListHdr(eHdrType) == SIP_TRUE)
    {
        if (m_HeaderArray[eHdrType] == SIP_NULL)
        {
            m_HeaderArray[eHdrType] = getNewHdrObj(eHdrType);
        }
        ((SipHeaderList*)m_HeaderArray[eHdrType])->InsertHdrAtPos(pobjHdr,nIndex);
    }
    else
    {
        if (m_HeaderArray[eHdrType] != SIP_NULL)
        {
           m_HeaderArray[eHdrType]->SipDelete();
        }
        m_HeaderArray[eHdrType] = pobjHdr;
        pobjHdr->increment();
    }
    return SIP_TRUE;
}

SIP_BOOL SipHeaders::EncodeMandatoryHdrs
(
 SIP_CHAR   **ppucCurrPos,
 SIP_UINT32   uiMsgOptions
 )
{
    SIP_INT32 arMandatoryHeaders[NUM_OF_MANDATORY_HEADERS] = {
        SipHeaderBase::VIA,
        SipHeaderBase::FROM,
        SipHeaderBase::TO,
        SipHeaderBase::CALL_ID,
        SipHeaderBase::CSEQ };

    for (SIP_INT32 iCount = 0; iCount < NUM_OF_MANDATORY_HEADERS; iCount++)
    {
        SipHeaderBase* pHeaderObj = getHdrObj(arMandatoryHeaders[iCount]);
        if (pHeaderObj == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,"Mandatory header %d unavailable",
                arMandatoryHeaders[iCount], SIP_ZERO);
            return SIP_FALSE;
        }

        sipEncodeHdrName(arMandatoryHeaders[iCount], ppucCurrPos, uiMsgOptions);
        if (pHeaderObj->EncodeHdr(ppucCurrPos) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,"Mandatory header %d encode fail",
                arMandatoryHeaders[iCount], SIP_ZERO);
            pHeaderObj->SipDelete();
            return SIP_FALSE;

        }
        pHeaderObj->SipDelete();
        SIP_ENC_CRLF(*ppucCurrPos);
    }

    return SIP_TRUE;
}

SIP_BOOL SipHeaders::EncodeContentHdrs
(
 SIP_CHAR   **ppucCurrPos,
 SIP_UINT32   uiMsgOptions
 )
{
    SipHeaderBase *pobjTemp = getHdrObj(SipHeaderBase::CONTENT_TYPE);
    if (pobjTemp != SIP_NULL)
    {
        sipEncodeHdrName(SipHeaderBase::CONTENT_TYPE, ppucCurrPos, uiMsgOptions);
        if (pobjTemp->EncodeHdr(ppucCurrPos) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Content type encode fail",
                SIP_ZERO, SIP_ZERO);
            pobjTemp->SipDelete();
            return SIP_FALSE;
         }
        pobjTemp->SipDelete();
        SIP_ENC_CRLF(*ppucCurrPos);
    }
    return SIP_TRUE;
}

SIP_BOOL SipHeaders::EncodeHdrs
(
 SIP_CHAR   **ppucCurrPos,
 SIP_UINT32   uiMsgOptions
 )
{
    if (EncodeMandatoryHdrs(ppucCurrPos, uiMsgOptions) == SIP_FALSE)
    {
        return SIP_FALSE;
    }

    SIP_INT32 iHdr = SipHeaderBase::ALLOW;
    while (iHdr < SipHeaderBase::TYPE_END)
    {
        /* Ignore mandatory headers & content headers which are separately encoded*/
        if ((iHdr == SipHeaderBase::VIA) ||
            (iHdr == SipHeaderBase::FROM) ||
            (iHdr == SipHeaderBase::TO) ||
            (iHdr == SipHeaderBase::CALL_ID) ||
            (iHdr == SipHeaderBase::CSEQ) ||
            (iHdr == SipHeaderBase::CONTENT_TYPE) ||
            (iHdr == SipHeaderBase::CONTENT_LENGTH) ||
            (iHdr == SipHeaderBase::UNKNOWN))
        {
            iHdr++;
            continue;
        }

        SipHeaderBase* pHeaderObj = getHdrObj(iHdr);
        if (pHeaderObj != SIP_NULL)
        {
            sipEncodeHdrName(iHdr, ppucCurrPos, uiMsgOptions);

            if (pHeaderObj->EncodeHdr(ppucCurrPos, SIP_TRUE, uiMsgOptions) == SIP_FALSE)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Encode %d header Fail", iHdr, SIP_ZERO);
                pHeaderObj->SipDelete();
                return SIP_FALSE;
            }
            pHeaderObj->SipDelete();
            SIP_ENC_CRLF(*ppucCurrPos);
        }
        iHdr++;
    }

    if (EncodeContentHdrs(ppucCurrPos, uiMsgOptions) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Encode content header Fail", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SipHeaderBase* pHeaderObj = getHdrObj(SipHeaderBase::UNKNOWN);

    if (pHeaderObj != SIP_NULL)
    {
        if (pHeaderObj->EncodeHdr(ppucCurrPos, SIP_TRUE, uiMsgOptions) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Encode Unknown header Fail",
                SIP_ZERO, SIP_ZERO);
            pHeaderObj->SipDelete();
            return SIP_FALSE;
        }

        pHeaderObj->SipDelete();
        SIP_ENC_CRLF(*ppucCurrPos);
    }

    return SIP_TRUE;
}


/******************************************************************************
 * Function name      : SipHeaders:: IsListHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipHeaders::IsListHdr(SIP_INT32 eHdrType)
{
    if ((eHdrType < SIP_ZERO) || (eHdrType >= SipHeaderBase::TYPE_END))
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Wrong Header Value", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    return (arrSipHeadersType[eHdrType] == 1) ? SIP_TRUE : SIP_FALSE;
}


/******************************************************************************
 * Function name      : SipHeaders::DecodeHdrs
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipHeaders::DecodeHdrs
(
 SIP_CHAR    *pucStartPt,
 SIP_UINT32     uiDecLen,
 SIP_CHAR    **pucHdrName,
 SIP_CHAR    **pucHdrBody
 )

{
    if (pucStartPt == SIP_NULL || uiDecLen == SIP_ZERO)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Invalid arguments", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    /*Skip The LWS form the back*/
    /*Update the End point*/
    SIP_CHAR    *pucEndPt = pucStartPt + uiDecLen - SIP_ONE;
    pucEndPt = sipSkipRwLWS(pucStartPt,pucEndPt);

    SIP_CHAR    *pucTempPos= SIP_NULL;

    /*Get the position previous to ":"*/
    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, COLON) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "colon not found", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR *pucTempNext = pucTempPos + SIP_TWO;
    pucTempNext = sipSkipFwLWS(pucTempNext,pucEndPt);

    /*skip the WSP form back*/
    pucTempPos = sipSkipRwWSP(pucStartPt,pucTempPos);

    /*Create  the header name*/
    SIP_CHAR *pszHdrName = sipCreateString(pucStartPt, pucTempPos);
    if (pszHdrName == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation fail", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    *pucHdrName = SipPf_Strdup(pszHdrName);

    /*this will return the type of header on passing name*/
    SIP_INT32 eHdrType = sipGetHdrType(pszHdrName);
    delete[] pszHdrName;

    /*Get the header object*/
    SipHeaderBase *pobjHdrBase = getNewHdrObj(eHdrType);

    if (pobjHdrBase == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Getting object of header %d fail",
            eHdrType, SIP_ZERO);
        return SIP_FALSE;
    }
    /*Case of Unknown Header*/
    if (eHdrType == SipHeaderBase::UNKNOWN)
    {
        SipUnknownHeader* pobjUnknown = new SipUnknownHeader();
        if (pobjUnknown == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation fail", SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        if (pobjUnknown->SetHeaderName(*pucHdrName) == SIP_FALSE)
        {
            pobjUnknown->SipDelete();
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Set header name fail", SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        SIP_CHAR *pszHdrValue = sipCreateString(pucTempNext, pucEndPt);
        if (pszHdrValue == SIP_NULL)
        {
            pobjUnknown->SipDelete();
            if (pucTempNext > pucEndPt)
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Unknown Hdr Contain Invalid Value",
                    SIP_ZERO, SIP_ZERO);
                return SIP_TRUE;
            }
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation fail", SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }

        *pucHdrBody = SipPf_Strdup(pszHdrValue);

        if (pobjUnknown->SetHeaderValue(pszHdrValue) == SIP_FALSE)
        {
            pobjUnknown->SipDelete();
            delete[] pszHdrValue;
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Set header value fail", SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
        delete[] pszHdrValue;

        /*Add the header into the unknown list*/
        if (((SipHeaderList*)m_HeaderArray[SipHeaderBase::UNKNOWN])->AddHeader(pobjUnknown)
            == SIP_FALSE)
        {
            pobjUnknown->SipDelete();
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Add to list Fail", SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
        pobjUnknown->SipDelete();
    }
    else
    {
        /*Update the Start Point to the start of hdr value*/
        pucStartPt = pucTempNext;

        /*Update the length for decoding*/
        uiDecLen = pucEndPt - pucStartPt + SIP_ONE;

        /*Call the Decoder function*/
        *pucHdrBody = sipCreateString(pucTempNext,  pucEndPt);
        if (pobjHdrBase->DecodeHdr(pucStartPt, uiDecLen) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Decode header %d fail", eHdrType, SIP_ZERO);
            return SIP_FALSE;
        }
    }

    return SIP_TRUE;
}


/*****************************************************************************
 * Function name      : sipEncodeHdrName
 *
 * Description        :
 *
 * Preconditions      :
 *
 * Side Effects          : none
 *****************************************************************************/

SIP_BOOL sipEncodeHdrName
(
 SIP_INT32        eHdrType,
 SIP_CHAR             **ppucMsgBuffCurrPos,
 SIP_UINT32             uiMsgOptions
 )
{
    if (eHdrType < SIP_ZERO || eHdrType >= SipHeaderBase::TYPE_END)
    {
        return SIP_FALSE;
    }

    if ((uiMsgOptions & ESIPMSGOPT_ENCSHORTFORM) == ESIPMSGOPT_ENCSHORTFORM)
    {
        sipEncodeShortHdrName(eHdrType, ppucMsgBuffCurrPos);
        return SIP_TRUE;
    }

    SipPf_Strcpy(*ppucMsgBuffCurrPos,gaszSipHdr[eHdrType]);
    SipEnc_UpdateCurrPos(ppucMsgBuffCurrPos);

    SIP_ENC_COLON(*ppucMsgBuffCurrPos);

    SIP_ENC_SP(*ppucMsgBuffCurrPos);

    return SIP_TRUE;
}

SIP_BOOL sipEncodeShortHdrName
(
 SIP_INT32        eHdrType,
 SIP_CHAR         **ppucMsgBuffCurrPos
 )
{
    if (eHdrType < SIP_ZERO || eHdrType >= SipHeaderBase::TYPE_END)
    {
        return SIP_FALSE;
    }

    switch (eHdrType)
    {
        case SipHeaderBase::VIA:
            SIP_ENC_SHORT_VIA(*ppucMsgBuffCurrPos);
            break;
        case SipHeaderBase::TO:
            SIP_ENC_SHORT_TO(*ppucMsgBuffCurrPos);
            break;
        case SipHeaderBase::FROM:
            SIP_ENC_SHORT_FROM(*ppucMsgBuffCurrPos);
            break;
        case SipHeaderBase::CALL_ID:
            SIP_ENC_SHORT_CALLID(*ppucMsgBuffCurrPos);
            break;
        case SipHeaderBase::CONTACT:
        case SipHeaderBase::CONTACT_WILD:
        case SipHeaderBase::CONTACT_ANY:
            SIP_ENC_SHORT_CONTACT(*ppucMsgBuffCurrPos);
            break;
        case SipHeaderBase::CONTENT_TYPE:
            SIP_ENC_SHORT_CONTENT_TYPE(*ppucMsgBuffCurrPos);
            break;
        case SipHeaderBase::CONTENT_LENGTH:
            SIP_ENC_SHORT_CONTENT_LENGTH(*ppucMsgBuffCurrPos);
            break;
        case SipHeaderBase::ACCEPT_CONTACT:
            SIP_ENC_SHORT_ACCEPT_CONTACT(*ppucMsgBuffCurrPos);
            break;
        case SipHeaderBase::SESSION_EXPIRES:
            SIP_ENC_SHORT_SESSION_EXPIRES(*ppucMsgBuffCurrPos);
            break;
        case SipHeaderBase::SUPPORTED:
            SIP_ENC_SHORT_SUPPORTED(*ppucMsgBuffCurrPos);
            break;
        case SipHeaderBase::REQUEST_DISPOSITION:
            SIP_ENC_SHORT_REQUEST_DISPOSITION(*ppucMsgBuffCurrPos);
            break;
        case SipHeaderBase::REFERRED_BY:
            SIP_ENC_SHORT_REFERRED_BY(*ppucMsgBuffCurrPos);
            break;
        case SipHeaderBase::REFER_TO:
            SIP_ENC_SHORT_REFER_TO(*ppucMsgBuffCurrPos);
            break;
        case SipHeaderBase::CONTENT_ENCODING:
            SIP_ENC_SHORT_CONTENT_ENCODING(*ppucMsgBuffCurrPos);
            break;
        case SipHeaderBase::SUBJECT:
            SIP_ENC_SHORT_SUBJECT(*ppucMsgBuffCurrPos);
            break;
        case SipHeaderBase::REJECT_CONTACT:
            SIP_ENC_SHORT_REJECT_CONTACT(*ppucMsgBuffCurrPos);
            break;
        case SipHeaderBase::EVENT:
            SIP_ENC_SHORT_EVENT(*ppucMsgBuffCurrPos);
            break;
        case SipHeaderBase::ALLOW_EVENTS:
            SIP_ENC_SHORT_ALLOW_EVENTS(*ppucMsgBuffCurrPos);
            break;
        case SipHeaderBase::IDENTITY:
            SIP_ENC_SHORT_IDENTITY(*ppucMsgBuffCurrPos);
            break;
        case SipHeaderBase::IDENTITY_INFO:
            SIP_ENC_SHORT_IDENTITY_INFO(*ppucMsgBuffCurrPos);
            break;
        default:
            SipPf_Strcpy(*ppucMsgBuffCurrPos,gaszSipHdr[eHdrType]);
            SipEnc_UpdateCurrPos(ppucMsgBuffCurrPos);
            break;
    }

    SIP_ENC_COLON(*ppucMsgBuffCurrPos);

    SIP_ENC_SP(*ppucMsgBuffCurrPos);

    return SIP_TRUE;
}
