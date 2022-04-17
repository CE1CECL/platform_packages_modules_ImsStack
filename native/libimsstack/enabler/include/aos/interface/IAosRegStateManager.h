#ifndef _INTERFACE_AOS_REG_STATE_MANAGER_H_
#define _INTERFACE_AOS_REG_STATE_MANAGER_H_

class IAosRegStateManager
{
public:
    virtual IMS_SINT32 GetSlotId() const = 0;
    virtual void SetSlotId(IN IMS_SINT32 nSlotId) = 0;

    virtual void SetImsRegState(IN IMS_UINT32 nState, IN IMS_BOOL bLimited) = 0;
    virtual IMS_SINT32 GetImsRegState() = 0;
    virtual void SetEImsRegState(IN IMS_UINT32 nState) = 0;
    virtual void SetRegState(IN IMS_UINT32 nServiceType, IN IMS_UINT32 nState) = 0;

    virtual void SetDetailState(IN IMS_SINT32 nState) = 0;
    virtual IMS_SINT32 GetDetailState() = 0;
    virtual void SetReason(IN IMS_UINT32 nReason) = 0;
    virtual void EnforceUpdateRegistration() = 0;
    virtual void UpdateRegistration() = 0;

    virtual void ClearRegServices() = 0;
    virtual IMS_UINT32 GetRegServices() const = 0;
    virtual void UpdateRegServices(IN IMS_BOOL bUpdateCurrState = IMS_FALSE) = 0;

    virtual void SetRegRespCode(IN IMS_SINT32 nRespCode) = 0;

    virtual IMS_BOOL IsLimitedMode() const = 0;
};
#endif // _INTERFACE_AOS_REG_STATE_MANAGER_H_
