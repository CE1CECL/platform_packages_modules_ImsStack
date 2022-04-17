/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename          : SipStackManager.cpp
 * Purpose           : This file define all the APIs for stack manager for accessing txn/transport layer
 * Platform          : Windows OR Android
 * Author(s)         :
 * E-mail id.        : name@
 * Creation date        : 29-Jul-2010
 *
 * Edit History             Modification                         Description(s)
 *
 * Date                Name            Version        Bug-ID        Description
 * ----------        ----------        -------        ------        -------------
 * 29 Jul 10        syed            0.0a                    Initial creation
 * 26 Apr 11        birender        --                        Control flow for txn and transport is modified
 *****************************************************************************/

/*****************************************************************************
  Header Inclusions
 *****************************************************************************/
#include "sip_pf_datatypes.h"
#include "platform/sip_pf_memory.h"
#include "msg/sip_comdef.h"
#include "txn/SipTxn.h"
#include "transport/SipTransportInfo.h"
#include "SipStackManager.h"
#include "txn/SipTxnHandler.h"
#include "transport/SipTransportHandler.h"
#include "sip_debug.h"
#include "SipUtil.h"
#include "SIPMessageBuffer.h"

//#include "ServiceTrace.h"
//__IMS_TRACE_TAG_SIP__;

/****************************************************************************
  Macro Definitions
 *****************************************************************************/
extern SIP_VOID sip_cbk_preProcessMessageSentByStack(IN SIP_VOID* pvSipMsg,
        IN ISipUserData* pUserData);
extern SIP_VOID sip_cbk_postProcessMessageSentByStack(IN SIP_VOID* pvSipMsg,
        IN SIP_CHAR* pcBuffer, IN SIP_UINT32 uiBufferLen, IN ISipUserData* pUserData);

/****************************************************************************
  Class Member Function Implementations
 *****************************************************************************/
static SipStackManager* gpobjStackMngr =  SIP_NULL;

/******************************************************************************
 * Function name        : SipStackManager
 * Description          : Default Constructor ,
 * Return type          : None
 *
 * Argument             : None
 * Preconditions/
 * Side Effects            : None
 *****************************************************************************/
SipStackManager::SipStackManager()
{
    SipTrace_Construct();    /* Singleton Tracel Obj, maintained by Trace module*/
    SipUtil_Construct();    /* Singleton Util Obj, maintained by Util module*/
}

/******************************************************************************
 * Function name        : SipStackManager
 * Description          : Destructor ,
 * Return type          : None
 *
 * Argument             : None
 * Preconditions/
 * Side Effects            : None
 *****************************************************************************/
SipStackManager::~SipStackManager()
{
    SipUtil_Destruct();
    SipTrace_Destruct();
}
/******************************************************************************
 * Function name        : SipStackManager
 * Description          : Create single instance of stack manager
 * Return type          : None
 *
 * Argument             : None
 * Preconditions/
 * Side Effects            : None
 *****************************************************************************/
SipStackManager* SipStackManager::GetInstance()
{
    if (gpobjStackMngr == SIP_NULL)
    {
        gpobjStackMngr = new SipStackManager();
    }
    return gpobjStackMngr;

}

/******************************************************************************
 * Function name        : SipStackManager
 * Description          : delete instance of stack manager
 * Return type          : None
 *
 * Argument             : None
 * Preconditions/
 * Side Effects            : None
 *****************************************************************************/
void SipStackManager::Destruct()
{
    if (gpobjStackMngr != SIP_NULL)
    {
        delete gpobjStackMngr;
    }
    return;

}

/******************************************************************************
 * Function name        : GetSipUtil
 * Description          : Get the Instance of SipUtil ,
 * Return type          : SipUtil
 *
 * Argument             : None
 * Preconditions/
 * Side Effects            : None
 *****************************************************************************/
SipUtil* SipStackManager::GetSipUtil()
{
    return SipUtil_GetInstance();
}

/******************************************************************************
 * Function name        : GetTrace
 * Description          : Get the Instance of SipTrace ,
 * Return type          : SipTrace
 *
 * Argument             : None
 * Preconditions/
 * Side Effects            : None
 *****************************************************************************/
SipTrace* SipStackManager::GetTrace()
{
    return SipTrace_GetInstance();
}

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
 *    user with this parameter. For response message it contains NULL
 * @param[out]     pusError         : Appropriate error code as defined in SipEn_ErrorTypes in
 *    case of failure
 *
 * @return Status indicator
 * @retval SIP_TRUE If successful
 * @retval SIP_FALSE If function processing failed.
 * @retval Appropriate error code as defined in SipEn_ErrorTypes in case of failure
 *
 * Re-transmission of 2XX for INVITE is handled by stack user. @XX and successful ACK for INVITE
 * is not handled by the transaction and is directly passed to transport layer for encoding and
 * sending to network.
 *
 */
SIP_BOOL SipStackManager::SendMsg
(
 SipMessage* pobjSipMsg, IN_OUT
 SipTransportParameter* pobjTranspParam, IN
 ISipUserData *pobjUserData, IN
 SIP_CHAR* pcSipBuffer, IN
 SIP_UINT32 uiSipBufferLen, IN
 SipTxnKey** ppobjTxnKey, OUT
 SIP_UINT16* pusError
 )
{
    /* Input parameter validation */
    if ((pobjSipMsg == SIP_NULL) || (pobjTranspParam == SIP_NULL)
            || (ppobjTxnKey == SIP_NULL) || (pusError == SIP_NULL))
    {
        return SIP_FALSE;
    }

    *ppobjTxnKey = SIP_NULL;

    /* Transaction Processing
NOTE: In case of 2XX and successful ACK for INVITE, the Txn module does
not perform any processing, it simply return success. Also pobjTxnKey will be NULL
     */
    SipTxnInfo    objTxnInfo;
    SipTxnHandler objTxnHandler;
    SipTxnKey     *pobjTxnKey = SIP_NULL;

    if (objTxnHandler.OnSendTxn(pobjSipMsg, pobjTranspParam, pobjUserData,
            &pobjTxnKey, &objTxnInfo, pusError) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN, "SipTxnHandler::OnSendTxn:Failed",0,0);
        return SIP_FALSE;
    }

    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,"SipStackManager::SendMsg, OnSendTxn Processed",0,0);

    /* Transport layer processing
       pobjTranspInfo contains the encoded SIP message and remote transport information
     */
    SipTransportHandler  objTranspHandler;
    SipTransportInfo     *pobjTranspInfo = SIP_NULL;

    if (objTranspHandler.OnSendTransp(pobjSipMsg, pobjTranspParam, pcSipBuffer,
            uiSipBufferLen, &pobjTranspInfo, pusError) == SIP_FALSE)
    {
        /* Inform txn about transport error, this terminates Txn */
        objTxnHandler.OnSendTranspError(pobjTxnKey);

        if (objTxnInfo.bTxnCreated == SIP_TRUE)
        {
            // It will be destroyed by the caller.
            objTxnInfo.m_pobjUserData->SetUserData(SIP_NULL);
        }

        delete pobjTxnKey;

        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,"OnSendTransp: Failed",0,0);
        return SIP_FALSE;
    }

    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,"SipStackManager::SendMsg, OnSendTransp Processed",0,0);

    /* NOTE: All param passed in callback are read only parameters and callback
       implementor shall not deleted any parameters*/
    if (SendToNetwork(pobjTranspInfo, pobjUserData) == SIP_FALSE)
    {
        /* BSP_TODO: do we need to inform Txn layer for error
           objTxnHandler.OnSendTranspError */

        if (objTxnInfo.bTxnCreated == SIP_TRUE)
        {
            // It will be destroyed by the caller.
            objTxnInfo.m_pobjUserData->SetUserData(SIP_NULL);
        }

        delete pobjTranspInfo;
        delete pobjTxnKey;

        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN, "SendToNetwork: Failed",0,0);
        return SIP_FALSE;
    }

    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,"SipStackManager::SendMsg, SendToNetwork Processed",0,0);

    if (objTxnInfo.bTxnTerminated == SIP_TRUE)
    {
        pobjUserData->SetUserData(objTxnInfo.m_pobjUserData->GetUserData());
        pobjUserData->SetDeleteFlag(SIP_TRUE);
        objTxnInfo.m_pobjUserData->SetUserData(SIP_NULL);

        /* Notifies to Transaction User using registered listener
           Delete Txn entry from DB and delete the instance*/
        objTxnHandler.DeleteTxn(pobjTxnKey);

        delete pobjTxnKey;
        delete pobjTranspInfo;

        SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                "SendMsg: TxnTerminated",SIP_ZERO,SIP_ZERO);
    }
    else if (pobjTxnKey != SIP_NULL)
    {
        SIP_BOOL bStatus = objTxnHandler.UpdateTxnDetails(pobjTxnKey, pobjTranspInfo, pusError);

        if (bStatus == SIP_FALSE)
        {
            delete pobjTranspInfo;
        }

        //if ACK no need to maintain transaction
        if ((bStatus == SIP_FALSE) && (pobjSipMsg->GetMethodType() != SipMessage::METHOD_ACK))
        {
            if (objTxnInfo.bTxnCreated == SIP_TRUE)
            {
                // It will be destroyed by the caller.
                objTxnInfo.m_pobjUserData->SetUserData(SIP_NULL);
            }

            delete pobjTxnKey;

            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "UpdateTxnDetails: Failed",SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }
        else if (objTxnInfo.bTxnCreated == SIP_TRUE)
        {
            /* return Key of newly created txn */
            *ppobjTxnKey = pobjTxnKey;
        }
        else
        {
            delete pobjTxnKey;
        }
    }

    // If it's a new request, then the user data is managed by the transaction object.
    if (objTxnInfo.bTxnCreated == SIP_TRUE)
    {
        pobjUserData->SetUserData(SIP_NULL);
    }

    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
            "SipStackManager::SendMsg Processed successfully",0,0);

    return SIP_TRUE;
}

/*!
 * @brief This API is called by stack user when SIP message is received and successfully parsed.
 *    This function handle transport, transaction and return transport status.
 *
 * @param[in] pobjSipMsg        : SIP message object
 * @param[in] pobjTranspParam   : Transport information from where SIP message is received
 * @param[out] peTranspStatus   : Transport status based on message validity, type and txn state
 * @param[out] ppobjTxnKey    : For new transaction it return txn key else NULL
 * @param[in] pobjUserData    : User data as passed by the user, store this data for new txn,
 * for existing txn, data from txn obj is used.
 * @param[out]     pusError        : Appropriate error code as defined in SipEn_ErrorTypes in case
 * of failure
 *
 * @return Status indicator
 * @retval SIP_TRUE If successful
 * @retval SIP_FALSE If function processing failed.
 * @retval Appropriate error code as defined in SipEn_ErrorTypes in case of failure
 *
 * 2XX and successful ACK for which INVITE transaction is not existing is valid message and shall be
 * handled by stack user. These messages are not handled by the transaction and is directly passed to user.
 * This function check the validity of incoming message and identify re-transmission, stray message.
 * Re-transmission is handled by this function and any stray and unexpected messages are ignored and
 * and function return failure in such cases.
 * Caller shall take action if function return Transport Status as SipTxn::STATUS_NEW_REQ_RECVD or
 * SipTxn::STATUS_VALID_MESSAGE. For all other status caller no need to take any action.
 *
 */
SIP_BOOL SipStackManager::OnRecvMessage
(
 IN    SipMessage                *pobjSipMsg,
 IN    SipTransportParameter                *pobjTranspParam,
 IN    ISipUserData                *pobjUserData,
 OUT    SIP_INT32            *peTxnStatus,
 OUT    SipTxnKey                    **ppobjTxnKey,
 OUT    SIP_UINT16                *pusError
 )
{
    /* Input parameter validation */
    if ((pobjSipMsg == SIP_NULL) || (pobjTranspParam == SIP_NULL)
            || (peTxnStatus == SIP_NULL) || (ppobjTxnKey == SIP_NULL)
            || (pusError == SIP_NULL))
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,"OnRecvMessage, invalid params",0,0);
        return SIP_FALSE;
    }

    *peTxnStatus = SipTxn::STATUS_INVALID;
    *ppobjTxnKey = SIP_NULL;

    /* Transport layer processing
       1. Validate incoming SIP Message
       2. Match transaction
       3. Return Transport status based on req/resp type and Method and validity of Txn state
NOTE: for valid re-transmitted msg received, pobjTranspInfo contains the massege that needs to be
send to network
     */
    SipTransportHandler    objTranspHandler;
    SipTxnKey          *pobjTxnKey = SIP_NULL;
    SIP_BOOL           bTxnExist = SIP_FALSE;

    if (objTranspHandler.OnRecvTransp(pobjSipMsg, pobjTranspParam,
            peTxnStatus, &bTxnExist, &pobjTxnKey, pusError) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,"OnRecvMessage returned failure",0,0);
        return SIP_FALSE;
    }

    /*
       CASE-1 :: Ignore request, ignore response and stray response
       In case of ignore req, ignore resp and stray resp --> simply return
       stack user can simply ignore these cases
     */
    switch (*peTxnStatus)
    {
        case SipTxn::STATUS_IGNORE_REQ:
        case SipTxn::STATUS_IGNORE_RESP:
        case SipTxn::STATUS_STRAY_RESP:
            {
                delete pobjTxnKey;
                SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                        "OnRecvTransp, Drop Message, TxnStatus[%d]",*peTxnStatus,0);
                return SIP_TRUE;
            }
        case SipTxn::STATUS_INVALID_MESSAGE:
        case SipTxn::STATUS_INVALID:
            {
                delete pobjTxnKey;
                SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,"OnRecvTransp, TxnStatus-->Invalid",0,0);
                return SIP_FALSE;
            }
        default:
            {
                SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,"OnRecvTransp, TxnStatus[%d]",*peTxnStatus,0);
                break;
            }
    }

    /*
CASE:2: Handling ACK for INVITE (txn not existing)
Successful ACK Request for INVITE is received and No Txn exists,
stack user must process this request and can decide whether to ignore or not
     */
    if ((*peTxnStatus == SipTxn::STATUS_NEW_REQ_RECVD) &&
            (pobjSipMsg->GetMethodType() == SipMessage::METHOD_ACK))
    {
        if (objTranspHandler.IsInviteTxnPresentForAckTxn(pobjTxnKey) != SIP_TRUE)
        {
            *peTxnStatus = SipTxn::STATUS_RETRANSMISSION;
        }

        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN, "OnRecvMessage :: ACK - %s",
                (*peTxnStatus == SipTxn::STATUS_NEW_REQ_RECVD) ?
                        "new request" : "retransmission or stray", 0);

        pobjTxnKey->SetTxnType(SipTxn::INV_SER_TXN);
        pobjTxnKey->SetRespCode(200);
        *ppobjTxnKey = pobjTxnKey;
        return SIP_TRUE;
    }

    /*
       CASE-3 :: Handling 2xx resp for INVITE (Txn not existing)
       2xx Response for INVITE is received and No Txn exists. This might be re-transmitted 2xx.
       stack user must process this request and can decide whether to ignore or not, in case user
       process this response, user must re-transmit successful ACK
     */
    if ((bTxnExist == SIP_FALSE) &&
            (pobjSipMsg->GetMsgType() == SipMessage::RESP_TYPE))
    {
        if (pobjSipMsg->GetMethodType() == SipMessage::METHOD_INVITE)
        {
            SIP_UINT16 usStatusCode = pobjSipMsg->GetStatusCode();

            if (SIP_SUCCESSFUL_RESP(usStatusCode))
            {
                *peTxnStatus = SipTxn::STATUS_2XX_STRAY_RESP;

                delete pobjTxnKey;

                SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                        "OnRecvMessage: INVITE 2xx received (No txn exist)", 0, 0);
                return SIP_TRUE;
            }
        }
    }

    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN, "OnRecvMessage: OnRecvTxn processing...",0,0);

    /* Invoke Txn layer handling
       pobjSipMsg2Send --> contains either 100 Trying when new INVITE request is received or
       failure ACK when failure response for INVITE is received
     */

    SipTxnInfo    objTxnInfo;
    SipTxnHandler objTxnHandler;

    if (objTxnHandler.OnRecvTxn(pobjSipMsg, pobjTxnKey, pobjUserData,
            &objTxnInfo, pusError) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN, "OnRecvMessage: OnRecvTxn Failed",0,0);

        delete pobjTxnKey;

        *peTxnStatus = SipTxn::STATUS_INVALID_MESSAGE;

        return SIP_FALSE;
    }

    *peTxnStatus = objTxnInfo.eTxnStatus;

    // If it's a new request, then the user data is managed by the transaction object.
    if (objTxnInfo.bTxnCreated == SIP_TRUE)
    {
        pobjUserData->SetUserData(SIP_NULL);
    }
    else
    {
        if (objTxnInfo.m_pobjUserData != SIP_NULL)
        {
            pobjUserData->SetUserData(objTxnInfo.m_pobjUserData->GetUserData());
        }
    }

    /*
       CASE-1 :: Ignore request & ignore response case
       In case of ignore req, ignore resp --> simply return
       stack user can simply ignore these cases
     */
    SIP_DEBUG_WARNING(ESIPTRACE_MODTXN, "OnRecvTxn :: txnStatus=%d", objTxnInfo.eTxnStatus, 0);

    switch (objTxnInfo.eTxnStatus)
    {
        case SipTxn::STATUS_IGNORE_REQ:
        case SipTxn::STATUS_IGNORE_RESP:
        case SipTxn::STATUS_STRAY_RESP:
            {
                delete pobjTxnKey;
                return SIP_TRUE;
            }
        case SipTxn::STATUS_INVALID_MESSAGE:
        case SipTxn::STATUS_INVALID:
            {
                delete pobjTxnKey;
                return SIP_FALSE;
            }
        case SipTxn::STATUS_STRAY_PRACK:
            {
                delete pobjTxnKey;
                return SIP_TRUE;
            }
        case SipTxn::STATUS_NEW_REQ_RECVD:
        case SipTxn::STATUS_RETRANSMISSION:
        case SipTxn::STATUS_VALID_MESSAGE:
            {
                break;
            }
        default:
            {
                delete pobjTxnKey;
                return SIP_FALSE;
            }
    }

    /*
       CASE-2 :: Handling of Re-Transmitted request/response received in valid txn states
       INV Req--> Re-Transmitted INVITE request recv in proceeding state and completed state,
       re-transmit the last response.
       Non-INV Req --> Re-Transmitted Non-INVITE request recv in proceeding state and completed state,
       re-transmit the last response.
       INV Failure Resp --> Failure response recv is completed state, re-transmit the failure ACK
     */
    if (objTxnInfo.eTxnStatus == SipTxn::STATUS_RETRANSMISSION)
    {
        if (SendToNetwork(objTxnInfo.m_pobjTranspInfo, objTxnInfo.m_pobjUserData) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "SendToNetwork Failed in retransmitted case",0,0);
        }

        delete pobjTxnKey;

        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,"OnRecvMessage: Re-Transmitted Msg received",0,0);
        return SIP_TRUE;
    }

    /* Check if any message needs to be send to network.
       Case of Sending of Failure ACK when failure response is received for INVITE
       Case of Sending 100 Trying response, on receive of INVITE
     */
    if ((objTxnInfo.m_pobjSendSipMsg != SIP_NULL) &&
            (objTxnInfo.m_pobjUserData != SIP_NULL))
    {
        SipTransportInfo *pobjTranspInfo = SIP_NULL;
        RCPtr<SIPMessageBuffer> pMessageBuffer = SIPMessageBuffer::GetInstance();
        SIP_UINT32 uiSipBufferLen = pMessageBuffer->GetLength();
        SIP_CHAR* pcSipBuffer = reinterpret_cast<SIP_CHAR*>(pMessageBuffer->GetBuffer());

        SipPf_Memset(pcSipBuffer, 0x00, uiSipBufferLen);

        if (objTxnInfo.m_pobjSendSipMsg->EncodeMsg(&pcSipBuffer,
                &uiSipBufferLen, pobjUserData->GetMsgOptions()) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN, "OnRecvMessage:EncodeMsg Fail",
                    SIP_ZERO, SIP_ZERO);
            objTxnHandler.OnSendTranspError(pobjTxnKey);
            delete pobjTxnKey;
            return SIP_TRUE;
        }

        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,"OnRecvMessage: Invoking OnSendTransp... ",0,0);

        if (objTranspHandler.OnSendTransp(objTxnInfo.m_pobjSendSipMsg, pobjTranspParam,
                pcSipBuffer, uiSipBufferLen, &pobjTranspInfo, pusError) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,"OnRecvMessage: OnSendTransp fail",0,0);

            /* Inform txn about transport error, this terminates Txn */
            objTxnHandler.OnSendTranspError( pobjTxnKey );

            delete pobjTxnKey;
            return SIP_TRUE;
        }

        // Notify SIP message sent by stack to the application for a proper handling
        sip_cbk_preProcessMessageSentByStack(
                reinterpret_cast<SIP_VOID*>(objTxnInfo.m_pobjSendSipMsg),
                objTxnInfo.m_pobjUserData);

        /* BSP_TODO:
           In case of 100 Trying, Txn Obj may not have user date, better to use user passed data
           For failure ACK, better to user Txn stored data */
        if (SendToNetwork(pobjTranspInfo, objTxnInfo.m_pobjUserData) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "OnRecvMessage: SendToNetwork Failed",0,0);
            delete pobjTxnKey;
            delete pobjTranspInfo;
            return SIP_TRUE;
        }

        SipTransportBuffer* pTransBuffer = pobjTranspInfo->GetTranspSipBuffer();

        sip_cbk_postProcessMessageSentByStack(
                reinterpret_cast<SIP_VOID*>(objTxnInfo.m_pobjSendSipMsg),
                (pTransBuffer != SIP_NULL) ? pTransBuffer->GetSipBuffer() : SIP_NULL,
                (pTransBuffer != SIP_NULL) ? pTransBuffer->GetSipBufferLen() : 0,
                objTxnInfo.m_pobjUserData);

        /*Update Txn details for last message send to network */
        /* BSP_TODO: it's overwriting the existing transpInfo... is it correct way to do
           why can't only update the necessary details and maintaining old key data for security */
        if (objTxnHandler.UpdateTxnDetails(pobjTxnKey, pobjTranspInfo, pusError) == SIP_FALSE)
        {
            /* BSP_TODO: oops what to doooooooooo???*/
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "OnRecvMessage: UpdateTxnDetails Failed",0,0);

        }
    }

    /* INV Client Txn --> For INV 2xx case, txn is terminated
       INV Serv Txn --> For TCP, on ACK recv Txn is terminated */
    if (objTxnInfo.bTxnTerminated == SIP_TRUE)
    {
        if (objTxnInfo.m_pobjUserData != SIP_NULL)
        {
            objTxnInfo.m_pobjUserData->SetUserData(SIP_NULL);
        }

        pobjUserData->SetDeleteFlag(SIP_TRUE);

        /* Notifies to Transaction User using registered listener
           Delete Txn entry from DB and delete the instance*/
        objTxnHandler.DeleteTxn(pobjTxnKey);

        delete pobjTxnKey;

        SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                "OnRecvTxn: Txn Terminated",SIP_ZERO,SIP_ZERO);
    }
    else if ((objTxnInfo.bTxnCreated == SIP_TRUE)
             || (pobjSipMsg->GetMethodType() == SipMessage::METHOD_ACK))
    {
        /* return Key of newly created txn */
        *ppobjTxnKey = pobjTxnKey;
    }
    else
    {
        pobjTxnKey->SipDelete();
        pobjTxnKey = SIP_NULL;
    }

    return SIP_TRUE;
}

/*!
 * @brief This API is called by stack user on any transport error
 *
 * @param[in]     eTranspError  : Type of transport error occurred
 * @param[in]     pobjTxnKey    : transaction for which transport error has occurred
 * @param[out]    pusError      : Appropriate error code as defined in SipEn_ErrorTypes in case
 * of failure
 *
 * @return Status indicator
 * @retval SIP_TRUE If successful
 * @retval SIP_FALSE If function processing failed.
 * @retval Appropriate error code as defined in SipEn_ErrorTypes in case of failure
 *
 */
SIP_BOOL SipStackManager:: OnRecvTanspError
(
 IN    SIP_INT32    eTranspError,
 IN    SipTxnKey            *pobjTxnKey,
 OUT    SIP_UINT16        *pusError
 )
{
    SipTransportHandler    objTranspHandler;
    SIP_INT32    eTxnStatus = SipTxn::STATUS_INVALID;
    SipTransportInfo        *pobjTranspInfo = SIP_NULL;
    ISipUserData        objUserData;

    /* If error is occurred due to Msg Constraint , switch back to UDP and transport */

    if (objTranspHandler.OnRecvTanspError(eTranspError, pobjTxnKey,
            &eTxnStatus, &pobjTranspInfo, &objUserData, pusError) == SIP_FALSE)
    {
        return SIP_FALSE;
    }

    /* Resend to Network due to message size constraint switch */
    if (eTxnStatus == SipTxn::STATUS_RETRANSMISSION)
    {
        if (SendToNetwork(pobjTranspInfo, &objUserData) == SIP_TRUE)
        {
            return SIP_TRUE;
        }
    }

    /* Invoke Transaction layer for transport error */
    SipTxnHandler       objTxnHandler;
    return objTxnHandler.OnRecvTranspError((SIP_INT32)(*pusError), pobjTxnKey,pusError);
}

/*!
 * @brief This API is called by stack user in order to terminated the existing transaction, this
 *    fxn is used when transaction key is available with the user
 *
 * @param[in]     pobjTxnKey    : Key for the transaction that needs to be terminated
 * @param[out]    pusError        : Appropriate error code as defined in SipEn_ErrorTypes in
 * case of failure
 * @return Status indicator
 * @retval SIP_TRUE If successful
 * @retval SIP_FALSE If function processing failed.
 * @retval Appropriate error code as defined in SipEn_ErrorTypes in case of failure
 *
 */
SIP_BOOL SipStackManager::TerminateTxn
(
 SipTxnKey        *pobjTxnKey
 )
{
    SipTxnHandler    objTxnHandler;

    return objTxnHandler.TerminateTxn(pobjTxnKey);
}

/*!
 * @brief This API is called by stack user in order to terminated the existing transaction, this
 * fxn is used when transaction key is not available with the user
 *
 * @param[in]     pobjTxnKey    : Key for the transaction that needs to be terminated
 * @param[out]     pusError       : Appropriate error code as defined in SipEn_ErrorTypes in case
 * of failure
 *
 * @return Status indicator
 * @retval SIP_TRUE If successful
 * @retval SIP_FALSE If function processing failed.
 * @retval Appropriate error code as defined in SipEn_ErrorTypes in case of failure
 *
 */
SIP_BOOL SipStackManager::TerminateTxn
(
 IN    SipMessage    *pobjSipMsg
 )
{
    SIP_UINT16    nError;

    SipTxnKey *pobjTxnKey = new SipTxnKey(pobjSipMsg, &nError);
    if (pobjTxnKey == SIP_NULL)
    {
        return SIP_FALSE;
    }

    SipTxnHandler    objTxnHandler;
    SIP_BOOL bStatus = objTxnHandler.TerminateTxn(pobjTxnKey);

    delete pobjTxnKey;
    return bStatus;
}

/*!
 * @brief This API is invoked by the stack while sending SIP message to the network
 *
 * @param[in]     pobjTranspInfo : Contains Raw SIP message with transport details
 * @param[in]     pobjUserData : User Data as provide by the user
 * @param[in]     pobjTxnKey   : Key for the transaction to which the SIP message belongs
 * @param[out]     pusError      : Appropriate error code as defined in SipEn_ErrorTypes in case
 * of failure
 *
 * @return Status indicator
 * @retval SIP_TRUE If successful
 * @retval SIP_FALSE If function processing failed.
 * @retval Appropriate error code as defined in SipEn_ErrorTypes in case of failure
 *
 */
PRIVATE SIP_BOOL SipStackManager::SendToNetwork
(
 IN    SipTransportInfo    *pobjTranspInfo,
 IN    ISipUserData    *pobjUserData
 )
{
    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,"SendToNetwork: Processing..",0,0);

    if (pobjTranspInfo == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SendToNetwork Failed transpinfo is NULL",SIP_ZERO,SIP_ZERO);

        return SIP_FALSE;
    }

    /* Call Sent to Network Callback */
    SipUtil *pobjUtil = SipUtil_GetInstance();
    ISipNetworkUtil *pobjNetworkUtil = pobjUtil->GetNetwork();
    SipTransportBuffer *pobjTransSipBuffer = pobjTranspInfo->GetTranspSipBuffer();
    SipTransportParameter *pobjActualDestParam = pobjTranspInfo->GetMsgSentTranspParam();

    if (pobjNetworkUtil->SendToNetwork(pobjTransSipBuffer, pobjActualDestParam, pobjUserData)
            == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "pobjNetworkUtil->SendToNetwork utility fxn failed",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    return SIP_TRUE;
}
