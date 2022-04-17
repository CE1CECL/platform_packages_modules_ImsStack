/*
   Author
   <table>
   date      author                 description
   --------  --------------         ----------
   20100000  syed.malgimani@        Created
   20170110  vijay.nair@            Modified
   </table>

   Description

 */

#include "txn/SipTxn.h"
#include "msg/SipMessage.h"
#include "txn/SipTxnHandler.h"
#include "SipTrace.h"
#include "sip_debug.h"
#include "txn/sip_txn_fsm.h"
#include "platform/sip_pf_string.h"
#include "SipUtil.h"
#include "SipContextUtils.h"

#ifdef SIP_TRACE_ENABLE
static SIP_CHAR gszTxnTypeStr[SipTxn::INVALID_TXN+1][SIP_20] =
{
    "INV-CLI-TXN",
    "INV-SER-TXN",
    "NON-INV-CLI-TXN",
    "NON-INV-SER-TXN",
    "INVALID-TXN"
};
#endif

static SIP_INT32 GetNonInvCliFsmEvt(SIP_UINT16 usStatusCode);
static SIP_INT32 GetNonInvSerFsmEvt(SIP_UINT16 usStatusCode);
static SIP_INT32 GetInvCliFsmEvt(SIP_UINT16 usStatusCode);
static SIP_INT32 GetInvSerFsmEvt(SIP_UINT16 usStatusCode);

SIP_BOOL SipTxnHandler::OnSendTxn
(
    SipMessage    *pobjSipMsg,         IN_OUT
    SipTransportParameter    *pobjTranspParam,    IN
    ISipUserData    *pobjUserData,        IN
    SipTxnKey        **ppobjTxnKey,        OUT
    SipTxnInfo    *pobjTxnInfo,        OUT
    SIP_UINT16    *pusError
)
{
    SipTxnFsmData  objTxnFsmData(pobjSipMsg, pobjTranspParam, pobjUserData);

    SipTxnKey        *pobjTxnKey = SIP_NULL;
    SIP_INT32     eTxnType = SipTxn::INVALID_TXN;
    /* Validate txn params from sip message and returns txn key and txn type */
    if (ValidateSendTxn(pobjSipMsg, &eTxnType, &pobjTxnKey, pusError) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                           "OnSendTxn:ValidateSendTxn Failed",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    //SetTxnType, InvCli, NonInvCli, InvSer and NonInvSer
    pobjTxnKey->SetTxnType(eTxnType);

    /* Get txn object from txn DB */
    SipTxn  *pobjTxn = SIP_NULL;
    SIP_BOOL     bTxnExist = SIP_FALSE;
    if (GetTxnObjFromDb(pobjTxnKey, &pobjTxn, &bTxnExist, pusError) == SIP_FALSE)
    {
        SIP_DEBUG_STACKBUG(ESIPTRACE_MODTXN,
                "OnSendTxn:GetTxnObjFromDb Fails \n",
                SIP_ZERO,SIP_ZERO);
        delete pobjTxnKey;
        return SIP_FALSE;
    }

    /* Check if Txn already exists for Req */
    if (bTxnExist == SIP_YES)
    {
        if (pobjTxnKey->GetMsgType() == SipMessage::REQ_TYPE)
        {
            *pusError = ETXN_ALREADYTRANSACTIONINPROCESSERROR;

            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "OnSendTxn: Txn Already in Progress\n",SIP_ZERO,SIP_ZERO);

            delete pobjTxnKey;
            return SIP_FALSE;
        }
    }
    else /* Txn not Found */
    {
        /* handle Response case */
        if (pobjTxnKey->GetMsgType() == SipMessage::RESP_TYPE)
        {
            /* 2xx retransmission for invite message should be sent directly to transport */
            if (pobjSipMsg->GetMethodType() == SipMessage::METHOD_INVITE)
            {
                SIP_UINT16 usStatusCode = pobjSipMsg->GetStatusCode();
                if (SIP_SUCCESSFUL_RESP(usStatusCode))
                {
                    *ppobjTxnKey = pobjTxnKey;
                    return SIP_TRUE;
                }
            }

            delete pobjTxnKey;
            return SIP_FALSE;
        }
        else
        {
            /* For ACK send directly to Transport : request check is for better understanding */
            if ((pobjTxnKey->GetMsgType() == SipMessage::REQ_TYPE) &&
                    (pobjSipMsg->GetMethodType() == SipMessage::METHOD_ACK))
            {
                /* Send ACK Directly to N/w */
                *ppobjTxnKey = pobjTxnKey;
                return SIP_TRUE;
            }
        }
    }/* end of txn not found */

    /* Invoke FSM for Transaction state handling */

    switch (eTxnType)
    {
        case SipTxn::INV_CLI_TXN:
        case SipTxn::NON_INV_CLI_TXN:
            {
                /* Invoking client FSM to process and send request. It create new txn object */
                if (HandleClientTxnSend(eTxnType, pobjTxnKey, &objTxnFsmData, pusError)
                        == SIP_FALSE)
                {
                    delete pobjTxnKey;

                    SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                            "OnSendTxn: HandleClientTxnSend fail",SIP_ZERO,SIP_ZERO);
                    return SIP_FALSE;
                }
            }
            break;
        case SipTxn::INV_SER_TXN:
        case SipTxn::NON_INV_SER_TXN:
            {
                /* Invokes server FSM to process and send response */
                if (HandleServerTxnSend(eTxnType,
                            pobjTxnKey, &objTxnFsmData, pusError) == SIP_FALSE)
                {
                    delete pobjTxnKey;

                    SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                            "OnSendTxn: HandleServerTxnSend fail",SIP_ZERO,SIP_ZERO);
                    return SIP_FALSE;
                }
            }
            break;
        default:
            {
                delete pobjTxnKey;

                *pusError = SipTxn::INVALID_TXN;
                SIP_DEBUG_STACKBUG(ESIPTRACE_MODTXN,     "OnSendTxn: INVALID Txn Type",
                        SIP_ZERO,SIP_ZERO);
                return SIP_FALSE;
            }
            break;
    }

    pobjTxnInfo->bTxnTerminated = objTxnFsmData.bTxnTerminated;
    pobjTxnInfo->bTxnCreated = objTxnFsmData.bTxnCreated;

    if ((objTxnFsmData.bTxnCreated == SIP_TRUE) || (objTxnFsmData.bTxnTerminated == SIP_TRUE))
    {
        pobjTxnInfo->m_pobjUserData = objTxnFsmData.m_pobjOutUserData;
    }

    *ppobjTxnKey = pobjTxnKey;
    return SIP_TRUE;
}

SIP_BOOL SipTxnHandler::OnRecvTxn
(
    IN    SipMessage                *pobjSipMsg,
    IN    SipTxnKey                    *pobjTxnKey,
    IN    ISipUserData                *pobjUserData,
    OUT    SipTxnInfo                *pobjTxnInfo,
    OUT    SIP_UINT16                *pusError
)
{
    SipTxnFsmData    objTxnFsmData(
            pobjSipMsg,
            SIP_NULL,  // Transport param
            pobjUserData);

    if ((pobjSipMsg == SIP_NULL) || (pobjTxnKey == SIP_NULL))
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,  "OnRecvTxn: NULL param",0,0);
        return SIP_FALSE;
    }

    SIP_INT32    eTxnType = SipTxn::INVALID_TXN;
    /* Validate Txn params from sip message and returns txn type */
    if (ValidateRecvTxn(pobjSipMsg, &eTxnType) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "OnRecvTxn: ValidateRecvTxn failure \n",0,0);
        return SIP_FALSE;
    }

    //SetTxnType, InvCli, NonInvCli, InvSer and NonInvSer
    pobjTxnKey->SetTxnType(eTxnType);

    switch (eTxnType)
    {
        case SipTxn::INV_CLI_TXN:
        case SipTxn::NON_INV_CLI_TXN:
            {
                /* Invoking client FSM to process received response */
                if (HandleClientTxnRecv(eTxnType, pobjTxnKey,
                        &objTxnFsmData, IN_OUT pusError) == SIP_FALSE)
                {
                    SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                            "OnRecvTxn:HandleClientTxnRecv fail",SIP_ZERO,SIP_ZERO);

                    return SIP_FALSE;
                }

                /* INV Client Txn --> For INV 2xx case, txn is terminated */
                if (objTxnFsmData.bTxnTerminated == SIP_TRUE)
                {
                    /* stack manager to Notifies to Transaction User using registered listener
                       and Delete Txn entry from DB and delete the instance*/
                    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                            "OnRecvTxn: Client Txn Terminated",SIP_ZERO,SIP_ZERO);
                }
            }
            break;

        case SipTxn::INV_SER_TXN:
        case SipTxn::NON_INV_SER_TXN:
            {
                /* For new request(no txn exist, pobjTxn is NULL), create new txn and process
                request. For existing txn, it should be Failure ACK message, process the message */
                if (HandleServerTxnRecv(eTxnType, pobjTxnKey,
                        &objTxnFsmData, IN_OUT pusError) == SIP_FALSE)
                {
                    SIP_DEBUG_WARNING(ESIPTRACE_MODTXN, "OnRecvTxn:HandleServerTxnRecv fail",0,0);
                    return SIP_FALSE;
                }

                /* INV Serv Txn --> For TCP, on ACK recv Txn is terminated */
                if (objTxnFsmData.bTxnTerminated == SIP_TRUE)
                {
                    /* stack manager to Notifies to Transaction User using registered listener
                       and Delete Txn entry from DB and delete the instance*/
                    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                            "OnRecvTxn: Server Txn Terminated",SIP_ZERO,SIP_ZERO);
                }
            }
            break;

        default:
            {
                *pusError = SipTxn::INVALID_TXN;
                SIP_DEBUG_STACKBUG(ESIPTRACE_MODTXN,     "OnRecvTxn: INVALID Txn Type",
                        SIP_ZERO,SIP_ZERO);
                return SIP_FALSE;
            }
            break;
    }

    if ((eTxnType == SipTxn::INV_CLI_TXN) || (eTxnType == SipTxn::INV_SER_TXN))
    {
        /* INV Client Txn : when failure response is received, stack send failure ACK
           inform to stack manager for sending of failure ACK request.
           NOTE--> For all other cases, it contains NULL

           INV Serv Txn: when new INV request is received, stack send 100 Trying
           response. inform to stack manager for sending of 100 Trying resp message.
           NOTE--> For all other cases, it contains NULL
         */
        pobjTxnInfo->m_pobjSendSipMsg = objTxnFsmData.m_pobjSendSipMsg;
    }

    if (objTxnFsmData.eTxnStatus == SipTxn::STATUS_RETRANSMISSION)
    {
        pobjTxnInfo->m_pobjTranspInfo = objTxnFsmData.m_pobjTranspInfo;
    }

    pobjTxnInfo->m_pobjUserData = objTxnFsmData.m_pobjOutUserData;
    pobjTxnInfo->bTxnTerminated = objTxnFsmData.bTxnTerminated;
    pobjTxnInfo->bTxnCreated = objTxnFsmData.bTxnCreated;
    pobjTxnInfo->eTxnStatus = objTxnFsmData.eTxnStatus;

    return SIP_TRUE;
}

SIP_BOOL SipTxnHandler::OnRecvTranspError
(
    SIP_INT32    eTransErrro,
    SipTxnKey            *pobjTxnKey,
    SIP_UINT16        *pusError
)
{
    if (pobjTxnKey == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "OnRecvTranspError: pobjTxnKey  Null \n",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    /* Get Transaction Object from Db, based on Key */
    SipTxn* pobjTxn = SIP_NULL;
    SIP_BOOL bTxnExist = SIP_FALSE;

    if (GetTxnObjFromDb(pobjTxnKey, &pobjTxn, &bTxnExist, pusError) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "OnRecvTranspError:GetTxnObjFromDb Fails \n",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    if (pobjTxn == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "OnRecvTranspError: No SipTxn \n",
                SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_INT32 eTxnType = pobjTxn->GetTxnType();

    switch (eTxnType)
    {
        case SipTxn::INV_CLI_TXN:
            {
                if (pobjTxn->InvokeFsm(SipTxn::INV_CLI_TRANSP_ERROR_EVT,
                        &eTransErrro, pusError) == SIP_FALSE)
                {
                    SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                            "OnRecvTranspError: INV-CLI-FSM fails\n",
                            SIP_ZERO,SIP_ZERO);
                    return SIP_FALSE;
                }
            }
            break;

        case SipTxn::NON_INV_CLI_TXN:
            {
                if (pobjTxn->InvokeFsm(SipTxn::NON_INV_CLI_TRANSP_ERROR_EVT,
                        &eTransErrro, pusError) == SIP_FALSE)
                {
                    SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                            "OnRecvTranspError: Non-INV-CLI-FSM fails\n",
                            SIP_ZERO,SIP_ZERO);
                    return SIP_FALSE;
                }
            }
            break;

        case SipTxn::INV_SER_TXN:
            {
                if (pobjTxn->InvokeFsm(SipTxn::INV_SER_TRANSP_ERROR_EVT,
                        &eTransErrro, pusError) == SIP_FALSE)
                {
                    SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                            "OnRecvTranspError: INV-SERV-FSM fails\n",
                            SIP_ZERO,SIP_ZERO);
                    return SIP_FALSE;
                }
            }
            break;
        case SipTxn::NON_INV_SER_TXN:
            {
                if (pobjTxn->InvokeFsm(SipTxn::NON_INV_SER_TRANSP_ERROR_EVT,
                        &eTransErrro, pusError) == SIP_FALSE)
                {
                    SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                            "OnRecvTranspError: Non-INV-SER-FSM fails\n",
                            SIP_ZERO,SIP_ZERO);
                    return SIP_FALSE;
                }
            }
            break;
        default:
            {
                *pusError = SipTxn::INVALID_TXN;
                SIP_DEBUG_STACKBUG(ESIPTRACE_MODTXN,
                        "OnRecvTranspError: INVALID Txn Type\n",
                        SIP_ZERO,SIP_ZERO);
            }
            break;
    }

    /* Notify Txn Termination, need not send retransmission */
    if (pobjTxn->IsTxnTerminated() == SIP_TRUE)
    {
        /* Notifies to Transaction User using registered listener
           Delete Txn entry from DB and delete the instance*/

        DeleteTxn(pobjTxnKey);

        SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                "OnRecvTranspError: Transaction Terminated\n",SIP_ZERO,SIP_ZERO);
        return SIP_TRUE;
    }
    return SIP_TRUE;
}

SIP_BOOL SipTxnHandler::OnSendTranspError
(
    SipTxnKey        *pobjTxnKey
)
{
    if (pobjTxnKey == SIP_NULL) {
        return SIP_TRUE;
    }

    if (TerminateTxn(pobjTxnKey) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "OnSendTranspError: TerminateTxn Error",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    return SIP_TRUE;
}

SIP_BOOL SipTxnHandler::TerminateTxn
(
    SipTxnKey        *pobjTxnKey
)
{
    if (pobjTxnKey == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "TerminateTxn: pobjTxnKey NULL",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SipTxn* pobjTxn = SIP_NULL;
    SipTxnKey* pobjOutTxnKey = SIP_NULL;
    SIP_BOOL bTxnExist = sip_cbk_releaseTransaction(
            reinterpret_cast<SIP_VOID*>(pobjTxnKey),
            TXN_OPT_REMOVE,
            reinterpret_cast<SIP_VOID**>(&pobjOutTxnKey),
            reinterpret_cast<SIP_VOID**>(&pobjTxn));

    if (bTxnExist == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "TerminateTxn: Txn Not existing",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    if (pobjTxn != SIP_NULL)
    {
        if (pobjTxn->AbortTxn() == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                    "TerminateTxn: AbortTxn Fail",SIP_ZERO,SIP_ZERO);
        }

        ISipUserData* pUserData = pobjTxn->GetUserData();

        if (pUserData != SIP_NULL)
        {
            pUserData->SetDeleteFlag(SIP_TRUE);
        }

        /* Notifies to Transaction User using registered listener */
        NotifyTxnTermination(pobjTxn);

        pobjTxn->SipDelete();
    }

    if (pobjOutTxnKey != SIP_NULL)
    {
        pobjOutTxnKey->SipDelete();
    }

    return SIP_TRUE;
}

SIP_BOOL SipTxnHandler::DeleteTxn
(
    SipTxnKey        *pobjTxnKey
)
{
    if (pobjTxnKey == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "DeleteTxn: pobjTxnKey NULL",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SipTxn* pobjTxn = SIP_NULL;
    SipTxnKey* pobjOutTxnKey = SIP_NULL;
    SIP_BOOL bTxnExist = sip_cbk_releaseTransaction(
            reinterpret_cast<SIP_VOID*>(pobjTxnKey),
            TXN_OPT_REMOVE,
            reinterpret_cast<SIP_VOID**>(&pobjOutTxnKey),
            reinterpret_cast<SIP_VOID**>(&pobjTxn));

    if (bTxnExist == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "TerminateTxn: Txn Not existing",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    if (pobjTxn != SIP_NULL)
    {
        /* Notifies to Transaction User using registered listener */
        NotifyTxnTermination(pobjTxn);

        pobjTxn->SipDelete();
    }

    if (pobjOutTxnKey != SIP_NULL)
    {
        pobjOutTxnKey->SipDelete();
    }

    return SIP_TRUE;
}

SIP_BOOL SipTxnHandler::UpdateTxnDetails
(
    SipTxnKey        *pobjTxnKey,
    SipTransportInfo    *pobjTranspInfo,
    SIP_UINT16    *pusError
)
{
    if (pobjTxnKey == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "UpdateTxnDetails: Key is Null \n",
                SIP_ZERO,SIP_ZERO);

        return SIP_FALSE;
    }

    SipTxn     *pobjTxn = SIP_NULL;
    SIP_BOOL    bTxnExist = SIP_TRUE;
    if (GetTxnObjFromDb(pobjTxnKey, &pobjTxn, &bTxnExist, pusError) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "UpdateTxnDetails: GetTxnObjFromDb failure \n",
                SIP_ZERO,SIP_ZERO);

        return SIP_FALSE;
    }

    if (bTxnExist == SIP_NO)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "UpdateTxnDetails: GetTxnObjFromDb no object \n",
                SIP_ZERO,SIP_ZERO);

        return SIP_FALSE;
    }

    pobjTxn->increment();
    pobjTxn->UpdateTranspInfo(pobjTranspInfo );
    pobjTxn->SipDelete();
    return SIP_TRUE;
}

PRIVATE SIP_INT32 SipTxnHandler::GetTxnType
(
    SIP_INT32    eMsgDir,
    SIP_INT32    eMethodType,
    SIP_INT32    eMsgType
)
{
    if ( (eMsgDir == SipTxn::INVALID_DIR) ||
            (eMethodType == SipMessage::METHOD_INVALID) ||
            (eMsgType == SipMessage::TYPE_INVALID))
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "GetTxnType: ETXN_INVALID",SIP_ZERO,SIP_ZERO);
        return SipTxn::INVALID_TXN;
    }

    SIP_INT32 eTxnType = SipTxn::INVALID_TXN;
    if (eMsgDir == SipTxn::SEND)
    {
        if (eMsgType == SipMessage::REQ_TYPE)
        {
            if ((eMethodType == SipMessage::METHOD_INVITE)
                    || (eMethodType == SipMessage::METHOD_ACK))
            {
                eTxnType = SipTxn::INV_CLI_TXN;
            }
            else
            {
                eTxnType = SipTxn::NON_INV_CLI_TXN;
            }
        }
        else
        {
            if (eMethodType == SipMessage::METHOD_INVITE)
            {
                eTxnType = SipTxn::INV_SER_TXN;
            }
            else
            {
                eTxnType = SipTxn::NON_INV_SER_TXN;
            }
        }
    }
    else /* eMsgDir is SipTxn::RECV */
    {
        if (eMsgType == SipMessage::REQ_TYPE)
        {
            if ((eMethodType == SipMessage::METHOD_INVITE) ||
                    (eMethodType == SipMessage::METHOD_ACK))
            {
                eTxnType = SipTxn::INV_SER_TXN;
            }
            else
            {
                eTxnType = SipTxn::NON_INV_SER_TXN;
            }
        }
        else
        {
            if (eMethodType == SipMessage::METHOD_INVITE)
            {
                eTxnType = SipTxn::INV_CLI_TXN;
            }
            else
            {
                eTxnType = SipTxn::NON_INV_CLI_TXN;
            }
        }
    }

    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN," TxnType[%s]",gszTxnTypeStr[eTxnType],SIP_ZERO);
    return eTxnType;
}

PRIVATE SIP_BOOL SipTxnHandler::GetTxnObjFromDb
(
    SipTxnKey     *pobjTxnKey,
    SipTxn        **ppobjTxn,
    SIP_BOOL     *pbTxnExist,
    SIP_UINT16    *pusError
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

PRIVATE SIP_BOOL SipTxnHandler::GetTxnObjFromDb
(
    SipTxnKey     *pobjTxnKey,
    SipTxn        **ppobjTxn,
    SipTxnKey     **ppobjOutTxnKey,
    SIP_BOOL     *pbTxnExist,
    SIP_UINT16   *pusError
)
{
    SIP_BOOL bTxnExist = sip_cbk_fetchTransaction(
            reinterpret_cast<SIP_VOID*>(pobjTxnKey),
            TXN_OPT_FETCH,
            reinterpret_cast<SIP_VOID**>(ppobjOutTxnKey),
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

PRIVATE SIP_BOOL    SipTxnHandler::HandleClientTxnSend
(
    IN        SIP_INT32    eTxnType,
    IN        SipTxnKey        *pobjTxnKey,
    IN_OUT    SipTxnFsmData    *pObjTxnFsmData,
    OUT        SIP_UINT16    *pusError
)
{
    SipTimerContext *pSipTimerContext = SIP_NULL;

    if (pObjTxnFsmData->m_pobjUserData != SIP_NULL)
    {
        SIP_VOID* pUserData = pObjTxnFsmData->m_pobjUserData->GetUserData();
        if (pUserData != SIP_NULL)
        {
            pSipTimerContext = ((SipTxnContext*) pUserData)->pSipTimerContext;
        }
    }

    SipTxn* pobjTxn = new SipTxn(eTxnType, pobjTxnKey,
            pObjTxnFsmData->m_pobjSipMsgIn, pSipTimerContext, pusError );

    /* checking for txn object null or error parameter value from txn constructor */
    if ((pobjTxn == SIP_NULL) || (*pusError == E_ERR_PF_MALLOCFAILED))
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,    "HandleClientTxnSend --> memory fail",
                SIP_ZERO,SIP_ZERO);
        if (pobjTxn != SIP_NULL)
        {
            pobjTxn->SipDelete();
        }
        return SIP_FALSE;
    }

    SIP_UINT16 usEvent;

    if (eTxnType == SipTxn::INV_CLI_TXN)
    {
        usEvent = SipTxn::INV_CLI_SEND_INV_REQ_EVT;
    }
    else
    {
        usEvent = SipTxn::NON_INV_CLI_SEND_NON_INV_REQ_EVT;
    }

    if (pobjTxn->InvokeFsm(usEvent, pObjTxnFsmData, pusError) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "HandleClientTxnSend -> FSM handling fail eTxnType[%d] usEvent[%d]",
                eTxnType,usEvent);
        pobjTxn->SipDelete();
        return SIP_FALSE;
    }

    pobjTxn->SipDelete();
    return SIP_TRUE;
}

PRIVATE SIP_BOOL    SipTxnHandler::HandleServerTxnSend
(
    IN        SIP_INT32    eTxnType,
    IN        SipTxnKey        *pobjTxnKey,
    IN_OUT    SipTxnFsmData    *pObjTxnFsmData,
    IN        SIP_UINT16    *pusError
)
{
    SipTxn* pobjTxn = SIP_NULL;
    SipTxnKey* pobjOutTxnKey = SIP_NULL;
    SIP_BOOL bTxnExist = SIP_FALSE;

    /* Get Txn object from txn DB if exists */
    if (GetTxnObjFromDb(pobjTxnKey, &pobjTxn,
                &pobjOutTxnKey, &bTxnExist,pusError) == SIP_FALSE)
    {
        SIP_DEBUG_STACKBUG(ESIPTRACE_MODTXN,
                "HandleClientTxnRecv:GetTxnObjFromDb Fails \n",0,0);
        return SIP_FALSE;
    }

    if ((pobjTxn == SIP_NULL) || (bTxnExist == SIP_FALSE))
    {
        SIP_DEBUG_STACKBUG(ESIPTRACE_MODTXN,
                "HandleClientTxnRecv:No Txn Exists",0,0);
        return SIP_FALSE;
    }
    pobjTxn->increment();

    SIP_UINT16 usStatusCode = pObjTxnFsmData->m_pobjSipMsgIn->GetStatusCode();
    SIP_UINT16 usEvent;
    if (eTxnType == SipTxn::INV_SER_TXN)
    {
        usEvent = GetInvSerFsmEvt(usStatusCode);

        pobjTxn->SetRespCode(usStatusCode);

        if (pobjOutTxnKey != SIP_NULL)
        {
            pobjOutTxnKey->SetRespCode(usStatusCode);
        }
    }
    else
    {
        usEvent = GetNonInvSerFsmEvt(usStatusCode);
    }

    if ((usEvent == SipTxn::INV_SER_INVALID_EVT) || (usEvent == SipTxn::NON_INV_SER_INVALID_EVT))
    {
        pobjTxn->SipDelete();
        *pusError = EMSGERR_INVALIDSTATUSCODE;
        return SIP_FALSE;
    }

    // Update SIP server transaction's timers
    if (pObjTxnFsmData->m_pobjUserData != SIP_NULL)
    {
        SIP_VOID* pUserData = pObjTxnFsmData->m_pobjUserData->GetUserData();

        if (pUserData != SIP_NULL)
        {
            SipTimerContext* pTimerContext = ((SipTxnContext*)pUserData)->pSipTimerContext;

            if (pTimerContext != SIP_NULL)
            {
                SipTxnTimerValues& objTimerValues = const_cast<SipTxnTimerValues&>(
                        pobjTxn->GetSipTxnTimers());
                objTimerValues.UpdateSipTimers(pTimerContext->nTimerOptions,
                        pTimerContext->pTxnSipTxnTimers);
            }
        }
    }

    SIP_BOOL bStatus = pobjTxn->InvokeFsm((SIP_UINT16)usEvent, pObjTxnFsmData, pusError);

    pobjTxn->SipDelete();
    return bStatus;
}

PRIVATE SIP_BOOL    SipTxnHandler::HandleClientTxnRecv
(
    IN        SIP_INT32    eTxnType,
    IN        SipTxnKey        *pobjTxnKey,
    IN_OUT    SipTxnFsmData    *pObjTxnFsmData,
    OUT        SIP_UINT16    *pusError
)
{
    SipTxn        *pobjTxn = SIP_NULL;
    SIP_BOOL        bTxnExist = SIP_FALSE;

    /* Get Txn object from txn DB if exists */
    if (GetTxnObjFromDb(pobjTxnKey, &pobjTxn, &bTxnExist,pusError) == SIP_FALSE)
    {
        SIP_DEBUG_STACKBUG(ESIPTRACE_MODTXN,
                "HandleClientTxnRecv:GetTxnObjFromDb Fails \n",0,0);
        return SIP_FALSE;
    }

    if ((pobjTxn == SIP_NULL) || (bTxnExist == SIP_FALSE))
    {
        SIP_DEBUG_STACKBUG(ESIPTRACE_MODTXN,
                "HandleClientTxnRecv:No Txn Exists",0,0);
        return SIP_FALSE;
    }

    pobjTxn->increment();

    /* Received Response : Generate Event based on response code */
    SIP_UINT16 usStatusCode = pObjTxnFsmData->m_pobjSipMsgIn->GetStatusCode();
    SIP_UINT16    usEvent;
    if (eTxnType == SipTxn::INV_CLI_TXN)
    {
        usEvent = GetInvCliFsmEvt(usStatusCode);
    }
    else
    {
        usEvent= GetNonInvCliFsmEvt(usStatusCode);
    }

    if ((usEvent == SipTxn::INV_CLI_INVALID_EVT) || (usEvent == SipTxn::NON_INV_CLI_INVALID_EVT))
    {
        pobjTxn->SipDelete();

        *pusError = EMSGERR_INVALIDSTATUSCODE;
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                           "HandleClientTxnRecv, Invalid Status Code", SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    SIP_BOOL bStatus = pobjTxn->InvokeFsm(usEvent, pObjTxnFsmData, pusError);

    pobjTxn->SipDelete();

    return bStatus;
}

PRIVATE SIP_BOOL    SipTxnHandler::HandleServerTxnRecv
(
    IN        SIP_INT32    eTxnType,
    IN        SipTxnKey        *pobjTxnKey,
    IN_OUT    SipTxnFsmData    *pObjTxnFsmData,
    OUT        SIP_UINT16    *pusError
)
{
    SipTxn        *pobjTxn = SIP_NULL;
    SIP_BOOL        bTxnExist = SIP_FALSE;

    /* Get Txn object from txn DB if exists */
    if (GetTxnObjFromDb(pobjTxnKey, &pobjTxn,
                &bTxnExist,pusError) == SIP_FALSE)
    {
        SIP_DEBUG_STACKBUG(ESIPTRACE_MODTXN,
                "HandleClientTxnRecv:GetTxnObjFromDb Fails \n",0,0);
        return SIP_FALSE;
    }

    if (pobjTxn != SIP_NULL)
    {
        pobjTxn->increment();
    }

    SIP_UINT16    usEvent = SIP_ZERO;
    SIP_INT32    eMethod = pObjTxnFsmData->m_pobjSipMsgIn->GetMethodType();
    if (eTxnType == SipTxn::INV_SER_TXN)
    {
        if (eMethod == SipMessage::METHOD_INVITE)
        {
            usEvent = SipTxn::INV_SER_RECV_INV_REQ_EVT;
        }
        else
        {
            usEvent = SipTxn::INV_SER_RECV_ACK_REQ_EVT;

            SipTxnKey* pInviteTxnKey = (pobjTxn != SIP_NULL) ? pobjTxn->GetTxnKey() : SIP_NULL;

            if (pInviteTxnKey != SIP_NULL)
            {
                pobjTxnKey->SetRespCode(pInviteTxnKey->GetRespCode());
            }
        }
    }
    else
    {
        usEvent = SipTxn::NON_INV_SER_RECV_NON_INV_REQ_EVT;
    }

    if (pobjTxn == SIP_NULL)
    {
        SipTimerContext *pSipTimerContext = SIP_NULL;
        if (pObjTxnFsmData->m_pobjUserData != SIP_NULL)
        {
            SIP_VOID* objUserData = pObjTxnFsmData->m_pobjUserData->GetUserData();
            if (objUserData != SIP_NULL)
            {
                 pSipTimerContext = ((SipTxnContext*) objUserData)->pSipTimerContext;
            }
        }

        /* This is new request(INV or non-INV) case and hence create the transaction obj */
        pobjTxn = new SipTxn(eTxnType, pobjTxnKey,pObjTxnFsmData->m_pobjSipMsgIn,
            pSipTimerContext,pusError);

        if (pobjTxn == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN, "SipTxn: malloc fails", SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }
    }

    /* Invoking Server FSM */
    if (pobjTxn->InvokeFsm(usEvent, pObjTxnFsmData, pusError) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                           "HandleServerTxnRecv -> FSM handling fail eTxnType[%d] usEvent[%d]",
                           eTxnType,usEvent);

        pobjTxn->SipDelete();

        return SIP_FALSE;
    }

    pobjTxn->SipDelete();
    return SIP_TRUE;
}

PRIVATE SIP_VOID SipTxnHandler::NotifyTxnTermination
(
    IN    SipTxn        *pobjTxn
)
{
    SipUtil *pobjUtil = SipUtil_GetInstance();
    if (pobjUtil == SIP_NULL)
    {
        return;
    }

    ISipUserData *pobjUserData = pobjTxn->GetUserData();
    ISipTxnListener *pobjTxnListener = pobjUtil->GetTxnListener();

    if (pobjTxnListener != SIP_NULL)
    {
        pobjTxnListener->TxnTerminated(pobjUserData);
    }
}

PRIVATE SIP_BOOL SipTxnHandler::ValidateRecvTxn
(
    SipMessage    *pobjSipMsg,    IN
    SIP_INT32    *peTxnType        OUT
)
{
    SIP_INT32     eMsgType     = SipMessage::TYPE_INVALID;
    SIP_INT32    eMethodType    = SipMessage::METHOD_INVALID;

    /* Check if it's proper to start Transaction Ref: RFC 3261 8.1.1.*/
    if (CheckTxnMadatoryParams(pobjSipMsg, &eMsgType, &eMethodType) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "ValidateRecvTxn: CheckTxnMadatoryParams fails\n",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    *peTxnType  = GetTxnType(SipTxn::RECV,eMethodType,eMsgType);

    SIP_TRACE_NORMAL(ESIPTRACE_MODTXN,
                      "ValidateRecvTxn: TxnType[%s]",gszTxnTypeStr[*peTxnType],SIP_ZERO);

    if (*peTxnType == SipTxn::INVALID_TXN)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "ValidateRecvTxn: Invalid Txn",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    return SIP_TRUE;
}

PRIVATE SIP_BOOL SipTxnHandler::ValidateSendTxn
(
    IN    SipMessage    *pobjSipMsg,
    OUT    SIP_INT32    *peTxnType,
    OUT    SipTxnKey        **ppobjTxnKey,
    OUT    SIP_UINT16    *pusError
)
{
    SIP_INT32         eMsgType = SipMessage::TYPE_INVALID;
    SIP_INT32        eMethodType = SipMessage::METHOD_INVALID;

    /* Check if it's proper to start Transaction Ref: RFC 3261 8.1.1.*/
    if (CheckTxnMadatoryParams(pobjSipMsg, &eMsgType, &eMethodType) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SipTxnHandler: CheckTxnMadatoryParams fails\n",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_INT32 eTxnType = GetTxnType(SipTxn::SEND,eMethodType,eMsgType);

    if (eTxnType == SipTxn::INVALID_TXN)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SipTxnHandler: Invalid Txn\n",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }


    SipTxnKey *pobjTxnKey = new SipTxnKey(pobjSipMsg, pusError);
    if (pobjTxnKey == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SipTxnHandler:key Creation Fails \n",
                SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    else if (*pusError == EERR_MALLOCFAILED)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN,
                "SipTxnHandler:key Creation Fails \n",
                SIP_ZERO,SIP_ZERO);
        delete pobjTxnKey;
        return SIP_FALSE;
    }

    *peTxnType = eTxnType;
    *ppobjTxnKey = pobjTxnKey;

    return SIP_TRUE;
}

static SIP_INT32 GetNonInvCliFsmEvt(SIP_UINT16 usStatusCode)
{
    if (SIP_PROVISIONAL_RESP(usStatusCode))
    {
        return SipTxn::NON_INV_CLI_RECV_1XX_RESP_EVT;
    }
    else if (SIP_NONPROVISIONAL_RESP(usStatusCode))
    {
        return SipTxn::NON_INV_CLI_RECV_2XX_6XX_RESP_EVT;
    }
    else
    {
        return SipTxn::NON_INV_CLI_INVALID_EVT;
    }
}

static SIP_INT32 GetNonInvSerFsmEvt(SIP_UINT16 usStatusCode)
{
    if (SIP_PROVISIONAL_RESP(usStatusCode))
    {
        return SipTxn::NON_INV_SER_SEND_1XX_RESP_EVT;
    }
    else if (SIP_NONPROVISIONAL_RESP(usStatusCode))
    {
        return SipTxn::NON_INV_SER_SEND_2XX_6XX_RESP_EVT;
    }
    else
    {
        return SipTxn::NON_INV_SER_INVALID_EVT;
    }
}

static SIP_INT32 GetInvCliFsmEvt(SIP_UINT16 usStatusCode)
{
    if (SIP_PROVISIONAL_RESP(usStatusCode))
    {
        return SipTxn::INV_CLI_RECV_1XX_RESP_EVT;
    }
    else if (SIP_SUCCESSFUL_RESP(usStatusCode))
    {
        return SipTxn::INV_CLI_RECV_2XX_RESP_EVT;
    }
    else if (SIP_FAILURE_RESP(usStatusCode))
    {
        return SipTxn::INV_CLI_RECV_3XX_6XX_RESP_EVT;
    }
    else
    {
        return SipTxn::INV_CLI_INVALID_EVT;
    }
}

static SIP_INT32 GetInvSerFsmEvt(SIP_UINT16 usStatusCode)
{
    if (SIP_PROVISIONAL_RESP(usStatusCode))
    {
        return SipTxn::INV_SER_SEND_NON_100_PROV_RESP_EVT;
    }
    else if (SIP_SUCCESSFUL_RESP(usStatusCode))
    {
        return SipTxn::INV_SER_SEND_2XX_SUCCESS_RESP_EVT;
    }
    else if (SIP_FAILURE_RESP(usStatusCode))
    {
        return SipTxn::INV_SER_SEND_3XX_6XX_FAILURE_RESP_EVT;
    }
    else
    {
        return SipTxn::INV_SER_INVALID_EVT;
    }
}
