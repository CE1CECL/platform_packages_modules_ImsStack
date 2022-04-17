/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20091106  toastops@                 Created
    </table>

    Description

*/

#ifndef _IMS_REGISTRY_LOADER_H_
#define _IMS_REGISTRY_LOADER_H_

#include "ImsRegistry.h"

class ImsRegistryLoader
{
private:
    ImsRegistryLoader();

public:
    static IMS_BOOL GetRegistry(IN const AString &strAppId, IN IMS_SINT32 nSlotId,
            OUT ImsRegistry &objRegistry);
};

#endif // _IMS_REGISTRY_LOADER_H_
