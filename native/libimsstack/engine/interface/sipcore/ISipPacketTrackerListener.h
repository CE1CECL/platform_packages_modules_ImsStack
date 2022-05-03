#ifndef _INTERFACE_SIP_PACKET_TRACKER_LISTENER_H_
#define _INTERFACE_SIP_PACKET_TRACKER_LISTENER_H_

#include "ByteArray.h"

class ISipMessage;

/**
 * @brief This class provides a listener interface to notify SIP packets
 *        when the SIP messages are sent or received.
 *
 * @see ISipPacketTracker, ISipMessage
 */
class ISipPacketTrackerListener
{
public:
    /**
     * @brief Notifies the application that SIP packet is sent to the network.
     *
     * @param piSIPMsg Pointer to ISipMessage object
     * @param objMsg Byte array to SIP Message
     * @param bIsRetransmission Flag that SIP message is retransmitted
     */
    virtual void PacketTracker_NotifyMessageSent(IN ISipMessage *piSIPMsg,
            IN CONST ByteArray &objMsg, IN IMS_BOOL bIsRetransmission) = 0;

    /**
     * @brief Notifies the application that SIP packet is received from the network.
     *
     * @param piSIPMsg Pointer to ISipMessage object
     * @param objMsg Byte array to SIP Message
     * @param bIsRetransmission Flag that SIP message is retransmitted
     */
    virtual void PacketTracker_NotifyMessageReceived(IN ISipMessage *piSIPMsg,
            IN CONST ByteArray &objMsg, IN IMS_BOOL bIsRetransmission) = 0;
};

#endif // _INTERFACE_SIP_PACKET_TRACKER_LISTENER_H_
