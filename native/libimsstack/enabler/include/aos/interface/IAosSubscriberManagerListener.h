#ifndef _INTERFACE_AOS_SUBSCRIBER_MANAGER_LISTENER_H_
#define _INTERFACE_AOS_SUBSCRIBER_MANAGER_LISTENER_H_

#include "IMSTypeDef.h"

class IAosSubscriberManagerListener
{
public:
    virtual void AosSubscriberManager_NotifyState(IMS_UINT32 nState) = 0;
};

#endif // _INTERFACE_AOS_SUBSCRIBER_MANAGER_LISTENER_H_