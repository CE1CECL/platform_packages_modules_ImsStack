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

#ifndef MOCK_I_AOS_TRM_H_
#define MOCK_I_AOS_TRM_H_

#include <gmock/gmock.h>
#include "interface/IAosTrm.h"

class MockIAosTrmListener : public IAosTrmListener
{
public:
    MOCK_METHOD(void, Trm_PriorityChanged, (), (override));
};

class MockIAosTrm : public IAosTrm
{
public:
    MOCK_METHOD(void, SetListener, (IN IAosTrmListener * piListener), (override));
    MOCK_METHOD(void, RemoveListener, (IN IAosTrmListener * piListener), (override));
    MOCK_METHOD(IMS_BOOL, IsReady, (), (override));
    MOCK_METHOD(IMS_BOOL, IsTRMSupported, (), (override));
    MOCK_METHOD(void, Set, (IN IMS_UINT32 nType, IN IMS_BOOL bStart), (override));
    MOCK_METHOD(void, SetEmegency, (IN IMS_UINT32 nType, IN IMS_BOOL bStart), (override));
    MOCK_METHOD(void, SetIPCAN, (IN IN IMS_UINT32 nCategory), (override));
};

#endif  // MOCK_I_AOS_TRM_H_
