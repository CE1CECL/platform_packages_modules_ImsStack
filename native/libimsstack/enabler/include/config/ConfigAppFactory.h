/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20170717  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _CONFIG_APP_FACTORY_H_
#define _CONFIG_APP_FACTORY_H_

#include "base/ConfigApp.h"

class ConfigAppFactory
{
private:
    ConfigAppFactory();

public:
    static ConfigApp* Create(IN IMS_SINT32 nSlotId);
    static void Destroy(IN_OUT ConfigApp*& pConfigApp);
};

#endif // _CONFIG_APP_FACTORY_H_
