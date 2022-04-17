#ifndef _INTERFACE_AOS_NET_TRACKER_LISTENER_H_
#define _INTERFACE_AOS_NET_TRACKER_LISTENER_H_

#include "INetWatcher.h"

class IAosNetTrackerListener
{
public:
    virtual void NetTracker_StatusChanged() = 0;
};
#endif
