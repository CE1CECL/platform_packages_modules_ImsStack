#ifndef INTERFACE_CONFERENCE_SUBSCRIPTION_LISTENER_H_
#define INTERFACE_CONFERENCE_SUBSCRIPTION_LISTENER_H_

enum class SubscriptionUpdateType
{
    SUCCEEDED = 0,
    FAILED,
    UNSUBSCRIBED,
    TERMINATED,
    NOTIFY_RECEIVED
};

class IConferenceSubscriptionListener
{
public:
    virtual void OnSubscriptionUpdated(IN SubscriptionUpdateType eType) = 0;
};

#endif
