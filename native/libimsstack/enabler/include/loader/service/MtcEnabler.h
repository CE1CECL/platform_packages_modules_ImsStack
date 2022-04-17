#ifndef MTC_ENABLER_H_
#define MTC_ENABLER_H_

#include "Enabler.h"
#include "IMSTypeDef.h"

class MtcEnabler :
        public Enabler
{
public:
    explicit MtcEnabler(IN IMS_SINT32 nSlotId);
    virtual ~MtcEnabler();
    MtcEnabler(IN const MtcEnabler&) = delete;
    MtcEnabler& operator=(IN const MtcEnabler&) = delete;

    // IEnabler interface implementation
    virtual void Start() override;
    virtual void Stop() override;

};

#endif
