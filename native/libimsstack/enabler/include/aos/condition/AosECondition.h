#ifndef _AOS_E_CONDITION_H_
#define _AOS_E_CONDITION_H_

#include "condition/AosCondition.h"

class AosECondition
    : public AosCondition
{
public:
    AosECondition(IN IAosAppContext* piAppContext);
    virtual ~AosECondition();

    IMS_BOOL IsReady() final;

private:
    inline void AddServiceAvailable() final {};
    inline void RemoveServiceAvailable() final {};

    inline void AddEventListener() final {};
    inline void RemoveEventListener() final {};

    void AddAosServiceListener() final;
    void RemoveAosServiceListener() final;

    // IAosBlockListener
    void Block_Changed(IN IMS_UINT32 nType, IN IMS_UINT32 nParam) final;

    // AosServicePhoneListener
    void ServicePhone_AosStart() final;
    inline void ServicePhone_LocationInfoChanged(IN LocationInfo /*eState*/) final {};
    inline void ServicePhone_PhoneNumberStateChanged(IN IMS_BOOL /*bIsRefresh*/,
            IN PhoneNumberState /*eState*/) final {};
    inline void ServicePhone_PlmnChanged() final {};
    inline void ServicePhone_PowerOff() final {};
};

#endif // _AOS_E_CONDITION_H_