/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    </table>

    Description
*/

#include "CarrierConfig.h"
#include "ServiceMemory.h"
#include "ServiceTrace.h"
#include "ServiceUtil.h"
#include "ServiceEvent.h"
#include "IMSStrLib.h"
#include "ImsConfig.h"
#include "operator/VZWProperty.h"

__IMS_TRACE_TAG_USER_DECL__("VZW");

PRIVATE
VZWProperty::VZWProperty()
    : bIsVideoSettingEnabled(IMS_TRUE)
    , bIsVoipSettingEnabled(IMS_TRUE)
    , bIsMobileDataSettingEnabled(IMS_TRUE)
    , bIsVoWiFiSettingEnabled(IMS_FALSE)
    , bIsVoPSActivated(IMS_FALSE)
    , bIsVLTEnabledFromDM(IMS_TRUE)
    , bIsLVCEnabledFromDM(IMS_TRUE)
    , bIsMultiDeviceEnabledFromDM(IMS_FALSE)
    , bIsVoWiFiEnabledFromDM(IMS_TRUE)
    , bIsEABEnabledFromDM(IMS_TRUE)
    , bIsVoPSFeatureChecked(IMS_TRUE)
    , bIsVTAllowedRegardlessOfEAB(IMS_FALSE)
    , bIsWifiHandovered(IMS_FALSE)
    , bIsWifiTestMode(IMS_FALSE)
    , bIsBarringSSAC(IMS_FALSE)
    , bIsCsRoaming(IMS_FALSE)
    , bIsLteRoaming(IMS_FALSE)
    , bIsSsacVoiceBlocked(IMS_FALSE)
    , bIsChipsetMTK(IMS_FALSE)
    , nTestMask(0)
    , nDeviceMode(DEVICE_MODE_HVOLTE)
    , nCurrentRAT(RAT_NONE)
    , nLTERatDelayedInterval(0)
    , nRoamingPreferredVoiceCallNetwork(IMS_ROAMING_PREFERRED_CELLULAR_NETWORK)
    , nLteAttachType(IMS_LTE_INFO_UNKNOWN)
    , strPANI(AString::ConstNull())
    , bVoWiFiEnabled(IMS_FALSE)
    , bNetworkModeSupportsCDMA(IMS_TRUE)
{
}

PRIVATE VIRTUAL
VZWProperty::~VZWProperty()
{
}

/*

Remarks

*/
PUBLIC GLOBAL
VZWProperty* VZWProperty::GetInstance()
{
    static VZWProperty *pProperty = IMS_NULL;

    if (pProperty == IMS_NULL)
    {
        pProperty = new VZWProperty();
    }

    return pProperty;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsVideoSettingEnabled() const
{
    return bIsVideoSettingEnabled;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsVoipSettingEnabled() const
{
    return bIsVoipSettingEnabled;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsMobileDataSettingEnabled() const
{
    return bIsMobileDataSettingEnabled;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsVoWiFiSettingEnabled() const
{
    return bIsVoWiFiSettingEnabled;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsVoPSActivated() const
{
    return bIsVoPSActivated;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsVLTEnabledFromDM() const
{
    return bIsVLTEnabledFromDM;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsLVCEnabledFromDM() const
{
    return bIsLVCEnabledFromDM;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsMultiDeviceEnabledFromDM() const
{
    return bIsMultiDeviceEnabledFromDM;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsVoWiFiEnabledFromDM() const
{
    return bIsVoWiFiEnabledFromDM;
}
/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsEABEnabledFromDM() const
{
    return bIsEABEnabledFromDM;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsVoPSFeatureChecked() const
{
    return bIsVoPSFeatureChecked;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsVTAllowedRegardlessOfEAB() const
{
    return bIsVTAllowedRegardlessOfEAB;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsSRLTE() const
{
    return (nDeviceMode == DEVICE_MODE_SRLTE);
}

/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsHVoLTE() const
{
    return (nDeviceMode == DEVICE_MODE_HVOLTE);
}

/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsSVLTE() const
{
    return (nDeviceMode == DEVICE_MODE_SVLTE);
}

/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsCdmaLess() const
{
    return (nDeviceMode >= DEVICE_MODE_CDMA_LESS_TYPE3);
}

/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsCsRoaming() const
{
    return bIsCsRoaming;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsLteRoaming() const
{
    return bIsLteRoaming;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsSsacVoiceBlocked() const
{
    return bIsSsacVoiceBlocked;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsChipsetMTK() const
{
    return bIsChipsetMTK;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsWifiTestMode() const
{
    return bIsWifiTestMode;
}

/*

Remarks

*/
PUBLIC
void VZWProperty::SetIsWifiTestMode(IN IMS_BOOL bWifiTestMode)
{
    bIsWifiTestMode = bWifiTestMode;

    if (bIsWifiTestMode)
    {
        bIsVoPSFeatureChecked = IMS_FALSE;
    }
}

/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsWifiHandovered() const
{
    return bIsWifiHandovered;
}

/*

Remarks

*/
PUBLIC
void VZWProperty::SetIsWifiHandovered(IN IMS_BOOL bWifiHandover)
{
    bIsWifiHandovered = bWifiHandover;
}

/*

Remarks

*/
PUBLIC
IMS_UINT32 VZWProperty::GetCurrentRat() const
{
    return nCurrentRAT;
}

/*

Remarks

*/
PUBLIC
void VZWProperty::SetCurrentRat(IN IMS_UINT32 nRAT)
{
    nCurrentRAT = nRAT;
}

/*

Remarks

*/
PUBLIC
void VZWProperty::SetBarringSSAC(IN IMS_BOOL bBrarring)
{
    bIsBarringSSAC = bBrarring;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsBarringSSAC() const
{
    return bIsBarringSSAC;
}


/*

Remarks

*/
PUBLIC
IMS_UINT32 VZWProperty::GetTestMask() const
{
    return nTestMask;
}

/*

Remarks

*/
PUBLIC
IMS_UINT32 VZWProperty::GetDeviceMode() const
{
    return nDeviceMode;
}

/*

Remarks

*/
PUBLIC
IMS_UINT32 VZWProperty::GetLteRatDelayedInterval() const
{
    return nLTERatDelayedInterval;
}

/*

Remarks

*/
PUBLIC
IMS_UINT32 VZWProperty::GetRoamingPreferredVoiceCallNetwork() const
{
    return nRoamingPreferredVoiceCallNetwork;
}

/*

Remarks

*/
PUBLIC
IMS_UINT32 VZWProperty::GetLteAttachType() const
{
    return nLteAttachType;
}

/*

Remarks

*/
PUBLIC
AString VZWProperty::GetPANI() const
{
    return strPANI;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL VZWProperty::IsNetworkModeSupportCDMA() const
{
    return bNetworkModeSupportsCDMA;
}

/*

Remarks

*/
PRIVATE
void VZWProperty::Initialize()
{
    // add event
    IMS_EVENT_AddListenerForSlotId(IMS_EVENT_CONFIG_UPDATE, this, IMS_SLOT_0);
    IMS_EVENT_AddListenerForSlotId(IMS_EVENT_OMADM_UPDATED, this, IMS_SLOT_0);
    IMS_EVENT_AddListenerForSlotId(IMS_EVENT_VOIP_SETTING, this, IMS_SLOT_0);
    IMS_EVENT_AddListenerForSlotId(IMS_EVENT_VIDEO_SETTING, this, IMS_SLOT_0);
    IMS_EVENT_AddListenerForSlotId(IMS_EVENT_MOBILE_DATA_SETTING, this, IMS_SLOT_0);
    IMS_EVENT_AddListenerForSlotId(IMS_EVENT_IMS_VOICE_OVER_PS_STATE, this, IMS_SLOT_0);
    IMS_EVENT_AddListenerForSlotId(IMS_EVENT_ROAMING_STATE, this, IMS_SLOT_0);
    IMS_EVENT_AddListenerForSlotId(IMS_EVENT_LTE_STATE, this, IMS_SLOT_0);
    IMS_EVENT_AddListenerForSlotId(IMS_EVENT_LTE_INFO, this, IMS_SLOT_0);
    IMS_EVENT_AddListenerForSlotId(IMS_EVENT_NETWORK_MODE_SUPPORTS_CDMA, this, IMS_SLOT_0);

    if (CarrierConfig::IsWfcEnabled(IMS_SLOT_0))
    {
        bVoWiFiEnabled = IMS_TRUE;

        IMS_EVENT_AddListenerForSlotId(IMS_EVENT_WFC_SETTING_CHANGED, this, IMS_SLOT_0);
        IMS_EVENT_AddListenerForSlotId(IMS_EVENT_ROAMING_PREFERRED_VOICE_CALL_NETWORK,
                this, IMS_SLOT_0);
    }

    // check WIFI test mode
    IImsPrivateProperty* piProperty = UtilService::GetUtilService()->GetPrivateProperty();

    bIsWifiTestMode = (piProperty->GetPersistentInt(
            ImsPrivateProperties::Persistent::KEY_WIFI_TEST, 0) == 1);

    // check MTK chipset
    bIsChipsetMTK = UtilService::GetUtilService()->GetSystemProperty(
            )->GetChipsetVendor().EqualsIgnoreCase("MediaTek");

    // TODO: These values should be read by the test settings.
    #if 0
    bIsVideoSettingEnabled = piBuffer->ReadValueBoolean("video");
    bIsVoipSettingEnabled= piBuffer->ReadValueBoolean("voip");
    bIsMobileDataSettingEnabled = piBuffer->ReadValueBoolean("mobile_data");


    bIsVoPSFeatureChecked = (bIsWifiTestMode) ?
            IMS_FALSE : piBuffer->ReadValueBoolean("vops_feature");
    nLTERatDelayedInterval = piBuffer->ReadValueInt("irat_transition_time");

    nTestMask = piBuffer->ReadValueInt("test_mask");
    nDeviceMode = piBuffer->ReadValueInt("device_mode");

    // HWAN_PORTING {
    if (static_cast<IMS_SINT32>(nTestMask) == -1)
    {
        nTestMask = 0;
    }

    if (static_cast<IMS_SINT32>(nDeviceMode) == -1)
    {
        nDeviceMode = DEVICE_MODE_HVOLTE;
    }
    // HWAN_PORTING }

    const AString& strCurrPANI = piBuffer->ReadValue("emergency_pan_info");
    if (strCurrPANI.GetLength() > 0)
    {
        strPANI = strCurrPANI;
    }
    #endif

    IMS_CHAR acLog[512+1] = {0, };
    IMS_Sprintf(acLog, 512, "wifi test(%s) , video(%s) , voip(%s) , moblie data(%s)"
        " , check VoPS(%s) , mtk(%s), LTE rat delay(%d) , test mask(%X) , device mode(%d)",
        _TRACE_B_(bIsWifiTestMode),
        _TRACE_B_(bIsVideoSettingEnabled),
        _TRACE_B_(bIsVoipSettingEnabled),
        _TRACE_B_(bIsMobileDataSettingEnabled),
        _TRACE_B_(bIsVoPSFeatureChecked),
        _TRACE_B_(bIsChipsetMTK),
        nLTERatDelayedInterval, nTestMask, nDeviceMode);

    IMS_TRACE_D("Initialize :: (%s)", acLog, 0, 0);
}

/*

Remarks

*/
PRIVATE
void VZWProperty::Uninitialize()
{
    IMS_TRACE_D("Uninitialize", 0, 0, 0);

    // remove event
    IMS_EVENT_RemoveListenerForSlotId(IMS_EVENT_CONFIG_UPDATE, this, IMS_SLOT_0);
    IMS_EVENT_RemoveListenerForSlotId(IMS_EVENT_OMADM_UPDATED, this, IMS_SLOT_0);
    IMS_EVENT_RemoveListenerForSlotId(IMS_EVENT_VOIP_SETTING, this, IMS_SLOT_0);
    IMS_EVENT_RemoveListenerForSlotId(IMS_EVENT_VIDEO_SETTING, this, IMS_SLOT_0);
    IMS_EVENT_RemoveListenerForSlotId(IMS_EVENT_MOBILE_DATA_SETTING, this, IMS_SLOT_0);
    IMS_EVENT_RemoveListenerForSlotId(IMS_EVENT_IMS_VOICE_OVER_PS_STATE, this, IMS_SLOT_0);
    IMS_EVENT_RemoveListenerForSlotId(IMS_EVENT_ROAMING_STATE, this, IMS_SLOT_0);
    IMS_EVENT_RemoveListenerForSlotId(IMS_EVENT_LTE_STATE, this, IMS_SLOT_0);
    IMS_EVENT_RemoveListenerForSlotId(IMS_EVENT_LTE_INFO, this, IMS_SLOT_0);
    IMS_EVENT_RemoveListenerForSlotId(IMS_EVENT_NETWORK_MODE_SUPPORTS_CDMA, this, IMS_SLOT_0);

    if (bVoWiFiEnabled)
    {
        IMS_EVENT_RemoveListenerForSlotId(IMS_EVENT_WFC_SETTING_CHANGED, this, IMS_SLOT_0);
        IMS_EVENT_RemoveListenerForSlotId(IMS_EVENT_ROAMING_PREFERRED_VOICE_CALL_NETWORK,
                this, IMS_SLOT_0);
    }

    bIsVideoSettingEnabled = IMS_TRUE;
    bIsVoipSettingEnabled = IMS_TRUE;
    bIsMobileDataSettingEnabled = IMS_TRUE;
    bIsVoWiFiSettingEnabled = IMS_FALSE;
    bIsVoPSActivated = IMS_FALSE;
    bIsVLTEnabledFromDM = IMS_TRUE;
    bIsLVCEnabledFromDM = IMS_TRUE;
    bIsMultiDeviceEnabledFromDM = IMS_FALSE;
    bIsVoWiFiEnabledFromDM = IMS_TRUE;
    bIsEABEnabledFromDM = IMS_TRUE;
    bIsVoPSFeatureChecked = IMS_TRUE;
    bIsVTAllowedRegardlessOfEAB = IMS_FALSE;
    bIsWifiHandovered = IMS_FALSE;
    bIsWifiTestMode = IMS_FALSE;
    bIsBarringSSAC = IMS_FALSE;
    bIsCsRoaming = IMS_FALSE;
    bIsLteRoaming = IMS_FALSE;
    bIsSsacVoiceBlocked = IMS_FALSE;
    bIsChipsetMTK = IMS_FALSE;
    nTestMask = 0;
    nDeviceMode = DEVICE_MODE_HVOLTE;
    nCurrentRAT = RAT_NONE;
    nLTERatDelayedInterval = 0;
    nRoamingPreferredVoiceCallNetwork = IMS_ROAMING_PREFERRED_CELLULAR_NETWORK;
    nLteAttachType = IMS_LTE_INFO_UNKNOWN;
    strPANI = AString::ConstNull();
    bVoWiFiEnabled = IMS_FALSE;
    bNetworkModeSupportsCDMA = IMS_TRUE;
}

/*

Remarks

*/
PRIVATE
void VZWProperty::ProcessSettingChanged(IN IMS_UINT32 nEvent, IN IMS_UINT32 nValue)
{
    IMS_BOOL bON = (nValue > 0) ? IMS_TRUE : IMS_FALSE;

    switch (nEvent)
    {
        case IMS_EVENT_VOIP_SETTING:
            bIsVoipSettingEnabled = bON;
            break;
        case IMS_EVENT_VIDEO_SETTING:
            bIsVideoSettingEnabled = bON;
            break;
        case IMS_EVENT_MOBILE_DATA_SETTING:
            bIsMobileDataSettingEnabled = bON;
            break;
        case IMS_EVENT_WFC_SETTING_CHANGED:
            bIsVoWiFiSettingEnabled = bON;
            break;

        default:
            break;
    }
}

/*

Remarks

*/
PRIVATE
void VZWProperty::ProcessVoPSChanged(IN IMS_BOOL bActivated)
{
    bIsVoPSActivated = bActivated;
}

/*

Remarks

*/
PRIVATE
void VZWProperty::ProcessConfigUpdated(IN IMS_UINT32 nEvent, IN IMS_UINT32 nItem)
{
    if ( (nEvent == IMS_CONFIG_CAT_0 && nItem == ImsConfig::SDM_I_TDELAY)
        || (nEvent == IMS_CONFIG_CAT_1 && nItem == ImsConfig::PST_I_TDELAY) )
    {
        // TODO: These values should be read by the test settings.
        // nLTERatDelayedInterval = piBuffer->ReadValueInt("irat_transition_time");

        IMS_TRACE_D("ProcessConfigUpdated :: LTE RAT delay interval (%d)",
                nLTERatDelayedInterval, 0, 0);
    }
}

/*

Remarks

*/
PRIVATE
void VZWProperty::ProcessOMADMUpdated(IN IMS_UINT32 nEvent, IN IMS_UINT32 nValue)
{
    IMS_BOOL bON = (nValue > 0) ? IMS_TRUE : IMS_FALSE;

    switch (nEvent)
    {
        case IMS_OMADM_VLT:
            bIsVLTEnabledFromDM = bON;
            break;
        case IMS_OMADM_LVC:
            bIsLVCEnabledFromDM = bON;
            break;
        case IMS_OMADM_MULTIDEVICE:
            bIsMultiDeviceEnabledFromDM = bON;
            break;
        case IMS_OMADM_VOWIFI:
            bIsVoWiFiEnabledFromDM = bON;
            break;
        case IMS_OMADM_EAB:
            bIsEABEnabledFromDM = bON;
            break;

        case IMS_OMADM_ALLOW_VT_REGARDLESS_OF_EAB:
            bIsVTAllowedRegardlessOfEAB = bON;
            break;

        default:
            break;
    }
}

/*

Remarks

*/
PRIVATE
void VZWProperty::ProcessRoamingChanged(IN IMS_UINT32 nPsState, IN IMS_UINT32 nCsState)
{
    bIsCsRoaming = (nCsState == IMS_ROAMING_STATE_ON) ? IMS_TRUE : IMS_FALSE;
    bIsLteRoaming = (nPsState == IMS_ROAMING_STATE_ON) ? IMS_TRUE : IMS_FALSE;
}

/*

Remarks

*/
PRIVATE
void VZWProperty::ProcessLteState(IN IMS_UINT32 nType, IN IMS_UINT32 nValue)
{
    switch (nType)
    {
        case IMS_LTE_BARRING_SSAC:
            ProcessSsacChanged(nValue);
            break;

        default:
            break;
    }
}

/*

Remarks

*/
PRIVATE
void VZWProperty::ProcessSsacChanged(IN IMS_UINT32 nValue)
{
    // 8bit type + 8bit factor + 16bit time
    IMS_UINT32 nType = nValue >> 24;

    // nType 0 : voice
    if (nType == 0)
    {
        IMS_UINT32 nVoiceFactor = (nValue << 8) >> 24;
        bIsSsacVoiceBlocked = (nVoiceFactor == 0);

        IMS_TRACE_D("ProcessSsacChanged :: voice ssac block (%s)",
            _TRACE_B_(bIsSsacVoiceBlocked), 0, 0);
    }
}

/*

Remarks

*/
PRIVATE
void VZWProperty::ProcessLteAttachTypeChanged(IN IMS_UINT32 nValue)
{
    if (nLteAttachType != nValue)
    {
        IMS_TRACE_D("ProcessLteAttachTypeChanged :: old(%d), new(%d)", nLteAttachType, nValue, 0);
        nLteAttachType = nValue;
    }
}

/*

Remarks

*/
PRIVATE
void VZWProperty::ProcessNetworkModeSupportsCdmaChanged(IN IMS_BOOL bSupported)
{
    IMS_TRACE_D("ProcessNetworkModeSupportCdmaChanged :: bSupported (%s)",
            _TRACE_B_(bSupported), 0, 0);
    bNetworkModeSupportsCDMA = bSupported;
}

/*

Remarks

*/
PRIVATE VIRTUAL
void VZWProperty::Event_NotifyEvent(IN IMS_SINT32 nEvent,
    IN IMS_UINT32 nWParam, IN IMS_UINT32 nLParam)
{
    switch (nEvent)
    {
        case IMS_EVENT_CONFIG_UPDATE:
            ProcessConfigUpdated(IMS_HIWORD(nWParam), IMS_LOWORD(nWParam));
            break;

        case IMS_EVENT_OMADM_UPDATED:
            ProcessOMADMUpdated(nWParam, nLParam);
            break;

        case IMS_EVENT_VOIP_SETTING: // FALL-THROUGH
        case IMS_EVENT_VIDEO_SETTING: // FALL-THROUGH
        case IMS_EVENT_MOBILE_DATA_SETTING: // FALL-THROUGH
        case IMS_EVENT_WFC_SETTING_CHANGED:
            ProcessSettingChanged(nEvent, nWParam);
            break;

        case IMS_EVENT_IMS_VOICE_OVER_PS_STATE:
            ProcessVoPSChanged((nWParam == IMS_VOICE_OVER_PS_SUPPORTED) ? IMS_TRUE : IMS_FALSE);
            break;

        case IMS_EVENT_ROAMING_STATE:
            ProcessRoamingChanged(nWParam, nLParam);
            break;

        case IMS_EVENT_ROAMING_PREFERRED_VOICE_CALL_NETWORK:
            nRoamingPreferredVoiceCallNetwork = nWParam;
            break;

        case IMS_EVENT_LTE_STATE:
            ProcessLteState(nWParam, nLParam);
            break;

        case IMS_EVENT_LTE_INFO:
            ProcessLteAttachTypeChanged(nWParam);
            break;

        case IMS_EVENT_NETWORK_MODE_SUPPORTS_CDMA:
            ProcessNetworkModeSupportsCdmaChanged(
                (nWParam == IMS_CDMA_SUPPORTED) ? IMS_TRUE : IMS_FALSE);
            break;

        default:
            break;
    }
}
