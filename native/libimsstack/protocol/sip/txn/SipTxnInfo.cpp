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
#include "txn/SipTxnInfo.h"

SipTxnInfo::SipTxnInfo    ():
    m_pobjSendSipMsg(SIP_NULL),m_pobjUserData(SIP_NULL),
    m_pobjTranspInfo(SIP_NULL),eTxnStatus(SipTxn::STATUS_INVALID),
    bTxnTerminated(SIP_FALSE),bTxnCreated(SIP_FALSE)
{
}

SipTxnInfo::~SipTxnInfo()
{
    if (m_pobjSendSipMsg != SIP_NULL)
    {
        m_pobjSendSipMsg->SipDelete();
    }
}
