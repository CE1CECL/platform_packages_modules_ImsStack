/*
   Author
   <table>
   date      author                description
   --------  --------------        ----------
   20170110  vijay.nair@           Created
   </table>

   Description

 */

#ifndef __SIP_TXN_FSM_H__
#define __SIP_TXN_FSM_H__

#include "txn/sip_txn_common.h"

/* Invite Client FSM */
extern SIP_BOOL (*gpfSipInvClientTxnFsm[SipTxn::INV_CLI_INVALID_ST+1]
                                        [SipTxn::INV_CLI_INVALID_EVT+1])
    (
     SipTxn        *pobjTxnInfo,
     SIP_VOID        *pvData,
     SIP_UINT16    *pusError
    );

/* Non Invite Client FSM */
extern SIP_BOOL (*gpfSipNonInvClientTxnFsm[SipTxn::NON_INV_CLI_INVALID_ST+1]
                                           [SipTxn::NON_INV_CLI_INVALID_EVT+1])
    (
     SipTxn        *pobjTxnInfo,
     SIP_VOID        *pvData,
     SIP_UINT16    *pusError
    );

/* Invite Server FSM */
extern SIP_BOOL (*gpfSipInvSerTxnFsm [SipTxn::INV_SER_INVALID_ST+1][SipTxn::INV_SER_INVALID_EVT+1])
    (
     SipTxn        *pobjTxnInfo,
     SIP_VOID        *pvData,
     SIP_UINT16    *pusError
    );

/* Non Invite Server FSM */
extern SIP_BOOL (*gpfSipNonInvSerTxnFsm[SipTxn::NON_INV_SER_INVALID_ST+1]
                                        [SipTxn::NON_INV_SER_INVALID_EVT+1])
    (
     SipTxn        *pobjTxnInfo,
     SIP_VOID        *pvData,
     SIP_UINT16    *pusError
    );


#endif //__SIP_TXN_FSM_H__
