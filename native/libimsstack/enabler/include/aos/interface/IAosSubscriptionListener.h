#ifndef _INTERFACE_AOS_SUBSCRIPTION_LISTENER_H_
#define _INTERFACE_AOS_SUBSCRIPTION_LISTENER_H_

#include "IMSTypeDef.h"

/**
 * @brief This class provies a listener interface for registration.
 */
class IAosSubscriptionListener
{
public:
    /**
     * @brief Notifies the application that the state is changed.
     *
     * @param nState Value of the AosSubscription state
     * @param nReason The reason the state of AosSubscription changed
     */
    virtual void Subscription_StateChanged(IN IMS_SINT32 nState, IN IMS_SINT32 nReason = 0) = 0;

    /**
     * @brief Notifies the application that the SUBSCRIBE message can be transmitted.
     *
     * Then, the application can request that the SUBSCRIBE message is sent later.
     *
     * @return IMS_BOOL
     */
    virtual IMS_BOOL Subscription_CanBeTransmitted() = 0;

    /**
     * @brief Notifies the application when the NOTIFY message is received.
     *
     * @param nEvent The event information of NOTIFY message
     */
    virtual void Subscription_NotifyReceived(IN IMS_SINT32 nEvent) = 0;

    /**
     * @brief Notifies the application when it needs to request AosRegistration
     *
     * @param nCommand Request to AosRegistration
     * @param nRetryAfter Time to wait before the above request is proceed
     */
    virtual void Subscription_Request(IN IMS_SINT32 nCommand, IN IMS_SINT32 nRetryAfter = 0);
};

#endif // _INTERFACE_AOS_SUBSCRIPTION_LISTENER_H_
