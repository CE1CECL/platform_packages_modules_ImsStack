/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20170717  hwangoo.park@             Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "ConfigAppFactory.h"

LOCAL
AString configAppFactory_CreateName(IN IMS_SINT32 nSlotId)
{
    AString strName;

    strName.Sprintf("ConfigApp%02d", nSlotId);

    return strName;
}

ConfigApp* ConfigAppFactory::Create(IN IMS_SINT32 nSlotId)
{
    ConfigApp* pConfigApp = new ConfigApp(configAppFactory_CreateName(nSlotId));

    if (pConfigApp != IMS_NULL)
    {
        pConfigApp->Start();
    }

    return pConfigApp;
}

void ConfigAppFactory::Destroy(IN_OUT ConfigApp*& pConfigApp)
{
    if (pConfigApp != IMS_NULL)
    {
        delete pConfigApp;
        pConfigApp = IMS_NULL;
    }
}
