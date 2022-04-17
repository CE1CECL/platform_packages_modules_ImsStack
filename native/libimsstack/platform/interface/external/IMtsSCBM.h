#ifndef INTERFACE_IMS_IMTSSCBM_H_
#define INTERFACE_IMS_IMTSSCBM_H_

#include "ImsWmsLiteTypeDef.h"

class IMtsSCBM
{
public:
    virtual IMS_RESULT ConnectSCBM(IN IMS_SINT32 nSlotID) = 0;
    virtual void DisconnectSCBM(IN IMS_SINT32 nSlotID) = 0;
    virtual void RequestRATSelection(IN IMSWMS_UINT32 bIsSCBMMode) = 0;
    virtual void RequestExitRATSelection() = 0;
    virtual void RequestSetE911State() = 0;
};

#endif
