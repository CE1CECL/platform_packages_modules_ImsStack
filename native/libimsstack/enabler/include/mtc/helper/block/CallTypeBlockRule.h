#ifndef CALL_TYPE_BLOCK_RULE_H_
#define CALL_TYPE_BLOCK_RULE_H_

#include "IMSTypeDef.h"
#include "helper/block/IMtcBlockRule.h"

class IMtcCallContext;
class IMtcCallManager;
class MtcConfigurationProxy;
struct CallInfo;

class CallTypeBlockRule final : public IMtcBlockRule
{
public:
    explicit CallTypeBlockRule(IN IMtcCallContext& objContext);
    virtual ~CallTypeBlockRule();
    CallTypeBlockRule(IN const CallTypeBlockRule&) = delete;
    CallTypeBlockRule& operator=(IN const CallTypeBlockRule&) = delete;

    Result Check(IN IMtcBlockRuleCheckListener& objListener) override;

private:
    MtcConfigurationProxy& m_objConfiguration;
    IMtcCallManager& m_objCallManager;
    const CallInfo& m_objCallInfo;
};

#endif
