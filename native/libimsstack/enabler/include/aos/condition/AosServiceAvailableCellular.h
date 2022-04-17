#ifndef _AOS_SERVICE_AVAILABLE_CELLULAR_H_
#define _AOS_SERVICE_AVAILABLE_CELLULAR_H_

#include "condition/AosServiceAvailable.h"

class AosServiceAvailableCellular
    : public AosServiceAvailable
{
public:
    AosServiceAvailableCellular();
    virtual ~AosServiceAvailableCellular();

private:
    void HandleNetworkStateChanged() final;
    void HandleRoamingChanged(IN IMS_UINT32 nState) final;
    void HandleAirplaneModeChanged(IN IMS_UINT32 nState) final;
    void HandleVolteSettingChanged(IN IMS_UINT32 nState) final;
    void HandleVopsChanged(IN IMS_UINT32 nState) final;
    IMS_BOOL CheckServiceAvailable() final;
    IMS_BOOL IsAvailableRatForVops(IN IMS_UINT32 nState);

private:
    IMS_BOOL m_bVolteSetting;
    IMS_BOOL m_bVopsState;
    IMS_BOOL m_bNetworkServiceIn;
};

#endif // _AOS_SERVICE_AVAILABLE_CELLULAR_H_
