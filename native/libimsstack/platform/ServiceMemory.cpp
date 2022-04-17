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
#include "PlatformFactory.h"

PRIVATE
MemService::MemService()
{
}

PRIVATE
MemService::~MemService()
{
}

PUBLIC
IMemHeap* MemService::GetMemHeap()
{
    IMemHeap *piHeap = PlatformFactory::GetHeap();

    IMS_ASSERT(piHeap != IMS_NULL);

    return piHeap;
}

PUBLIC GLOBAL
MemService* MemService::GetMemService()
{
    static MemService *pMemService = IMS_NULL;

    if (pMemService == IMS_NULL)
    {
        IMemHeap *piHeap = PlatformFactory::GetHeap();
        void *pvMemService = piHeap->Alloc(sizeof(MemService));

        pMemService = new(pvMemService) MemService();
    }

    return pMemService;
}


#ifdef __cplusplus
extern "C"
{
#endif

GLOBAL
void* IMS_MEM_Realloc_C(IN void *pMem, IN IMS_SIZE_T nSize)
{
    return MemService::GetMemService()->GetMemHeap()->Realloc(pMem, nSize);
}

GLOBAL
void* IMS_MEM_Alloc_C(IN IMS_SIZE_T nSize)
{
    return MemService::GetMemService()->GetMemHeap()->Alloc(nSize);
}

GLOBAL
void IMS_MEM_Free_C(IN void *pMem)
{
    return MemService::GetMemService()->GetMemHeap()->Free(pMem);
}

#ifdef __cplusplus
}
#endif
