/*
   Author
   <table>
   date      author                description
   --------  --------------        ----------
   20170110  vijay.nair@           Created
   </table>

   Description

 */

#ifndef _SIP_TXN_FSM_DATA_H_
#define _SIP_TXN_FSM_DATA_H_

#include "txn/sip_txn_common.h"

#include "msg/SipMessage.h"
#include "transport/SipTransportInfo.h"

class SipTxnFsmData
{
    public:
        SipTxnFsmData();
        virtual ~SipTxnFsmData();
        SipTxnFsmData(SipMessage *pobjSipMsg,SipTransportParameter *pobjTranspParam,
            ISipUserData *pobjUserData);

        /************ INPUT Variables ***********/
        /* SIP Req/Resp Object */
        SipMessage            *m_pobjSipMsgIn;

        /*Transport parameter given by User */
        SipTransportParameter            *m_pobjTranspParam;

        /* User Specific Data or User Key Info. This is passed by the user */
        ISipUserData            *m_pobjUserData;

        /************ OUTPUT Variables ***********/

        /*Recv Txn handling: When Txn Sending Failure ACK or 100 Trying Response */
        SipMessage            *m_pobjSendSipMsg;

        /* Recv Txn handling: layer status on received side */
        SIP_INT32        eTxnStatus;

        /* Recv Txn handling: User while re-tranamission of message on receive of re-transmitted message */
        ISipUserData            *m_pobjOutUserData;

        /* Recv Txn handling: User while re-tranamission of message on receive of re-transmitted message */
        SipTransportInfo            *m_pobjTranspInfo;

        /* Used for both Send and Recv Txn */
        SIP_BOOL                bTxnTerminated;
        SIP_BOOL                bTxnCreated;
};

#endif
