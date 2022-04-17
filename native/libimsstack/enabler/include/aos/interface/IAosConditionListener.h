#ifndef _INTERFACE_AOS_CONDITION_LISTENER_H_
#define _INTERFACE_AOS_CONDITION_LISTENER_H_

#include "IMSTypeDef.h"

class IAosConditionListener
{
public:
    virtual void Condition_Changed(IN IMS_UINT32 nReason = 0) = 0;
    virtual void Condition_RequestCommand(IN IMS_UINT32 nCommand, IN IMS_UINT32 nReason = 0) = 0;
};
#endif // _INTERFACE_AOS_CONDITION_LISTENER_H_
