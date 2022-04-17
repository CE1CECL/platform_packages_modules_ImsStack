/*
 * Copyright (C) 2022 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <pthread.h>
#include <unistd.h>

#include "IMSMsgDef.h"
#include "OsMutex.h"
#include "OsTimer.h"
#include "OsTimerService.h"
#include "ServiceMSG.h"
#include "ServiceMemory.h"
#include "ServiceThread.h"
#include "system-intf/System.h"
#include "system-intf/SystemConstants.h"

__IMS_TRACE_TAG_ADAPT__;



#define TIMER_DURATION                 100  // ms
// 10s
#define ALARM_TIMER_MIN                10000

// All timers will be installed as an alarm timer
#define __IMS_TIMER_ALARM_ONLY__



LOCAL
void androidTimerService_AddListener(IN ISystemListener* piListener)
{
    System::GetInstance()->AddListener(
            SystemConstants::CATEGORY_ALARM, piListener, IMS_SLOT_0);
}

LOCAL
void androidTimerService_RemoveListener(IN ISystemListener* piListener)
{
    System::GetInstance()->RemoveListener(
            SystemConstants::CATEGORY_ALARM, piListener, IMS_SLOT_0);
}

LOCAL
IMS_BOOL androidTimerService_KillAlarm(IN IMS_UINTP nTimerId)
{
    IMS_SINT32 nResult = System::GetInstance()->KillAlarm(nTimerId);

    return (nResult == 0) ? IMS_FALSE : IMS_TRUE;
}

LOCAL
IMS_BOOL androidTimerService_SetAlarm(IN IMS_UINTP nTimerId, IN IMS_UINT32 nDuration)
{
    IMS_SINT32 nResult = System::GetInstance()->SetAlarm(nDuration, nTimerId);

    return (nResult == 0) ? IMS_FALSE : IMS_TRUE;
}



class OsTimerObject
{
public:
    explicit OsTimerObject(IN IMS_UINT32 nDuration, IN IMS_BOOL bRepeat = IMS_FALSE);
    virtual ~OsTimerObject();

public:
    inline virtual IMS_UINTP GetTimerId() const
    { return 0; }

    void DecreaseTick();
    void InitTickForRepeat();
    IMS_BOOL IsTimerExpired() const;

public:
    IMS_UINT32 m_nDuration;
    IMS_UINT32 m_nTotalTick;
    IMS_UINT32 m_nRemainTick;
    IMS_BOOL m_bRepeat;
    IMS_BOOL m_bWakeupTimer;
    IMS_BOOL m_bReadyDeleted;

    //Only when repeat flag is not set.
    IMS_BOOL m_bAlreadyExpired;
};



class OsTimerWrapper
    : public OsTimerObject
{
public:
    OsTimerWrapper(IN IMS_UINT32 nDuration, IN OsTimer* pTimer,
            IN IMS_BOOL bRepeat = IMS_FALSE);
    virtual ~OsTimerWrapper();

public:
    inline IMS_UINTP GetTimerId() const override
    { return (m_pTimer != IMS_NULL) ? m_pTimer->GetTimerId() : 0; }

public:
    OsTimer* m_pTimer;
};



class OsCoreTimerWrapper
    : public OsTimerObject
{
public:
    OsCoreTimerWrapper(IN IMS_UINT32 nDuration, IN OsCoreTimer* pTimer,
            IN IMS_BOOL bRepeat = IMS_FALSE);
    virtual ~OsCoreTimerWrapper();

public:
    inline IMS_UINTP GetTimerId() const override
    { return (m_pCoreTimer != IMS_NULL) ? m_pCoreTimer->GetTimerId() : 0; }

public:
    OsCoreTimer* m_pCoreTimer;
};



PUBLIC
OsTimerObject::OsTimerObject(IN IMS_UINT32 nDuration,
        IN IMS_BOOL bRepeat /*= IMS_FALSE*/)
    : m_nDuration(nDuration)
    , m_nTotalTick(static_cast<IMS_UINT32>(nDuration / TIMER_DURATION))
    , m_nRemainTick(static_cast<IMS_UINT32>(nDuration / TIMER_DURATION))
    , m_bRepeat(bRepeat)
    , m_bWakeupTimer(IMS_FALSE)
    , m_bReadyDeleted(IMS_FALSE)
    , m_bAlreadyExpired(IMS_FALSE)
{
}

PUBLIC VIRTUAL
OsTimerObject::~OsTimerObject()
{
}

PUBLIC
void OsTimerObject::DecreaseTick()
{
    if (m_nRemainTick > 0)
    {
        m_nRemainTick--;
    }
}

PUBLIC
void OsTimerObject::InitTickForRepeat()
{
    if (m_nRemainTick <= 0)
    {
        if (m_bRepeat)
        {
            m_nRemainTick = m_nTotalTick;
        }
        else
        {
            m_nRemainTick = 0;
        }
    }
}

PUBLIC
IMS_BOOL OsTimerObject::IsTimerExpired() const
{
    return (m_nRemainTick > 0) ? IMS_FALSE : IMS_TRUE;
}



PUBLIC
OsTimerWrapper::OsTimerWrapper(IN IMS_UINT32 nDuration,
        IN OsTimer* pTimer, IN IMS_BOOL bRepeat /*= IMS_FALSE*/)
    : OsTimerObject(nDuration, bRepeat)
    , m_pTimer(pTimer)
{
}

PUBLIC VIRTUAL
OsTimerWrapper::~OsTimerWrapper()
{
    m_pTimer = IMS_NULL;
}



PUBLIC
OsCoreTimerWrapper::OsCoreTimerWrapper(IN IMS_UINT32 nDuration,
        IN OsCoreTimer* pTimer, IN IMS_BOOL bRepeat /*= IMS_FALSE*/)
    : OsTimerObject(nDuration, bRepeat)
    , m_pCoreTimer(pTimer)
{
}

PUBLIC VIRTUAL
OsCoreTimerWrapper::~OsCoreTimerWrapper()
{
    m_pCoreTimer = IMS_NULL;
}



class OsTimerThread
    : public IThreadImpListener
    , public ISystemListener
{
public:
    OsTimerThread();
    virtual ~OsTimerThread();

    OsTimerThread(IN const OsTimerThread&) = delete;
    OsTimerThread& operator=(IN const OsTimerThread&) = delete;

public:
    // IThreadImpListener class
    void RunImp() override;

    IMS_BOOL SetTimer(IN IMS_UINT32 nDuration, IN OsTimer* pTimer,
            IN IMS_BOOL bRepeat = IMS_FALSE);
    IMS_BOOL KillTimer(IN OsTimer* pTimer);

    IMS_BOOL SetCoreTimer(IN IMS_UINT32 nDuration, IN OsCoreTimer* pTimer,
            IN IMS_BOOL bRepeat = IMS_FALSE);
    IMS_BOOL KillCoreTimer(IN OsCoreTimer* pTimer);

    void TerminateSystemTimer();

private:
    void CheckExpiredTimer();
    void CheckExpiredCoreTimer();

    void DeleteExpiredTimer();
    void DeleteExpiredCoreTimer();

    void TimerCallBackOnExpired(IN OsTimerWrapper* pTimerWrapper);
    void CoreTimerCallBackOnExpired(IN OsCoreTimerWrapper* pTimerWrapper);

    IMS_BOOL InsertAlarmTimer(IN IMS_UINT32 nDuration, IN OsTimer* pTimer,
            IN IMS_BOOL bRepeat = IMS_FALSE);
    IMS_BOOL InsertCoreTimer(IN IMS_UINT32 nDuration, IN OsCoreTimer* pTimer,
            IN IMS_BOOL bRepeat = IMS_FALSE);
    IMS_BOOL InsertTimer(IN IMS_UINT32 nDuration, IN OsTimer* pTimer,
            IN IMS_BOOL bRepeat = IMS_FALSE);

    IMS_BOOL IsNoActiveTimerPresent();
    IMS_SINT32 GetState();
    void SetState(IN IMS_SINT32 nState);
    void StartSystemTimer();

    // ISystemListener class
    void System_NotifyEvent(IN IMS_UINT32 nEvent,
            IN IMS_UINTP nWParam, IN IMS_UINTP nLParam) override;

    static const IMS_CHAR* StateToString(IN IMS_SINT32 nState);

private:
    /// State of the system timer
    enum
    {
        STATE_INVALID = (-1),
        STATE_IDLE = 0,
        STATE_PENDING,
        STATE_ACTIVE,
        STATE_TERMINATED
    };

    OsMutex m_objMutexForCoreTimer;
    OsMutex m_objMutexForTimer;

#if !defined(__IMS_TIMER_ALARM_ONLY__)
    IMS_SINT32 nState;

    // For timer worker thread
    OsMutex objMutex_SystemTimer;
    pthread_cond_t stCond_SystemTimer;
    OsMutex objMutex_State;
    OsPthread* pThreadImp;
#endif

    IMSList<OsTimerObject*> m_objTimers;
    IMSList<OsTimerObject*> m_objAlarmTimers;
    IMSList<OsTimerObject*> m_objCoreTimers;
};



PUBLIC
OsTimerThread::OsTimerThread()
#if !defined(__IMS_TIMER_ALARM_ONLY__)
    : nState(STATE_IDLE)
    , pThreadImp(IMS_NULL)
#endif
{
    // 11.10.18 all timers use alarm timer because of lock screen issue
#if !defined(__IMS_TIMER_ALARM_ONLY__)
    pthread_cond_init(&stCond_SystemTimer, IMS_NULL);

    pThreadImp = new OsPthread();

    if (pThreadImp != IMS_NULL)
    {
        pThreadImp->Create("OsTimerService");
        pThreadImp->SetImpListener(this);
        pThreadImp->Activate();
    }
#endif

    androidTimerService_AddListener(this);
}

PUBLIC VIRTUAL
OsTimerThread::~OsTimerThread()
{
#if !defined(__IMS_TIMER_ALARM_ONLY__)
    if (pThreadImp != IMS_NULL)
    {
        pThreadImp->Deactivate();
        delete pThreadImp;
        pThreadImp = IMS_NULL;
    }

    pthread_cond_destroy(&stCond_SystemTimer);
#endif

    while (!m_objAlarmTimers.IsEmpty())
    {
        OsTimerObject* pTimerObject = m_objAlarmTimers.GetAt(0);

        if (pTimerObject != IMS_NULL)
        {
            androidTimerService_KillAlarm(pTimerObject->GetTimerId());
            delete pTimerObject;
        }

        m_objAlarmTimers.RemoveAt(0);
    }

    while (!m_objTimers.IsEmpty())
    {
        OsTimerObject* pTimerObject = m_objTimers.GetAt(0);

        if (pTimerObject != IMS_NULL)
        {
            delete pTimerObject;
        }

        m_objTimers.RemoveAt(0);
    }

    while (!m_objCoreTimers.IsEmpty())
    {
        OsTimerObject* pTimerObject = m_objCoreTimers.GetAt(0);

        if (pTimerObject != IMS_NULL)
        {
            delete pTimerObject;
        }

        m_objCoreTimers.RemoveAt(0);
    }

    androidTimerService_RemoveListener(this);
}

PUBLIC VIRTUAL
void OsTimerThread::RunImp()
{
#if !defined(__IMS_TIMER_ALARM_ONLY__)

    IMS_SINT32 nWaitResult = (-1);

    while (1)
    {
        if (IsNoActiveTimerPresent())
        {
            SetState(STATE_IDLE);

            nWaitResult = pthread_cond_wait(&stCond_SystemTimer,
                    reinterpret_cast<pthread_mutex_t*>(objMutex_SystemTimer.GetMutexObj()));

            IMS_SINT32 nCurrentState = GetState();

            if (nCurrentState == STATE_TERMINATED)
            {
                break;
            }
            else if (nCurrentState == STATE_PENDING)
            {
                SetState(STATE_ACTIVE);
            }

            continue;
        }
        else
        {
            nWaitResult = pthread_cond_timeout_np(&stCond_SystemTimer,
                    reinterpret_cast<pthread_mutex_t*>(objMutex_SystemTimer.GetMutexObj()),
                    TIMER_DURATION);

            if (nWaitResult == 0)
            {
                if (GetState() == STATE_TERMINATED)
                {
                    break;
                }
            }
        }

        CheckExpiredTimer();
        CheckExpiredCoreTimer();
        DeleteExpiredTimer();
        DeleteExpiredCoreTimer();
    }
#endif
}

PUBLIC
IMS_BOOL OsTimerThread::SetTimer(IN IMS_UINT32 nDuration, IN OsTimer* pTimer,
        IN IMS_BOOL bRepeat /*= IMS_FALSE*/)
{
    if (pTimer == IMS_NULL)
    {
        IMS_TRACE_E(0, "Timer is null", 0, 0, 0);
        return IMS_FALSE;
    }

    IMS_BOOL bResult = IMS_FALSE;

#if !defined(__IMS_TIMER_ALARM_ONLY__)
    if (nDuration >= ALARM_TIMER_MIN)
    {
        bResult = InsertAlarmTimer(nDuration, pTimer, bRepeat);
    }
    else
    {
        IMS_BOOL bIsFirstTimer = IsNoActiveTimerPresent();

        bResult = InsertTimer(nDuration, pTimer, bRepeat);

        if (bIsFirstTimer && bResult)
        {
            StartSystemTimer();
        }
    }
#else
    // 11.10.18 all timers use alarm timer because of lock screen issue
    bResult = InsertAlarmTimer(nDuration, pTimer, bRepeat);
#endif

    if (!bResult)
    {
        IMS_TRACE_E(0, "Inserting a timer failed", 0, 0, 0);
    }

    return bResult;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL OsTimerThread::KillTimer(IN OsTimer* pTimer)
{
    IMS_BOOL bKilled = IMS_FALSE;
    IMS_UINTP nAlarmTimerId = 0;
    IMS_BOOL bAlarmExpired = IMS_FALSE;

    if (pTimer == IMS_NULL)
    {
        IMS_TRACE_E(0, "Timer is null", 0, 0, 0);
        return IMS_FALSE;
    }

    m_objMutexForTimer.Lock();

#if !defined(__IMS_TIMER_ALARM_ONLY__)
    for (IMS_UINT32 i = 0; i < objTimers.GetSize(); i++)
    {
        OsTimerObject *pTimerObject = objTimers.GetAt(i);

        if ((pTimerObject != IMS_NULL) && !pTimerObject->bReadyDeleted)
        {
            if (pTimerObject->GetTimerId() == pTimer->GetTimerId())
            {
                pTimerObject->bReadyDeleted = IMS_TRUE;
                bKilled = IMS_TRUE;
                break;
            }
        }
    }
#endif

    if (!bKilled)
    {
        for (IMS_UINT32 i = 0; i < m_objAlarmTimers.GetSize(); i++)
        {
            OsTimerObject* pTimerObject = m_objAlarmTimers.GetAt(i);

            if (pTimerObject != IMS_NULL)
            {
                if (pTimerObject->GetTimerId() == pTimer->GetTimerId())
                {
                    nAlarmTimerId = pTimerObject->GetTimerId();
                    bAlarmExpired = pTimerObject->m_bReadyDeleted;
                    m_objAlarmTimers.RemoveAt(i);
                    delete pTimerObject;
                    break;
                }
            }
        }
    }

    m_objMutexForTimer.Unlock();

    if ((nAlarmTimerId != 0) && !bAlarmExpired)
    {
        androidTimerService_KillAlarm(nAlarmTimerId);
    }

    if (m_objAlarmTimers.IsEmpty())
    {
        IMS_TRACE_D("No active timers", 0, 0, 0);
    }

    return IMS_TRUE;
}

PUBLIC
IMS_BOOL OsTimerThread::SetCoreTimer(IN IMS_UINT32 nDuration, IN OsCoreTimer* pTimer,
        IN IMS_BOOL bRepeat /*= IMS_FALSE*/)
{
    if (pTimer == IMS_NULL)
    {
        IMS_TRACE_E(0, "Timer is null", 0, 0, 0);
        return IMS_FALSE;
    }

    IMS_BOOL bIsFirstTimer = IsNoActiveTimerPresent();
    IMS_BOOL bResult = InsertCoreTimer(nDuration, pTimer, bRepeat);

    if (bIsFirstTimer && bResult)
    {
        StartSystemTimer();
    }

    if (!bResult)
    {
        IMS_TRACE_E(0, "Inserting a timer failed", 0, 0, 0);
    }

    return bResult;
}

PUBLIC
IMS_BOOL OsTimerThread::KillCoreTimer(IN OsCoreTimer* pTimer)
{
    if (pTimer == IMS_NULL)
    {
        IMS_TRACE_E(0, "CoreTimer is null", 0, 0, 0);
        return IMS_FALSE;
    }

    m_objMutexForCoreTimer.Lock();

    for (IMS_UINT32 i = 0; i < m_objCoreTimers.GetSize(); i++)
    {
        OsTimerObject* pTimerObject = m_objCoreTimers.GetAt(i);

        if (pTimerObject != IMS_NULL)
        {
            if (pTimerObject->GetTimerId() == pTimer->GetTimerId())
            {
                pTimerObject->m_bReadyDeleted = IMS_TRUE;
                break;
            }
        }
    }

    m_objMutexForCoreTimer.Unlock();

    return IMS_TRUE;
}

PUBLIC
void OsTimerThread::TerminateSystemTimer()
{
#if !defined(__IMS_TIMER_ALARM_ONLY__)
    IMS_SINT32 nCurrentState = GetState();

    switch (nCurrentState)
    {
    case STATE_IDLE:
        IMS_TRACE_D("System timer is in IDLE", 0, 0, 0);

        SetState(STATE_TERMINATED);

        objMutex_SystemTimer.Lock();
        pthread_cond_signal(&stCond_SystemTimer);
        objMutex_SystemTimer.Unlock();
        break;

    case STATE_PENDING:
        IMS_TRACE_D("System timer is in PENDING", 0, 0, 0);

        SetState(STATE_TERMINATED);
        break;

    case STATE_ACTIVE:
        IMS_TRACE_D("System timer is in ACTIVE", 0, 0, 0);

        SetState(STATE_TERMINATED);

        objMutex_SystemTimer.Lock();
        pthread_cond_signal(&stCond_SystemTimer);
        objMutex_SystemTimer.Unlock();
        break;

    case STATE_TERMINATED:
        IMS_TRACE_D("System timer is in TERMINATED", 0, 0, 0);
        break;

    default:
        break;
    }
#endif
}

PRIVATE
void OsTimerThread::CheckExpiredTimer()
{
    m_objMutexForTimer.Lock();

    for (IMS_UINT32 i = 0; i < m_objTimers.GetSize(); i++)
    {
        OsTimerObject* pTimerObject = m_objTimers.GetAt(i);

        if ((pTimerObject != IMS_NULL) && !pTimerObject->m_bWakeupTimer)
        {
            if (pTimerObject->m_bReadyDeleted)
            {
                IMS_TRACE_D("Timer (%" PFLS_u ") is already stopped",
                        pTimerObject->GetTimerId(), 0, 0);
                continue;
            }

            pTimerObject->DecreaseTick();

            if (!pTimerObject->m_bAlreadyExpired && pTimerObject->IsTimerExpired())
            {
                // Even though the repeat is not set to TRUE
                // and the application didn't invoke KillTimer(...)
                // after the timer is expired, the callback will not be invoked at this moment.
                if (!pTimerObject->m_bRepeat)
                {
                    pTimerObject->m_bAlreadyExpired = IMS_TRUE;
                }

                TimerCallBackOnExpired(DYNAMIC_CAST(OsTimerWrapper*, pTimerObject));
            }

            pTimerObject->InitTickForRepeat();
        }
    }

    m_objMutexForTimer.Unlock();
}

PRIVATE
void OsTimerThread::CheckExpiredCoreTimer()
{
    m_objMutexForCoreTimer.Lock();

    for (IMS_UINT32 i = 0; i < m_objCoreTimers.GetSize(); i++)
    {
        OsTimerObject* pTimerObject = m_objCoreTimers.GetAt(i);

        if ((pTimerObject != IMS_NULL) && !pTimerObject->m_bWakeupTimer)
        {
            if (pTimerObject->m_bReadyDeleted)
            {
                IMS_TRACE_D("CoreTimer (%" PFLS_u ") is already stopped",
                        pTimerObject->GetTimerId(), 0, 0);
                continue;
            }

            pTimerObject->DecreaseTick();

            if (!pTimerObject->m_bAlreadyExpired && pTimerObject->IsTimerExpired())
            {
                // Even though the repeat is not set to TRUE
                // and the application didn't invoke KillTimer(...)
                // after the timer is expired, the callback will not be invoked at this moment.
                if (!pTimerObject->m_bRepeat)
                {
                    pTimerObject->m_bAlreadyExpired = IMS_TRUE;
                }

                CoreTimerCallBackOnExpired(DYNAMIC_CAST(OsCoreTimerWrapper*, pTimerObject));
            }

            pTimerObject->InitTickForRepeat();
        }
    }

    m_objMutexForCoreTimer.Unlock();
}

PUBLIC
void OsTimerThread::DeleteExpiredTimer()
{
    m_objMutexForTimer.Lock();

    for (IMS_UINT32 i = 0; i < m_objTimers.GetSize(); i++)
    {
        OsTimerObject* pTimerObject = m_objTimers.GetAt(i);

        IMS_ASSERT(pTimerObject != IMS_NULL);

        if ((pTimerObject != IMS_NULL)
                && !pTimerObject->m_bWakeupTimer
                && pTimerObject->m_bReadyDeleted)
        {
            m_objTimers.RemoveAt(i);

            delete pTimerObject;
            i--;
        }
    }

    m_objMutexForTimer.Unlock();
}

PUBLIC
void OsTimerThread::DeleteExpiredCoreTimer()
{
    m_objMutexForCoreTimer.Lock();

    for (IMS_UINT32 i = 0; i < m_objCoreTimers.GetSize(); i++)
    {
        OsTimerObject* pTimerObject = m_objCoreTimers.GetAt(i);

        IMS_ASSERT(pTimerObject != IMS_NULL);

        if ((pTimerObject != IMS_NULL)
                && !pTimerObject->m_bWakeupTimer
                && pTimerObject->m_bReadyDeleted)
        {
            m_objCoreTimers.RemoveAt(i);

            delete pTimerObject;
            i--;
        }
    }

    m_objMutexForCoreTimer.Unlock();
}

PRIVATE
void OsTimerThread::TimerCallBackOnExpired(IN OsTimerWrapper* pTimerWrapper)
{
    if (pTimerWrapper == IMS_NULL)
    {
        IMS_TRACE_E(0, "OsTimerWrapper is null", 0, 0, 0);
        return;
    }

    if (pTimerWrapper->m_pTimer == IMS_NULL)
    {
        IMS_TRACE_E(0, "OsTimer is null", 0, 0, 0);
        return;
    }

    IThread* piOwnerThread = pTimerWrapper->m_pTimer->GetOwner();

    if (piOwnerThread == IMS_NULL)
    {
        IMS_TRACE_D("Timer (%" PFLS_u ") is already stopped", pTimerWrapper->GetTimerId(), 0, 0);
        return;
    }

    IMSMSG objMsg(IMS_MSG_TIMER,
            pTimerWrapper->m_pTimer->GetTimerId(),
            pTimerWrapper->m_pTimer->GetInternalTimerId());

    IMS_TRACE_D("Timer (%" PFLS_u ", %d) is expired; size=%d",
            pTimerWrapper->m_pTimer->GetTimerId(),
            pTimerWrapper->m_pTimer->GetInternalTimerId(),
            m_objAlarmTimers.GetSize());

    MSGService::PostMessageThread(piOwnerThread, objMsg);
}

PRIVATE
void OsTimerThread::CoreTimerCallBackOnExpired(IN OsCoreTimerWrapper* pTimerWrapper)
{
    if (pTimerWrapper == IMS_NULL)
    {
        IMS_TRACE_E(0, "OsCoreTimerWrapper is null", 0, 0, 0);
        return;
    }

    IMS_TRACE_D("CoreTimer (%" PFLS_u ") is expired", pTimerWrapper->GetTimerId(), 0, 0);

    if (pTimerWrapper->m_pCoreTimer == IMS_NULL)
    {
        IMS_TRACE_E(0, "OsCoreTimer is null", 0, 0, 0);
        return;
    }

    ICoreTimerListener* piListener = pTimerWrapper->m_pCoreTimer->GetListener();

    if (piListener == IMS_NULL)
    {
        IMS_TRACE_E(0, "Listener is null", 0, 0, 0);
        return;
    }

    if (!pTimerWrapper->m_bReadyDeleted)
    {
        piListener->CoreTimer_TimerExpired(pTimerWrapper->m_pCoreTimer);
    }
}

PRIVATE
IMS_BOOL OsTimerThread::InsertAlarmTimer(IN IMS_UINT32 nDuration, IN OsTimer* pTimer,
        IN IMS_BOOL bRepeat /*= IMS_FALSE*/)
{
    OsTimerWrapper* pTimerWrapper = new OsTimerWrapper(nDuration, pTimer, bRepeat);

    if (pTimerWrapper == IMS_NULL)
    {
        return IMS_FALSE;
    }

    m_objMutexForTimer.Lock();

    androidTimerService_SetAlarm(pTimerWrapper->GetTimerId(), nDuration);

    IMS_BOOL bResult = m_objAlarmTimers.Append(pTimerWrapper);

    m_objMutexForTimer.Unlock();

    if (!bResult)
    {
        androidTimerService_KillAlarm(pTimerWrapper->GetTimerId());

        delete pTimerWrapper;
    }

    return bResult;
}

PRIVATE
IMS_BOOL OsTimerThread::InsertCoreTimer(IN IMS_UINT32 nDuration, IN OsCoreTimer* pTimer,
        IN IMS_BOOL bRepeat /*= IMS_FALSE*/)
{
    OsCoreTimerWrapper* pTimerWrapper = new OsCoreTimerWrapper(nDuration, pTimer, bRepeat);

    if (pTimerWrapper == IMS_NULL)
    {
        return IMS_FALSE;
    }

    m_objMutexForCoreTimer.Lock();

    IMS_BOOL bResult = m_objCoreTimers.Append(pTimerWrapper);

    m_objMutexForCoreTimer.Unlock();

    if (!bResult)
    {
        delete pTimerWrapper;
    }

    return bResult;
}

PRIVATE
IMS_BOOL OsTimerThread::InsertTimer(IN IMS_UINT32 nDuration, IN OsTimer* pTimer,
        IN IMS_BOOL bRepeat /*= IMS_FALSE*/)
{
    OsTimerWrapper* pTimerWrapper = new OsTimerWrapper(nDuration, pTimer, bRepeat);

    if (pTimerWrapper == IMS_NULL)
    {
        return IMS_FALSE;
    }

    m_objMutexForTimer.Lock();

    IMS_BOOL bResult = m_objTimers.Append(pTimerWrapper);

    m_objMutexForTimer.Unlock();

    if (!bResult)
    {
        delete pTimerWrapper;
    }

    return bResult;
}

PRIVATE
IMS_BOOL OsTimerThread::IsNoActiveTimerPresent()
{
    IMS_BOOL bIsTimerEmpty = IMS_FALSE;
    IMS_BOOL bIsCoreTimerEmpty = IMS_FALSE;

    m_objMutexForTimer.Lock();
    bIsTimerEmpty = m_objTimers.IsEmpty();
    m_objMutexForTimer.Unlock();

    m_objMutexForCoreTimer.Lock();
    bIsCoreTimerEmpty = m_objCoreTimers.IsEmpty();
    m_objMutexForCoreTimer.Unlock();

    return (bIsTimerEmpty && bIsCoreTimerEmpty);
}

PRIVATE
IMS_SINT32 OsTimerThread::GetState()
{
#if !defined(__IMS_TIMER_ALARM_ONLY__)
    objMutex_State.Lock();
    IMS_SINT32 nTmpState = nState;
    objMutex_State.Unlock();

    return nTmpState;
#else
    return STATE_INVALID;
#endif
}

PRIVATE
void OsTimerThread::SetState(IN IMS_SINT32 nState)
{
#if !defined(__IMS_TIMER_ALARM_ONLY__)
    objMutex_State.Lock();

    if (this->nState != nState)
    {
        IMS_TRACE_D("System timer :: %s >> %s",
                StateToString(this->nState), StateToString(nState), 0);

        this->nState = nState;
    }

    objMutex_State.Unlock();
#else
    (void) nState;
#endif
}

PRIVATE
void OsTimerThread::StartSystemTimer()
{
#if !defined(__IMS_TIMER_ALARM_ONLY__)
    IMS_SINT32 nCurrentState = GetState();

    switch (nCurrentState)
    {
    case STATE_IDLE:
        IMS_TRACE_D("System timer is in IDLE", 0, 0, 0);

        SetState(STATE_PENDING);

        objMutex_SystemTimer.Lock();
        pthread_cond_signal(&stCond_SystemTimer);
        objMutex_SystemTimer.Unlock();
        break;

    case STATE_PENDING:
        IMS_TRACE_D("System timer is in PENDING", 0, 0, 0);
        break;

    case STATE_ACTIVE:
        IMS_TRACE_D("System timer is in ACTIVE", 0, 0, 0);
        break;

    case STATE_TERMINATED:
        IMS_TRACE_D("System timer is in TERMINATED, " \
                "don't set any timer from this moment ...", 0, 0, 0);
        break;

    default:
        break;
    }
#endif
}

PRIVATE VIRTUAL
void OsTimerThread::System_NotifyEvent(IN IMS_UINT32 nEvent,
        IN IMS_UINTP nWParam, IN IMS_UINTP nLParam)
{
    (void)nLParam;

    if (nEvent != IMS_SYSTEM_TIMER_EXPIRED)
    {
        IMS_TRACE_D("Event(%d) will be ignored", nEvent, 0, 0);
        return;
    }

    IMS_TRACE_D("TimerExpired (timerId=%" PFLS_u ")", nWParam, 0, 0);

    m_objMutexForTimer.Lock();

    for (IMS_UINT32 i = 0; i < m_objAlarmTimers.GetSize(); i++)
    {
        OsTimerObject* pTimerObject = m_objAlarmTimers.GetAt(i);

        if ((pTimerObject != IMS_NULL) && (pTimerObject->GetTimerId() == nWParam))
        {
            // Alarm Timer Case - Don't Call StopAlarmTimer
            pTimerObject->m_bReadyDeleted = IMS_TRUE;
            TimerCallBackOnExpired(DYNAMIC_CAST(OsTimerWrapper*, pTimerObject));
            break;
        }
    }

    m_objMutexForTimer.Unlock();
}

PRIVATE GLOBAL
const IMS_CHAR* OsTimerThread::StateToString(IN IMS_SINT32 nState)
{
    switch (nState)
    {
        case STATE_IDLE:
            return "STATE_IDLE";
        case STATE_PENDING:
            return "STATE_PENDING";
        case STATE_ACTIVE:
            return "STATE_ACTIVE";
        case STATE_TERMINATED:
            return "STATE_TERMINATED";
        default:
            return "__INVALID__";
    }
}



PUBLIC GLOBAL
OsTimerService* OsTimerService::s_pTimerService = IMS_NULL;

PUBLIC
OsTimerService::OsTimerService()
    : m_pWorkerThread(new OsTimerThread())
{
}

PUBLIC VIRTUAL
OsTimerService::~OsTimerService()
{
    if (m_pWorkerThread != IMS_NULL)
    {
        delete m_pWorkerThread;
        m_pWorkerThread = IMS_NULL;
    }

    IMS_TRACE_D("Destructor :: TimerService", 0, 0, 0);
}

PUBLIC
void OsTimerService::KillCoreTimer(IN OsCoreTimer* pTimer)
{
    if (m_pWorkerThread != IMS_NULL)
    {
        m_pWorkerThread->KillCoreTimer(pTimer);
    }
}

PUBLIC
void OsTimerService::KillTimer(IN OsTimer* pTimer)
{
    if (m_pWorkerThread != IMS_NULL)
    {
        m_pWorkerThread->KillTimer(pTimer);
    }
}

PUBLIC
IMS_BOOL OsTimerService::SetCoreTimer(IN IMS_UINT32 nDuration,
        IN OsCoreTimer* pTimer, IN IMS_BOOL bRepeat /*= IMS_FALSE*/)
{
    if (m_pWorkerThread == IMS_NULL)
    {
        return IMS_FALSE;
    }

    return m_pWorkerThread->SetCoreTimer(nDuration, pTimer, bRepeat);
}

PUBLIC
IMS_BOOL OsTimerService::SetTimer(IN IMS_UINT32 nDuration,
        IN OsTimer* pTimer, IN IMS_BOOL bRepeat /*= IMS_FALSE*/)
{
    if (m_pWorkerThread == IMS_NULL)
    {
        return IMS_FALSE;
    }

    return m_pWorkerThread->SetTimer(nDuration, pTimer, bRepeat);
}

PUBLIC GLOBAL
OsTimerService* OsTimerService::GetTimerService()
{
    if (s_pTimerService == IMS_NULL)
    {
        s_pTimerService = new OsTimerService();
    }

    return s_pTimerService;
}

PUBLIC GLOBAL
void OsTimerService::CleanUp()
{
    if (s_pTimerService != IMS_NULL)
    {
        if (s_pTimerService->m_pWorkerThread != IMS_NULL)
        {
            s_pTimerService->m_pWorkerThread->TerminateSystemTimer();
        }

        delete s_pTimerService;
        s_pTimerService = IMS_NULL;
    }
}

PUBLIC GLOBAL
void OsTimerService::StartUp()
{
    OsTimerService::GetTimerService();
}
