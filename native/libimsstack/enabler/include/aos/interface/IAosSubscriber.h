#ifndef _INTERFACE_AOS_SUBSCRIBER_H_
#define _INTERFACE_AOS_SUBSCRIBER_H_

#include "IMSTypeDef.h"
#include "AStringArray.h"

class IAosSubscriberListener;
class ISubscriberConfig;

class IAosSubscriber
{
public:
    virtual IMS_BOOL IsReady() const = 0;

    virtual void SetListener(IN IAosSubscriberListener* piListener) = 0;

    virtual const AStringArray& GetConfiguredImpus() const = 0;
    virtual const AStringArray& GetFakeImpus() const = 0;

    virtual const ISubscriberConfig* GetSubscriberConfig(IMS_SINT32 nType = NORMAL) const = 0;

    // Subscriber_StateChanged(nState)
    enum
    {
        NOT_READY = 0,
        READY = 1,

        REFRESH_STARTED = 2,
        REFRESH_COMPLETED = 3,
        REFRESH_FAILED = 4 // refresh is done but credentials are missing.
    };

    // GetSubscriberConfig(nType)
    enum
    {
        NORMAL = 0,
        FAKE = 1
    };

protected:
    friend class AosBuildDirector;
    friend class AosAppContext;
    virtual void Init() = 0;
    virtual void CleanUp() = 0;
};

#endif // _INTERFACE_AOS_SUBSCRIBER_H_
