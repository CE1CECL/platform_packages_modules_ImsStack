#ifndef _INTERFACE_AOS_REGISTRATION_LISTENER_H_
#define _INTERFACE_AOS_REGISTRATION_LISTENER_H_

#include "IMSTypeDef.h"

class IAosRegistrationListener
{
public:
    virtual void Registration_StateChanged(IN IMS_UINT32 nResult, IN IMS_UINT32 nReason = 0) = 0;

    virtual void Registration_PreNotify(IN IMS_UINT32 nPreReason) = 0;
};
#endif // _INTERFACE_AOS_REGISTRATION_LISTENER_H_
