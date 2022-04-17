#ifndef _INTERFACE_AOS_SUBSCRIBER_MANAGERH_
#define _INTERFACE_AOS_SUBSCRIBER_MANAGERH_

#include "IMSTypeDef.h"
#include "interface/IAosSubscriber.h"
#include "interface/IAosSubscriberManagerListener.h"

class IAosSubscriberManager
{
public:
    virtual IMS_BOOL IsReady(IN IMS_BOOL bIsFake = IMS_FALSE) const = 0;

    virtual void AddListener(IN IAosSubscriberManagerListener* piListener) = 0;
    virtual void RemoveListener(IN IAosSubscriberManagerListener* piListener) = 0;
    virtual void AddListenerForMonitor(IN IAosSubscriberManagerListener* piListener) = 0;
    virtual void RemoveListenerForMonitor(IN IAosSubscriberManagerListener* piListener) = 0;

    virtual const AStringArray& GetConfiguredImpus(IN IMS_BOOL bIsFake = IMS_FALSE) const = 0;
    virtual const AStringArray& GetFakeImpus() const = 0;

    virtual const ISubscriberConfig* GetSubscriberConfig(
            IN IMS_SINT32 nType = IAosSubscriber::NORMAL) const = 0;

protected:
    friend class AosBuildDirector;
    friend class AosAppContext;
};

#endif // _INTERFACE_AOS_SUBSCRIBER_MANAGERH_