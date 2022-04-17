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
#ifndef OS_TIMER_SERVICE_H_
#define OS_TIMER_SERVICE_H_

#include "IMSList.h"
#include "OsMutex.h"
#include "OsPthread.h"

class OsCoreTimer;
class OsTimer;
class OsTimerThread;

class OsTimerService
{
public :
    OsTimerService();
    virtual ~OsTimerService();

    OsTimerService(IN const OsTimerService&) = delete;
    OsTimerService& operator=(IN const OsTimerService&) = delete;

public:
    void KillCoreTimer(IN OsCoreTimer* pTimer);
    void KillTimer(IN OsTimer* pTimer);

    IMS_BOOL SetCoreTimer(IN IMS_UINT32 nDuration,
            IN OsCoreTimer* pCoreTimer, IN IMS_BOOL bRepeat = IMS_FALSE);
    IMS_BOOL SetTimer(IN IMS_UINT32 nDuration,
            IN OsTimer* pTimer, IN IMS_BOOL bRepeat = IMS_FALSE);

    static void CleanUp();
    static void StartUp();
    static OsTimerService* GetTimerService();

private:
    static OsTimerService* s_pTimerService;

    OsTimerThread* m_pWorkerThread;
};

#endif
