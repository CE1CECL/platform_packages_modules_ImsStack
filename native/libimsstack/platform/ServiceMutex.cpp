/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090819  YR@                       Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "ImsMutex.h"
#include "PlatformFactory.h"
#include "ServiceMutex.h"

PUBLIC
MutexService::MutexService()
{
}

PUBLIC
MutexService::~MutexService()
{
}

PUBLIC
IMutex* MutexService::CreateMutex(IN const AString &strName /* = AString::ConstNull() */)
{
    ImsMutex *pMutex = PlatformFactory::CreateMutex(strName);

    IMS_ASSERT(pMutex != IMS_NULL);

    return pMutex;
}

PUBLIC
void MutexService::DestroyMutex(IN IMutex *&piMutex)
{
    ImsMutex *pMutex = DYNAMIC_CAST(ImsMutex*, piMutex);

    if (pMutex != IMS_NULL)
    {
        delete pMutex;
        piMutex = IMS_NULL;
    }
}

PUBLIC GLOBAL
MutexService* MutexService::GetMutexService()
{
    static MutexService *pMutexService = IMS_NULL;

    if (pMutexService == IMS_NULL)
    {
        pMutexService = new MutexService();
    }

    return pMutexService;
}
