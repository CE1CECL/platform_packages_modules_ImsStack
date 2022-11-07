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
#include "DeviceConfig.h"
#include "SystemConfig.h"

PRIVATE GLOBAL IMS_SINT32 SystemConfig::s_nGlobalConfigs = CONFIG_MULTI_IMS;

PUBLIC GLOBAL IMS_SINT32 SystemConfig::GetActiveSimCount()
{
    return DeviceConfig::GetActiveSimCount();
}

PUBLIC GLOBAL IMS_SINT32 SystemConfig::GetSupportedSimCount()
{
    return DeviceConfig::GetSupportedSimCount();
}

PUBLIC GLOBAL IMS_BOOL SystemConfig::IsMultiImsEnabled()
{
    // As a default, single IMS is required on dual SIM environment.
    return (s_nGlobalConfigs & CONFIG_MULTI_IMS) != 0;
}

PUBLIC GLOBAL IMS_BOOL SystemConfig::IsMultiSimEnabled()
{
    return GetActiveSimCount() > 1;
}
