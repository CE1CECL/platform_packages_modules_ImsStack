/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "SipPrivate.h"
#include "SipTxnContextData.h"
#include "SipStackState.h"
#include "SipFactoryProxy.h"
// SIP_PACKET_TRACKER
#include "SipMessage.h"
#include "ISipTransactionStateListener.h"
#include "SipRtConfigUtils.h"
// SIP_MESSAGE_TRACKER
#include "SipMessageTracker.h"
// SIP_PACKET_TRACKER
#include "SipPacketTracker.h"
// SIP_IPSEC_STATE
#include "SipIpSecState.h"
#include "SipTransport.h"
#include "SipTransactionState.h"

__IMS_TRACE_TAG_SIP__;

PUBLIC
SipTransactionState::SipTransactionState() :
        RCObject(),
        nType(0),
        nClass(CLASS_NONE),
        nCSeqNumber(1)
        // MULTI_REG_SIP_PROFILE
        ,
        pSIPProfile(IMS_NULL),
        piListener(IMS_NULL),
        pDialogEx(IMS_NULL),
        pTransport(IMS_NULL),
        pstMessage(IMS_NULL),
        pstLastMessage(IMS_NULL),
        pstTxnKey(IMS_NULL),
        pstRPRTxnKey(IMS_NULL)
{
}

PUBLIC
SipTransactionState::SipTransactionState(IN SipDialogEx* pDialogEx_) :
        RCObject(),
        nType(0),
        nClass(CLASS_NONE),
        nCSeqNumber(1)
        // MULTI_REG_SIP_PROFILE
        ,
        pSIPProfile(IMS_NULL),
        piListener(IMS_NULL),
        pDialogEx(pDialogEx_),
        pTransport(IMS_NULL),
        pstMessage(IMS_NULL),
        pstLastMessage(IMS_NULL),
        pstTxnKey(IMS_NULL),
        pstRPRTxnKey(IMS_NULL)
{
}

PUBLIC
SipTransactionState::SipTransactionState(IN const SipTransactionState& objRHS) :
        RCObject(objRHS),
        nType(objRHS.nType),
        nClass(objRHS.nClass),
        nCSeqNumber(objRHS.nCSeqNumber)
        // MULTI_REG_SIP_PROFILE
        ,
        pSIPProfile(objRHS.pSIPProfile),
        piListener(objRHS.piListener),
        pDialogEx(objRHS.pDialogEx),
        pTransport(IMS_NULL),
        pstMessage(IMS_NULL),
        pstLastMessage(IMS_NULL),
        pstTxnKey(IMS_NULL),
        pstRPRTxnKey(IMS_NULL)
{
    // NOTE: If reference count is not used, you MUST implement this copy constructor
}

PUBLIC VIRTUAL SipTransactionState::~SipTransactionState()
{
    //---------------------------------------------------------------------------------------------

    if (pTransport != IMS_NULL)
        delete pTransport;

    SipStack::FreeMessage(pstLastMessage);
    SipStack::FreeMessage(pstMessage);
    SipStack::FreeTxnKey(pstTxnKey);
    SipStack::FreeTxnKey(pstRPRTxnKey);

    IMS_TRACE_D("Destructor :: SipTransactionState", 0, 0, 0);
}

/*

Remarks

*/
PUBLIC VIRTUAL void SipTransactionState::Abort()
{
    ::SipTxnKey* pstAbortTxnKey = pstTxnKey;

    //---------------------------------------------------------------------------------------------

    if ((pstAbortTxnKey == IMS_NULL) && (nType == TYPE_CLIENT) && (pstLastMessage != IMS_NULL))
    {
        pstAbortTxnKey = SipStack::CreateTxnKey(pstLastMessage, SipStack::SIP_TXN_MSG_SENT);
        pstTxnKey = pstAbortTxnKey;
    }

    // SIP_IPSEC_STATE
    SipFactoryProxy* pFactoryProxy = SipFactoryProxy::GetInstance();

    if (pFactoryProxy->IsIpSecStateEnabled(GetSlotId()))
    {
        SipIpSecState* pIPSecState = pFactoryProxy->GetIpSecState(GetSlotId());
        pIPSecState->NotifyTransactionAborted(pstTxnKey);
    }

    // Invoke the stack API to abort the transaction.
    (void)SipStackState::GetInstance()->AbortTransaction(pstAbortTxnKey, this);

    if (pstRPRTxnKey != IMS_NULL)
    {
        (void)SipStackState::GetInstance()->AbortTransaction(pstRPRTxnKey, this);
        SipStack::FreeTxnKey(pstRPRTxnKey);
    }

    SipStack::FreeTxnKey(pstTxnKey);
}

/*
 Terminates SIP transaction if it's not available anymore.

Remarks

*/
PUBLIC VIRTUAL void SipTransactionState::Terminate()
{
    IMS_TRACE_D("STS::Terminate", 0, 0, 0);

    // If there is ongoing transaction, it will be terminated promptly.
    (void)SipStackState::GetInstance()->AbortTransaction(pstTxnKey, this);
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_SINT32 SipTransactionState::CheckMessageValidity()
{
    //---------------------------------------------------------------------------------------------

    return SipPrivate::MESSAGE_VALID;
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_BOOL SipTransactionState::FormMessage()
{
    // no-op
    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_BOOL SipTransactionState::InitTxnDetails(IN CONST SipTransactionState* pTState)
{
    //---------------------------------------------------------------------------------------------

    if (pTState == IMS_NULL)
    {
        return IMS_FALSE;
    }

    nClass = pTState->nClass;
    nCSeqNumber = pTState->nCSeqNumber;
    // MULTI_REG_SIP_PROFILE
    pSIPProfile = pTState->pSIPProfile;
    pDialogEx = pTState->pDialogEx;

    if (!pTransport->InitTransportDetails(pTState->pTransport))
    {
        IMS_TRACE_E(0, "Initializing SIP transport details failed", 0, 0, 0);
        return IMS_FALSE;
    }

    SipStack::FreeMessage(pstLastMessage);
    SipStack::FreeMessage(pstMessage);
    SipStack::FreeTxnKey(pstTxnKey);
    SipStack::FreeTxnKey(pstRPRTxnKey);

    pstMessage = pTState->pstMessage;
    SipStack::AddReference(pstMessage);

    pstLastMessage = pTState->pstLastMessage;
    SipStack::AddReference(pstLastMessage);

    pstTxnKey = pTState->pstTxnKey;
    SipStack::AddReference(pstTxnKey);

    pstRPRTxnKey = pTState->pstRPRTxnKey;
    SipStack::AddReference(pstRPRTxnKey);

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL void SipTransactionState::NotifyTimerExpired()
{
    //---------------------------------------------------------------------------------------------

    if (piListener != IMS_NULL)
        piListener->TransactionState_TimerExpired();
}

/*
 It's invoked by SIP stack after sending SIP message successfully.

Remarks

*/
PUBLIC VIRTUAL void SipTransactionState::PostProcessMessageSentByStack(
        IN ::SipMessage* pstSipMsg, IN const ByteArray& objBuffer)
{
    SipFactoryProxy* pFactoryProxy = SipFactoryProxy::GetInstance();

    // SIP_PACKET_TRACKER
    if (pFactoryProxy->IsPacketTrackerEnabled(GetSlotId()))
    {
        SipPacketTracker* pPacketTracker = pFactoryProxy->GetPacketTracker(GetSlotId());
        sipcore::SipMessage objSIPMsg(pstSipMsg);

        pPacketTracker->NotifyMessageSent(&objSIPMsg, objBuffer, IMS_FALSE);
    }

    // SIP_IPSEC_STATE
    if (pFactoryProxy->IsIpSecStateEnabled(GetSlotId()))
    {
        SipIpSecState* pIPSecState = pFactoryProxy->GetIpSecState(GetSlotId());
        const SipTransportAddress& objNearEnd = pTransport->GetAddress(SipTransport::TA_NEAR);
        const SipTransportAddress& objFarEnd = pTransport->GetAddress(SipTransport::TA_FAR);

        pIPSecState->NotifyMessageSent(objNearEnd, objFarEnd, pstSipMsg);
    }
}

/*
 It's invoked by SIP stack before sending SIP message.

Remarks

*/
PUBLIC VIRTUAL void SipTransactionState::PreProcessMessageSentByStack(IN ::SipMessage* pstSipMsg)
{
    SipFactoryProxy* pFactoryProxy = SipFactoryProxy::GetInstance();

    // SIP_MESSAGE_TRACKER
    if (pFactoryProxy->IsMessageTrackerEnabled(GetSlotId()))
    {
        SipMessageTracker* pMessageTracker = pFactoryProxy->GetMessageTracker(GetSlotId());
        const SipMethod objMethod = SipStack::GetMethod(pstSipMsg);

        if (SipStack::IsRequestMessage(pstSipMsg))
        {
            pMessageTracker->NotifyMessageSent(
                    objMethod, 0, pDialogEx->GetDialogState()->GetCallId());
        }
        else
        {
            pMessageTracker->NotifyMessageSent(objMethod, SipStack::GetStatusCode(pstSipMsg),
                    pDialogEx->GetDialogState()->GetCallId());
        }
    }

    // LOG_EXCLUDING_SERVER_INFO
    if (SipStack::IsRequestMessage(pstSipMsg) &&
            SipRtConfigUtils::IsMessageHiddenInLog(GetSlotId()))
    {
        SipStack::DisplayUnknownHeaders(pstSipMsg);
    }

    // Reset the retransmission flag when sending INVITE response
    if ((this->nType == TYPE_SERVER) && (this->nClass == CLASS_INVITE))
    {
        pTransport->InitRetransmissionFlag();
    }
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_BOOL SipTransactionState::Send(IN SipTimerValues* pTV /* = IMS_NULL */)
{
    //---------------------------------------------------------------------------------------------

    return Send(pstMessage, pTV);
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_RESULT SipTransactionState::RetransmitMessage()
{
    ByteArray objBuffer;

    //---------------------------------------------------------------------------------------------

    if (pTransport == IMS_NULL)
    {
        IMS_TRACE_E(0, "Transport is already destroyed", 0, 0, 0);
        return IMS_FAILURE;
    }

    if (!pTransport->EncodeMessage(pstMessage, objBuffer))
    {
        IMS_TRACE_E(0, "Encoding ACK or 2xx to INVITE request (Retransmission) failed", 0, 0, 0);
        return IMS_FAILURE;
    }

    if (!pTransport->SendToNetwork(objBuffer.GetData(), objBuffer.GetLength(), IMS_FALSE))
    {
        IMS_TRACE_E(0, "Retransmitting ACK or 2xx to INVITE request failed", 0, 0, 0);
        return IMS_FAILURE;
    }

    // SIP_PACKET_TRACKER
    SipFactoryProxy* pFactoryProxy = SipFactoryProxy::GetInstance();

    if (pFactoryProxy->IsPacketTrackerEnabled(GetSlotId()))
    {
        SipPacketTracker* pPacketTracker = pFactoryProxy->GetPacketTracker(GetSlotId());
        sipcore::SipMessage objSIPMsg(pstMessage);

        pPacketTracker->NotifyMessageSent(&objSIPMsg, objBuffer, IMS_TRUE);
    }

    return IMS_SUCCESS;
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_BOOL SipTransactionState::UpdateTransportDetails()
{
    //---------------------------------------------------------------------------------------------

    if (!pTransport->UpdateDestinationInfo(pstMessage))
        return IMS_FALSE;

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 SipTransactionState::GetSlotId() const
{
    return pTransport->GetSlotId();
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipTransactionState::IsIpSecRequired() const
{
    //---------------------------------------------------------------------------------------------

    return pTransport->IsIpSecRequired();
}

/*

Remarks
 MULTI_REG_SIP_PROFILE
*/
PUBLIC
void SipTransactionState::SetSipProfile(IN SipProfile* pProfile)
{
    //---------------------------------------------------------------------------------------------

    this->pSIPProfile = pProfile;
}

/*

Remarks

*/
PUBLIC
void SipTransactionState::SetTransactionListener(IN ISipTransactionStateListener* piListener)
{
    //---------------------------------------------------------------------------------------------

    this->piListener = piListener;
}

/*

Remarks

*/
PUBLIC
void SipTransactionState::SetTransportListener(IN ISipTransportErrorListener* piListener)
{
    //---------------------------------------------------------------------------------------------

    pTransport->SetListener(piListener);
}

/*

Remarks
 RFC5626_FLOW_CONTROL, MULTI_REG_TRANSPORT
*/
PUBLIC
void SipTransactionState::SetTransportTuple(IN CONST IPAddress& objIPA, IN IMS_SINT32 nPortS,
        IN IMS_SINT32 nPortC, IN IMS_SINT32 nPortFC /* = 0xFFFF */,
        IN IMS_SINT32 nTransportExt /* = 0 (ANY) */)
{
    //---------------------------------------------------------------------------------------------

    pTransport->SetTransportTuple(objIPA, nPortS, nPortC, nPortFC, nTransportExt);
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipTransactionState::SendToNetwork(IN CONST IMS_BYTE* pBuffer, IN IMS_SINT32 nBuffLen)
{
    //---------------------------------------------------------------------------------------------

    if (pTransport == IMS_NULL)
        return IMS_FALSE;

    return pTransport->SendToNetwork(pBuffer, nBuffLen);
}

/*

Remarks

*/
PUBLIC
void SipTransactionState::UpdateMessage(IN ::SipMessage* pstMessage)
{
    //---------------------------------------------------------------------------------------------

    SipStack::FreeMessage(this->pstMessage);

    this->pstMessage = pstMessage;
    SipStack::AddReference(this->pstMessage);
}

/*

Remarks

*/
PROTECTED VIRTUAL SipTransactionState* SipTransactionState::Clone()
{
    //---------------------------------------------------------------------------------------------

    // The subclass MUST implement this method to create a new SipTransactionState (client/server)

    return IMS_NULL;
}

/*

Remarks

*/
PROTECTED
IMS_BOOL SipTransactionState::Send(IN ::SipMessage* pSipMsg, IN SipTimerValues* pTV)
{
    SipTransportParameter objTranspParam;

    IMS_TRACE_D("Send", 0, 0, 0);

    /* Fill transport details */
    IMS_SINT32 nType = pTransport->GetProtocol(SipTransport::TA_FAR);

    if (nType == SipTransportAddress::PROTOCOL_TCP)
        objTranspParam.setTranspProtocol(SipTransportInfo::PROTOCOL_TCP);
    else if (nType == SipTransportAddress::PROTOCOL_TLS)
        objTranspParam.setTranspProtocol(SipTransportInfo::PROTOCOL_TLS);
    else
        objTranspParam.setTranspProtocol(SipTransportInfo::PROTOCOL_UDP);

    SipTransportAddress objTA = pTransport->GetAddress(SipTransport::TA_FAR);

    objTranspParam.setHostAddress(objTA.GetIpAddress().ToString().GetStr());
    objTranspParam.setPort((IMS_UINT16)objTA.GetPort());

    if (objTA.GetIpAddress().IsIPv4Address() == IMS_TRUE)
    {
        objTranspParam.setTanspIpType(SipTransportInfo::NETWORK_IPV4);
    }
    else
    {
        objTranspParam.setTanspIpType(SipTransportInfo::NETWORK_IPV6);
    }

    // Prepare User Data
    SipTxnContext* pstTxnContext = SipStack::CreateTxnContext();

    if (pstTxnContext == IMS_NULL)
    {
        return IMS_FALSE;
    }

    SetTimerValues(pTV, pstTxnContext);

    SipTxnContextData* pTxnContextData = new SipTxnContextData();

    if (pTxnContextData != IMS_NULL)
    {
        IMS_BOOL bIsTxnStateSet = IMS_FALSE;

        // FIX_TXN_HANDLING_ON_401_407_TO_INVITE
        if ((this->nType == TYPE_CLIENT) && (this->nClass == CLASS_INVITE))
        {
            IMS_SINT32 nStatusCode = SipStack::GetStatusCode(pSipMsg);

            if ((nStatusCode == SipStatusCode::SC_401) || (nStatusCode == SipStatusCode::SC_407))
            {
                const SipMethod objMethod = SipStack::GetMethod(pSipMsg);

                if (objMethod.Equals(SipMethod::ACK))
                {
                    IMS_TRACE_D("AUTH_CHALLENGE_TO_INVITE :: ACK (%d) "
                                "& transaction state is cloned",
                            nStatusCode, 0, 0);

                    bIsTxnStateSet = IMS_TRUE;
                    pTxnContextData->SetTxnState(this->Clone());
                }
            }
        }

        if (!bIsTxnStateSet)
        {
            pTxnContextData->SetTxnState(this);
        }
    }

    pstTxnContext->pTxnContextData = (SIP_VOID*)pTxnContextData;

    ISipUserData objUserData;
    objUserData.SetUserData((SIP_VOID*)pstTxnContext);

    ByteArray objBuffer;

    // Form a raw SIP message
    if (!pTransport->EncodeMessage(pSipMsg, objBuffer))
    {
        SipStack::DestroyTxnContext(pstTxnContext);
        IMS_TRACE_E(0, "Encoding SIP message failed", 0, 0, 0);
        return IMS_FALSE;  // throw exception : syntax error
    }

    /* Reserver transport resources */
    if (!pTransport->ReserveResource(GetSipProfile()))
    {
        SipStack::DestroyTxnContext(pstTxnContext);
        IMS_TRACE_E(0, "Reserving the transport resource failed", 0, 0, 0);
        return IMS_FALSE;  // throw exception : network not available
    }

    // SIP_MESSAGE_TRACKER
    SipFactoryProxy* pFactoryProxy = SipFactoryProxy::GetInstance();

    if (pFactoryProxy->IsMessageTrackerEnabled(GetSlotId()))
    {
        SipMessageTracker* pMessageTracker = pFactoryProxy->GetMessageTracker(GetSlotId());
        const SipMethod objMethod = SipStack::GetMethod(pSipMsg);

        if (SipStack::IsRequestMessage(pSipMsg))
        {
            pMessageTracker->NotifyMessageSent(
                    objMethod, 0, pDialogEx->GetDialogState()->GetCallId());
        }
        else
        {
            pMessageTracker->NotifyMessageSent(objMethod, SipStack::GetStatusCode(pSipMsg),
                    pDialogEx->GetDialogState()->GetCallId());
        }
    }

    // LOG_EXCLUDING_SERVER_INFO
    if (SipStack::IsRequestMessage(pSipMsg) && SipRtConfigUtils::IsMessageHiddenInLog(GetSlotId()))
    {
        SipStack::DisplayUnknownHeaders(pSipMsg);
    }

    // Reset the retransmission flag when sending INVITE response
    if ((this->nType == TYPE_SERVER) && (this->nClass == CLASS_INVITE))
    {
        pTransport->InitRetransmissionFlag();
    }

    ::SipTxnKey* pTxnKey = IMS_NULL;
    IMS_UINT16 nError = 0;
    SIP_BOOL bStatus = SipStackManager::GetInstance()->SendMsg(pSipMsg, &objTranspParam,
            &objUserData, reinterpret_cast<SIP_CHAR*>(objBuffer.GetData()), objBuffer.GetLength(),
            &pTxnKey, &nError);

    if (objUserData.GetUserData() != IMS_NULL)
    {
        if (objUserData.GetDeleteFlag() == SIP_TRUE)
        {
            SipTxnContext* pOldTxnContext =
                    reinterpret_cast<SipTxnContext*>(objUserData.GetUserData());

            if (pOldTxnContext != pstTxnContext)
            {
                SipStack::DestroyTxnContext(pOldTxnContext);
            }
        }

        SipStack::DestroyTxnContext(pstTxnContext);
    }

    if (bStatus == SIP_FALSE)
    {
        IMS_TRACE_E(nError, "Sending SIP message to the transaction layer failed", 0, 0, 0);
        return IMS_FALSE;
    }

    // SIP_PACKET_TRACKER
    if (pFactoryProxy->IsPacketTrackerEnabled(GetSlotId()))
    {
        SipPacketTracker* pPacketTracker = pFactoryProxy->GetPacketTracker(GetSlotId());
        sipcore::SipMessage objSIPMsg(pSipMsg);

        pPacketTracker->NotifyMessageSent(&objSIPMsg, objBuffer, IMS_FALSE);
    }

    // SIP_IPSEC_STATE
    if (pFactoryProxy->IsIpSecStateEnabled(GetSlotId()))
    {
        SipIpSecState* pIPSecState = pFactoryProxy->GetIpSecState(GetSlotId());
        const SipTransportAddress& objNearEnd = pTransport->GetAddress(SipTransport::TA_NEAR);
        const SipTransportAddress& objFarEnd = pTransport->GetAddress(SipTransport::TA_FAR);

        pIPSecState->NotifyMessageSent(objNearEnd, objFarEnd, pSipMsg);
    }

    // Store the transaction key here.
    if (pTxnKey != IMS_NULL)
    {
        if (SipStack::IsMessageRpr(pSipMsg))
        {
            SipStack::FreeTxnKey(pstRPRTxnKey);
            pstRPRTxnKey = pTxnKey;
        }
        else
        {
            SipStack::FreeTxnKey(pstTxnKey);
            pstTxnKey = pTxnKey;
        }

        if (pstTxnKey->GetTxnType() == ETXN_INVSERTXN)
        {
            // If the method is INVITE, then store the txn key in the InvTxnKey.
            // This will be used when the application calls AbortCall().
            IMS_SINT32 nStatusCode = SipStack::GetStatusCode(pSipMsg);

            if (nStatusCode != SipStatusCode::SC_INVALID)
                pstTxnKey->SetRespCode(static_cast<SIP_UINT16>(nStatusCode));
        }
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PROTECTED
void SipTransactionState::SetTimerValues(
        IN SipTimerValues* pTV, IN_OUT SipTxnContext*& pstTxnContext)
{
    //---------------------------------------------------------------------------------------------

    SipStack::SetTimerValues(pTV, pstTxnContext);
}

/*

Remarks

*/
PROTECTED
void SipTransactionState::SetFlowControlOption(IN CONST SipMethod& objMethod)
{
    if (objMethod.Equals(SipMethod::REGISTER))
    {
        pTransport->SetTransactionFlowControlRequired(IMS_FALSE);
    }
    else
    {
        pTransport->SetTransactionFlowControlRequired(IMS_TRUE);
    }
}
