/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20170221  hwangoo.park@             Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "Enabler.h"

PUBLIC
Enabler::Enabler(IN IMS_SINT32 nSlotId_)
    : nSlotId(nSlotId_)
{
}

PUBLIC VIRTUAL
Enabler::~Enabler()
{
}

PUBLIC VIRTUAL
void Enabler::Start()
{
    // no-op
}

PUBLIC VIRTUAL
void Enabler::Stop()
{
    // no-op
}

PUBLIC VIRTUAL
void Enabler::Destroy()
{
    delete this;
}
