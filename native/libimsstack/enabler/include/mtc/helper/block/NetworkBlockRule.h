#ifndef NETWORK_BLOCK_RULE_H_
#define NETWORK_BLOCK_RULE_H_

#include "IMSTypeDef.h"
#include "helper/block/IMtcBlockRule.h"

class INetWatcherInfo;

class NetworkBlockRule final:
        public IMtcBlockRule
{
public:
    explicit NetworkBlockRule(
            IN const IMtcService& objService, IN INetWatcherInfo& objNetWatcherInfo);
    virtual ~NetworkBlockRule();
    NetworkBlockRule(IN const NetworkBlockRule&) = delete;
    NetworkBlockRule& operator=(IN const NetworkBlockRule&) = delete;

    Result Check(IN IMtcBlockRuleCheckListener& objListener) override;

private:
    IMS_BOOL IsInEpdg(IN const IMtcService& objService);
    IMS_BOOL IsWifiRegistered(IN MtcAosConnector* pAosConnector);

    const IMtcService& m_objService;
    INetWatcherInfo& m_objNetWatcherInfo;
};

#endif
