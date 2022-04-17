#ifndef _AOS_SUBSCRIBER_H_
#define _AOS_SUBSCRIBER_H_

#include "interface/IAosSubscriberManagerListener.h"
#include "interface/IAosSubscriber.h"

class IAosAppContext;
class AosStaticProfile;
class AosUtil;
class AosServicePhoneListener;

class AosSubscriber
    : public IAosSubscriber
    , public IAosSubscriberManagerListener
{
public:
    AosSubscriber(IN IAosAppContext* piAppContext);
    virtual ~AosSubscriber();

    // IAosSubscriber
    IMS_BOOL IsReady() const override;
    void SetListener(IN IAosSubscriberListener* piListener) override;
    const AStringArray& GetConfiguredImpus() const override;
    const AStringArray& GetFakeImpus() const override;
    const ISubscriberConfig* GetSubscriberConfig(IN IMS_SINT32 nType = NORMAL) const override;

private:
    // IAosSubscriber
    void Init() override;
    void CleanUp() override;
    void Notify(IN IMS_UINT32 nState);

    // IAosSubscriberManagerListener
    void AosSubscriberManager_NotifyState(IN IMS_UINT32 nState) override;

private:
    IAosAppContext* m_piAppContext;
    IMS_SINT32 m_nSlotId;
    IAosSubscriberListener* m_piListener;

    AosRegistrationType m_eRegType;

    AString m_strTag;
};

#endif // _AOS_SUBSCRIBER_H_