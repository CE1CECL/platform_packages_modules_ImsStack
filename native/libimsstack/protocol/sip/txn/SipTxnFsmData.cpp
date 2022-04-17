/*
   Author
   <table>
   date      author                description
   --------  --------------        ----------
   20170110  vijay.nair@           Created
   </table>

   Description

 */

#include "txn/SipTxn.h"
#include "txn/SipTxnFsmData.h"

SipTxnFsmData::SipTxnFsmData()
    : m_pobjSipMsgIn(SIP_NULL)
    , m_pobjTranspParam(SIP_NULL)
    , m_pobjUserData(SIP_NULL)
    , m_pobjSendSipMsg(SIP_NULL)
    , eTxnStatus(SipTxn::STATUS_INVALID)
    , m_pobjOutUserData(SIP_NULL)
    , m_pobjTranspInfo(SIP_NULL)
    , bTxnTerminated(SIP_FALSE)
    , bTxnCreated(SIP_FALSE)
{
}

SipTxnFsmData::~SipTxnFsmData()
{
}

SipTxnFsmData::SipTxnFsmData(SipMessage *pobjSipMsg,
        SipTransportParameter *pobjTranspParam, ISipUserData *pobjUserData)
    : m_pobjSipMsgIn(pobjSipMsg)
    , m_pobjTranspParam(pobjTranspParam)
    , m_pobjUserData(pobjUserData)
    , m_pobjSendSipMsg(SIP_NULL)
    , eTxnStatus(SipTxn::STATUS_INVALID)
    , m_pobjOutUserData(SIP_NULL)
    , m_pobjTranspInfo(SIP_NULL)
    , bTxnTerminated(SIP_FALSE)
    , bTxnCreated(SIP_FALSE)
{
}
