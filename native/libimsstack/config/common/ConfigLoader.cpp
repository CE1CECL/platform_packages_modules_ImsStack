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
#include "ServiceMemory.h"
#include "ServiceTrace.h"

#include "ConfigBuffer.h"
#include "ConfigLoader.h"
#include "conf/ConfigFileBuffer.h"
#include "db/ConfigDBBuffer.h"
#include "private/ConfigurationManager.h"

__IMS_TRACE_TAG_CONF__;

#define CONFIG_KEY_BASE(ID)    ((ID) * (ConfigLoader::CONFIG_MAX))

LOCAL
const IMS_CHAR* PRE_DEFINED_CONFIG_TABLE[ConfigLoader::CONFIG_MAX] =
{
    "subscriber,subscriber_fake",
    "media"
};

PRIVATE GLOBAL
IMSMap<IMS_SINT32, AString> ConfigLoader::s_objIndexConfig;

PUBLIC GLOBAL
IConfigBuffer* ConfigLoader::GetConfig(IN const AString &strConfName, IN IMS_SINT32 nId)
{
    ConfigurationManager* pConfigMngr = ConfigurationManager::GetInstance();
    ConfigBuffer* pBuffer = IMS_NULL;

    if (pConfigMngr->GetConfigMode() != ConfigurationManager::MODE_DB)
    {
        IMS_TRACE_E(0, "ConfigLoader :: Not Implemented - Unknown Mode", 0, 0, 0);
        return IMS_NULL;
    }

    pBuffer = new ConfigDBBuffer(pConfigMngr->GetConfigLocator(), strConfName);

    if (pBuffer == IMS_NULL)
    {
        IMS_TRACE_E(0, "Allocating a config (%s, %d) failed", strConfName.GetStr(), nId, 0);
        return IMS_NULL;
    }

    if (pBuffer->Create(nId) != IMS_TRUE)
    {
        delete pBuffer;

        IMS_TRACE_D("Not found - Config (%s, %d)", strConfName.GetStr(), nId, 0);
        return IMS_NULL;
    }

    return pBuffer;
}

PUBLIC GLOBAL
IConfigBuffer* ConfigLoader::GetConfig(IN const AString& strContent)
{
    if (strContent.GetLength() == 0)
    {
        IMS_TRACE_E(0, "ConfigLoader: No configuration data.", 0, 0, 0);
        return IMS_NULL;
    }

    return ConfigFileBuffer::CreateFileBuffer(strContent);
}

PUBLIC GLOBAL
const AString& ConfigLoader::GetConfName(IN IMS_SINT32 nConfName, IN IMS_SINT32 nId)
{
    IMS_SINT32 nKey = CONFIG_KEY_BASE(nId) + nConfName;
    IMS_SLONG nIndex = s_objIndexConfig.GetIndexOfKey(nKey);

    if (nIndex < 0)
    {
        IMS_TRACE_E(0, "ConfigLoader :: Invalid argument (%d, %d)", nConfName, nId, 0);
        return AString::ConstNull();
    }

    return s_objIndexConfig.GetValueAt(nIndex);
}

PUBLIC GLOBAL
IMS_BOOL ConfigLoader::LoadIndexConfig(IN IMS_SINT32 nId)
{
    IMS_SINT32 nConfigKeyBase = CONFIG_KEY_BASE(nId);

    // Get the config value from the specified name
    for (IMS_UINT32 i = 0; i < CONFIG_MAX; ++i)
    {
        s_objIndexConfig.SetValue(nConfigKeyBase + i, PRE_DEFINED_CONFIG_TABLE[i]);
    }

    return IMS_TRUE;
}
