#ifndef MTS_CARRIER_CONFIG_ITEMS_H_
#define MTS_CARRIER_CONFIG_ITEMS_H_

#include "IMSTypeDef.h"
#include "IMSVector.h"

struct MtsCarrierConfigItems
{
public:
    MtsCarrierConfigItems() :
            bSmsCsfbRetryOnFailure(IMS_FALSE),
            nSmsOverImsFormat(0)
    {
    }

    MtsCarrierConfigItems(IN const MtsCarrierConfigItems&) = delete;
    MtsCarrierConfigItems& operator=(IN const MtsCarrierConfigItems&) = delete;

public:
    // sms configurations
    IMS_BOOL bSmsCsfbRetryOnFailure;
    IMS_SINT32 nSmsOverImsFormat;
};

#endif
