/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    </table>

    Description
*/

#ifndef _VZW_PROPERTY_
#define _VZW_PROPERTY_

#include "AString.h"
#include "IEventListener.h"

class VZWProperty
    : public IEventListener
{
private:
    VZWProperty();
    virtual ~VZWProperty();

public:
    static VZWProperty* GetInstance();

public:
    IMS_BOOL IsVideoSettingEnabled() const;
    IMS_BOOL IsVoipSettingEnabled() const;
    IMS_BOOL IsMobileDataSettingEnabled() const;
    IMS_BOOL IsVoWiFiSettingEnabled() const;
    IMS_BOOL IsVoPSActivated() const;
    IMS_BOOL IsVLTEnabledFromDM() const;
    IMS_BOOL IsLVCEnabledFromDM() const;
    IMS_BOOL IsMultiDeviceEnabledFromDM() const;
    IMS_BOOL IsVoWiFiEnabledFromDM() const;
    IMS_BOOL IsEABEnabledFromDM() const;
    IMS_BOOL IsVoPSFeatureChecked() const;
    IMS_BOOL IsVTAllowedRegardlessOfEAB() const;
    IMS_BOOL IsSRLTE() const;
    IMS_BOOL IsHVoLTE() const;
    IMS_BOOL IsSVLTE() const;
    IMS_BOOL IsCdmaLess() const;
    IMS_BOOL IsCsRoaming() const;
    IMS_BOOL IsLteRoaming() const;
    IMS_BOOL IsSsacVoiceBlocked() const;
    IMS_BOOL IsChipsetMTK() const;

    IMS_BOOL IsWifiTestMode() const;
    void SetIsWifiTestMode(IN IMS_BOOL bWifiTestMode);

    IMS_BOOL IsWifiHandovered() const;
    void SetIsWifiHandovered(IN IMS_BOOL bWifiHandover);

    IMS_UINT32 GetCurrentRat() const;
    void SetCurrentRat(IN IMS_UINT32 nRAT);

    void SetBarringSSAC(IN IMS_BOOL bBrarring);
    IMS_BOOL IsBarringSSAC() const;

    IMS_UINT32 GetTestMask() const;
    IMS_UINT32 GetDeviceMode() const;
    IMS_UINT32 GetLteRatDelayedInterval() const;
    IMS_UINT32 GetRoamingPreferredVoiceCallNetwork() const;
    IMS_UINT32 GetLteAttachType() const;
    AString GetPANI() const;

    IMS_BOOL IsNetworkModeSupportCDMA() const;

    enum
    {
        RAT_NONE = 0,
        RAT_RTT1X,
        RAT_EVDO,
        RAT_EHRPD,
        RAT_LTE,
        RAT_NR,
        RAT_WLAN
    };

    enum
    {
        DEVICE_MODE_SVLTE = 0,
        DEVICE_MODE_SRLTE,
        DEVICE_MODE_HVOLTE,
        DEVICE_MODE_CDMA_LESS_TYPE3,
        DEVICE_MODE_CDMA_LESS_TYPE4
    };

private:
    void Initialize();
    void Uninitialize();

    void ProcessSettingChanged(IN IMS_UINT32 nEvent, IN IMS_UINT32 nValue);
    void ProcessVoPSChanged(IN IMS_BOOL bActivated);
    void ProcessConfigUpdated(IN IMS_UINT32 nEvent, IN IMS_UINT32 nItem);
    void ProcessOMADMUpdated(IN IMS_UINT32 nEvent, IN IMS_UINT32 nValue);
    void ProcessRoamingChanged(IN IMS_UINT32 nPsState, IN IMS_UINT32 nCsState);
    void ProcessLteState(IN IMS_UINT32 nType, IN IMS_UINT32 nValue);
    void ProcessSsacChanged(IN IMS_UINT32 nValue);
    void ProcessLteAttachTypeChanged(IN IMS_UINT32 nValue);
    void ProcessNetworkModeSupportsCdmaChanged(IN IMS_BOOL bSupported);

    // IEventListener
    virtual void Event_NotifyEvent(IN IMS_SINT32 nEvent, IN IMS_UINT32 nWParam, IN IMS_UINT32 nLParam);

private:
    friend class ConfigEnabler;

    IMS_BOOL bIsVideoSettingEnabled;
    IMS_BOOL bIsVoipSettingEnabled;
    IMS_BOOL bIsMobileDataSettingEnabled;
    IMS_BOOL bIsVoWiFiSettingEnabled;
    IMS_BOOL bIsVoPSActivated;
    IMS_BOOL bIsVLTEnabledFromDM;
    IMS_BOOL bIsLVCEnabledFromDM;
    IMS_BOOL bIsMultiDeviceEnabledFromDM;
    IMS_BOOL bIsVoWiFiEnabledFromDM;
    IMS_BOOL bIsEABEnabledFromDM;
    IMS_BOOL bIsVoPSFeatureChecked;
    IMS_BOOL bIsVTAllowedRegardlessOfEAB;
    IMS_BOOL bIsWifiHandovered;
    IMS_BOOL bIsWifiTestMode;
    IMS_BOOL bIsBarringSSAC;
    IMS_BOOL bIsCsRoaming;
    IMS_BOOL bIsLteRoaming;
    IMS_BOOL bIsSsacVoiceBlocked;
    IMS_BOOL bIsChipsetMTK;

    IMS_UINT32 nTestMask;
    // SVLTE, SRLTE, HVoLTE
    IMS_UINT32 nDeviceMode;
    IMS_UINT32 nCurrentRAT;
    IMS_UINT32 nLTERatDelayedInterval;
    IMS_UINT32 nRoamingPreferredVoiceCallNetwork;
    IMS_UINT32 nLteAttachType;

    AString strPANI;

    IMS_BOOL bVoWiFiEnabled;
    IMS_BOOL bNetworkModeSupportsCDMA;
};
#endif // _VZW_PROPERTY_
