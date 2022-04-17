/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************
 * Filename          : SipTransportInfo.hander.h
 * Purpose           : SIP Transport Functions : Ref: SipTransportInfo.handler.h
 * Platform          : Windows OR Android
 * Author(s)         : Syed Malgimani
 * E-mail id.        : syed.malgimani@
 * Creation date     : July. 26,2010
 *
 * Edit History             Modification                         Description(s)
 *
 * Date                Name            Version        Bug-ID        Description
 * ----------        ----------        -------        ------        -------------
 * Month. Date,10        Name                 0.0a            Initial creation

 *****************************************************************************/

#ifndef __SIP_TRANSPORT_HANDLER_H__
#define __SIP_TRANSPORT_HANDLER_H__

#include "txn/SipTxn.h"
#include "transport/SipTransportInfo.h"

/*****************************************************************************
  Enum Declaration
 *****************************************************************************/

/****************************************************************************
SipTransportHandler: Class Declaration starts
 *****************************************************************************/
class SipTransportHandler
{
    public:
        SipTransportHandler(){};
        virtual ~SipTransportHandler(){}
    private:
        /************************************************************
          Member Functions
         **************************************************************/
        SIP_BOOL UpdateViaSipMsg
            (
             SipMessage            *pobjSipMsg,
             SipTransportBuffer        *pobjSentBuffer,
             SIP_INT32    eChangeProto
            );

        PRIVATE SIP_BOOL GetTxnKeyFromSipMsg
            (
             IN    SipMessage    *pobjSipMsg,
             OUT    SipTxnKey        **ppobjTxnKey,
             OUT    SIP_UINT16    *pusError
            );

        PRIVATE SIP_BOOL GetTxnObjFromDb
            (
             IN    SipTxnKey     *pobjTxnKey,
             OUT    SipTxn        **ppobjTxn,
             OUT    SIP_BOOL     *pbTxnExist,
             OUT    SIP_UINT16    *pusError
            );

    public:

        /************************************************************
          Member Functions
         **************************************************************/

        SIP_BOOL OnSendTransp
            (
             IN  SipMessage* pobjSipMsg,
             IN  SipTransportParameter* pobjTranspParam,
             IN  SIP_CHAR* pcSipBuffer,
             IN  SIP_UINT32 uiSipBufferLen,
             OUT SipTransportInfo** ppobjTranspInfo,
             OUT SIP_UINT16* pusError
            );

        SIP_BOOL OnRecvTransp
            (
             IN     SipMessage        *pobjSipMsg,
             IN     SipTransportParameter        *pobjTranspParam,
             OUT SIP_INT32    *peTxnStatus,
             OUT SIP_BOOL         *pbTxnExist,
             OUT    SipTxnKey            **ppobjTxnKey,
             OUT    SIP_UINT16        *pusError
            );

        SIP_BOOL OnRecvTanspError
            (
             SIP_INT32    eTranspError,
             SipTxnKey            *pobjTxnKey,
             SIP_INT32    *peTxnStatus,
             SipTransportInfo        **ppobjTranspInfo,
             ISipUserData        *pobjUserData,
             SIP_UINT16        *pusError
            );

        SIP_BOOL IsInviteTxnPresentForAckTxn(IN SipTxnKey* pAckTxnKey);
};
/****************************************************************************
SipTransportHandler: Class Declaration End
 *****************************************************************************/

#endif //__SIP_TRANSPORT_HANDLER_H__
