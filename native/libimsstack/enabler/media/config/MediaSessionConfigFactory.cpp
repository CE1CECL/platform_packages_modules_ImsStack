/**
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

#include "ServiceTrace.h"
#include "config/MediaSessionConfigFactory.h"

static MediaSessionConfigFactory* g_pMediaSessionConfigFactory = IMS_NULL;

__IMS_TRACE_TAG_USER_DECL__("MED.CONF");

PRIVATE
MediaSessionConfigFactory::MediaSessionConfigFactory()
{
}

PUBLIC VIRTUAL
MediaSessionConfigFactory::~MediaSessionConfigFactory()
{
    for (IMS_UINT32 nIdx = 0; nIdx <= IMS_SLOT_1; nIdx++)
    {
        DestroyListSessionConfig(nIdx);
        m_mapListMediaSessionConfig.Remove(nIdx);
    }
}

PUBLIC
void MediaSessionConfigFactory::CreateMediaSessionConfig(IN IMS_SINT32 nSlotId,
        IN MEDIA_SERVICE_TYPE eServiceType)
{
    IMSList<MediaSessionConfig*>* pListMediaSessionConfig = GetListSessionConfig(nSlotId);

    if (pListMediaSessionConfig == IMS_NULL)
    {
        pListMediaSessionConfig = new IMSList<MediaSessionConfig*>();
        m_mapListMediaSessionConfig.Add(nSlotId, pListMediaSessionConfig);
    }

    MediaSessionConfig* mediaSessionConfig = new MediaSessionConfig(nSlotId, eServiceType);
    pListMediaSessionConfig->Append(mediaSessionConfig);

    IMS_TRACE_D("CreateMediaSessionConfig - nSlotId[%d], listSize[%d], svc[%d]",
            nSlotId, pListMediaSessionConfig->GetSize(), (IMS_SINT32)eServiceType);
}

PUBLIC
void MediaSessionConfigFactory::AddMediaSessionConfig(IN IMS_SINT32 nSlotId,
        IN MediaSessionConfig* mediaSessionConfig)
{
    IMSList<MediaSessionConfig*>* pListMediaSessionConfig = GetListSessionConfig(nSlotId);

    if (pListMediaSessionConfig == IMS_NULL)
    {
        pListMediaSessionConfig = new IMSList<MediaSessionConfig*>();
        pListMediaSessionConfig->Append(mediaSessionConfig);

        IMS_TRACE_D("AddMediaSessionConfig - nSlotId[%d], listSize[%d]",
                nSlotId, pListMediaSessionConfig->GetSize(), 0);

        m_mapListMediaSessionConfig.Add(nSlotId, pListMediaSessionConfig);
    }
    else
    {
        for (IMS_UINT32 nIdxList = 0; nIdxList < pListMediaSessionConfig->GetSize(); nIdxList++)
        {
            MediaSessionConfig* tempmediaSessionConfig = pListMediaSessionConfig->GetAt(nIdxList);
            if (tempmediaSessionConfig == mediaSessionConfig) /// Already Have
            {
                return;
            }
        }

        pListMediaSessionConfig->Append(mediaSessionConfig);

        IMS_TRACE_D("AddMediaSessionConfig - nSlotId[%d], listSize[%d]",
                nSlotId, pListMediaSessionConfig->GetSize(), 0);
    }
}

PUBLIC
void MediaSessionConfigFactory::DestroyListSessionConfig(IN IMS_SINT32 nSlotId)
{
    IMSList<MediaSessionConfig*>* pListMediaSessionConfig = GetListSessionConfig(nSlotId);

    if (pListMediaSessionConfig != IMS_NULL)
    {
        IMS_TRACE_D("DestroyListSessionConfig - list[%d]",
                pListMediaSessionConfig->GetSize(), 0, 0);

        for (IMS_UINT32 nIdxList = 0; nIdxList < pListMediaSessionConfig->GetSize(); nIdxList++)
        {
            MediaSessionConfig* mediaSessionConfig = pListMediaSessionConfig->GetAt(nIdxList);
            DestroySessionConfig(mediaSessionConfig);
        }

        pListMediaSessionConfig->Clear();
        delete pListMediaSessionConfig;
        pListMediaSessionConfig = IMS_NULL;
    }
}

PUBLIC
IMSList<MediaSessionConfig*>* MediaSessionConfigFactory::GetListSessionConfig(IN IMS_SINT32 nSlotId)
{
    if (m_mapListMediaSessionConfig.GetIndexOfKey(nSlotId) >= 0)
    {
        return m_mapListMediaSessionConfig.GetValue(nSlotId);
    }

    return IMS_NULL;
}

MediaSessionConfig* MediaSessionConfigFactory::FindMediaSessionConfig(IN IMS_SINT32 nSlotId,
        IN MEDIA_SERVICE_TYPE eServiceType)
{
    IMSList<MediaSessionConfig*>* pListMediaSessionConfig = GetListSessionConfig(nSlotId);

    MediaSessionConfig* mediaSessionConfig = IMS_NULL;

    if (pListMediaSessionConfig != IMS_NULL)
    {
        for (IMS_UINT32 nIdxList = 0; nIdxList < pListMediaSessionConfig->GetSize(); nIdxList++)
        {
            mediaSessionConfig = pListMediaSessionConfig->GetAt(nIdxList);

            if (mediaSessionConfig->GetServiceType() == eServiceType ||
                    MEDIA_SERVICE_NONE == eServiceType)
            {
                return mediaSessionConfig;
            }
        }
    }

    if (pListMediaSessionConfig != IMS_NULL)
    {
        if (mediaSessionConfig == IMS_NULL && pListMediaSessionConfig->GetSize() > 0)
        {
            mediaSessionConfig = pListMediaSessionConfig->GetAt(0);
        }
    }

    if (mediaSessionConfig != NULL)
    {
        IMS_TRACE_D("FindMediaSessionConfig() - eServiceType[%d]",
                mediaSessionConfig->GetServiceType(), 0, 0);
    }

    return mediaSessionConfig;
}

PUBLIC
void MediaSessionConfigFactory::DestroySessionConfig(IN MediaSessionConfig* pMediaSessionConfig)
{
    for (IMS_UINT32 nIdxSlot = 0; nIdxSlot <= IMS_SLOT_1; nIdxSlot++)
    {
        IMSList<MediaSessionConfig*>* pListMediaSessionConfig = GetListSessionConfig(nIdxSlot);

        if (pListMediaSessionConfig != IMS_NULL)
        {
            IMS_TRACE_D("DestroySessionConfig - list[%d]",
                    pListMediaSessionConfig->GetSize(), 0, 0);

            for (IMS_UINT32 nIdxList = 0; nIdxList < pListMediaSessionConfig->GetSize(); nIdxList++)
            {
                MediaSessionConfig* mediaSessionConfig = pListMediaSessionConfig->GetAt(nIdxList);

                if (mediaSessionConfig == pMediaSessionConfig)
                {
                    pListMediaSessionConfig->RemoveAt(nIdxList);
                    return;
                }
            }
        }

    }
}

MediaSessionConfigFactory* MediaSessionConfigFactory::GetInstance()
{
    if (g_pMediaSessionConfigFactory == IMS_NULL)
    {
        g_pMediaSessionConfigFactory = new MediaSessionConfigFactory();
    }

    return g_pMediaSessionConfigFactory;
}

void MediaSessionConfigFactory::ReleaseInstance(MediaSessionConfigFactory* pSessionConfigFactory)
{
    if (pSessionConfigFactory != IMS_NULL && pSessionConfigFactory == g_pMediaSessionConfigFactory)
    {
        delete pSessionConfigFactory;
        g_pMediaSessionConfigFactory = IMS_NULL;
    }
}
