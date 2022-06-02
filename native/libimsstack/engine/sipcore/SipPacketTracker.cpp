/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20131024  seungjin82.choi@          Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "ISipPacketTrackerListener.h"
#include "SipPacketTracker.h"

PUBLIC
SipPacketTracker::SipPacketTracker() :
        piListener(IMS_NULL)
{
}

PUBLIC VIRTUAL SipPacketTracker::~SipPacketTracker() {}

/*

Remarks

*/
PRIVATE VIRTUAL void SipPacketTracker::SetListener(IN ISipPacketTrackerListener* piListener)
{
    //---------------------------------------------------------------------------------------------

    this->piListener = piListener;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipPacketTracker::IsPacketTrackerEnabled() const
{
    //---------------------------------------------------------------------------------------------

    return (piListener != IMS_NULL);
}

/*

Remarks

*/
PUBLIC
void SipPacketTracker::NotifyMessageSent(
        IN ISipMessage* piSIPMsg, IN CONST ByteArray& objMsg, IN IMS_BOOL bIsRetransmission)
{
    //---------------------------------------------------------------------------------------------

    if (piListener == IMS_NULL)
        return;

    piListener->PacketTracker_NotifyMessageSent(piSIPMsg, objMsg, bIsRetransmission);
}

/*

Remarks

*/
PUBLIC
void SipPacketTracker::NotifyMessageReceived(
        IN ISipMessage* piSIPMsg, IN CONST ByteArray& objMsg, IN IMS_BOOL bIsRetransmission)
{
    //---------------------------------------------------------------------------------------------

    if (piListener == IMS_NULL)
        return;

    piListener->PacketTracker_NotifyMessageReceived(piSIPMsg, objMsg, bIsRetransmission);
}
