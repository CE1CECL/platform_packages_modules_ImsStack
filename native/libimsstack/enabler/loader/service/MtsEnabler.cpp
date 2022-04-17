#include "ServiceTrace.h"

#include "MtsFactory.h"
#include "service/MtsEnabler.h"

__IMS_TRACE_TAG_USER_DECL__("SMS");

PUBLIC
MtsEnabler::MtsEnabler(IN IMS_SINT32 nSlotId) :
        Enabler(nSlotId)
{
    IMS_TRACE_D("+MtsEnabler", 0, 0, 0);
    MtsFactory::GetInstance();
}

PUBLIC
MtsEnabler::~MtsEnabler()
{
    MtsFactory::GetInstance()->Destroy(GetSlotId());
}

PUBLIC
void MtsEnabler::Start()
{
    IMS_TRACE_D("MtsEnabler Start", 0, 0, 0);

    MtsFactory::GetInstance()->StartMts(GetSlotId());
}

PUBLIC
void MtsEnabler::Stop()
{
    IMS_TRACE_D("Stop", 0, 0, 0);

    MtsFactory::GetInstance()->StopMts(GetSlotId());
}
