/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************
 * Filename          : SipTransportInfo.hander.cpp
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
 * 26 Jul 10        syed            0.0a                    Initial creation
 * 26 Apr 11        birender        --                        Transport handling corrected

 *****************************************************************************/
#include "transport/SipTransportHandler.h"
#include "transport/SipTransportInfo.h"
#include "transport/SipTransportParameter.h"
#include "SipTrace.h"
#include "sip_debug.h"
#include "platform/sip_pf_string.h"
#include "sip_error.h"
#include "platform/sip_pf_memory.h"
#include "SipUtil.h"

/*****************************************************************************
  Macro
 *****************************************************************************/
#define SIP_TRANSP_TCP "TCP"
#define SIP_TRANSP_UDP "UDP"
#define SIP_TRANSP_TLS "TLS"

#define SIP_VIA_LINE_TCP "/TCP"
#define SIP_VIA_LINE_UDP "/UDP"
#define SIP_VIA_LINE_TLS "/TLS"

#define SIP_VIA_ENC_FORMAT_1    "\r\nVia:"
#define SIP_VIA_ENC_FORMAT_2    "\r\nv:"
/*****************************************************************************
  Extern Declaration
 *****************************************************************************/

/*****************************************************************************
  STATIC LOCAL VARUABLES.
 ******************************************************************************/

/****************************************************************************
  Member Function Implementations
 *****************************************************************************/

/*!
 * @brief This API encode SIP message and fill transport params
 *
 * @param[in,out] pobjSipMsg        : SIP message object used for forming raw SIP message
 * @param[in]     pobjTranspParam   : For Request message it contains transport information where
 *    request to be send. For response, remote transport information is fetched from the Via header
 * @param[out]     ppobjTranspInfo    : Contains Encoder SIP message and final transport details.
 * @param[out]     pusError        : Appropriate error code as defined in SipEn_ErrorTypes in case
 * of failure
 *
 * @return Status indicator
 * @retval SIP_TRUE If successful
 * @retval SIP_FALSE If function processing failed.
 * @retval Appropriate error code as defined in SipEn_ErrorTypes in case of failure
 *
 */
SIP_BOOL SipTransportHandler::OnSendTransp
(
 IN  SipMessage* pobjSipMsg,
 IN  SipTransportParameter* pobjTranspParam,
 IN  SIP_CHAR* pcSipBuffer,
 IN  SIP_UINT32 uiSipBufferLen,
 OUT SipTransportInfo** ppobjTranspInfo,
 OUT SIP_UINT16* pusError
 )
{
    (void)pusError;

    /* Input parameter validation */
    if ((pobjSipMsg == SIP_NULL) || (pobjTranspParam == SIP_NULL))
    {
        SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,"SendMsg invalid params",0,0);
        return SIP_FALSE;
    }

    SIP_CHAR* pcNewBuffer = new SIP_CHAR[uiSipBufferLen + 1];
    SipPf_Memcpy(pcNewBuffer, pcSipBuffer, uiSipBufferLen);
    pcNewBuffer[uiSipBufferLen] = '\0';

    SipTransportBuffer* pobjTranspBuffer = new SipTransportBuffer(pcNewBuffer, uiSipBufferLen);

    if (pobjTranspBuffer == SIP_NULL)
    {
        delete[] pcNewBuffer;
        return SIP_FALSE;
    }

    SIP_INT32 eMsgType = pobjSipMsg->GetMsgType();
    SIP_INT32 eMethod = pobjSipMsg->GetMethodType();

    /*No need to check Message Size Constraint for Response type and ACK which is sent directly */
    if (((SipMessage::REQ_TYPE == eMsgType) && (SipMessage::METHOD_ACK == eMethod)) ||
        (SipMessage::RESP_TYPE == eMsgType))
    {
        /* Send Directly to N/W */

        SIP_TRACE_NORMAL(ESIPTRACE_MODTRANSP,
                "OnSendTransp: Send Directly to NW",SIP_ZERO,SIP_ZERO);

        SipTransportInfo *pobjTranspInfo = new SipTransportInfo(pobjTranspParam, pobjTranspBuffer);

        if (pobjTranspInfo == SIP_NULL)
        {
            delete pobjTranspBuffer;
            return SIP_FALSE;
        }

        SipTransportParameter* pobjActualDestParam = new SipTransportParameter(pobjTranspParam);
        pobjTranspInfo->SetMsgSentTranspParam(pobjActualDestParam);

        /* store sip message for retransmit purpose. deleted when transinfo is deleted */
        /* storing can be done outside of this api call also. for failure ack and 100 response txn
        gives sip message */
        SipMessage* pStoredSipMsg = new SipMessage(*pobjSipMsg);
        pobjTranspInfo->SetSentSipMsg(pStoredSipMsg);

        *ppobjTranspInfo = pobjTranspInfo;

        /* No need to check the length */
        return SIP_TRUE;
    }

    /* Check for Message Exceed Constraints */
    SIP_BOOL bExceedMTU = SIP_FALSE;

    SipTransportParameter* pobjActualDestParam = new SipTransportParameter(pobjTranspParam);

    SipTransportInfo *pobjTranspInfo = new SipTransportInfo(pobjTranspParam, pobjTranspBuffer);

    if (pobjTranspInfo == SIP_NULL)
    {
        delete pobjActualDestParam;
        delete pobjTranspBuffer;
        return SIP_FALSE;
    }
    pobjTranspInfo->SetMsgSentTranspParam(pobjActualDestParam);
    pobjTranspInfo->SetExceedMTUFlag(bExceedMTU);

    // store sip message for retransmit purpose. deleted when transinfo is deleted
    SipMessage* pStoredSipMsg = new SipMessage(*pobjSipMsg);
    pobjTranspInfo->SetSentSipMsg(pStoredSipMsg);

    *ppobjTranspInfo = pobjTranspInfo;

    return SIP_TRUE;
}

/*!
 * @brief This API validates via header, txn mandatory headers and check the validiaty of the
 * received message as per the txn state
 *
 * @param[in,out] pobjSipMsg        : Parsed SIP message object of the received SIP RAW message
 * @param[in]    pobjTranspParam   : Transport details from where SIP message is received
 * @param[out]    peTranspStatus   : return the status of transport layer after processing.
 * Caller shall take necessary actions based on this status
 * @param[out]    ppobjTxnKey   :  New instance key of txn to which the message belongs
 * @param[out]     ppobjTranspInfo    : transport details of the existing transaction, obj of same
 * ref as in txn obj
 * @param[out]     ppobjUserData    : It contains user data as given by the user. this data is
 * retrieve from the txn obj for txn not existing it contains NULL.
 * NOTE: User data that is return is of same ref as in txn obj
 * @param[out]     pusError        : Appropriate error code as defined in SipEn_ErrorTypes in case
 * of failure
 *
 * @return Status indicator
 * @retval SIP_TRUE If successful
 * @retval SIP_FALSE If function processing failed.
 * @retval Appropriate error code as defined in SipEn_ErrorTypes in case of failure
 *
 */
SIP_BOOL SipTransportHandler::OnRecvTransp
(
 IN     SipMessage        *pobjSipMsg,
 IN     SipTransportParameter        *pobjTranspParam,
 OUT SIP_INT32      *peTxnStatus,
 OUT SIP_BOOL         *pbTxnExist,
 OUT    SipTxnKey            **ppobjTxnKey,
 OUT    SIP_UINT16        *pusError
 )
{
    (void) pobjTranspParam;

    /* RFC 3261: Section 18.2.1
       Fetch value of the "sent-by" parameter in the top Via header field
       from Request. If host portion differs, then add a "received"
       parameter to that Via header field value.Set parameter of source
       address to address from which the packet was received.
     */
    /* It's handled by the upper layer in the client side. */

    /* returns txn key from Sip message */
    SipTxnKey    *pobjTxnKey = SIP_NULL;
    if (SIP_FALSE == GetTxnKeyFromSipMsg(pobjSipMsg, &pobjTxnKey, pusError))
    {
        SIP_DEBUG_STACKBUG(ESIPTRACE_MODTRANSP,
                "OnRecvTransp: GetTxnKeyFromSipMsg fail",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    if (pobjTxnKey->GetMsgType() == SipMessage::REQ_TYPE)
    {
        if ((SipPf_Strcmp(ACK_METHOD, pobjTxnKey->GetMethod()) == SIP_EQUALS) ||
            (SipPf_Strcmp(INVITE_METHOD, pobjTxnKey->GetMethod()) == SIP_EQUALS))
        {
            pobjTxnKey->SetTxnType(SipTxn::INV_SER_TXN);
        }
        else
        {
            pobjTxnKey->SetTxnType(SipTxn::NON_INV_SER_TXN);
        }
    }
    else
    {
        if (SipPf_Strcmp(INVITE_METHOD, pobjTxnKey->GetMethod()) == SIP_EQUALS)
        {
            pobjTxnKey->SetTxnType(SipTxn::INV_CLI_TXN);
        }
        else
        {
            pobjTxnKey->SetTxnType(SipTxn::NON_INV_CLI_TXN);
        }
    }

    /* Fetches object from Database[Utility func within txn] */
    SipTxn     *pobjTxn = SIP_NULL;
    SIP_BOOL    bTxnExist = SIP_TRUE;
    if (GetTxnObjFromDb(pobjTxnKey, &pobjTxn, &bTxnExist, pusError) == SIP_FALSE)
    {
        SIP_DEBUG_STACKBUG(ESIPTRACE_MODTRANSP,
                "OnRecvTransp: GetTxnObjFromDb fail",SIP_ZERO,SIP_ZERO);

        delete pobjTxnKey;
        return SIP_FALSE;
    }

    *pbTxnExist = bTxnExist;

    if (bTxnExist == SIP_NO )
    {
        if (pobjTxnKey->GetMsgType() == SipMessage::RESP_TYPE)
        {
            /* Transaction for the response doesn't exist */

            SIP_UINT16 usStatusCode = pobjSipMsg->GetStatusCode();

            /* In case of retransmitted 2xx for INVITE, return as valid sip message */
            if ((pobjSipMsg->GetMethodType() == SipMessage::METHOD_INVITE)
                    && SIP_SUCCESSFUL_RESP(usStatusCode))
            {
                *peTxnStatus = SipTxn::STATUS_VALID_MESSAGE;
            }
            else
            {
                *peTxnStatus = SipTxn::STATUS_STRAY_RESP;
            }
        }
        else
        {
            /* Transaction for the Request doesn't exist */
            *peTxnStatus = SipTxn::STATUS_NEW_REQ_RECVD;
        }

        *ppobjTxnKey = pobjTxnKey;    // new instance
        return SIP_TRUE;
    }

    *ppobjTxnKey  = pobjTxnKey; // New instance
    *peTxnStatus = SipTxn::STATUS_VALID_MESSAGE;

    /* For Both Request and Response Notify Transaction Layer */

    return SIP_TRUE;
}

/*****************************************************************************
 * Function name    : OnRecvTanspError
 * Description      : If Error is occurred due to Mesg Constraint , swith back to UDP and transport
 * Preconditions/   :
 * Side Effects     :
 *****************************************************************************/
SIP_BOOL SipTransportHandler::OnRecvTanspError
(
 SIP_INT32    eTranspError,
 SipTxnKey            *pobjTxnKey,
 SIP_INT32    *peTxnStatus,
 SipTransportInfo        **ppobjTranspInfo,
 ISipUserData        *pobjUserData,
 SIP_UINT16        *pusError
 )
{
    (void)pobjUserData;
    (void)eTranspError;

    /*Fetches object from Database[Utility func within txn] */
    SipTxn  *pobjTxn = SIP_NULL;
    SIP_BOOL bTxnExist = SIP_FALSE;
    if (GetTxnObjFromDb(pobjTxnKey, &pobjTxn, &bTxnExist, pusError) == SIP_FALSE)
    {
        return SIP_FALSE;
    }

    if (bTxnExist == SIP_NO)
    {
        /* Transaction for the response doesn't exist */
        SIP_DEBUG_WARNING(ESIPTRACE_MODTRANSP,
                "OnRecvTanspError: Transaction Not Found",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    pobjTxn->increment();

    SipTransportInfo *pobjTranspInfo = pobjTxn->GetTranspInfo();
    if (pobjTranspInfo == SIP_NULL)
    {
        pobjTxn->SipDelete();

        /*  stack error */
        SIP_DEBUG_STACKBUG(ESIPTRACE_MODTRANSP,
                "OnRecvTanspError: sipFetchElement Error",SIP_ZERO,SIP_ZERO);
        *pusError = ETXN_STACKERROR;
        return SIP_FALSE;
    }

    SipTransportParameter *pobjMsgSentTransParam = pobjTranspInfo->GetMsgSentTranspParam();
    if (pobjMsgSentTransParam == SIP_NULL)
    {
        pobjTxn->SipDelete();

        /*  stack error */
        SIP_DEBUG_STACKBUG(ESIPTRACE_MODTRANSP,
                "OnRecvTanspError: sipFetchElement Error",SIP_ZERO,SIP_ZERO);
        *pusError = ETXN_STACKERROR;
        return SIP_FALSE;
    }

    SIP_INT32 eTransportType = pobjMsgSentTransParam->GetTranspProtocol();
    if (eTransportType == SipTransportInfo::PROTOCOL_INVALID)
    {
        pobjTxn->SipDelete();

        SIP_DEBUG_STACKBUG(ESIPTRACE_MODTRANSP,
                "OnRecvTanspError: SipTransportInfo::PROTOCOL_INVALID\n",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_BOOL bExceedMTU = pobjTranspInfo->IsExceedMTU();
    /* Check if Error occurred due to switching to TCP (Message Len Constraint)*/
    if ((bExceedMTU == SIP_TRUE) && (eTransportType == SipTransportInfo::PROTOCOL_TCP))
    {
        /* Switch to previous Transport Information as initially given by stack user */
        /* Change Protocol in Buffer */
        SIP_BOOL bStatus = UpdateViaSipMsg(
                pobjTranspInfo->GetSentSipMsg(),
                pobjTranspInfo->GetTranspSipBuffer(),
                SipTransportInfo::PROTOCOL_UDP);
        if (bStatus == SIP_FALSE)
        {
            pobjTxn->SipDelete();

            SIP_DEBUG_WARNING(ESIPTRACE_MODTRANSP,
                    "OnRecvTanspError: sipUpdateViaSipMsg Fails\n",SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }

        pobjTranspInfo->SetExceedMTUFlag(SIP_FALSE);

        /* Change Protocol in Transport Information */
        pobjMsgSentTransParam->setTranspProtocol(SipTransportInfo::PROTOCOL_UDP);

        *peTxnStatus = SipTxn::STATUS_RETRANSMISSION;

        *ppobjTranspInfo = pobjTranspInfo;

        pobjTxn->SipDelete();
        return SIP_TRUE;
    }

    pobjTxn->SipDelete();

    return SIP_TRUE;
}

/*****************************************************************************
 * Function name         : IsInviteTxnPresentForAckTxn
 * Description            : Checks if INVITE server transaction is present which is matched
 *                         with the specified ACK transaction key.
 * Preconditions/        :
 * Side Effects            :
 *****************************************************************************/
SIP_BOOL SipTransportHandler::IsInviteTxnPresentForAckTxn(IN SipTxnKey* pAckTxnKey)
{
    SIP_UINT16 usError = 0;
    SipTxnKey* pobjInviteTxnKey = new SipTxnKey(pAckTxnKey, &usError);

    pobjInviteTxnKey->SetMethod(INVITE_METHOD);
    pobjInviteTxnKey->RemoveRule(SipTxnKey::RULE_COMPARE_VIA_BRANCH);

    SipTxn* pobjTxn = SIP_NULL;
    SIP_BOOL bTxnExist = SIP_TRUE;
    if (GetTxnObjFromDb(pobjInviteTxnKey, &pobjTxn, &bTxnExist, &usError) == SIP_FALSE)
    {
        bTxnExist = SIP_FALSE;
        SIP_DEBUG_STACKBUG(ESIPTRACE_MODTRANSP,
                "IsInviteTxnPresentForAckTxn: GetTxnObjFromDb fail",SIP_ZERO,SIP_ZERO);
    }

    delete pobjInviteTxnKey;

    return (bTxnExist == SIP_TRUE) ? SIP_TRUE : SIP_FALSE;
}

/*****************************************************************************
 * Function name         : UpdateViaSipMsg
 * Description            : Changing Transport with in VIA Header
 * Preconditions/        :
 * Side Effects            :
 *****************************************************************************/
PRIVATE SIP_BOOL SipTransportHandler::UpdateViaSipMsg
(
 SipMessage            *pobjSipMsg,
 SipTransportBuffer        *pobjSentBuffer,
 SIP_INT32    eChangeProto
 )
{
    SIP_CHAR    *pucSipBuffer = pobjSentBuffer->GetSipBuffer();
    SIP_CHAR *pucTemp = SipPf_Strstr(pucSipBuffer,SIP_VIA_ENC_FORMAT_1);
    if (pucTemp == SIP_NULL)
    {
        pucTemp = SipPf_Strstr(pucSipBuffer,SIP_VIA_ENC_FORMAT_2);
        if (pucTemp == SIP_NULL)
        {
            return SIP_FALSE;
        }
    }

    if (eChangeProto == SipTransportInfo::PROTOCOL_TCP)
    {
        pucTemp = SipPf_Strstr(pucTemp,SIP_VIA_LINE_UDP);
        if (pucTemp == SIP_NULL)
        {
            return SIP_FALSE;
        }
        /* Curpos = "/UDP"  */
        pucTemp = pucTemp + SIP_ONE;

        /* Curpos = "UDP"  */
        /* Change UDP to TCP */
        *pucTemp = 'T';
        pucTemp = pucTemp + SIP_ONE;
        *pucTemp = 'C';
    }
    else if (eChangeProto == SipTransportInfo::PROTOCOL_UDP)
    {
        pucTemp = SipPf_Strstr(pucTemp,SIP_VIA_LINE_TCP);
        if (pucTemp == SIP_NULL)
        {
            return SIP_FALSE;
        }

        /* Curpos = "/TCP"  */
        pucTemp = pucTemp + SIP_ONE;

        /* Curpos = "TCP"  */
        /* Change TCP to UDP */
        *pucTemp = 'U';
        pucTemp = pucTemp + SIP_ONE;
        *pucTemp = 'D';
    }
    else
    {
        return SIP_FALSE;
    }

    if (pobjSipMsg != SIP_NULL)
    {
        /* Update Via Hdr of Sip Message from UDP to TCP */
        SipViaHeader *pobjViaHdr = (SipViaHeader *)pobjSipMsg->GetHdrObj(SipHeaderBase::VIA);
        if (pobjViaHdr == SIP_NULL)
        {
            SIP_DEBUG_STACKBUG(ESIPTRACE_MODTRANSP, "Memory allocation fail", SIP_ZERO, SIP_ZERO);

            return SIP_FALSE;
        }
        if (eChangeProto == SipTransportInfo::PROTOCOL_TCP)
        {
            pobjViaHdr->SetProtocolName(SIP_TRANSP_TCP);
        }
        else
        {
            pobjViaHdr->SetProtocolName(SIP_TRANSP_UDP);
        }
        pobjViaHdr->SipDelete();
    }
    return SIP_TRUE;
}

/*****************************************************************************
 * Function name         : GetTxnKeyFromSipMsg
 * Description            : returns txn key from Sip message
 * Preconditions/        :
 * Side Effects            :
 *****************************************************************************/
PRIVATE SIP_BOOL SipTransportHandler::GetTxnKeyFromSipMsg
(
 IN        SipMessage    *pobjSipMsg,
 OUT        SipTxnKey        **ppobjTxnKey,
 OUT        SIP_UINT16    *pusError
 )
{
    SIP_INT32     eMsgType     = SipMessage::TYPE_INVALID;
    SIP_INT32    eMethodType    = SipMessage::METHOD_INVALID;

    /* Check if it's proper to start Transaction Ref: RFC 3261 8.1.1.*/
    SIP_BOOL bStatus  = CheckTxnMadatoryParams(    pobjSipMsg, &eMsgType,
            &eMethodType);
    if (bStatus == SIP_FALSE)
    {
        SIP_DEBUG_WARNING ( ESIPTRACE_MODTRANSP,
                "GetTxnKeyFromSipMsg: CheckTxnMadatoryParams fails\n",SIP_ZERO,SIP_ZERO );

        return SIP_FALSE;
    }

    SIP_TRACE_NORMAL(ESIPTRACE_MODTRANSP,
            "GetTxnKeyFromSipMsg: eMsgType[%d], Method[%d]",eMsgType,eMethodType);

    SipTxnKey *pobjTxnKey = new SipTxnKey(pobjSipMsg, pusError);
    if (pobjTxnKey == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTRANSP,
                "GetTxnKeyFromSipMsg:key Creation Fails",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    else if (*pusError == EERR_MALLOCFAILED)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTRANSP,
                "GetTxnKeyFromSipMsg:key Creation Fails",SIP_ZERO,SIP_ZERO);
        delete pobjTxnKey;
        return SIP_FALSE;
    }

    *ppobjTxnKey = pobjTxnKey;
    return SIP_TRUE;
}


/*****************************************************************************
 * Function name         : GetTxnObjFromDb
 * Description            : Fetches object from Database[Utility func within txn]
 * Preconditions/        :
 * Side Effects            :
 *****************************************************************************/
PRIVATE SIP_BOOL SipTransportHandler::GetTxnObjFromDb
(
 IN        SipTxnKey        *pobjTxnKey,
 OUT        SipTxn        **ppobjTxn,
 OUT        SIP_BOOL     *pbTxnExist,
 OUT        SIP_UINT16    *pusError
 )
{
    SIP_BOOL bTxnExist = sip_cbk_fetchTransaction(
            reinterpret_cast<SIP_VOID*>(pobjTxnKey),
            TXN_OPT_FETCH,
            SIP_NULL,
            reinterpret_cast<SIP_VOID**>(ppobjTxn));

    if ((bTxnExist == SIP_YES) && (*ppobjTxn == SIP_NULL))
    {
        /*  stack error */
        *pusError = ETXN_STACKERROR;
        SIP_DEBUG_STACKBUG(ESIPTRACE_MODTXN,
                "GetTxnObjFromDb: Stack Error,Db Status Error\n",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    *pbTxnExist = bTxnExist;
    return SIP_TRUE;
}
