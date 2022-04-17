#ifndef _AOS_CARRIER_CONFIG_BUNDLE_H_
#define _AOS_CARRIER_CONFIG_BUNDLE_H_

#include "IMSTypeDef.h"
#include "AString.h"
#include "IMSVector.h"

/// ims.
struct AosMmtelRequiresProvisioningBundle
{
public:
    AosMmtelRequiresProvisioningBundle()
        : objCapabilityTypeVoice(IMSVector<IMS_SINT32>())
        , objCapabilityTypeVideo(IMSVector<IMS_SINT32>())
        , objCapabilityTypeSms(IMSVector<IMS_SINT32>())
    {
    }

    AosMmtelRequiresProvisioningBundle(IN const AosMmtelRequiresProvisioningBundle&) = delete;
    AosMmtelRequiresProvisioningBundle& operator=(
            IN const AosMmtelRequiresProvisioningBundle&) = delete;

public:
    IMSVector<IMS_SINT32> objCapabilityTypeVoice;
    IMSVector<IMS_SINT32> objCapabilityTypeVideo;
    IMSVector<IMS_SINT32> objCapabilityTypeSms;
};

struct AosNotifyTerminatedForRegEventWithInitialRegistrationBundle
{
public:
    AosNotifyTerminatedForRegEventWithInitialRegistrationBundle()
        : nWaitTimeForInitRegOnTerminatedstate(0)
        , objEventForInitRegOnTerminatedState(IMSVector<IMS_SINT32>())
        , objEventToFollowWtForInitRegOnTerminatedState(IMSVector<IMS_SINT32>())
    {
    }

    AosNotifyTerminatedForRegEventWithInitialRegistrationBundle(
            IN const AosNotifyTerminatedForRegEventWithInitialRegistrationBundle&) = delete;
    AosNotifyTerminatedForRegEventWithInitialRegistrationBundle& operator=(
            IN const AosNotifyTerminatedForRegEventWithInitialRegistrationBundle&) = delete;

public:
    IMS_SINT32 nWaitTimeForInitRegOnTerminatedstate;
    IMSVector<IMS_SINT32> objEventForInitRegOnTerminatedState;
    IMSVector<IMS_SINT32> objEventToFollowWtForInitRegOnTerminatedState;
};

struct AosRegistrationRetryIntervalBundle
{
public:
    AosRegistrationRetryIntervalBundle()
        : objRegistrationRetryRandomUpperValueSec(IMSVector<IMS_SINT32>())
        , objRegistrationRetryIntervalSec(IMSVector<IMS_SINT32>())
        , bUseRegistrationRetryIntervalForSubscriptionRetry(IMS_TRUE)
    {
    }

    AosRegistrationRetryIntervalBundle(IN const AosRegistrationRetryIntervalBundle&) = delete;
    AosRegistrationRetryIntervalBundle& operator=(
            IN const AosRegistrationRetryIntervalBundle&) = delete;

public:
    IMSVector<IMS_SINT32> objRegistrationRetryRandomUpperValueSec;
    IMSVector<IMS_SINT32> objRegistrationRetryIntervalSec;
    IMS_BOOL bUseRegistrationRetryIntervalForSubscriptionRetry;
};

struct AosRegistrationPermanentErrorCodeBundle
{
public:
    AosRegistrationPermanentErrorCodeBundle()
        : objRegistrationPermanentErrorCode(IMSVector<IMS_SINT32>())
        , objRegistrationPermanentErrorMaxCount(IMSVector<IMS_SINT32>())
    {

    }

    AosRegistrationPermanentErrorCodeBundle(
            IN const AosRegistrationPermanentErrorCodeBundle&) = delete;
    AosRegistrationPermanentErrorCodeBundle& operator=(
            IN const AosRegistrationPermanentErrorCodeBundle&) = delete;

public:
    IMSVector<IMS_SINT32> objRegistrationPermanentErrorCode;
    IMSVector<IMS_SINT32> objRegistrationPermanentErrorMaxCount;
};

#endif // _AOS_CARRIER_CONFIG_BUNDLE_H_
