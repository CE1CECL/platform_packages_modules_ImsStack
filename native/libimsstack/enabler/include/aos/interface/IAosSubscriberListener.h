#ifndef _INTERFACE_AOS_SUBSCRIBER_LISTENER_H_
#define _INTERFACE_AOS_SUBSCRIBER_LISTENER_H_

#include "IMSTypeDef.h"

class IAosSubscriberListener
{
public:
    virtual void Subscriber_StateChanged(IN IMS_UINT32 nState, IN IMS_UINT32 nParam = 0) = 0;
};
#endif // _INTERFACE_AOS_SUBSCRIBER_LISTENER_H_
