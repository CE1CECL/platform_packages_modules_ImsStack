/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "UceApp.h"
#include "service/UceEnabler.h"

PUBLIC
UceEnabler::UceEnabler(IN IMS_SINT32 nSlotId)
    : Enabler(nSlotId)
    , pApp(IMS_NULL)
{
}

PUBLIC VIRTUAL
UceEnabler::~UceEnabler()
{
}

PUBLIC VIRTUAL
void UceEnabler::Start()
{
    pApp = UceApp::GetInstance(GetSlotId());
}

PUBLIC VIRTUAL
void UceEnabler::Stop()
{
    if (pApp != IMS_NULL)
    {
        delete pApp;
        pApp = IMS_NULL;
    }
}
