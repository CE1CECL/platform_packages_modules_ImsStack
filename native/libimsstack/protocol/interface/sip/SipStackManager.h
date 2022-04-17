/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************
 * Filename          : SipStackManager.h
 * Purpose           : Stack Manager which is called by Stack User
 * Platform          : Windows OR Android
 * Author(s)         : Syed Malgimani
 * E-mail id.        : syed.malgimani@
 * Creation date     : July. 28,2010
 *
 * Edit History             Modification                         Description(s)
 *
 * Date                Name            Version        Bug-ID        Description
 * ----------        ----------        -------        ------        -------------
 * Month. Date,10        Name                 0.0a            Initial creation

 *****************************************************************************/

#ifndef __SIP_STACK_MANAGER_H__
#define __SIP_STACK_MANAGER_H__


/*****************************************************************************
  Header Files
 *****************************************************************************/
#include "sip_pf_datatypes.h"
#include "msg/sip_comdef.h"
#include "msg/SipMessage.h"
#include "SipTrace.h"
#include "txn/SipTxnKey.h"
#include "transport/SipTransportInfo.h"

class SipUtil;

/****************************************************************************
SipTransportParameter: Class Declaration Starts
 *****************************************************************************/
/**
  This is the entry point for the SIP stack. This can handle transaction and transport.
  The message to be sent is encoded and sent. It processes the received responses. It starts suitable
  transaction timers.
 */
class SipStackManager
{
    SipStackManager();
    public:

    virtual ~SipStackManager();
    SipUtil* GetSipUtil();
    SipTrace* GetTrace();
    static SipStackManager* GetInstance();
    static void Destruct();

    /*!
     * @brief This API is called by stack user to send SIP message(request/response) to other end.
     *    This function handle transaction, transport and issue callback for sending sip message to network
     *
     * @param[in,out] pobjSipMsg        : SIP message object used for forming raw SIP message
     * @param[in]     pobjTranspParam   : For Request message it contains transport information where
     *    request to be send. For response, remote transport information is fetched from the Via header
     * @param[in]     pobjUserData        : For request message, it contains user data which will be
     *    returned in the SendToNetwork call back.
     * @param[out]     ppobjTxnKey        : For request message, new key will be formed and return to
     *    user in this parameter. for response message, ket which is available in the Txn object is returned.
     *
     * @return Status indicator
     * @retval SIP_TRUE If successful
     * @retval SIP_FALSE If function processing failed.
     * @retval Appropriate error code as defined in SipEn_ErrorTypes in case of failure
     *
     * Re-transmission of 2XX for INVITE is handled by stack user. 2XX and successful ACK for INVITE is
     * not handled by the transaction layer and is directly passed to transport layer for encoding and
     * sending to network.
     */
    SIP_BOOL SendMsg
        (
         SipMessage* pobjSipMsg, IN_OUT
         SipTransportParameter* pobjTranspParam, IN
         ISipUserData* pobjUserData, IN
         SIP_CHAR* pcSipBuffer, IN
         SIP_UINT32 uiSipBufferLen, IN
         SipTxnKey** ppobjTxnKey, OUT
         SIP_UINT16* pusError
        );

    /**
      @param pobjSipMsg        : Received SIP Msg (Req/Resp)
      @param pobjTranspParam    : source address from where data is received
      and response to be sent.
      @param peTranspStatus    : status of handling of received msg.
     */
    SIP_BOOL OnRecvMessage
        (
         SipMessage        *pobjSipMsg,        IN_OUT
         SipTransportParameter        *pobjTranspParam,    IN
         ISipUserData        *pobjUserData,        IN
         SIP_INT32    *peTxnStatus,        OUT
         SipTxnKey            **ppobjTxnKey,      OUT
         SIP_UINT16        *pusError
        );

    /**
      This function is used to convey transport errors to the stack. When application
      detects transport errors, it will inform the stack using this function.
      @param eTranspError        : Received transport Error
      @param pobjTxnKey        : key of Txn's, for which transport error occurred.
     */
    SIP_BOOL OnRecvTanspError
        (
         SIP_INT32    eTranspError,
         SipTxnKey            *pobjTxnKey,
         SIP_UINT16        *pusError
        );

    /**
      Any ongoing transaction can be terminated by the application using this function with
      the help of the transaction key
      @param pobjTxnKey        : key of Txn's for terminating.
     */
    SIP_BOOL TerminateTxn
        (
         SipTxnKey        *pobjTxnKey
        );

    /* Any ongoing transaction can be terminated by the application using this function with
       the help of the SIP Message

       @param pobjSipMsg        : Received SIP Msg (Req/Resp)
     */
    SIP_BOOL TerminateTxn
        (
         SipMessage    *pobjSipMsg
        );

    private:
    PRIVATE SIP_BOOL SendToNetwork
        (
         IN        SipTransportInfo    *pobjTranspInfo,
         IN        ISipUserData    *pobjUserData
        );

};

#endif //__SIP_STACK_MANAGER_H__
