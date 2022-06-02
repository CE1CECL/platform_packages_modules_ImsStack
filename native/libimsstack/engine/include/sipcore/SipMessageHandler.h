/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#ifndef _SIP_MESSAGE_HANDLER_H_
#define _SIP_MESSAGE_HANDLER_H_

#include "ISipTransportListener.h"

class SipTransactionState;

class SipMessageHandler : public ISipTransportListener
{
private:
    SipMessageHandler();
    SipMessageHandler(IN CONST SipMessageHandler& objRHS);

public:
    ~SipMessageHandler();

public:
    static SipMessageHandler* GetInstance();

private:
    // ISipTransportListener interface
    virtual void Transport_PacketReceived(IN IMS_SINT32 nSlotId, IN CONST ByteArray& objBuffer,
            IN CONST SipTransportAddress& objNearEnd, IN CONST SipTransportAddress& objFarEnd);

    void NotifyPacketReceived(IN IMS_SINT32 nSlotId, IN CONST ByteArray& objBuffer,
            IN ::SipMessage* pstMessage, IN IMS_SINT32 nProcessingResult);
    IMS_SINT32 NotifyRequest(IN IMS_SINT32 nSlotId, IN ::SipMessage* pstMessage,
            IN CONST SipTransportAddress& objNearEnd, IN CONST SipTransportAddress& objFarEnd);
    IMS_SINT32 NotifyResponse(IN IMS_SINT32 nSlotId, IN ::SipMessage* pstMessage,
            IN CONST SipTransportAddress& objNearEnd, IN CONST SipTransportAddress& objFarEnd);

    IMS_BOOL CheckIPSecValidityForRequest(IN IMS_SINT32 nSlotId, IN SipTransactionState* pTState,
            IN CONST SipTransportAddress& objNearEnd, IN CONST SipTransportAddress& objFarEnd);
    IMS_BOOL CheckIPSecValidityForResponse(IN IMS_SINT32 nSlotId, IN ::SipMessage* pstMessage,
            IN CONST SipTransportAddress& objNearEnd, IN CONST SipTransportAddress& objFarEnd);
    IMS_BOOL IsIPSecSAMatched(IN IMS_SINT32 nSlotId, IN CONST SipTransportAddress& objNearEnd,
            IN CONST SipTransportAddress& objFarEnd);
    IMS_BOOL IsIPSecSAMatchedForUS(
            IN IMS_SINT32 nSlotId, IN CONST IPAddress& ojbIP, IN IMS_SINT32 nPort);
    IMS_BOOL IsSecuredMessage(IN IMS_SINT32 nSlotId, IN ::SipMessage* pstMessage);

    IMS_BOOL CheckRegContactValidity(IN IMS_SINT32 nSlotId, IN ::SipMessage* pstMessage);
};

#endif  // _SIP_MESSAGE_HANDLER_H_
