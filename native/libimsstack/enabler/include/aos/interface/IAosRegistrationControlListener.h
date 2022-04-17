#ifndef _INTERFACE_AOS_REGISTRATION_CONTROL_LISTENER_H_
#define _INTERFACE_AOS_REGISTRATION_CONTROL_LISTENER_H_

#include "IMSMap.h"
#include "IMSTypeDef.h"
#include "AString.h"

class IAosRegistrationControlListener
{
public:
    virtual void RegistrationControl_UpdateSipDelegateRegistration() = 0;
    virtual void RegistrationControl_TriggerSipDelegateDeregistration() = 0;
    virtual void RegistrationControl_TriggerFullNetworkRegistration(IN IMS_SINT32 nSipCode,
            IN const AString& strTarget) = 0;
    virtual void RegistrationControl_NotifyCapabilitiesChanged(
            IN const IMSMap<IMS_UINT32, IMS_UINT32>& objCapabilities) = 0;
};

class AosRegistrationControlListener
    : public IAosRegistrationControlListener
{
public:
    inline virtual void RegistrationControl_UpdateSipDelegateRegistration(){};
    inline virtual void RegistrationControl_TriggerSipDelegateDeregistration(){};
    inline virtual void RegistrationControl_TriggerFullNetworkRegistration(
            IN IMS_SINT32 /*nSipCode*/, IN const AString& /*strTarget*/){};
    inline virtual void RegistrationControl_NotifyCapabilitiesChanged(
            IN const IMSMap<IMS_UINT32, IMS_UINT32>& /*objCapabilities*/){};
};

#endif // _INTERFACE_AOS_REGISTRATION_CONTROL_LISTENER_H_