#include "AString.h"
#include "utility/MtsStrName.h"

PUBLIC
AString MtsStrName::GetMtsAppId()
{
    return AString("ims.app.mts");
}

PUBLIC
AString MtsStrName::GetMtsServiceId()
{
    return AString("ims.service.mts");
}

PUBLIC
AString MtsStrName::GetMtsConnectorName()
{
    return AString("imscore://ims.app.mts;serviceId=ims.service.mts");
}
