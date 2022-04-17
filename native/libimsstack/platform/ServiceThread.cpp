/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090819  YR@                       Created
    20131212  yongnam.cha@              Modify ~ThreadService()
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "ServiceMutex.h"
#include "ImsThread.h"
#include "PlatformFactory.h"
#include "ServiceThread.h"

PRIVATE
ThreadService::ThreadService()
{
    piMutex = MutexService::GetMutexService()->CreateMutex();
}

PRIVATE
ThreadService::~ThreadService()
{
    if (piMutex != IMS_NULL)
    {
        MutexService::GetMutexService()->DestroyMutex(piMutex);
    }
}

PUBLIC
IThread* ThreadService::Create(IN const AString &strName, IN IMS_SINT32 nSlotId/* = IMS_SLOT_0*/)
{
    ImsThread *pThread = PlatformFactory::CreateThread();

    IMS_ASSERT(pThread != IMS_NULL);

    if (pThread == IMS_NULL)
    {
        return IMS_NULL;
    }

    // Create a thread and manage it in thread pool.
    LockThreadPool();

    if (!pThread->CreateEx(strName, nSlotId))
    {
        delete pThread;

        UnlockThreadPool();
        return IMS_NULL;
    }

    objThreads.Append(pThread);

    UnlockThreadPool();

    return pThread;
}

PUBLIC
IThread* ThreadService::CreateEx(IN const AString &strName, IN IMS_SINT32 nSlotId/* = IMS_SLOT_0*/)
{
    ImsThread *pThread = PlatformFactory::CreateThreadEx();

    IMS_ASSERT(pThread != IMS_NULL);

    if (pThread == IMS_NULL)
    {
        return IMS_NULL;
    }

    // Create a thread and manage it in thread pool.
    LockThreadPool();

    if (!pThread->CreateEx(strName, nSlotId))
    {
        delete pThread;

        UnlockThreadPool();
        return IMS_NULL;
    }

    objThreads.Append(pThread);

    UnlockThreadPool();

    return pThread;
}

PUBLIC
void ThreadService::Destroy(IN IThread *&piThread)
{
    ImsThread *pThread = DYNAMIC_CAST(ImsThread*, piThread);

    if (pThread == IMS_NULL)
    {
        return;
    }

    IMS_BOOL bThreadFound = IMS_FALSE;

    LockThreadPool();

    for (IMS_UINT32 i = 0; i < objThreads.GetSize(); ++i)
    {
        IThread *piExThread = objThreads.GetAt(i);

        if (piExThread == piThread)
        {
            objThreads.RemoveAt(i);
            bThreadFound = IMS_TRUE;
            break;
        }
    }

    UnlockThreadPool();

    if (bThreadFound)
    {
        delete pThread;
        piThread = IMS_NULL;
    }
}

PUBLIC
IMS_BOOL ThreadService::Contains(IN const IThread *piThread) const
{
    if (piThread == IMS_NULL)
    {
        return IMS_FALSE;
    }

    for (IMS_UINT32 i = 0; i < objThreads.GetSize(); ++i)
    {
        IThread *piExThread = objThreads.GetAt(i);

        IMS_ASSERT(piExThread != IMS_NULL);

        if (piExThread != IMS_NULL)
        {
            if (piExThread->Equals(piThread))
            {
                return IMS_TRUE;
            }
        }
    }

    return IMS_FALSE;
}

PUBLIC
IMS_BOOL ThreadService::ContainsLocked(IN const IThread *piThread) const
{
    if (piThread == IMS_NULL)
    {
        return IMS_FALSE;
    }

    LockThreadPool();

    IMS_BOOL bResult = Contains(piThread);

    UnlockThreadPool();

    return bResult;
}

PUBLIC
IThread* ThreadService::GetCurrentThread() const
{
    IMS_ULONG nCurrentThreadId = 0;

    LockThreadPool();

    // According to the platform specific API, we will find the current thread ...
    nCurrentThreadId = PlatformFactory::GetCurrentThreadId();

    if (nCurrentThreadId == 0)
    {
        UnlockThreadPool();
        return IMS_NULL;
    }

    for (IMS_UINT32 i = 0; i < objThreads.GetSize(); ++i)
    {
        ImsThread *pThread = DYNAMIC_CAST(ImsThread*, objThreads.GetAt(i));

        if (pThread != IMS_NULL)
        {
            if (nCurrentThreadId == pThread->GetThreadId())
            {
                UnlockThreadPool();
                return pThread;
            }
        }
    }

    UnlockThreadPool();

    return IMS_NULL;
}

PUBLIC
IThread* ThreadService::GetThread(IN const AString &strName) const
{
    for (IMS_UINT32 i = 0; i < objThreads.GetSize(); ++i)
    {
        IThread *piExThread = objThreads.GetAt(i);

        IMS_ASSERT(piExThread != IMS_NULL);

        if (piExThread != IMS_NULL)
        {
            if (piExThread->GetName().Equals(strName))
            {
                return piExThread;
            }
        }
    }

    return IMS_NULL;
}

PUBLIC
IThread* ThreadService::GetThreadLocked(IN const AString &strName) const
{
    LockThreadPool();

    IThread *piThread = GetThread(strName);

    UnlockThreadPool();

    return piThread;
}

PUBLIC GLOBAL
ThreadService* ThreadService::GetThreadService()
{
    static ThreadService *pThreadService = IMS_NULL;

    if (pThreadService == IMS_NULL)
    {
        pThreadService = new ThreadService();
    }

    return pThreadService;
}

PUBLIC GLOBAL
IMS_SINT32 ThreadService::GetCurrentSlotId(IN IMS_SINT32 nDefaultSlotId/* = IMS_SLOT_ANY*/)
{
    IThread* piThread = GetThreadService()->GetCurrentThread();
    return (piThread != IMS_NULL) ? piThread->GetSlotId() : nDefaultSlotId;
}

PRIVATE
void ThreadService::LockThreadPool() const
{
    piMutex->Lock();
}

PRIVATE
void ThreadService::UnlockThreadPool() const
{
    piMutex->Unlock();
}
