#ifndef MTS_ENABLER_H_
#define MTS_ENABLER_H_

#include "Enabler.h"

class MtsEnabler final :
        public Enabler
{
public:
    explicit MtsEnabler(IN IMS_SINT32 nSlotId);
    ~MtsEnabler();

public:
    // IEnabler class
    void Start() override;
    void Stop() override;
};

#endif
