#ifndef _INTERFACE_AOS_CONNECTOR_LISTENER_H_
#define _INTERFACE_AOS_CONNECTOR_LISTENER_H_

#include "IMSTypeDef.h"

class IAosConnectorListener
{
public:
    virtual void Connector_Activated() = 0;
    virtual void Connector_Deactivated(IN IMS_UINT32 nReason) = 0;
    virtual void Connector_Updated(IN IMS_UINT32 nReason) = 0;
};
#endif // _INTERFACE_AOS_CONNECTOR_LISTENER_H_
