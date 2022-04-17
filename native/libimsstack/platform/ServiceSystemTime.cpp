/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090819  yhrhee@                   Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "PlatformFactory.h"
#include "ServiceSystemTime.h"

class SystemTimeServicePrivate
{
public:
    inline SystemTimeServicePrivate()
        : piSysTime(IMS_NULL)
    {}
    inline ~SystemTimeServicePrivate()
    {
        PlatformFactory::DestroySystemTime(piSysTime);
    }

private:
    SystemTimeServicePrivate(IN const SystemTimeServicePrivate& objRHS);
    SystemTimeServicePrivate& operator=(IN const SystemTimeServicePrivate& objRHS);

public:
    inline ISystemTime* GetSystemTime()
    {
        if (piSysTime == IMS_NULL)
        {
            piSysTime = PlatformFactory::CreateSystemTime();
        }

        return piSysTime;
    }

private:
    ISystemTime *piSysTime;
};



PRIVATE
SystemTimeService::SystemTimeService()
    : pPrivate(new SystemTimeServicePrivate())
{
}

PRIVATE
SystemTimeService::~SystemTimeService()
{
    delete pPrivate;
}

PUBLIC
ISystemTime* SystemTimeService::GetSystemTime()
{
    return pPrivate->GetSystemTime();
}

PUBLIC GLOBAL
SystemTimeService* SystemTimeService::GetSystemTimeService()
{
    static SystemTimeService *pSystemService = IMS_NULL;

    if (pSystemService == IMS_NULL)
    {
        pSystemService = new SystemTimeService();
    }

    return pSystemService;
}
