/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipHeaders.h
 * Purpose               :
 * Platform              : Windows OR Android
 * Author(s)             : Vijay Nair
 * E-mail id.            : vijay.nair@
 * Creation date         : Jan 7, 2015
 *
 * Edit History         Modification description(s)
 * Date                Name            Version        Bug-ID        Description
 * ----------        ----------        -------        ------        -------------
 * Month. Date,10        Name                 0.0a            Initial creation
 *****************************************************************************/

#ifndef __SIP_HEADERS_H__
#define __SIP_HEADERS_H__

/*****************************************************************************
  Header Inclusions
 *****************************************************************************/
#include "sip_pf_datatypes.h"
#include "SipRefBase.h"
#include "msg/SipHeaderBase.h"
#include "msg/SipHeaderList.h"
#include "msg/SipAcceptHeader.h"
#include "msg/SipAcceptContactHeader.h"
#include "msg/SipAllowEventsHeader.h"
#include "msg/SipAuthBase.h"
#include "msg/SipCSeqHeader.h"
#include "msg/SipFromHeader.h"
#include "msg/SipIntegerHeader.h"
#include "msg/SipPrivacyHeader.h"
#include "msg/SipRequestDispositionHeader.h"
#include "msg/SipRetryAfterHeader.h"
#include "msg/SipTimeStampHeader.h"
#include "msg/SipToHeader.h"
#include "msg/SipUserAgentHeader.h"
#include "msg/SipViaHeader.h"
#include "msg/SipWarningHeader.h"
#include "msg/SipContactHeader.h"
#include "msg/SipContentTypeHeader.h"
#include "msg/SipCSeqHeader.h"
#include "msg/SipHeaderBase.h"
#include "msg/SipHeaderList.h"
#include "msg/SipEventHeader.h"
#include "msg/SipReferSubHeader.h"
#include "msg/SipRAcKHeader.h"
#include "msg/SipRSeqHeader.h"
#include "msg/SipInfoBase.h"
#include "msg/SipDateHeader.h"
#include "msg/SipIdentityHeader.h"
#include "msg/SipRejectContactHeader.h"
#include "msg/SipResourcePriorityHeader.h"
#include "msg/SipAcceptResourcePriorityHeader.h"
#include "msg/SipAuthInfoHeader.h"
#include "msg/SipPVisitedNetworkIdHeader.h"
#include "msg/SipTriggerConsentHeader.h"
#include "msg/SipPChargingFunctionAddressesHeader.h"
#include "msg/SipPChargingVectorHeader.h"

#include "msg/SipFeatureCapsHeader.h"
#include "msg/SipGeolocationErrorHeader.h"
#include "msg/SipGeolocationRoutingHeader.h"
#include "msg/SipPAssertedServiceHeader.h"
#include "msg/SipPolicyContactHeader.h"
#include "msg/SipPPreferredServiceHeader.h"
#include "msg/SipUnknownHeader.h"

#include "msg/SipMsgBody.h"
#include "msg/SipRequestLine.h"
#include "msg/SipStatusLine.h"
#include "SipConfiguration.h"

/****************************************************************************
  Class Declaration Starts
 *****************************************************************************/
class SipHeaders
{
    //Request headers

    SipHeaderBase* m_HeaderArray[SipHeaderBase::TYPE_END+SIP_ONE];

    public:
    SipHeaders();
    virtual ~SipHeaders();
    SIP_BOOL CopyHdrs(SipHeaders *pobjHdrs);
    SIP_BOOL EncodeHdrs
        (
         SIP_CHAR     **ppucCurrPos,
         SIP_UINT32     uiMsgOptions
        );

    SIP_BOOL DecodeHdrs
        (
         SIP_CHAR    *pucStartPt,
         SIP_UINT32     uiDecLen,
         SIP_CHAR     **pucHdrName,
         SIP_CHAR     **pucHdrBody
        );

    SipHeaderBase* getHdrObj(SIP_INT32 eHdrType);

    SipHeaderBase* getHdrObj
        (
         SIP_INT32 eIndex,
         SIP_UINT16 usPos
        );
    SipHeaderBase* getListHdrObj(SIP_INT32 eIndex);

    SipHeaderBase* getNewHdrObj
        (
         SIP_INT32    eHdrType
        );

    SIP_BOOL SetHdr
        (
         SipHeaderBase* pstObjHdr
        ); //RTBD
     SIP_BOOL  AppendHdr
        (
         SipHeaderBase* pobjHdr
        );
     SIP_BOOL InsertHdr
        (
         SipHeaderBase* pobjHdr,
         SIP_UINT32 nIndex
        );
    SIP_VOID OverWriteHdrObj(IN SipHeaders* pobjSrcHdrs, IN SIP_BOOL bIgnoreUnknownHeader);
    SIP_BOOL RemoveHdr(SIP_INT32 eHdrType);
    static SipHeaderBase* CreateCoreHdrObj
        (
        SIP_INT32     eHdrType
        );
    static SipHeaderBase* CloneHdrObj
        (
         SipHeaderBase* pobjHdr
        );
    static SIP_BOOL IsListHdr(SIP_INT32 eHdrType);
    private:
    SIP_BOOL EncodeMandatoryHdrs
        (
         SIP_CHAR     **ppucCurrPos,
         SIP_UINT32     uiMsgOptions
        );

    SIP_BOOL EncodeContentHdrs
        (
         SIP_CHAR     **ppucCurrPos,
         SIP_UINT32     uiMsgOptions
        );


};

/******************************************************************************
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
 );

/******************************************************************************
 * Function name      : sipEncodeHdrName
 *
 * Description        :
 *
 * Preconditions      :
 *
 * Side Effects          : none
 *****************************************************************************/
SIP_BOOL sipEncodeShortHdrName
(
 SIP_INT32        eHdrType,
 SIP_CHAR         **ppucMsgBuffCurrPos
 );

#endif //__SIP_HEADERS_H__
