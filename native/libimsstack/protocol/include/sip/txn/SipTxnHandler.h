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

#ifndef __SIP_TXN_HANDLER_H__
#define __SIP_TXN_HANDLER_H__

#include "txn/sip_txn_common.h"
#include "txn/SipTxn.h"
#include "txn/SipTxnFsmData.h"
#include "txn/SipTxnInfo.h"
#include "msg/SipMessage.h"

/* Transaction Key Used to Match Response to Request vice versa */
class SipTxnHandler
{
    public:
        SipTxnHandler(){};
        virtual ~SipTxnHandler(){};

    private:
        /* Based on Send/Receiving, Method and Request/Respose type Transaction is returnred */
        SIP_INT32 GetTxnType
            (
             SIP_INT32    eMsgDir, /* ETXN_SEND or ETXN_RECV */
             SIP_INT32    eMethodType,
             SIP_INT32    eMsgType
            );

        SIP_BOOL GetTxnObjFromDb
            (
             SipTxnKey     *pobjTxnKey,
             SipTxn        **ppobjTxn,
             SIP_BOOL     *pbTxnExist,
             SIP_UINT16    *pusError
            );

        SIP_BOOL GetTxnObjFromDb
            (
             SipTxnKey     *pobjTxnKey,
             SipTxn        **ppobjTxn,
             SipTxnKey     **ppobjOutTxnKey,
             SIP_BOOL     *pbTxnExist,
             SIP_UINT16   *pusError
            );

        PRIVATE SIP_BOOL ValidateSendTxn
            (
             IN    SipMessage    *pobjSipMsg,
             OUT    SIP_INT32     *peTxnType,
             OUT    SipTxnKey        **ppobjTxnKey,
             OUT    SIP_UINT16    *pusError
            );

        /* method validates txn params from sip message and returns txn type */
        PRIVATE SIP_BOOL ValidateRecvTxn
            (
             IN    SipMessage    *pobjSipMsg,
             OUT    SIP_INT32    *peTxnType
            );

        /* invoking client FSM to send request. returns new txn object*/
        PRIVATE SIP_BOOL    HandleClientTxnSend
            (
             IN    SIP_INT32    eTxnType,
             IN    SipTxnKey        *pobjTxnKey,
             IN    SipTxnFsmData    *pObjTxnFsmData,
             OUT    SIP_UINT16    *pusError
            );

        /* invokes server FSM to send response */
        PRIVATE SIP_BOOL HandleServerTxnSend
            (
             IN    SIP_INT32    eTxnType,
             IN    SipTxnKey        *pobjTxnKey,
             IN    SipTxnFsmData    *pObjTxnFsmData,
             IN    SIP_UINT16    *pusError
            );

        /* invoking client FSM to to handle received response. */
        PRIVATE SIP_BOOL HandleClientTxnRecv
            (
             IN    SIP_INT32    eTxnType,
             IN    SipTxnKey        *pobjTxnKey,
             IN    SipTxnFsmData    *pObjTxnFsmData,
             OUT SIP_UINT16    *pusError
            );

        /* create server FSM to handle new received request. returns new txn object */
        PRIVATE SIP_BOOL    HandleServerTxnRecv
            (
             IN    SIP_INT32    eTxnType,
             IN    SipTxnKey        *pobjTxnKey,
             IN    SipTxnFsmData    *pObjTxnFsmData,
             OUT    SIP_UINT16    *pusError
            );

        /* Notifies to Transaction User using registered listener */
        PRIVATE SIP_VOID NotifyTxnTermination
            (
             SipTxn            *pobjTxn
            );

    public:

        SIP_BOOL OnSendTxn
            (
             SipMessage    *pobjSipMsg,         IN_OUT
             SipTransportParameter    *pobjTranspParam,    IN
             ISipUserData    *pobjUserData,        IN
             SipTxnKey        **ppobjTxnKey,        OUT
             SipTxnInfo    *pobjTxnInfo,        OUT
             SIP_UINT16    *pusError
            );

        SIP_BOOL OnRecvTxn
            (
             IN    SipMessage                *pobjSipMsg,
             IN    SipTxnKey                 *pobjTxnKey,
             IN    ISipUserData                *pobjUserData,
             OUT SipTxnInfo                *pobjTxnInfo,
             OUT SIP_UINT16                *pusError
            );

        SIP_BOOL UpdateTxnDetails
            (
             SipTxnKey        *pobjTxnKey,
             SipTransportInfo    *pobjTranspInfo,
             SIP_UINT16    *pusError
            );

        SIP_BOOL OnRecvTranspError
            (
             SIP_INT32    eTransErrro,
             SipTxnKey            *pobjTxnKey,
             SIP_UINT16        *pusError
            );

        SIP_BOOL OnSendTranspError
            (
             SipTxnKey            *pobjTxnKey
            );

        SIP_BOOL TerminateTxn
            (
             SipTxnKey        *pobjTxnKey
            );

        SIP_BOOL DeleteTxn
            (
             SipTxnKey        *pobjTxnKey
            );

};
#endif //__SIP_TXN_HANDLER_H__
