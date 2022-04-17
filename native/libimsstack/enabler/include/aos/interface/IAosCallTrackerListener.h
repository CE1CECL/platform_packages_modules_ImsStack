#ifndef _INTERFACE_AOS_CALL_TRACKER_LISTENER_H_
#define _INTERFACE_AOS_CALL_TRACKER_LISTENER_H_

#include "IMSTypeDef.h"

class IAosCallTrackerListener
{
public:
    virtual void CallTracker_StateChanged(IN IMS_UINT32 nType, IN IMS_UINT32 nState) = 0;
};
#endif // _INTERFACE_AOS_CALL_TRACKER_LISTENER_H_
