#ifndef _INTERFACE_AoS_CONNECTION_LISTENER_H_
#define _INTERFACE_AoS_CONNECTION_LISTENER_H_

#include "IMSTypeDef.h"

class IAosConnectionListener
{
public:
    virtual void AosConnection_StateChanged(IN IMS_UINT32 nState) = 0;
    virtual void AosConnection_IpChanged() = 0;
    virtual void AosConnection_IpcanCatChanged() = 0;
    virtual void AosConnection_PcscfChanged() = 0;
    virtual void AosConnection_ConnectionFailed() = 0;
};
#endif
