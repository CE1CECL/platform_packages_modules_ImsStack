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

#ifndef _MEDIA_SESSION_CONFIG_H_
#define _MEDIA_SESSION_CONFIG_H_

#include "MediaDef.h"
#include "IMSList.h"
#include "AString.h"

class IConfigBuffer;
class AudioConfiguration;
class VideoConfiguration;
class TextConfiguration;

class MediaSessionConfig
{
public:
    // "audio"/"video" parameters
    static const IMS_CHAR KEY_REF[];
    static const IMS_CHAR KEY_SESSION_TYPE[];
    static const IMS_CHAR KEY_AUDIO[];
    static const IMS_CHAR KEY_AUDIO_CODEC[];
    static const IMS_CHAR KEY_AUDIO_REF[];
    static const IMS_CHAR KEY_VIDEO[];
    static const IMS_CHAR KEY_VIDEO_CODEC[];
    static const IMS_CHAR KEY_VIDEO_REF[];
    static const IMS_CHAR KEY_TEXT[];
    static const IMS_CHAR KEY_TEXT_CODEC[];
    static const IMS_CHAR KEY_TEXT_REF[];
    // General parameters
    static const IMS_CHAR KEY_JITTER_BUFFER_SIZE[];
    static const IMS_CHAR KEY_TELEPHONE_EVENT_DURATION[];
    static const IMS_CHAR KEY_CODECS[];
    static const IMS_CHAR KEY_CODECS_PROFILE_NUM[];
    static const IMS_CHAR KEY_CODEC_LIST_SIZE[];
    static const IMS_CHAR KEY_CODEC_REF[];
    static const IMS_CHAR KEY_CODEC_TYPE[];

protected:
    // "media" section & parameters
    static const IMS_CHAR SECTION_MEDIA[];
    static const IMS_CHAR KEY_LOOPBACK[];
    static const IMS_CHAR KEY_DEBUG_PCAP[];
    static const IMS_CHAR KEY_SESSION_LEVEL_BW[];
    static const IMS_CHAR KEY_MEDIA_MANDATORY_NEGO[];
    static const IMS_CHAR KEY_RTP_ANALYZER[];
    static const IMS_CHAR KEY_RTP_ANALYZER_OPTION[];
    static const IMS_CHAR KEY_DEBUG_MMPF_AP_LOG[];
    static const IMS_CHAR KEY_DEBUG_MMPF_CP_LOG[];

public:
    MediaSessionConfig(IN IMS_SINT32 nSlotId = 0,
            IN MEDIA_SERVICE_TYPE eServiceType = MEDIA_SERVICE_NONE);
    virtual ~MediaSessionConfig();

    MEDIA_SERVICE_TYPE GetServiceType() const;
    void SetServiceType(IN MEDIA_SERVICE_TYPE eServiceType);

private:
    MediaSessionConfig(IN CONST MediaSessionConfig &objRHS);
    MediaSessionConfig& operator=(IN CONST MediaSessionConfig &objRHS);

public:
    virtual IMS_BOOL Create(IN IMS_SINT32 nSlotId, IN IConfigBuffer *piBuffer,
            IN IMS_BOOL bVideo = IMS_TRUE);
    virtual void ToDebugString() const;

public:
    AudioConfiguration* GetAudioConfiguration(
                IN MEDIA_CONTENT_TYPE sessionType = MEDIA_TYPE_INVALID);
    VideoConfiguration* GetVideoConfiguration(
                IN MEDIA_CONTENT_TYPE sessionType = MEDIA_TYPE_INVALID);
    TextConfiguration* GetTextConfiguration(
                IN MEDIA_CONTENT_TYPE sessionType = MEDIA_TYPE_INVALID);

    AudioConfiguration* GetAudioConfiguration(IN IMS_UINT32 nIndex,
            OUT MEDIA_CONTENT_TYPE* pSessionType = NULL);
    VideoConfiguration* GetVideoConfiguration(IN IMS_UINT32 nIndex,
            OUT MEDIA_CONTENT_TYPE* pSessionType = NULL);
    TextConfiguration* GetTextConfiguration(IN IMS_UINT32 nIndex,
            OUT MEDIA_CONTENT_TYPE* pSessionType = NULL);

    IMS_UINT32 GetAudioConfigSessionNum();
    IMS_UINT32 GetVideoConfigSessionNum();
    IMS_UINT32 GetTextConfigSessionNum();

    IMS_BOOL Update(IN IConfigBuffer *piBuffer);
    IMS_BOOL IsLoopback() const;
    IMS_BOOL IsDebugPcap() const;
    IMS_BOOL IsSessionLevelBandwidth() const;

    IMS_SINT32 GetMediaMandatoryNego() const;
    IMS_SINT32 GetRTPAnalyzerList() const;
    tRTP_ANALYZER_OPTION* GetRTPAnalyzerOptionList();

    IMS_UINT32 GetMmpfDebugApLog() const;
    IMS_UINT32 GetMmpfDebugCpLog() const;

protected:
    virtual AString GetMediaRef() const;
    void Clear();

public:
    AudioConfiguration* CreateAudioConfiguration(
                IN MEDIA_CONTENT_TYPE nSessionType = MEDIA_TYPE_INVALID);
    VideoConfiguration* CreateVideoConfiguration(
                IN MEDIA_CONTENT_TYPE nSessionType = MEDIA_TYPE_INVALID);
    TextConfiguration* CreateTextConfiguration(
                IN MEDIA_CONTENT_TYPE nSessionType = MEDIA_TYPE_INVALID);

    IMS_BOOL CreateAudioConfigs(IN CONST AString &strRef,
            IN IMS_SINT32 nCount, IN IMS_SINT32 nSlotId = IMS_SLOT_0);
    IMS_BOOL CreateVideoConfigs(IN CONST AString &strRef,
            IN IMS_SINT32 nCount, IN IMS_SINT32 nSlotId = IMS_SLOT_0);
    IMS_BOOL CreateTextConfigs(IN CONST AString &strRef,
            IN IMS_SINT32 nCount, IN IMS_SINT32 nSlotId = IMS_SLOT_0);

    IMS_BOOL UpdateAudioConfigs(IN CONST AString &strRef,
            IN IMS_SINT32 nCount, IN IMS_SINT32 nSlotId = IMS_SLOT_0);
    IMS_BOOL UpdateVideoConfigs(IN CONST AString &strRef,
            IN IMS_SINT32 nCount, IN IMS_SINT32 nSlotId = IMS_SLOT_0);
    IMS_BOOL UpdateTextConfigs(IN CONST AString &strRef,
            IN IMS_SINT32 nCount, IN IMS_SINT32 nSlotId = IMS_SLOT_0);

private:
    MEDIA_CONTENT_TYPE ConvertSessionType(IN CONST AString& strSessionType);
    void MakeAnalyzerList(IN CONST AString& strList);
    void MakeAnalyzerOptionList(IN CONST AString& strList);

protected:
    // Media parameters
    IMS_UINT32 nIsLoopback;
    IMS_BOOL bIsDebugPcap;
    IMS_BOOL bIsSessLevelBW;

    // Media RTP KPI Analyzer
    IMS_SINT32 nRtpAnalyzerList;
    tRTP_ANALYZER_OPTION* pRtpAnalyzerOptionList;

    // Media - audio/video
    IMSList<AudioConfiguration*> objAudioConfigs;
    IMSList<VideoConfiguration*> objVideoConfigs;
    IMSList<TextConfiguration*> objTextConfigs;

    MEDIA_CONTENT_TYPE eMediaMandatoryNego;

    IMS_SINT32 nDebugMmpfApLog;
    IMS_SINT32 nDebugMmpfCpLog;

private:
    MEDIA_SERVICE_TYPE m_nServiceType;

};
#endif // _MEDIA_SESSION_CONFIG_H_
