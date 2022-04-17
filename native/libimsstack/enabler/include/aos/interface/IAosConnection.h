#ifndef _INTERFACE_AOS_CONNECTION_H_
#define _INTERFACE_AOS_CONNECTION_H_

#include "IMSTypeDef.h"
#include "IIPCAN.h"
#include "AStringArray.h"
#include "IPAddress.h"

class IAosConnectionListener;

class IAosConnection
{
public:
    virtual IMS_BOOL Activate() = 0;
    virtual void Deactivate() = 0;
    virtual IMS_UINT32 GetState() = 0;

    virtual IMS_SINT32 GetConnectionType() = 0;
    virtual IMS_SINT32 GetPreferredIpVersion() = 0;

    virtual void SetListener(IN IAosConnectionListener* piListener) = 0;
    virtual void RemoveListener(IN IAosConnectionListener* piListener) = 0;

    // INetConnection Util
    virtual IMS_SINT32 GetMtu() = 0;
    virtual const IPAddress& GetLocalAddress(IN IMS_SINT32 nIpVersion = 0) = 0;
    virtual const AStringArray& GetPcscfAddress(IN IMS_SINT32 nIpVersion = 0) = 0;
    virtual IMS_SINT32 GetHostByName(IN const AString& strHostName,
                    OUT IMSList<IPAddress>& objIps, IN IMS_SINT32 nIpVersion = 0) = 0;
    virtual const AString& GetIfaceName() = 0;
    virtual IMS_BOOL IsEpdgEnabled() = 0;
    virtual IMS_SINT32 GetIpcanCategory() = 0;
    virtual IMS_BOOL SendPingToHostAddress(IN const IPAddress& objHostAddress) = 0;

    enum
    {
        STATE_IDLE = 0,
        STATE_ACTIVE,
        STATE_ACTIVATING
    };

    enum
    {
        IP_VERSION_4 = 4,
        IP_VERSION_6 = 6,
        IP_VERSION_46 = 46,
        IP_VERSION_64 = 64
    };
};
#endif // _INTERFACE_AOS_CONNECTION_H_
