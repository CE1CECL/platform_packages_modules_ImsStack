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

#include "ServiceMemory.h"
#include "ServiceTrace.h"
#include "config/MediaSessionConfigFactory.h"
#include "ServiceUtil.h"
#include "config/MediaSessionConfig.h"
#include "IConfigBuffer.h"
#include "Configuration.h"
#include "TextParser.h"
#include "config/AudioConfiguration.h"
#include "config/VideoConfiguration.h"
#include "config/TextConfiguration.h"

__IMS_TRACE_TAG_USER_DECL__("CONF");

PUBLIC GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_REF[] = "ref";
PUBLIC GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_SESSION_TYPE[] = "session_type";
PUBLIC GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_AUDIO[] = "audio";
PUBLIC GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_AUDIO_CODEC[] = "audiocodec";
PUBLIC GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_AUDIO_REF[] = "audio_ref";
PUBLIC GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_VIDEO[] = "video";
PUBLIC GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_VIDEO_CODEC[] = "videocodec";
PUBLIC GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_VIDEO_REF[] = "video_ref";
PUBLIC GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_TEXT[] = "text";
PUBLIC GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_TEXT_CODEC[] = "textcodec";
PUBLIC GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_TEXT_REF[] = "text_ref";
PUBLIC GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_JITTER_BUFFER_SIZE[] = "jitter_buffer_size";
PUBLIC GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_TELEPHONE_EVENT_DURATION[] = "telephone_event_duration";
PUBLIC GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_CODECS[] = "codecs";
PUBLIC GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_CODECS_PROFILE_NUM[] = "codecs_profile_num";
PUBLIC GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_CODEC_LIST_SIZE[] = "codec_list_size";
PUBLIC GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_CODEC_REF[] = "codec_ref";
PUBLIC GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_CODEC_TYPE[] = "codec_type";
PROTECTED GLOBAL
const IMS_CHAR MediaSessionConfig::SECTION_MEDIA[] = "media";
PROTECTED GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_LOOPBACK[] = "loopback";
PROTECTED GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_DEBUG_PCAP[] = "debug_pcap";
PROTECTED GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_SESSION_LEVEL_BW[] = "session_level_bw";
PROTECTED GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_MEDIA_MANDATORY_NEGO[] = "mandatory_nego";
PROTECTED GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_RTP_ANALYZER[] = "rtp_analyzer";
PROTECTED GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_RTP_ANALYZER_OPTION[] = "rtp_analyzer_option";
PROTECTED GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_DEBUG_MMPF_AP_LOG[] = "debug_mmpf_ap_log";
PROTECTED GLOBAL
const IMS_CHAR MediaSessionConfig::KEY_DEBUG_MMPF_CP_LOG[] = "debug_mmpf_cp_log";

PUBLIC
MediaSessionConfig::MediaSessionConfig(IN IMS_SINT32 nSlotId, IN MEDIA_SERVICE_TYPE serviceType) :
        nIsLoopback(IMS_FALSE),
        bIsDebugPcap(IMS_FALSE),
        bIsSessLevelBW(IMS_FALSE),
        nRtpAnalyzerList(0),
        pRtpAnalyzerOptionList(IMS_NULL),
        objAudioConfigs(IMSList<AudioConfiguration*>()),
        objVideoConfigs(IMSList<VideoConfiguration*>()),
        objTextConfigs(IMSList<TextConfiguration*>()),
        eMediaMandatoryNego(MEDIA_TYPE_INVALID),
        nDebugMmpfApLog(0),
        nDebugMmpfCpLog(0),
        m_nServiceType(serviceType)
{
    IMS_TRACE_D("MediaSessionConfig() - nSlotId(%d), serviceType(%d)",
            nSlotId, serviceType, 0);

    //add for MediaSessionConfig acqusition in media layer
    MediaSessionConfigFactory::GetInstance()->AddMediaSessionConfig(nSlotId, this);
}

PUBLIC VIRTUAL
MediaSessionConfig::~MediaSessionConfig()
{
    Clear();
    MediaSessionConfigFactory::GetInstance()->DestroySessionConfig(this);
}

PUBLIC
MEDIA_SERVICE_TYPE MediaSessionConfig::GetServiceType() const
{
    return m_nServiceType;
}

PUBLIC
void MediaSessionConfig::SetServiceType(IN MEDIA_SERVICE_TYPE serviceType)
{
    IMS_TRACE_D("SetServiceType(%d)", serviceType, 0, 0);
    m_nServiceType = serviceType;
}

PUBLIC VIRTUAL
IMS_BOOL MediaSessionConfig::Create(IN IMS_SINT32 nSlotId,
        IN IConfigBuffer *piBuffer, IN IMS_BOOL bVideo)
{
    (void) bVideo;

    if (piBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    if (!piBuffer->CaptureSection(SECTION_MEDIA))
    {
        return IMS_FALSE;
    }

    Clear();

    // Media loopback enabled or not
    nIsLoopback = piBuffer->ReadValueBoolean(KEY_LOOPBACK);
    bIsDebugPcap = piBuffer->ReadValueBoolean(KEY_DEBUG_PCAP);
    bIsSessLevelBW = piBuffer->ReadValueBoolean(KEY_SESSION_LEVEL_BW);

    // Media RTP Analyzer List
    const AString &strAnalyzerList = piBuffer->ReadValue(KEY_RTP_ANALYZER);
    MakeAnalyzerList(strAnalyzerList);

    // Media RTP Analyzer Option List
    const AString &strAnalyzerOptionList = piBuffer->ReadValue(KEY_RTP_ANALYZER_OPTION);
    MakeAnalyzerOptionList(strAnalyzerOptionList);

    const AString &strMandatoryNego = piBuffer->ReadValue(KEY_MEDIA_MANDATORY_NEGO);
    IMSList<AString> objTokens = strMandatoryNego.Split(TextParser::CHAR_COMMA);

    for (IMS_UINT32 i=0; i<objTokens.GetSize(); i++)
    {
        if (objTokens.GetAt(i).EqualsIgnoreCase("audio"))
        {
            eMediaMandatoryNego = (MEDIA_CONTENT_TYPE)((IMS_SINT32)eMediaMandatoryNego |
                    (IMS_SINT32)MEDIA_TYPE_AUDIO);
        }
        else if (objTokens.GetAt(i).EqualsIgnoreCase("video"))
        {
            eMediaMandatoryNego = (MEDIA_CONTENT_TYPE)((IMS_SINT32)eMediaMandatoryNego |
                    (IMS_SINT32)MEDIA_TYPE_VIDEO);
        }
        else if (objTokens.GetAt(i).EqualsIgnoreCase("text"))
        {
            eMediaMandatoryNego = (MEDIA_CONTENT_TYPE)((IMS_SINT32)eMediaMandatoryNego |
                    (IMS_SINT32)MEDIA_TYPE_TEXT);
        }
    }

    IMS_TRACE_D("Create() - loopback(%d),mediadump(%d),eMediaMandatoryNego(%d)",
            nIsLoopback, bIsDebugPcap, eMediaMandatoryNego);

    //"audio_count" /"video_count" /"text_count" parameter
    IMS_SINT32 nAudioCount = piBuffer->ReadKeyCount(KEY_AUDIO);
    IMS_SINT32 nVideoCount = piBuffer->ReadKeyCount(KEY_VIDEO);
    IMS_SINT32 nTextCount = piBuffer->ReadKeyCount(KEY_TEXT);

    IMS_TRACE_D("Create() - audio(%d), video(%d), text(%d)",
            nAudioCount, nVideoCount, nTextCount);

    nDebugMmpfApLog = piBuffer->ReadValueInt(KEY_DEBUG_MMPF_AP_LOG);
    nDebugMmpfCpLog = piBuffer->ReadValueInt(KEY_DEBUG_MMPF_CP_LOG);

    IMS_TRACE_D("Create() - nDebugMmpfApLog(%d), nDebugMmpfCpLog(%d)",
            nDebugMmpfApLog, nDebugMmpfCpLog, 0);

    // Creates an audio configuration
    if (nAudioCount > 0)
    {
        if (!CreateAudioConfigs(piBuffer->ReadValue(KEY_AUDIO_REF), nAudioCount, nSlotId))
        {
            piBuffer->ReleaseSection();
            return IMS_FALSE;
        }
    }

    // Creates a video configuration
    if (nVideoCount > 0)
    {
        if (!CreateVideoConfigs(piBuffer->ReadValue(KEY_VIDEO_REF), nVideoCount, nSlotId))
        {
            piBuffer->ReleaseSection();
            return IMS_FALSE;
        }
    }

    // Creates a text configuration
    if (nTextCount > 0)
    {
        if (!CreateTextConfigs(piBuffer->ReadValue(KEY_TEXT_REF), nTextCount, nSlotId))
        {
            piBuffer->ReleaseSection();
            return IMS_FALSE;
        }
    }

    piBuffer->ReleaseSection();

    MediaSessionConfigFactory::GetInstance()->AddMediaSessionConfig(nSlotId, this);

    return IMS_TRUE;
}

PUBLIC VIRTUAL
void MediaSessionConfig::ToDebugString() const
{
    IMS_TRACE_D("loopback(%s), meida_dump(%s), mandatory_nego(%d)",
            (nIsLoopback) ?"IMS_TRUE" :"IMS_FALSE", (bIsDebugPcap) ?"IMS_TRUE" :"IMS_FALSE",
            eMediaMandatoryNego);
    IMS_TRACE_D("nRtpAnalyzerList(%d), nDebugMmpfApLog(%d), nDebugMmpfCpLog(%d)",
            nRtpAnalyzerList, nDebugMmpfApLog, nDebugMmpfCpLog);
}

PUBLIC
AudioConfiguration* MediaSessionConfig::GetAudioConfiguration(IN MEDIA_CONTENT_TYPE sessionType)
{
    if (objAudioConfigs.IsEmpty())
    {
        IMS_TRACE_D("GetAudioConfiguration() objAudioConfigs is empty", 0, 0, 0);
        return IMS_NULL;
    }

    for (IMS_UINT32 nIndex = 0; nIndex < objAudioConfigs.GetSize(); ++nIndex)
    {
        AudioConfiguration* pCurrAudioConfig = objAudioConfigs.GetAt(nIndex);
        if (pCurrAudioConfig != IMS_NULL)
        {
            if (sessionType == pCurrAudioConfig->GetSessionType()
                || sessionType == MEDIA_TYPE_INVALID
                || pCurrAudioConfig->GetSessionType() == MEDIA_TYPE_INVALID)
            {
                return pCurrAudioConfig;
            }
        }
    }

    return objAudioConfigs.GetAt(0);
}

PUBLIC
VideoConfiguration* MediaSessionConfig::GetVideoConfiguration(IN MEDIA_CONTENT_TYPE sessionType)
{
    for (IMS_UINT32 nIndex = 0; nIndex < objVideoConfigs.GetSize(); ++nIndex)
    {
        VideoConfiguration* pCurrVideoConfig = objVideoConfigs.GetAt(nIndex);
        if (pCurrVideoConfig != IMS_NULL)
        {
            if (sessionType == pCurrVideoConfig->GetSessionType()
                || sessionType == MEDIA_TYPE_INVALID
                || pCurrVideoConfig->GetSessionType() == MEDIA_TYPE_INVALID)
            {
                return pCurrVideoConfig ;
            }
        }
    }

    // try again, w/o checking sessionType
    if (!objVideoConfigs.IsEmpty())
    {
        return objVideoConfigs.GetAt(0);
    }

    return IMS_NULL;
}

PUBLIC
TextConfiguration* MediaSessionConfig::GetTextConfiguration(IN MEDIA_CONTENT_TYPE sessionType)
{
    for (IMS_UINT32 nIndex = 0; nIndex < objTextConfigs.GetSize(); ++nIndex)
    {
        TextConfiguration* pCurrTextConfig = objTextConfigs.GetAt(nIndex);
        if (pCurrTextConfig != IMS_NULL)
        {
            if (sessionType == pCurrTextConfig->GetSessionType()
                    || sessionType == MEDIA_TYPE_INVALID
                    || pCurrTextConfig->GetSessionType() == MEDIA_TYPE_INVALID)
            {
                return pCurrTextConfig ;
            }

        }

    }

    // try again, w/o checking sessionType
    if (!objTextConfigs.IsEmpty())
    {
        return objTextConfigs.GetAt(0);
    }

    return IMS_NULL;
}

PUBLIC
AudioConfiguration* MediaSessionConfig::GetAudioConfiguration(IN IMS_UINT32 nIndex,
        IN MEDIA_CONTENT_TYPE* pSessionType)
{
    if (nIndex < objAudioConfigs.GetSize())
    {
        AudioConfiguration* pAudioConfig = objAudioConfigs.GetAt(nIndex);

        if (pAudioConfig != IMS_NULL && pSessionType != IMS_NULL)
        {
            *pSessionType = pAudioConfig->GetSessionType();
        }

        return pAudioConfig;
    }

    return IMS_NULL;
}

PUBLIC
VideoConfiguration* MediaSessionConfig::GetVideoConfiguration(IN IMS_UINT32 nIndex,
        IN MEDIA_CONTENT_TYPE* pSessionType)
{
    if (nIndex < objVideoConfigs.GetSize())
    {
        VideoConfiguration* pVideoConfig = objVideoConfigs.GetAt(nIndex);

        if (pVideoConfig != IMS_NULL && pSessionType != IMS_NULL)
        {
            *pSessionType = pVideoConfig->GetSessionType();
        }

        return pVideoConfig;
    }

    return IMS_NULL;
}

PUBLIC
TextConfiguration* MediaSessionConfig::GetTextConfiguration(IN IMS_UINT32 nIndex,
        IN MEDIA_CONTENT_TYPE* pSessionType)
{
    if (nIndex < objTextConfigs.GetSize())
    {
        TextConfiguration* pTextConfig = objTextConfigs.GetAt(nIndex);

        if (pTextConfig != IMS_NULL && pSessionType != IMS_NULL)
        {
            *pSessionType = pTextConfig->GetSessionType();
        }

        return pTextConfig;
    }

    return IMS_NULL;
}

PUBLIC
IMS_UINT32 MediaSessionConfig::GetAudioConfigSessionNum()
{
    return objAudioConfigs.GetSize();
}

PUBLIC
IMS_UINT32 MediaSessionConfig::GetVideoConfigSessionNum()
{
    return objVideoConfigs.GetSize();
}

PUBLIC
IMS_UINT32 MediaSessionConfig::GetTextConfigSessionNum()
{
    return objTextConfigs.GetSize();
}

PUBLIC
IMS_BOOL MediaSessionConfig::Update(IN IConfigBuffer *piBuffer)
{
    AString strMediaRef = GetMediaRef();

    if (strMediaRef.GetLength() == 0)
    {
        if (piBuffer == IMS_NULL)
        {
            return IMS_FALSE;
        }

        if (!piBuffer->CaptureSection(SECTION_MEDIA))
        {
            return IMS_FALSE;
        }

        strMediaRef = piBuffer->ReadValue(KEY_REF);

        piBuffer->ReleaseSection();
    }

    IMS_TRACE_D("Update() - strMediaRef(%s)", strMediaRef.GetStr(), 0, 0);

    if (Configuration::GetInstance() == IMS_NULL)
    {
        return IMS_FALSE;
    }

    IConfigBuffer *piMediaBuffer = Configuration::GetInstance()->CreateConfig(strMediaRef);
    if (piMediaBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    if (!piMediaBuffer->CaptureSection(SECTION_MEDIA))
    {
        piMediaBuffer->Destroy();
        return IMS_FALSE;
    }

    //"audio_count" /"video_count" parameter
    IMS_SINT32 nAudioCount = piMediaBuffer->ReadKeyCount(KEY_AUDIO);
    IMS_SINT32 nVideoCount = piMediaBuffer->ReadKeyCount(KEY_VIDEO);
    IMS_SINT32 nTextCount = piMediaBuffer->ReadKeyCount(KEY_TEXT);

    IMS_TRACE_I("Update() - VT configuration :: audio(%d), video(%d), text(%d)",
            nAudioCount, nVideoCount, nTextCount);

    // Creates an audio configuration
    if (nAudioCount > 0)
    {
        if (!UpdateAudioConfigs(piMediaBuffer->ReadValue(KEY_AUDIO_REF), nAudioCount))
        {
            piMediaBuffer->Destroy();
            return IMS_FALSE;
        }
    }

    // Creates a video configuration
    if (nVideoCount > 0)
    {
        if (!UpdateVideoConfigs(piMediaBuffer->ReadValue(KEY_VIDEO_REF), nVideoCount))
        {
            piMediaBuffer->Destroy();
            return IMS_FALSE;
        }
    }

    // Creates a text configuration
    if (nTextCount > 0)
    {
        if (!UpdateTextConfigs(piMediaBuffer->ReadValue(KEY_TEXT_REF), nTextCount))
        {
            piMediaBuffer->Destroy();
            return IMS_FALSE;
        }
    }

    piMediaBuffer->ReleaseSection();
    piMediaBuffer->Destroy();

    return IMS_TRUE;
}

PUBLIC
IMS_BOOL MediaSessionConfig::IsLoopback() const
{
    return (nIsLoopback > 0);
}

PUBLIC
IMS_BOOL MediaSessionConfig::IsDebugPcap() const
{
    return bIsDebugPcap;
}

PUBLIC
IMS_BOOL MediaSessionConfig::IsSessionLevelBandwidth() const
{
    return bIsSessLevelBW;
}

PUBLIC
IMS_SINT32 MediaSessionConfig::GetMediaMandatoryNego() const
{
    return eMediaMandatoryNego;
}

PUBLIC
IMS_SINT32 MediaSessionConfig::GetRTPAnalyzerList() const
{
#if 0
    ISystemProperty *piSysProperty = UtilService::GetUtilService()->GetSystemProperty();
    if (piSysProperty == IMS_NULL) return 0;

    IMS_BOOL bIsChipsetMTK = piSysProperty->GetChipsetVendor().EqualsIgnoreCase("MediaTek");

enum _RTP_ANALYZER_FEATURE_
{
    RTP_ANALYZER_FEATURE_INVALID            = 0,
    RTP_ANALYZER_FEATURE_RTP                = (0x00000001),
    RTP_ANALYZER_FEATURE_DRA                = (0x00000002),
    RTP_ANALYZER_FEATURE_CIQ                = (0x00000004),
    RTP_ANALYZER_FEATURE_HASATI             = (0x00000008),
    RTP_ANALYZER_FEATURE_CCT                = (0x00000010),
    RTP_ANALYZER_FEATURE_DEBUGSCREEN        = (0x00000020),
    RTP_ANALYZER_FEATURE_VTDATAUSAGE        = (0x00000040)
}RTP_ANALYZER_FEATURE;

    AString strBoardPlatform = piSysProperty->Get("ro.board.platform");

    if (strBoardPlatform.EqualsIgnoreCase("sdm845")
            || strBoardPlatform.EqualsIgnoreCase("msm8998")
            || strBoardPlatform.EqualsIgnoreCase("msm8996")
            || strBoardPlatform.EqualsIgnoreCase("msm8996e")
            || strBoardPlatform.EqualsIgnoreCase("msm8937"))
    {
        return nRtpAnalyzerList > 0 ? nRtpAnalyzerList : 0;
    }

    if (bIsChipsetMTK == IMS_TRUE)
    {
        if (nRtpAnalyzerList & RTP_ANALYZER_DEBUGSCREEN)
        {
            IMS_TRACE_I("GetRTPAnalyzerList() only debug screen analyzer for MTK [0x%04x]",
                    nRtpAnalyzerList, 0, 0);
            return RTP_ANALYZER_DEBUGSCREEN;
        }

        IMS_TRACE_I("GetRTPAnalyzerList() RTP Analyzer is disable for MTK", 0, 0, 0);
        return 0;
    }

#endif

    IMS_TRACE_I("GetRTPAnalyzerList() - RTP Analyzerlist[0x%04x]", nRtpAnalyzerList, 0, 0);

    return nRtpAnalyzerList;
}

PUBLIC
tRTP_ANALYZER_OPTION* MediaSessionConfig::GetRTPAnalyzerOptionList()
{
    return pRtpAnalyzerOptionList;
}

PUBLIC
IMS_UINT32 MediaSessionConfig::GetMmpfDebugApLog() const
{
    if (nDebugMmpfApLog < 0)
    {
        return 0;
    }

    return nDebugMmpfApLog;
}

PUBLIC
IMS_UINT32 MediaSessionConfig::GetMmpfDebugCpLog() const
{
    if (nDebugMmpfCpLog < 0)
    {
        return 0;
    }

    return nDebugMmpfCpLog;
}

PROTECTED VIRTUAL
AString MediaSessionConfig::GetMediaRef() const
{
    return AString::ConstNull();
}

PROTECTED
void MediaSessionConfig::Clear()
{
    while (objAudioConfigs.GetSize() > 0)
    {
        AudioConfiguration *pConfig = objAudioConfigs.GetAt(0);

        if (pConfig != IMS_NULL)
        {
            delete pConfig;
            pConfig = IMS_NULL;
        }
        objAudioConfigs.RemoveAt(0);
    }
    objAudioConfigs.Clear();

    while (objVideoConfigs.GetSize() > 0)
    {
        VideoConfiguration *pConfig = objVideoConfigs.GetAt(0);

        if (pConfig != IMS_NULL)
        {
            delete pConfig;
            pConfig = IMS_NULL;
        }
        objVideoConfigs.RemoveAt(0);
    }
    objVideoConfigs.Clear();

    while (objTextConfigs.GetSize() > 0)
    {
        TextConfiguration *pConfig = objTextConfigs.GetAt(0);

        if (pConfig != IMS_NULL)
        {
            delete pConfig;
            pConfig = IMS_NULL;
        }
        objTextConfigs.RemoveAt(0);
    }
    objTextConfigs.Clear();

    if (pRtpAnalyzerOptionList != IMS_NULL)
    {
        IMS_MEM_Free(pRtpAnalyzerOptionList);
        pRtpAnalyzerOptionList = IMS_NULL;
    }
}

PROTECTED
AudioConfiguration* MediaSessionConfig::CreateAudioConfiguration(IN MEDIA_CONTENT_TYPE nSessionType)
{
    return new AudioConfiguration(nSessionType);
}

PROTECTED
VideoConfiguration* MediaSessionConfig::CreateVideoConfiguration(IN MEDIA_CONTENT_TYPE nSessionType)
{
    return new VideoConfiguration(nSessionType);
}

PROTECTED
TextConfiguration* MediaSessionConfig::CreateTextConfiguration(IN MEDIA_CONTENT_TYPE nSessionType)
{
    return new TextConfiguration(nSessionType);
}

PROTECTED
IMS_BOOL MediaSessionConfig::CreateAudioConfigs(IN CONST AString &strRef,
        IN IMS_SINT32 nCount, IN IMS_SINT32 nSlotId)
{
    IConfigBuffer *piBuffer = IMS_NULL;

    if (Configuration::GetInstance() == IMS_NULL)
    {
        return IMS_FALSE;
    }

    piBuffer = Configuration::GetInstance()->CreateConfig(strRef, nSlotId);
    if (piBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    for (IMS_SINT32 i = 0; i < nCount; i++)
    {
        if (!piBuffer->CaptureSection(MediaSessionConfig::KEY_AUDIO, i))
        {
            IMS_TRACE_D("CreateAudioConfigs() - CaptureSection fail", 0, 0, 0);
            piBuffer->Destroy();
            return IMS_FALSE;
        }

        const AString &strSessionType = piBuffer->ReadValue(MediaSessionConfig::KEY_SESSION_TYPE);

        IMS_TRACE_D("CreateAudioConfigs() - type(%s), index(%d)", strSessionType.GetStr(), i, 0);

        AudioConfiguration *pConfig = CreateAudioConfiguration(ConvertSessionType(strSessionType));

        if (pConfig == IMS_NULL)
        {
            piBuffer->Destroy();
            return IMS_FALSE;
        }

        if (!pConfig->Create(piBuffer, i, nSlotId))
        {
            IMS_TRACE_E(0,"Creating an audio(%d) configuration failed", i, 0, 0);

            delete pConfig;
            pConfig = IMS_NULL;
            piBuffer->Destroy();
            return IMS_FALSE;
        }

        if (!objAudioConfigs.Append(pConfig))
        {
            IMS_TRACE_E(0,"Adding an audio(%d) configuration failed", i, 0, 0);

            delete pConfig;
            pConfig = IMS_NULL;
            piBuffer->Destroy();
            return IMS_FALSE;
        }
    }

    piBuffer->Destroy();

    return IMS_TRUE;
}

PROTECTED
IMS_BOOL MediaSessionConfig::CreateVideoConfigs(IN CONST AString &strRef,
        IN IMS_SINT32 nCount, IN IMS_SINT32 nSlotId)
{
    IConfigBuffer *piBuffer = IMS_NULL;

    if (Configuration::GetInstance() == IMS_NULL)
    {
        return IMS_FALSE;
    }

    piBuffer = Configuration::GetInstance()->CreateConfig(strRef, nSlotId);

    if (piBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    for (IMS_SINT32 i = 0; i < nCount; i++)
    {
        if (!piBuffer->CaptureSection(MediaSessionConfig::KEY_VIDEO, i))
        {
            IMS_TRACE_D("CreateVideoConfigs() - CaptureSection fail", 0, 0, 0);
            piBuffer->Destroy();
            return IMS_FALSE;
        }

        const AString &strSessionType = piBuffer->ReadValue(MediaSessionConfig::KEY_SESSION_TYPE);

        IMS_TRACE_I("Video configuration :: type(%s), index(%d)", strSessionType.GetStr(), i, 0);

        VideoConfiguration *pConfig = CreateVideoConfiguration(ConvertSessionType(strSessionType));

        if (pConfig == IMS_NULL)
        {
            piBuffer->Destroy();
            return IMS_FALSE;
        }

        if (!pConfig->Create(piBuffer, i, nSlotId))
        {
            IMS_TRACE_E(0,"Creating a video(%d) configuration failed", i, 0, 0);

            delete pConfig;
            pConfig = IMS_NULL;
            piBuffer->Destroy();
            return IMS_FALSE;
        }

        if (!objVideoConfigs.Append(pConfig))
        {
            IMS_TRACE_E(0,"Adding a video(%d) configuration failed", i, 0, 0);

            delete pConfig;
            pConfig = IMS_NULL;
            piBuffer->Destroy();
            return IMS_FALSE;
        }
    }

    piBuffer->Destroy();

    return IMS_TRUE;
}

PROTECTED
IMS_BOOL MediaSessionConfig::CreateTextConfigs(IN CONST AString &strRef,
        IN IMS_SINT32 nCount, IN IMS_SINT32 nSlotId)
{
    IConfigBuffer *piBuffer =  IMS_NULL;

    if (Configuration::GetInstance() == IMS_NULL)
    {
        return IMS_FALSE;
    }

    piBuffer = Configuration::GetInstance()->CreateConfig(strRef, nSlotId);

    if (piBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    for (IMS_SINT32 i = 0; i < nCount; i++)
    {
        if (!piBuffer->CaptureSection(MediaSessionConfig::KEY_TEXT, i))
        {
            IMS_TRACE_D("CreateTextConfigs() - CaptureSection fail", 0, 0, 0);
            piBuffer->Destroy();
            return IMS_FALSE;
        }

        const AString &strSessionType = piBuffer->ReadValue(MediaSessionConfig::KEY_SESSION_TYPE);

        IMS_TRACE_I("Text configuration :: type(%s), index(%d)", strSessionType.GetStr(), i, 0);

        TextConfiguration *pConfig = CreateTextConfiguration(ConvertSessionType(strSessionType));

        if (pConfig == IMS_NULL)
        {
            piBuffer->Destroy();
            return IMS_FALSE;
        }

        if (!pConfig->Create(piBuffer, i, nSlotId))
        {
            IMS_TRACE_E(0,"Creating a text(%d) configuration failed", i, 0, 0);

            delete pConfig;
            pConfig = IMS_NULL;
            piBuffer->Destroy();
            return IMS_FALSE;
        }

        if (!objTextConfigs.Append(pConfig))
        {
            IMS_TRACE_E(0,"Adding a text(%d) configuration failed", i, 0, 0);

            delete pConfig;
            pConfig = IMS_NULL;
            piBuffer->Destroy();
            return IMS_FALSE;
        }
    }

    piBuffer->Destroy();

    return IMS_TRUE;
}

PROTECTED
IMS_BOOL MediaSessionConfig::UpdateAudioConfigs(IN CONST AString &strRef,
        IN IMS_SINT32 nCount, IN IMS_SINT32 nSlotId)
{
    IConfigBuffer *piBuffer = IMS_NULL;

    if (Configuration::GetInstance() == IMS_NULL)
    {
        return IMS_FALSE;
    }

    piBuffer = Configuration::GetInstance()->CreateConfig(strRef, nSlotId);

    if (piBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    for (IMS_SINT32 i = 0; i < nCount; i++)
    {

        AudioConfiguration *pConfig = objAudioConfigs.GetAt(i);

        if (pConfig == IMS_NULL)
        {
            piBuffer->Destroy();
            return IMS_FALSE;
        }

        if (!pConfig->Update(piBuffer, i))
        {
            IMS_TRACE_E(0,"Creating an audio(%d) configuration failed", i, 0, 0);

            delete pConfig;
            pConfig = IMS_NULL;
            piBuffer->Destroy();
            return IMS_FALSE;
        }
    }

    piBuffer->Destroy();

    return IMS_TRUE;
}

PROTECTED
IMS_BOOL MediaSessionConfig::UpdateVideoConfigs(IN CONST AString &strRef,
        IN IMS_SINT32 nCount, IN IMS_SINT32 nSlotId)
{
    IConfigBuffer *piBuffer = IMS_NULL;

    if (Configuration::GetInstance() == IMS_NULL)
    {
        return IMS_FALSE;
    }

    piBuffer = Configuration::GetInstance()->CreateConfig(strRef, nSlotId);

    if (piBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    for (IMS_SINT32 i = 0; i < nCount; i++)
    {
        VideoConfiguration *pConfig = objVideoConfigs.GetAt(i);

        if (pConfig == IMS_NULL)
        {
            piBuffer->Destroy();
            return IMS_FALSE;
        }

        if (!pConfig->Update(piBuffer, i))
        {
            IMS_TRACE_E(0,"Creating a video(%d) configuration failed", i, 0, 0);

            delete pConfig;
            pConfig = IMS_NULL;
            piBuffer->Destroy();
            return IMS_FALSE;
        }
    }

    piBuffer->Destroy();
    return IMS_TRUE;
}

PROTECTED
IMS_BOOL MediaSessionConfig::UpdateTextConfigs(IN CONST AString &strRef,
        IN IMS_SINT32 nCount, IN IMS_SINT32 nSlotId)
{
    IConfigBuffer *piBuffer = Configuration::GetInstance()->CreateConfig(strRef, nSlotId);

    if (piBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    for (IMS_SINT32 i = 0; i < nCount; i++)
    {
        TextConfiguration *pConfig = objTextConfigs.GetAt(i);

        if (pConfig == IMS_NULL)
        {
            piBuffer->Destroy();
            return IMS_FALSE;
        }

        if (!pConfig->Update(piBuffer, i))
        {
            IMS_TRACE_E(0,"Creating a text(%d) configuration failed", i, 0, 0);

            delete pConfig;

            piBuffer->Destroy();
            return IMS_FALSE;
        }

    }

    piBuffer->Destroy();

    return IMS_TRUE;
}

PRIVATE
MEDIA_CONTENT_TYPE MediaSessionConfig::ConvertSessionType(IN CONST AString& strSessionType)
{
    MEDIA_CONTENT_TYPE nSessionType = MEDIA_TYPE_INVALID;

    if (strSessionType.EqualsIgnoreCase("volte"))
    {
        nSessionType = MEDIA_TYPE_AUDIO;
    }
    else if (strSessionType.EqualsIgnoreCase("vt"))
    {
        nSessionType = MEDIA_TYPE_AUDIOVIDEO;
    }
    else if (strSessionType.EqualsIgnoreCase("vsc"))
    {
        nSessionType = MEDIA_TYPE_VIDEO;
    }
    else
    {
        nSessionType = MEDIA_TYPE_INVALID;
        IMS_TRACE_E(0,"Invalid Session Type(%s)", strSessionType.GetStr(), 0, 0);
    }

    return nSessionType;
}

PRIVATE
void MediaSessionConfig::MakeAnalyzerList(IN CONST AString& strList)
{
    nRtpAnalyzerList = 0;
    IMSList<AString> objTokens = strList.Split(',');

    for (IMS_UINT32 i = 0 ; i < objTokens.GetSize(); i++)
    {
        const AString &strAnalyzer = objTokens.GetAt(i);

        if (strAnalyzer.EqualsIgnoreCase("rtp") == IMS_TRUE)
        {
            nRtpAnalyzerList += (IMS_SINT32)RTP_ANALYZER_RTP;
        }
        else if (strAnalyzer.EqualsIgnoreCase("dra") == IMS_TRUE)
        {
            nRtpAnalyzerList += (IMS_SINT32)RTP_ANALYZER_DRA;
        }
        else if (strAnalyzer.EqualsIgnoreCase("ciq") == IMS_TRUE)
        {
            nRtpAnalyzerList += (IMS_SINT32)RTP_ANALYZER_CIQ;
        }
        else if (strAnalyzer.EqualsIgnoreCase("hasati") == IMS_TRUE)
        {
            nRtpAnalyzerList += (IMS_SINT32)RTP_ANALYZER_HASATI;
        }
        else if (strAnalyzer.EqualsIgnoreCase("cct") == IMS_TRUE)
        {
            nRtpAnalyzerList += (IMS_SINT32)RTP_ANALYZER_CCT;
        }
        else if (strAnalyzer.EqualsIgnoreCase("ds") == IMS_TRUE)
        {
            nRtpAnalyzerList += (IMS_SINT32)RTP_ANALYZER_DEBUGSCREEN;
        }
        else if (strAnalyzer.EqualsIgnoreCase("datausage") == IMS_TRUE)
        {
            nRtpAnalyzerList += (IMS_SINT32)RTP_ANALYZER_DATAUSAGE;
        }
        else
        {
            IMS_TRACE_D("MakeAnalyzerList() Invalid strAnalyzer index[%d], type[%s]",
                    i, strAnalyzer.GetStr(), 0);
        }
    }

    IMS_TRACE_I("MakeAnalyzerList() nRtpAnalyzerList(%d)", nRtpAnalyzerList, 0, 0);
}

PRIVATE
void MediaSessionConfig::MakeAnalyzerOptionList(IN CONST AString& strList)
{
    if (nRtpAnalyzerList == 0 || strList.GetStr() == AString::ConstNull())
    {
        IMS_TRACE_D("MakeAnalyzerOptionList() Option List is null", 0, 0, 0);
        return;
    }

    if (pRtpAnalyzerOptionList != IMS_NULL)
    {
        return;
    }

    pRtpAnalyzerOptionList = static_cast<tRTP_ANALYZER_OPTION*>(IMS_MEM_Malloc(
                sizeof(tRTP_ANALYZER_OPTION)));
    IMS_MEM_Memset(pRtpAnalyzerOptionList, 0, sizeof(tRTP_ANALYZER_OPTION));

    IMSList<AString> objTokens = strList.Split(TextParser::CHAR_COMMA);

    for (IMS_UINT32 i = 0 ; i < objTokens.GetSize(); i++)
    {
        const AString &objOptions = objTokens.GetAt(i);

        if (objOptions.Contains(TextParser::CHAR_UNDERSCORE) == IMS_FALSE)
        {
            IMS_TRACE_D("MakeAnalyzerOptionList() Option[%s] doesn't have a underscore, '_' ...",
                    objOptions.GetStr(), 0, 0);
            continue;
        }

        IMSList<AString> strAnalyzerOption = objOptions.Split(TextParser::CHAR_UNDERSCORE);

        IMS_TRACE_I("MakeAnalyzerOptionList() Analyzer[%s] nAnalyzerOption[%d]",
                strAnalyzerOption.GetAt(0).GetStr(), strAnalyzerOption.GetAt(1).ToInt32(), 0);

        if (strAnalyzerOption.GetAt(0).EqualsIgnoreCase("ldb") == IMS_TRUE)
        {
            pRtpAnalyzerOptionList->nRtpAnalyzerOptionRTP = strAnalyzerOption.GetAt(1).ToInt32();
        }
        else if (strAnalyzerOption.GetAt(0).EqualsIgnoreCase("dra") == IMS_TRUE)
        {
            pRtpAnalyzerOptionList->nRtpAnalyzerOptionDRA = strAnalyzerOption.GetAt(1).ToInt32();
        }
        else if (strAnalyzerOption.GetAt(0).EqualsIgnoreCase("ciq") == IMS_TRUE)
        {
            pRtpAnalyzerOptionList->nRtpAnalyzerOptionCIQ = strAnalyzerOption.GetAt(1).ToInt32();
        }
        else if (strAnalyzerOption.GetAt(0).EqualsIgnoreCase("hasati") == IMS_TRUE)
        {
            pRtpAnalyzerOptionList->nRtpAnalyzerOptionHASATI = strAnalyzerOption.GetAt(1).ToInt32();
        }
        else if (strAnalyzerOption.GetAt(0).EqualsIgnoreCase("cct") == IMS_TRUE)
        {
            pRtpAnalyzerOptionList->nRtpAnalyzerOptionCCT = strAnalyzerOption.GetAt(1).ToInt32();
        }
        else if (strAnalyzerOption.GetAt(0).EqualsIgnoreCase("ds") == IMS_TRUE) // debug screen
        {
            pRtpAnalyzerOptionList->nRtpAnalyzerOptionDEBUGSCREEN =
                    strAnalyzerOption.GetAt(1).ToInt32();
        }
        else
        {
            IMS_TRACE_D("MakeAnalyzerOptionList() Invalid Option index[%d], strAnalyzerOption[%s]",
                    i, objOptions.GetStr(), 0);
        }
    }
}
