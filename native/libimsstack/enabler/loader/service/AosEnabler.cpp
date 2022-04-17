/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "manager/AosFactory.h"
#include "service/AosEnabler.h"

PUBLIC
AosEnabler::AosEnabler(IN IMS_SINT32 nSlotId)
    : Enabler(nSlotId)
{
}

PUBLIC VIRTUAL
AosEnabler::~AosEnabler()
{
}

PUBLIC VIRTUAL
void AosEnabler::Start()
{
    AosFactory::Start(GetSlotId());
}

PUBLIC VIRTUAL
void AosEnabler::Stop()
{
    AosFactory::Stop(GetSlotId());
}
