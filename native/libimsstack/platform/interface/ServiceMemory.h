/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090831  yhrhee@                   Created
    </table>

    Description

*/

#ifndef _SERVICE_IMS_MEMORY_H_
#define _SERVICE_IMS_MEMORY_H_

#include "IMemHeap.h"
#include "IMSNew.h"

#ifdef IMS_DEBUG_MEM

#define IMS_MEM_Malloc(SIZE) \
        MemService::GetMemService()->GetMemHeap()->AllocDebug(SIZE,__IMS_LINE__,__IMS_FILE__)

#define IMS_MEM_Realloc(MEM, SIZE) \
        MemService::GetMemService()->GetMemHeap()->ReallocDebug(MEM,SIZE,__IMS_LINE__,__IMS_FILE__)

#define IMS_MEM_Free(MEM) \
        MemService::GetMemService()->GetMemHeap()->FreeDebug(MEM,__IMS_LINE__,__IMS_FILE__)

#else // IMS_DEBUG_MEM

#define IMS_MEM_Malloc(SIZE) \
        MemService::GetMemService()->GetMemHeap()->Alloc(SIZE)

#define IMS_MEM_Realloc(MEM, SIZE) \
        MemService::GetMemService()->GetMemHeap()->Realloc(MEM, SIZE)

#define IMS_MEM_Free(MEM) \
        MemService::GetMemService()->GetMemHeap()->Free(MEM)

#endif // IMS_DEBUG_MEM

class MemService
{
private:
    MemService();
    ~MemService();

public:
    IMemHeap* GetMemHeap();

    static MemService* GetMemService();
};

#endif // _SERVICE_IMS_MEMORY_H_
