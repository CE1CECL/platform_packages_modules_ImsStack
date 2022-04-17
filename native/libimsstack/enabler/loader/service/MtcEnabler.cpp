#include "ServiceMemory.h"
#include "ServiceTrace.h"

#include "service/MtcEnabler.h"
#include "MtcFactory.h"


__IMS_TRACE_TAG_USER_DECL__("Mtc");

PUBLIC
MtcEnabler::MtcEnabler(IN IMS_SINT32 nSlotId) :
        Enabler(nSlotId)
{
    IMS_TRACE_D("+MtcEnabler", 0, 0, 0);
}

PUBLIC VIRTUAL
MtcEnabler::~MtcEnabler()
{
}

PUBLIC VIRTUAL
void MtcEnabler::Start()
{
    IMS_TRACE_D("Start", 0, 0, 0);
    MtcFactory::GetInstance()->Start(GetSlotId());
}

PUBLIC VIRTUAL
void MtcEnabler::Stop()
{
    IMS_TRACE_D("Stop", 0, 0, 0);
    MtcFactory::GetInstance()->Stop(GetSlotId());
}
