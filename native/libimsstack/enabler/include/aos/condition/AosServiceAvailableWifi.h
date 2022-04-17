#ifndef _AOS_SERVICE_AVAILABLE_WIFI_H_
#define _AOS_SERVICE_AVAILABLE_WIFI_H_

#include "INetPingListener.h"
#include "IWifiWatcher.h"
#include "condition/AosServiceAvailable.h"

class AosServiceAvailableWifi
    : public AosServiceAvailable
    , public INetPingListener
    , public IWifiWatcherListener
{
public:
    AosServiceAvailableWifi();
    virtual ~AosServiceAvailableWifi();

    void StartToCheckNetworkConnection() final;
    IMS_BOOL StopToCheckNetworkConnection(IN IMS_BOOL bNeedToCheckAvailable = IMS_TRUE) final;

private:
    void RegisterListener() final;
    void DeregisterListener() final;

    // IWifiWatcherListener
    void NotifyStateChanged(IN class IWifiWatcher* pIWifiWatcher) final;

    // INetPingListener
    void NetPing_NotifyResult(IN INetPing* piNetPing, IN IMS_SINT32 nResult) final;

    void HandleCallStateChanged(IN IMS_UINT32 nState, IN IMS_SINT32 nStateEx) final;
    void HandleRoamingChanged(IN IMS_UINT32 nState) final;
    void HandleAirplaneModeChanged(IN IMS_UINT32 nState) final;
    void HandleWfcSettingChanged(IN IMS_UINT32 nState) final;
    void HandleWiFiConnectionChanged() final;
    void HandleLocationInfoChanged() final;

    IMS_BOOL CheckServiceAvailable() final;

    void ProcessBadConnectionReported();
    void ClearBadNetworkState();

    IMS_BOOL RequestNetPing();
    static const IMS_CHAR* PingResultToString(IN IMS_SINT32 nResult);

    enum
    {
        STATE_BAD_NETWORK_NONE = 0,
        STATE_BAD_NETWORK_CHECKING,
        STATE_BAD_NETWORK_DETECTED
    };

private:
    AString m_strCountry;
    IMS_UINT32 m_nVoWiFiSetting;
    IMS_UINT32 m_nBadNetworkState;
    IMS_BOOL m_bWiFiState;
    INetPing* m_piNetPing;

    static const IMS_UINT32 TIME_BAD_NETWORK_CHECK = 3000;
};

#endif // _AOS_SERVICE_AVAILABLE_WIFI_H_
