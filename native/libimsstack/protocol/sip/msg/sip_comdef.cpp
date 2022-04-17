/******************************************************************************
 * Project Name   : SIP_RTP
 * Group    : IP-CS [MSG-2]
 * Security   : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename    : sip_comdef.cpp
 * Purpose     :
 * Platform    : Windows OR Android
 * Author(s)   :
 * E-mail id.    : name@
 * Creation date    : Month. Date,10
 *
 * Edit History     Modification         Description(s)
 *
 * Date      Name    Version    Bug-ID    Description
 * ----------    ----------    -------    ------    -------------
 * Month. Date,10    Name       0.0a    Initial creation
 *****************************************************************************/

/*****************************************************************************
  Header Inclusions
 *****************************************************************************/
#include "msg/sip_comdef.h"
#include "msg/SipMessage.h"
#include "SipTrace.h"
#include "sip_debug.h"
#include "platform/sip_pf_string.h"
#include "platform/sip_pf_memory.h"


SIP_UINT32 SipNPower(SIP_UINT16 usBase,SIP_UINT16 usIndex)
{
    SIP_UINT32 uiRes = SIP_ONE;

    while (usIndex)
    {
        uiRes = uiRes * usBase;
        usIndex = usIndex -1;
    }

    return uiRes;
}

SIP_BOOL SipMemCheck(SIP_VOID *pvData, SIP_UINT16 *pusError)
{
    if (pvData == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN, "New fails", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    if (*pusError == E_ERR_PF_MALLOCFAILED)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN, "Malloc fails", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    return SIP_TRUE;
}

/******************************************************************************
 * Function name    : CheckTxnMadatoryParams
 * Description    : Check Key Parameter to Start a Txn [Friend Function]
 * Return type    : SIP_BOOL
 *          Success/Failure
 * Argument     : pusError: <Out>
 *          Error code.
 * Preconditions/
 * Side Effects    : None
 *****************************************************************************/
    SIP_BOOL CheckTxnMadatoryParams
(
 SipMessage    *pobjSipMsg,
 SIP_INT32   *peMsgType,
 SIP_INT32    *peMethodType
 )
{
    SIP_INT32 eMsgType = pobjSipMsg->GetMsgType();
    if (eMsgType == SipMessage::TYPE_INVALID)
    {
        return SIP_FALSE;
    }

    SIP_INT32 eMethodType = pobjSipMsg->GetMethodType();

    if (eMethodType == SipMessage::METHOD_INVALID)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN, "Invalid Method Type(%d != %d)",
                eMethodType, pobjSipMsg->GetMethodType());
        return SIP_FALSE;
    }

    /* Step 1: Check for Mandatory Headers for Transaction key */
    /* Check for To Hdr */
    if (pobjSipMsg->HasHeader(SipHeaderBase::TO) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN, "'To' Hdr not Found", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    else if (pobjSipMsg->HasHeader(SipHeaderBase::FROM) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN, "'From' Hdr not Found", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    else if (pobjSipMsg->HasHeader(SipHeaderBase::CSEQ) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN, "'CSeq' Hdr not Found", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    else if (pobjSipMsg->HasHeader(SipHeaderBase::CALL_ID) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN, "'CallID' Hdr not Found", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }
    else if (pobjSipMsg->HasHeader(SipHeaderBase::VIA) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTXN, "'Via' Hdr not Found", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    /* Step 2: Check for Request line */
    if (eMsgType == SipMessage::REQ_TYPE)
    {
        if (pobjSipMsg->IsReqLineExists() == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN, "IsReqLineExists Fails", SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
    }
    else
    {
        if (pobjSipMsg->IsStatusLineExists() == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODTXN, "IsStatusLineExists Fails", SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
    }
    *peMsgType    =  eMsgType;
    *peMethodType  = eMethodType;

    return SIP_TRUE;
}

SIP_UINT32 GetRSeqNum
(
 SipMessage    *pstMessage,
 SIP_INT32 eHdrType
)
{
    SIP_UINT32 nRSeqNum = 0;

    if (pstMessage != SIP_NULL &&
        (eHdrType == SipHeaderBase::RSEQ || eHdrType == SipHeaderBase::RACK))
    {
        SipHeaderBase* pHeader = pstMessage->GetHeader(eHdrType, 0);

        if (pHeader != SIP_NULL)
        {
            if (eHdrType == SipHeaderBase::RSEQ)
            {
                SipRSeqHeader* pRSeq = (SipRSeqHeader*)pHeader;
                nRSeqNum = pRSeq->GetRSeqValue();
            }
            else
            {
                SipRAcKHeader* pRAck = (SipRAcKHeader*)pHeader;
                nRSeqNum = pRAck->GetResponseNum();
            }

            pHeader->SipDelete();
        }
    }

    return nRSeqNum;
}
