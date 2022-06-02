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
#include "ServiceNetwork.h"
#include "SystemConfig.h"
#include "ByteArray.h"
#include "SipPrivate.h"
#include "SipDebug.h"
#include "SipConfigProxy.h"
#include "SipManager.h"
#include "SipWakeLock.h"
#include "SipFactoryProxy.h"
#include "SipRtConfigUtils.h"
#include "SipMessage.h"
#include "SipTransport.h"
#include "SipClientTransactionState.h"
#include "SipServerTransactionState.h"
#include "ISipServerTransactionStateListener.h"
#include "SipRoutingRejectNotifier.h"
#include "SipPacketTracker.h"
#include "SipMessageHandler.h"

__IMS_TRACE_TAG_SIP__;

PRIVATE
SipMessageHandler::SipMessageHandler() {}

PUBLIC
SipMessageHandler::~SipMessageHandler() {}

PUBLIC GLOBAL SipMessageHandler* SipMessageHandler::GetInstance()
{
    static SipMessageHandler* pSIPMsgHandler = IMS_NULL;

    //---------------------------------------------------------------------------------------------

    if (pSIPMsgHandler == IMS_NULL)
    {
        pSIPMsgHandler = new SipMessageHandler();
    }

    return pSIPMsgHandler;
}

PRIVATE VIRTUAL void SipMessageHandler::Transport_PacketReceived(IN IMS_SINT32 nSlotId,
        IN CONST ByteArray& objBuffer, IN CONST SipTransportAddress& objNearEnd,
        IN CONST SipTransportAddress& objFarEnd)
{
    ::SipMessage* pstMessage = IMS_NULL;

    //---------------------------------------------------------------------------------------------

    // 3 Updates the operations in case of an exceptional operation

    // No need to call DecodeMessage() repeatedly till the entire buffer is parsed.
    // We do not take care of the situation where the multiple messages arrive
    // on a single buffer (as in the case of TCP).
    // Because, the checking of message completion will be evaluated in the SIP socket layer.

    if (!SipStack::DecodeMessage(
                objBuffer.GetData(), objBuffer.GetLength(), SipPrivate::OPTIONS_D, pstMessage))
    {
        SipStack::FreeMessage(pstMessage);

        IMS_TRACE_E(SipStack::GetLastError(),
                "DECODING FAILURE : The incoming packet MAY be a malformed SIP message", 0, 0, 0);
        return;
    }

    /// TRACE_DEBUG ....
    SipStack::DisplayUnknownHeaders(pstMessage);

    if (SipStack::GetBadHeaderCount(pstMessage) > 0)
    {
        SipStack::DisplayBadHeaders(pstMessage);

        if (!SipStack::HasMandatoryHeaders(pstMessage))
        {
            SipStack::FreeMessage(pstMessage);

            IMS_TRACE_E(SipStack::GetLastError(),
                    "DECODING FAILURE : Malformed SIP message - mandatory headers", 0, 0, 0);
            return;
        }
    }

    // SIP_PACKET_TRACKER
    NotifyPacketReceived(nSlotId, objBuffer, pstMessage, SipPrivate::MESSAGE_VALID);

    IMS_SINT32 nValidity;

    if (SipStack::IsRequestMessage(pstMessage))
    {
        if (SipWakeLock::IsSupported())
        {
            SipMethod objMethod = SipStack::GetMethod(pstMessage);
            SipWakeLock::Acquire(objMethod);
        }

        nValidity = NotifyRequest(nSlotId, pstMessage, objNearEnd, objFarEnd);
    }
    else
    {
        // REG-CONTACT-VALIDATION
        if (SipRtConfigUtils::IsRegContactAddressConfigured(nSlotId))
        {
            if (!CheckRegContactValidity(nSlotId, pstMessage))
            {
                IMS_TRACE_D("Ignore REGISTER response (reg-contact-mismatch)", 0, 0, 0);

                SipStack::FreeMessage(pstMessage);
                return;
            }
        }

        nValidity = NotifyResponse(nSlotId, pstMessage, objNearEnd, objFarEnd);
    }

    if (nValidity == SipPrivate::MESSAGE_FAILED)
    {
        IMS_TRACE_I("___ PROCESSING %s MESSAGE FAILED",
                SipStack::IsRequestMessage(pstMessage) ? "REQUEST" : "RESPONSE", 0, 0);
    }

    SipStack::FreeMessage(pstMessage);
}

PRIVATE
void SipMessageHandler::NotifyPacketReceived(IN IMS_SINT32 nSlotId, IN CONST ByteArray& objBuffer,
        IN ::SipMessage* pstMessage, IN IMS_SINT32 nProcessingResult)
{
    SipFactoryProxy* pFactoryProxy = SipFactoryProxy::GetInstance();

    if (pFactoryProxy->IsPacketTrackerEnabled(nSlotId))
    {
        SipPacketTracker* pPacketTracker = pFactoryProxy->GetPacketTracker(nSlotId);
        sipcore::SipMessage objSIPMsg(pstMessage);

        pPacketTracker->NotifyMessageReceived(&objSIPMsg, objBuffer,
                ((nProcessingResult == SipPrivate::MESSAGE_DISCARDED) ? IMS_TRUE : IMS_FALSE));
    }
}

PRIVATE
IMS_SINT32 SipMessageHandler::NotifyRequest(IN IMS_SINT32 nSlotId, IN ::SipMessage* pstMessage,
        IN CONST SipTransportAddress& objNearEnd, IN CONST SipTransportAddress& objFarEnd)
{
    ISipServerTransactionStateListener* piListener =
            SipManager::GetInstance()->LookupConnectionNotifier(objNearEnd);

    //---------------------------------------------------------------------------------------------

    // UE_TCP_CONNECTION_REUSED -- starts
    // When the transport protocol is TCP and the transaction re-use the TCP connection
    // which is created by UE, check the Request-URI again for port number.
    if ((piListener == IMS_NULL) &&
            ((objNearEnd.GetProtocol() == SipTransportAddress::PROTOCOL_TCP) ||
                    (objNearEnd.GetProtocol() == SipTransportAddress::PROTOCOL_TLS)))
    {
        // Gets the port number from the Request-URI
        SipAddrSpec* pstAddrSpec = SipStack::GetRequestUri(pstMessage);

        if (pstAddrSpec != IMS_NULL)
        {
            AString strHost;
            IMS_UINT32 nPort = Sip::PORT_UNSPECIFIED;

            if (!SipStack::GetHostAndPort(pstAddrSpec, strHost, nPort))
            {
                if (SipStack::IsLastErrorNoExist())
                {
                    // FIXME: should we get SIP profile?
                    nPort = SipConfigProxy::GetPort(nSlotId);
                }
            }
            else
            {
                // NO_EXIST (for port only)
                if ((nPort == Sip::PORT_UNSPECIFIED) && SipStack::IsLastErrorNoExist())
                {
                    // FIXME: should we get SIP profile?
                    nPort = SipConfigProxy::GetPort(nSlotId);
                    IMS_TRACE_D("ConnectionNotifier :: port(%d) from config.", nPort, 0, 0);
                }
            }

            if (nPort != Sip::PORT_UNSPECIFIED)
            {
                SipTransportAddress objTA = objNearEnd;

                objTA.SetPort(nPort);

                piListener = SipManager::GetInstance()->LookupConnectionNotifier(objTA);

                if (piListener != IMS_NULL)
                {
                    IMS_TRACE_D("ConnectionNotifier found by (%s, %d)",
                            SipDebug::GetIp(objTA.GetIpAddress()), objTA.GetPort(), 0);
                }
            }

            SipStack::FreeAddrSpec(pstAddrSpec);
        }
    }
    // UE_TCP_CONNECTION_REUSED -- ends

    if (SystemConfig::IsMultiSimEnabled())
    {
        IMS_TRACE_D("Incoming SIP request on SIM%d ...", nSlotId, 0, 0);
    }

    RCPtr<SipServerTransactionState> pSTState =
            new SipServerTransactionState(nSlotId, objNearEnd, objFarEnd);

    if (pSTState.IsNull())
    {
        // Discard the request
        return SipPrivate::MESSAGE_FAILED;
    }

    IMS_SINT32 nValidity = pSTState->MatchTransaction(pstMessage);
    IMS_BOOL bRejectRequest = IMS_FALSE;

    if (nValidity != SipPrivate::MESSAGE_VALID)
    {
        bRejectRequest = IMS_TRUE;
    }
    else if (SipStack::GetBadHeaderCount(pstMessage) > 0)
    {
        bRejectRequest = IMS_TRUE;
        nValidity = SipPrivate::MESSAGE_INVALID_400;
    }

    if (bRejectRequest)
    {
        IMS_SINT32 nResult = SipPrivate::MESSAGE_DISCARDED;

        // Send failure response if needs
        if ((nValidity == SipPrivate::MESSAGE_INVALID_400) ||
                (nValidity == SipPrivate::MESSAGE_INVALID_405) ||
                (nValidity == SipPrivate::MESSAGE_INVALID_481))
        {
            // Update the transport information
            if (piListener != IMS_NULL)
            {
                piListener->ServerTransactionState_RequestCreated(pSTState.Get());
            }
            else
            {
                pSTState->SetTransportTuple(
                        objNearEnd.GetIpAddress(), objNearEnd.GetPort(), objNearEnd.GetPort());
            }

            // To send failure response
            pSTState->CheckMessageValidity();

            RCPtr<SipDialogEx> pOrigDialogEx;
            nResult = pSTState->HandleRequest(pOrigDialogEx);
        }

        if ((nResult == SipPrivate::MESSAGE_VALID) || (nResult == SipPrivate::MESSAGE_VALID_FORKED))
        {
            if (nValidity == SipPrivate::MESSAGE_INVALID_481)
            {
                pSTState->RejectRequest(SipStatusCode::SC_481);
            }
            else if (nValidity == SipPrivate::MESSAGE_INVALID_400)
            {
                pSTState->RejectRequest(SipStatusCode::SC_400);
            }
            else if (nValidity == SipPrivate::MESSAGE_INVALID_405)
            {
                pSTState->RejectRequest(SipStatusCode::SC_405);
            }
        }

        pSTState->Abort();
        return nValidity;
    }

    if (piListener == IMS_NULL)
    {
        IMS_TRACE_D("NO LISTENER :: REQUEST MESSAGE IS DISCARDED", 0, 0, 0);

        // Update the transport information
        pSTState->SetTransportTuple(
                objNearEnd.GetIpAddress(), objNearEnd.GetPort(), objNearEnd.GetPort());

        // Send failure response
        pSTState->CheckMessageValidity();

        RCPtr<SipDialogEx> pOrigDialogEx;
        IMS_SINT32 nResult = pSTState->HandleRequest(pOrigDialogEx);

        if ((nResult == SipPrivate::MESSAGE_VALID) || (nResult == SipPrivate::MESSAGE_VALID_FORKED))
        {
            SipStatusCode objStatusCode(SipStatusCode::SC_404);
            SipFactoryProxy* pFactoryProxy = SipFactoryProxy::GetInstance();

            if (pFactoryProxy->IsRoutingRejectNotifierEnabled(nSlotId))
            {
                SipRoutingRejectNotifier* pRoutingRejectNotifier =
                        pFactoryProxy->GetRoutingRejectNotifier(nSlotId);
                sipcore::SipMessage objSIPMsg(pstMessage);

                pRoutingRejectNotifier->NotifyRequestReject(&objSIPMsg, objStatusCode);

                if (objStatusCode != SipStatusCode::SC_404)
                {
                    IMS_TRACE_D("SIPRoutingReject :: Status code is overwritten (404 >> %d)",
                            objStatusCode.ToInt(), 0, 0);
                }
            }

            pSTState->RejectRequest(objStatusCode.ToInt(), objStatusCode.GetReasonPhrase());
        }

        pSTState->Abort();

        return SipPrivate::MESSAGE_FAILED;
    }

    // After creating the transaction, we need to update some informations
    // related to this transaction.
    piListener->ServerTransactionState_RequestCreated(pSTState.Get());

    nValidity = pSTState->CheckMessageValidity();

    if (nValidity != SipPrivate::MESSAGE_VALID)
    {
        RCPtr<SipDialogEx> pOrigDialogEx;
        IMS_SINT32 nResult = pSTState->HandleRequest(pOrigDialogEx);

        if ((nResult == SipPrivate::MESSAGE_VALID) || (nResult == SipPrivate::MESSAGE_VALID_FORKED))
        {
            SipStatusCode objStatusCode(SipStatusCode::SC_400);
            SipFactoryProxy* pFactoryProxy = SipFactoryProxy::GetInstance();

            if (pFactoryProxy->IsRoutingRejectNotifierEnabled(nSlotId))
            {
                SipRoutingRejectNotifier* pRoutingRejectNotifier =
                        pFactoryProxy->GetRoutingRejectNotifier(nSlotId);
                sipcore::SipMessage objSIPMsg(pstMessage);

                pRoutingRejectNotifier->NotifyRequestReject(&objSIPMsg, objStatusCode);

                if (objStatusCode != SipStatusCode::SC_400)
                {
                    IMS_TRACE_D("SIPRoutingReject :: Status code is overwritten (400 >> %d)",
                            objStatusCode.ToInt(), 0, 0);
                }
            }

            pSTState->RejectRequest(objStatusCode.ToInt(), objStatusCode.GetReasonPhrase());
        }

        pSTState->Abort();

        return nValidity;
    }

    // If IPSec is applied, check the validity of source IP and port.
    if (!CheckIPSecValidityForRequest(nSlotId, pSTState.Get(), objNearEnd, objFarEnd))
    {
        // Discard the incoming request...
        IMS_TRACE_D("IPSEC :: Discarded...", 0, 0, 0);
        pSTState->Abort();
        return SipPrivate::MESSAGE_DISCARDED;
    }

    RCPtr<SipDialogEx> pOrigDialogEx;

    nValidity = pSTState->HandleRequest(pOrigDialogEx);

    if (nValidity == SipPrivate::MESSAGE_VALID)
    {
        piListener->ServerTransactionState_RequestReceived(pSTState.Get());
    }
    else if (nValidity == SipPrivate::MESSAGE_VALID_FORKED)
    {
        piListener->ServerTransactionState_ForkedRequestReceived(
                pSTState.Get(), pOrigDialogEx.Get());
    }
    else
    {
        pSTState->Abort();
        return nValidity;
    }

    return SipPrivate::MESSAGE_VALID;
}

PRIVATE
IMS_SINT32 SipMessageHandler::NotifyResponse(IN IMS_SINT32 nSlotId, IN ::SipMessage* pstMessage,
        IN CONST SipTransportAddress& objNearEnd, IN CONST SipTransportAddress& objFarEnd)
{
    RCPtr<SipClientTransactionState> pCTState;

    //---------------------------------------------------------------------------------------------

    // If IPSec is applied, check the validity of source IP and port.
    if (!CheckIPSecValidityForResponse(nSlotId, pstMessage, objNearEnd, objFarEnd))
    {
        // Discard the incoming response...
        IMS_TRACE_D("IPSEC :: Discarded...", 0, 0, 0);
        return SipPrivate::MESSAGE_DISCARDED;
    }

    IMS_SINT32 nValidity =
            SipClientTransactionState::MatchTransaction(pstMessage, objFarEnd, pCTState);

    if (nValidity != SipPrivate::MESSAGE_VALID)
    {
        return nValidity;
    }

    nValidity = pCTState->HandleResponse(pstMessage);

    if (nValidity != SipPrivate::MESSAGE_VALID)
    {
        return nValidity;
    }

    return SipPrivate::MESSAGE_VALID;
}

PRIVATE
IMS_BOOL SipMessageHandler::CheckIPSecValidityForRequest(IN IMS_SINT32 nSlotId,
        IN SipTransactionState* pTState, IN CONST SipTransportAddress& objNearEnd,
        IN CONST SipTransportAddress& objFarEnd)
{
    if (pTState->IsIpSecRequired() && SipRtConfigUtils::IsIpSecSaConfigured(nSlotId))
    {
        if (!IsIPSecSAMatched(nSlotId, objNearEnd, objFarEnd))
        {
            IMS_TRACE_D("IPSEC :: SIP request is dropped by non-SA", 0, 0, 0);
            return IMS_FALSE;
        }
    }

    return IMS_TRUE;
}

PRIVATE
IMS_BOOL SipMessageHandler::CheckIPSecValidityForResponse(IN IMS_SINT32 nSlotId,
        IN ::SipMessage* pstMessage, IN CONST SipTransportAddress& objNearEnd,
        IN CONST SipTransportAddress& objFarEnd)
{
    if (SipRtConfigUtils::IsIpSecSaConfigured(nSlotId) && IsSecuredMessage(nSlotId, pstMessage))
    {
        if (!IsIPSecSAMatched(nSlotId, objNearEnd, objFarEnd))
        {
            IMS_TRACE_D("IPSEC :: SIP response is dropped by non-SA", 0, 0, 0);
            return IMS_FALSE;
        }
    }

    return IMS_TRUE;
}

PRIVATE
IMS_BOOL SipMessageHandler::IsIPSecSAMatched(IN IMS_SINT32 nSlotId,
        IN CONST SipTransportAddress& objNearEnd, IN CONST SipTransportAddress& objFarEnd)
{
    SipRtConfigHelper* pConfigHelper = SipRtConfigUtils::GetConfigHelper(nSlotId);
    IMS_BOOL bAtLeastOneSAMatched = IMS_FALSE;
    const IMSList<SipRtConfig::IpSecSa>& objIPSecSAs = pConfigHelper->GetIpSecSas();

    for (IMS_UINT32 i = 0; i < objIPSecSAs.GetSize(); ++i)
    {
        const SipRtConfig::IpSecSa& objIPSecSA = objIPSecSAs.GetAt(i);

        if (objIPSecSA.IsEmpty())
        {
            continue;
        }

        if ((objIPSecSA.GetPortPc() != objFarEnd.GetPort()) &&
                (objIPSecSA.GetPortPs() != objFarEnd.GetPort()))
        {
            IMS_TRACE_I("IPSec :: PortP is mismatched; pc(%d), ps(%d), far(%d)",
                    objIPSecSA.GetPortPc(), objIPSecSA.GetPortPs(), objFarEnd.GetPort());
            continue;
        }

        if (!objIPSecSA.GetIpAddrP().Equals(objFarEnd.GetIpAddress()))
        {
            if (pConfigHelper->IsRoutingInfoHiddenInLog())
            {
                IMS_TRACE_I("IPSec :: IPP is mismatched", 0, 0, 0);
            }
            else
            {
                IMS_TRACE_I("IPSec :: IPP is mismatched; ipp(%s), far(%s)",
                        SipDebug::GetStr1(objIPSecSA.GetIpAddrP().ToString(), 5).GetStr(),
                        SipDebug::GetIp(objFarEnd.GetIpAddress()), 0);
            }
            continue;
        }

        if ((objIPSecSA.GetPortUc() != objNearEnd.GetPort()) &&
                (objIPSecSA.GetPortUs() != objNearEnd.GetPort()))
        {
            IMS_TRACE_I("IPSec :: PortU is mismatched; uc(%d), us(%d), near(%d)",
                    objIPSecSA.GetPortPc(), objIPSecSA.GetPortPs(), objNearEnd.GetPort());
            continue;
        }

        if (!bAtLeastOneSAMatched)
        {
            bAtLeastOneSAMatched = IMS_TRUE;
            break;
        }
    }

    return bAtLeastOneSAMatched;
}

PRIVATE
IMS_BOOL SipMessageHandler::IsIPSecSAMatchedForUS(
        IN IMS_SINT32 nSlotId, IN CONST IPAddress& objIP, IN IMS_SINT32 nPort)
{
    SipRtConfigHelper* pConfigHelper = SipRtConfigUtils::GetConfigHelper(nSlotId);
    const IMSList<SipRtConfig::IpSecSa>& objIPSecSAs = pConfigHelper->GetIpSecSas();

    for (IMS_UINT32 i = 0; i < objIPSecSAs.GetSize(); ++i)
    {
        const SipRtConfig::IpSecSa& objIPSecSA = objIPSecSAs.GetAt(i);

        if (objIPSecSA.IsEmpty())
        {
            continue;
        }

        if ((objIPSecSA.GetPortUs() == nPort) && objIPSecSA.GetIpAddrU().Equals(objIP))
        {
            return IMS_TRUE;
        }
    }

    return IMS_FALSE;
}

PRIVATE
IMS_BOOL SipMessageHandler::IsSecuredMessage(IN IMS_SINT32 nSlotId, IN ::SipMessage* pstMessage)
{
    IMS_SINT32 nPort = Sip::PORT_UNSPECIFIED;
    AString strHost;

    if (!SipTransport::GetHostNPortFromViaHeader(pstMessage, strHost, nPort))
    {
        return IMS_FALSE;
    }

    if (SipStack::IsRequestMessage(pstMessage))
    {
        // It can be identified by SIP transaction state.
    }
    else
    {
        IPAddress objIP(strHost);

        if (IsIPSecSAMatchedForUS(nSlotId, objIP, nPort))
        {
            return IMS_TRUE;
        }
    }

    return IMS_FALSE;
}

PRIVATE
IMS_BOOL SipMessageHandler::CheckRegContactValidity(
        IN IMS_SINT32 nSlotId, IN ::SipMessage* pstMessage)
{
    SipMethod objMethod = SipStack::GetMethod(pstMessage);

    if (!objMethod.Equals(SipMethod::REGISTER))
    {
        // Pass REGISTER response to the application.
        return IMS_TRUE;
    }

    IMS_SINT32 nStatusCode = SipStack::GetStatusCode(pstMessage);

    if (!SipStatusCode::IsFinalSuccess(nStatusCode))
    {
        // Pass REGISTER response to the application.
        return IMS_TRUE;
    }

    AString strCallId = SipStack::GetHeaderAsString(pstMessage, ISipHeader::CALL_ID);
    SipRtConfigHelper* pConfigHelper = SipRtConfigUtils::GetConfigHelper(nSlotId);
    const SipAddress* pContact = pConfigHelper->GetRegContactUri(strCallId);

    if (pContact == IMS_NULL)
    {
        // No Reg-Contact provisioned; Pass REGISTER response to the application.
        return IMS_TRUE;
    }

    IMS_SINT32 nHeaderCount = SipStack::GetHeaderCount(pstMessage, ISipHeader::CONTACT_ANY);

    if (nHeaderCount == 0)
    {
        // Drop REGISTER response.
        return IMS_FALSE;
    }

    AString strContact;
    SipAddress objContact;

    for (IMS_SINT32 i = 0; i < nHeaderCount; ++i)
    {
        strContact = SipStack::GetHeaderAsString(pstMessage, ISipHeader::CONTACT_ANY, IMS_FALSE, i);

        objContact.Create(strContact);

        if (pContact->Equals(objContact))
        {
            // Pass REGISTER response to the application.
            return IMS_TRUE;
        }
    }

    IMS_TRACE_D("Reg-Contact(%s) is not matched", SipDebug::GetUri1(pContact->ToString()).GetStr(),
            0, 0);

    // Drop REGISTER response.
    return IMS_FALSE;
}
