#ifndef _INTERFACE_AOS_TRM_H_
#define _INTERFACE_AOS_TRM_H_

class IAosTrmListener
{
public:
    virtual void Trm_PriorityChanged() = 0;
};

class IAosTrm
{
public:
    virtual void SetListener(IN IAosTrmListener *piListener) = 0;
    virtual void RemoveListener(IN IAosTrmListener *piListener) = 0;

    virtual IMS_BOOL IsReady() = 0;
    virtual IMS_BOOL IsTRMSupported() = 0;
    virtual void Set(IN IMS_UINT32 nType, IN IMS_BOOL bStart) = 0;
    virtual void SetEmegency(IN IMS_UINT32 nType, IN IMS_BOOL bStart) = 0;
    virtual void SetIPCAN(IN IN IMS_UINT32 nCategory) = 0;

    enum
    {
        TYPE_NONE            = 0,
        TYPE_REG             = (0x0001),
        TYPE_SUB             = (0x0002),
        TYPE_PDN             = (0x0004)
    };
};
#endif // _INTERFACE_AOS_TRM_H_
