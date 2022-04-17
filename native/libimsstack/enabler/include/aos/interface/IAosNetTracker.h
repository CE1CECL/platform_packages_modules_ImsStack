#ifndef _INTERFACE_AOS_NET_TRACKER_H_
#define _INTERFACE_AOS_NET_TRACKER_H_

#include "IMSTypeDef.h"

class IAosNetTrackerListener;

class IAosNetTracker
{
public:
    virtual IMS_BOOL IsServiceIN(IN IMS_UINT32 nType = TYPE_DEFAULT) = 0;
    virtual IMS_BOOL IsDataIN() = 0;
    virtual IMS_BOOL IsNetworkIN() = 0;
    virtual IMS_BOOL IsEmergencyLteAttach() = 0;
    virtual IMS_BOOL IsSuspended() = 0;
    virtual IMS_BOOL IsSessionContinuitySupported() = 0;
    virtual IMS_BOOL IsServiceTimerRunning() = 0;

    // the network type of moblie
    virtual IMS_UINT32 GetMobileChangingNetworkType() = 0;
    virtual IMS_UINT32 GetMobileNetworkType() = 0;
    virtual IMS_SINT32 GetMobileVoiceServiceState() = 0;
    virtual IMS_UINT32 GetMobileVoiceNetworkType() = 0;
    /*
        the network type of serive in network
        return : mobile or WLAN
    */
    virtual IMS_UINT32 GetNetworkType() = 0;


    virtual void SetRATGuardTime(IN IMS_UINT32 nGuardTime) = 0;
    virtual void SetSrvOutGuardTime(IN IMS_UINT32 nGuardTime) = 0;
    virtual void SetSrvInGuardTime(IN IMS_UINT32 nGuardTime) = 0;

    virtual void SetListener(IN IAosNetTrackerListener *piListener) = 0;
    virtual void RemoveListener(IN IAosNetTrackerListener *piListener) = 0;

    enum
    {
        TYPE_DEFAULT = 0,
        TYPE_MOBILE,
        TYPE_WLAN
    };
};
#endif
