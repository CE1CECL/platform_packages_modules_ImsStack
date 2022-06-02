/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20170622  hwangoo.park@             Created
    </table>

    Description

*/
#include "SipFactoryProxy.h"
#include "SipRtConfigUtils.h"

PUBLIC GLOBAL SipRtConfigHelper* SipRtConfigUtils::GetConfigHelper(IN IMS_SINT32 nSlotId)
{
    return SipFactoryProxy::GetInstance()->GetRtConfigHelper(nSlotId);
}

PUBLIC GLOBAL IMS_BOOL SipRtConfigUtils::IsMessageHiddenInLog(IN IMS_SINT32 nSlotId)
{
    SipRtConfigHelper* pConfigHelper = GetConfigHelper(nSlotId);

    return (pConfigHelper != IMS_NULL) && pConfigHelper->IsMessageHiddenInLog();
}

PUBLIC GLOBAL IMS_BOOL SipRtConfigUtils::IsRoutingInfoHiddenInLog(IN IMS_SINT32 nSlotId)
{
    SipRtConfigHelper* pConfigHelper = GetConfigHelper(nSlotId);

    return (pConfigHelper != IMS_NULL) && pConfigHelper->IsRoutingInfoHiddenInLog();
}

PUBLIC GLOBAL IMS_BOOL SipRtConfigUtils::IsFeatureSipTxPacketBlockedEnabled(IN IMS_SINT32 nSlotId)
{
    SipRtConfigHelper* pConfigHelper = GetConfigHelper(nSlotId);

    return (pConfigHelper != IMS_NULL) &&
            pConfigHelper->IsFeatureEnabled(SipRtConfig::FEATURE_SIP_TX_PACKET_BLOCKED);
}

PUBLIC GLOBAL IMS_BOOL SipRtConfigUtils::IsIpSecSaConfigured(IN IMS_SINT32 nSlotId)
{
    SipRtConfigHelper* pConfigHelper = GetConfigHelper(nSlotId);

    return (pConfigHelper != IMS_NULL) &&
            pConfigHelper->IsItemConfigured(SipRtConfig::CONFIG_I_IPSEC_SA);
}

PUBLIC GLOBAL IMS_BOOL SipRtConfigUtils::IsRegContactAddressConfigured(IN IMS_SINT32 nSlotId)
{
    SipRtConfigHelper* pConfigHelper = GetConfigHelper(nSlotId);

    return (pConfigHelper != IMS_NULL) &&
            pConfigHelper->IsItemConfigured(SipRtConfig::CONFIG_I_REG_CONTACT_ADDRESS);
}

PUBLIC GLOBAL IMS_BOOL SipRtConfigUtils::IsTcpPortRangeConfigured(IN IMS_SINT32 nSlotId)
{
    SipRtConfigHelper* pConfigHelper = GetConfigHelper(nSlotId);

    return (pConfigHelper != IMS_NULL) &&
            pConfigHelper->IsItemConfigured(SipRtConfig::CONFIG_I_TCP_PORT_RANGE);
}
