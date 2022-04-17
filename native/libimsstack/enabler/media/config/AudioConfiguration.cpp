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
#include "TextParser.h"
#include "IConfigBuffer.h"
#include "Configuration.h"
#include "config/MediaSessionConfig.h"
#include "config/CodecConfigFactory.h"
#include "config/AudioConfiguration.h"
#include "config/IMSCodec.h"
#include "config/CodecTelephoneEventConfigEx.h"
#include "config/CodecAMRConfigEx.h"
#include "config/CodecEVSConfig.h"
#include "mmpf/MMPFBoardConfigInfo.h"

__IMS_TRACE_TAG_USER_DECL__("CONF");

PRIVATE GLOBAL
const IMS_CHAR AudioConfiguration::KEY_PTIME[] = "ptime";
PRIVATE GLOBAL
const IMS_CHAR AudioConfiguration::KEY_MAXPTIME[] = "maxptime";
PRIVATE GLOBAL
const IMS_CHAR AudioConfiguration::KEY_CANDIDATE_PRIORITY[] = "candidate_priority";
PRIVATE GLOBAL
const IMS_CHAR AudioConfiguration::KEY_REARRANGE_MODE_SET_BY_AS[] = "rearrange_mode_set_by_as";
PRIVATE GLOBAL
const IMS_CHAR AudioConfiguration::KEY_VOCODER_INTERFACE[] = "vocoder_interface";
PRIVATE GLOBAL
const IMS_CHAR AudioConfiguration::KEY_DM_OPERATION_PREFERRED_MODE[] =
        "dm_operation_preferred_mode";
PRIVATE GLOBAL
const IMS_CHAR AudioConfiguration::KEY_USE_SINGLE_IP[] = "use_single_ip";
PRIVATE GLOBAL
const IMS_CHAR AudioConfiguration::KEY_MODE_CHANGE_CAPA_ALWAYS_ANSWER[] =
        "mode_change_capa_always_answer";

PUBLIC
AudioConfiguration::AudioConfiguration(MEDIA_CONTENT_TYPE eSessionType) :
        MediaConfiguration(eSessionType),
        nPdpProfileNum(-1),
        nPdpProfileNumOf3G(-1),
        nJitterBufferMaxSize(DEFAULT_JITTER_BUFFER_MAX_SIZE),
        nJitterBufferMinSize(DEFAULT_JITTER_BUFFER_MIN_SIZE),
        nJitterBufferSize(DEFAULT_JITTER_BUFFER_SIZE),
        nPacketTime(DEFAULT_PTIME),
        nMaxPacketTime(DEFAULT_MAXPTIME),
        nCandidatePriority(-1),
        nDtmfDuration(DEFAULT_DTMF_DURATION),
        nDtmfRetransmitDuration(DEFAULT_DTMF_RETRANSMIT_DURATION),
        nDtmfInterval(DEFAULT_DTMF_INTERVAL),
        nRearrangeModeSetByAs(1),
        nDmOperationPreferred_mode(0),
        nEvsCodecEnable(0),
        nModeChangeCapaAlwaysAnswer(0),
        nAudioHalfRateMode(0),
        objCodecConfigs(IMSList<CodecConfig*>())
{
    IMS_TRACE_I("+AudioConfiguration eSessionType(%d)", eSessionType, 0, 0);
}

PUBLIC
AudioConfiguration::~AudioConfiguration()
{
    Clear();
}

PUBLIC VIRTUAL
IMS_BOOL AudioConfiguration::Create(IN IConfigBuffer *piBuffer, IN IMS_UINT32 nIndex,
        IN IMS_SINT32 nSlotID)
{
    if (piBuffer == IMS_NULL)
    {
        IMS_TRACE_D("piBuffer == IMS_NULL", 0, 0, 0);
        return IMS_FALSE;
    }

    if (!piBuffer->CaptureSection(MediaSessionConfig::KEY_AUDIO, nIndex))
    {
        IMS_TRACE_D("CaptureSection", 0, 0, 0);
        return IMS_FALSE;
    }

    if (!MediaConfiguration::Create(piBuffer, nIndex))
    {
        IMS_TRACE_D("MediaConfiguration::Create", 0, 0, 0);
        piBuffer->ReleaseSection();
        return IMS_FALSE;
    }

    // Jitter buffer control (initial / min / max)
    const AString &strJitterBufferSize =
            piBuffer->ReadValue(MediaSessionConfig::KEY_JITTER_BUFFER_SIZE);
    IMSList<AString> objTokens = strJitterBufferSize.Split(TextParser::CHAR_COMMA);

    if (!objTokens.IsEmpty())
    {
        nJitterBufferSize = objTokens.GetAt(0).ToInt32();

        if (objTokens.GetSize() > 1)
        {
            nJitterBufferMinSize = objTokens.GetAt(1).ToInt32();

            if (objTokens.GetSize() > 2)
            {
                nJitterBufferMaxSize = objTokens.GetAt(2).ToInt32();
            }
        }
    }

    // DTMF Duration Parameter
    const AString &strDTMFDuration =
            piBuffer->ReadValue(MediaSessionConfig::KEY_TELEPHONE_EVENT_DURATION);
    objTokens = strDTMFDuration.Split(TextParser::CHAR_COMMA);

    if (!objTokens.IsEmpty())
    {
        nDtmfDuration= objTokens.GetAt(0).ToInt32();

        if (objTokens.GetSize() > 1)
        {
            nDtmfRetransmitDuration = objTokens.GetAt(1).ToInt32();

            if (objTokens.GetSize() > 2)
            {
                nDtmfInterval = objTokens.GetAt(2).ToInt32();
            }
        }
    }

    // Common attributes
    nPacketTime = piBuffer->ReadValueInt(KEY_PTIME);
    nMaxPacketTime = piBuffer->ReadValueInt(KEY_MAXPTIME);
    nCandidatePriority =  piBuffer->ReadValueInt(KEY_CANDIDATE_PRIORITY);
    nRearrangeModeSetByAs = piBuffer->ReadValueInt(KEY_REARRANGE_MODE_SET_BY_AS);

    IMS_SINT32 nDmOperation_mode = piBuffer->ReadValueInt(KEY_DM_OPERATION_PREFERRED_MODE);

    if (nDmOperation_mode > 0)
    {
        nDmOperationPreferred_mode = 1;
    }
    else
    {
        nDmOperationPreferred_mode = 0;
    }

    if (nDmOperation_mode == AUDIO_HALFRATE_SETTING)
    {
        nAudioHalfRateMode = 1;
    }
    else
    {
        nAudioHalfRateMode = 0;
    }

    IMS_TRACE_D("Create - nDmOperationPreferred_mode : %d, nAudioHalfRateMode : %d",
            nDmOperationPreferred_mode, nAudioHalfRateMode, 0);

    nModeChangeCapaAlwaysAnswer = piBuffer->ReadValueInt(KEY_MODE_CHANGE_CAPA_ALWAYS_ANSWER);

    // Codec information
    const IMS_SINT32 nCodecListSize =
            piBuffer->ReadValueInt(MediaSessionConfig::KEY_CODEC_LIST_SIZE);
    const AString &strCodecRef = piBuffer->ReadValue(MediaSessionConfig::KEY_CODEC_REF);

    Clear();

    //AP Audio
    //IFeatureUtil* iFeatureUtil = UtilService::GetUtilService()->GetFeatureUtil();
    //nEvsCodecEnable = iFeatureUtil->IsFeatureSupported(IFeatureUtil::FEATURE_MEDIA_EVS);

    // Creates a codec configuration
    if (!CreateCodecConfigs(strCodecRef, nCodecListSize, nSlotID))
    {
        IMS_TRACE_E(0, "Create - CreateCodecConfigs failure [%s]", strCodecRef.GetStr(), 0, 0);
        piBuffer->ReleaseSection();
        return IMS_FALSE;
    }

    piBuffer->ReleaseSection();

    return IMS_TRUE;
}

PUBLIC VIRTUAL
void AudioConfiguration::ToDebugString() const
{
    MediaConfiguration::ToDebugString();

    IMS_TRACE_D("jitter_buffer_size[%d], jitter_buffer_min_size[%d], jitter_buffer_max_size[%d]",
            nJitterBufferSize, nJitterBufferMinSize, nJitterBufferMaxSize);
    IMS_TRACE_D("ptime[%d], maxptime[%d]", nPacketTime, nMaxPacketTime, 0);
    IMS_TRACE_D("DTMFDuration[%d], DTMFRetransmitDuration[%d], DTMFInterval(%d)",
            nDtmfDuration, nDtmfRetransmitDuration, nDtmfInterval);
    IMS_TRACE_D("nCandidatePriority[%d]", nCandidatePriority, 0, 0);
    IMS_TRACE_D("octet-align[%d], nModeChangeCapaAlwaysAnswer(%d)",
            nModeChangeCapaAlwaysAnswer, 0, 0);

    for (IMS_UINT32 i = 0; i < objCodecConfigs.GetSize(); ++i)
    {
        CodecConfig *pCodecConfig = objCodecConfigs.GetAt(i);

        if (pCodecConfig == IMS_NULL)
        {
            continue;
        }

        pCodecConfig->ToDebugString();
    }
}

PUBLIC
IMS_SINT32 AudioConfiguration::GetPdpProfileNum() const
{
    return nPdpProfileNum;
}

PUBLIC
IMS_SINT32 AudioConfiguration::GetPdpProfileNumOf3G() const
{
    return nPdpProfileNumOf3G;
}

PUBLIC
IMS_SINT32 AudioConfiguration::GetJitterBufferMaxSize() const
{
    return nJitterBufferMaxSize;
}

PUBLIC
IMS_SINT32 AudioConfiguration::GetJitterBufferMinSize() const
{
    return nJitterBufferMinSize;
}

PUBLIC
IMS_SINT32 AudioConfiguration::GetJitterBufferSize() const
{
    return nJitterBufferSize;
}

PUBLIC
IMS_SINT32 AudioConfiguration::GetPacketTime() const
{
    return nPacketTime;
}

PUBLIC
IMS_SINT32 AudioConfiguration::GetMaxPacketTime() const
{
    return nMaxPacketTime;
}

PUBLIC
IMS_SINT32 AudioConfiguration::GetCandidatePriority() const
{
    return nCandidatePriority;
}

PUBLIC
IMS_SINT32 AudioConfiguration::GetDTMFDuration() const
{
    return nDtmfDuration;
}

PUBLIC
IMS_SINT32 AudioConfiguration::GetDTMFRetransmitDuration() const
{
    return nDtmfRetransmitDuration;
}

PUBLIC
IMS_SINT32 AudioConfiguration::GetDTMFInterval() const
{
    return nDtmfInterval;
}

PUBLIC
const CodecConfig* AudioConfiguration::GetCodecConfig() const
{
    if (objCodecConfigs.IsEmpty())
        return IMS_NULL;

    return objCodecConfigs.GetAt(0);
}

PUBLIC
const IMSList<CodecConfig*>& AudioConfiguration::GetCodecConfigs() const
{
    return objCodecConfigs;
}

PUBLIC
IMS_BOOL AudioConfiguration::GetRearrangeModeSetByAs() const
{
    return (nRearrangeModeSetByAs == 1);
}

PUBLIC
IMS_SINT32 AudioConfiguration::GetModeChangeCapaAlwaysAnswer() const
{
    return nModeChangeCapaAlwaysAnswer;
}

PUBLIC
IMS_BOOL AudioConfiguration::GetDmOperationPreferredMode() const
{
    return (nDmOperationPreferred_mode == 1);
}

PUBLIC
IMS_BOOL AudioConfiguration::GetAudioHalfRateMode() const
{
    return (nAudioHalfRateMode == 1);
}

PUBLIC
IMS_BOOL AudioConfiguration::Update(IN IConfigBuffer *piBuffer, IN IMS_UINT32 nIndex)
{
    if (piBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    if (!piBuffer->CaptureSection(MediaSessionConfig::KEY_AUDIO, nIndex))
    {
        return IMS_FALSE;
    }

    const AString &strSessionType = piBuffer->ReadValue(MediaSessionConfig::KEY_SESSION_TYPE);

    IMS_TRACE_I("Update - Audio configuration :: type [%s], index[%d]",
            strSessionType.GetStr(), nIndex, 0);

    // Remove writing code to DB
    /*
    // RTP / RTCP port numbers
    //    piBuffer->WriteValueInt(MediaSessionConfig::KEY_PORT_RTP, nPortRtp);
    //    piBuffer->WriteValueInt(MediaSessionConfig::KEY_PORT_RTCP, nPortRtcp);

    // Jitter buffer control
    AString strJitterBufferSize;
    strJitterBufferSize.Sprintf("%d,%d,%d", nJitterBufferSize, nJitterBufferMinSize, nJitterBufferMaxSize);

    piBuffer->WriteValue(MediaSessionConfig::KEY_JITTER_BUFFER_SIZE, strJitterBufferSize);

    // Common attributes
    piBuffer->WriteValueInt(KEY_PTIME, nPacketTime);
    piBuffer->WriteValueInt(KEY_MAXPTIME, nMaxPacketTime);

    piBuffer->WriteToMedium();
    */

    // AV Config update
    MediaConfiguration::Update(piBuffer, nIndex);

    // Codec information
    const IMS_SINT32 nCodecListSize =
            piBuffer->ReadValueInt(MediaSessionConfig::KEY_CODEC_LIST_SIZE);
    const AString &strCodecRef = piBuffer->ReadValue(MediaSessionConfig::KEY_CODEC_REF);

    Clear();

    // Re-Creates a codec configuration
    if (!CreateCodecConfigs(strCodecRef, nCodecListSize))
    {
        IMS_TRACE_E(0, "Create - CreateCodecConfigs failure [%s]", strCodecRef.GetStr(), 0, 0);
        piBuffer->ReleaseSection();
        return IMS_FALSE;
    }

    piBuffer->ReleaseSection();
    return IMS_TRUE;
}

PROTECTED
void AudioConfiguration::Clear()
{
    for (IMS_UINT32 i = 0; i < objCodecConfigs.GetSize(); ++i)
    {
        CodecConfig* pCodecConfig = objCodecConfigs.GetAt(i);

        if (pCodecConfig)
        {
            delete pCodecConfig;
        }
    }
    objCodecConfigs.Clear();
}

PROTECTED
IMS_BOOL AudioConfiguration::CreateCodecConfigs(IN CONST AString &strRef,
        IN CONST IMS_SINT32 nCodecListSize, IN IMS_SINT32 nSlotID)
{
    IConfigBuffer *piBuffer = Configuration::GetInstance()->CreateConfig(strRef, nSlotID);

    if (piBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    #if defined(IMS_CODEC_AMRWB_DISABLE)
    {
        IMS_TRACE_D("CreateCodecConfigs - IMS_CODEC_AMRWB_DISABLE", 0, 0, 0);
    }
    #endif

    for (IMS_SINT32 nIndex = 0; nIndex < nCodecListSize; ++nIndex)
    {
        if (!piBuffer->CaptureSection(MediaSessionConfig::KEY_AUDIO_CODEC, nIndex))
        {
            piBuffer->Destroy();
            return IMS_FALSE;
        }

        const AString &strCodecName = piBuffer->ReadValue(MediaSessionConfig::KEY_CODEC_TYPE);

        IMS_TRACE_D("CodecConfig audio - session_type[%d], index[%d], codec[%s]",
                GetSessionType(), nIndex, strCodecName.GetStr());

        if (strCodecName.EqualsIgnoreCase("None") || strCodecName.Equals("")) continue;

        CodecConfig *pCodecConfig = CodecConfigFactory::CreateACodecConfig(strCodecName, nIndex);

        if (pCodecConfig == IMS_NULL)
        {
            IMS_TRACE_E(0, "CreateCodecConfigs fail, can't create a codec config[%s]",
                    strCodecName.GetStr(), 0, 0);

            piBuffer->Destroy();
            return IMS_FALSE;
        }

        if (!pCodecConfig->Create(piBuffer))
        {
            IMS_TRACE_E(0, "CreateCodecConfigs fail, Create failure", 0, 0, 0);
            piBuffer->Destroy();
            Clear();
            delete pCodecConfig;
            return IMS_FALSE;
        }

        // Chipset Feature
        if (!MMPFBoardConfigInfo::IsAMRWBCodecSupport())
        {
            IMS_SINT32 nCodecType = IMSCodec::AtoType(pCodecConfig->GetCodecName());
            IMS_BOOL bRemove = IMS_FALSE;
            CodecTelephoneEventConfigEx* pTeleEventConfig = IMS_NULL;
            CodecAMRConfigEx *pAmrConfig = IMS_NULL;
            switch(nCodecType)
            {
                case IMSCodec::AUDIO_AMR:
                {
                    pAmrConfig = REINTERPRET_CAST(CodecAMRConfigEx*, pCodecConfig);
                    if (pAmrConfig->GetSamplingRate() == 16000)
                    {
                        bRemove = IMS_TRUE;
                    }
                }
                break;
                case IMSCodec::AUDIO_AMR_WB:
                {
                    bRemove = IMS_TRUE;
                }
                break;
                case IMSCodec::AUDIO_TELEPHONE_EVENT:
                {
                    pTeleEventConfig = REINTERPRET_CAST(CodecTelephoneEventConfigEx*, pCodecConfig);
                    if (pTeleEventConfig->GetSamplingRate() == 16000)
                    {
                        bRemove = IMS_TRUE;
                    }
                }
                break;
                default:
                {
                    // Do Nothing;
                }
                break;
            }

            if (bRemove)
            {
                if (pCodecConfig != IMS_NULL)
                {
                    IMS_TRACE_D("CreateCodecConfigs - Remove[Payload Num : %d, Name : %s]",
                            pCodecConfig->GetProfileNum(),
                            pCodecConfig->GetCodecName().GetStr(), 0);

                    delete pCodecConfig;
                    pCodecConfig = IMS_NULL;
                }

                continue;
            }
        }

        // Chipset Feature
        if (nEvsCodecEnable == 0)
        {
            IMS_SINT32 nCodecType = IMSCodec::AtoType(pCodecConfig->GetCodecName());
            if (nCodecType == IMSCodec::AUDIO_EVS)
            {
                IMS_TRACE_D("CreateCodecConfigs - Remove[Payload Num : %d, Name : EVS]",
                        pCodecConfig->GetProfileNum(), 0, 0);
                delete pCodecConfig;
                pCodecConfig = IMS_NULL;
                continue;
            }
        }

        //GPRI
        {
            IMS_SINT32 nCodecType = IMSCodec::AtoType(pCodecConfig->GetCodecName());
            IMS_SINT32 nSamplingRate = 0;
            IMS_SINT32 nOctetAlign = 0;
            IMS_SINT32 nBw = 0;

            eCodecNameList eCodecType = CODEC_NONE;

            switch(nCodecType)
            {
                case IMSCodec::AUDIO_AMR_WB:
                case IMSCodec::AUDIO_AMR:
                {
                    CodecAMRConfigEx *pAmrConfig = IMS_NULL;
                    pAmrConfig = REINTERPRET_CAST(CodecAMRConfigEx*, pCodecConfig);

                    if (pAmrConfig != NULL)
                    {
                        nSamplingRate = pAmrConfig->GetSamplingRate();
                        nOctetAlign = pAmrConfig->GetOctetAlign();
                    }

                    if (nSamplingRate == 16000)
                    {
                        eCodecType = CODEC_AMR_WB_BE;

                        if (nOctetAlign == 1)
                        {
                            eCodecType = CODEC_AMR_WB_OA;
                        }
                    }
                    else if (nSamplingRate == 8000)
                    {
                        eCodecType = CODEC_AMR_NB_BE;

                        if (nOctetAlign == 1)
                        {
                            eCodecType = CODEC_AMR_NB_OA;
                        }
                    }
                    break;
                }
                case IMSCodec::AUDIO_EVS:
                {
                    CodecEVSConfig *pEvsConfig = IMS_NULL;
                    pEvsConfig = REINTERPRET_CAST(CodecEVSConfig*, pCodecConfig);

                    eCodecType = CODEC_EVS;

                    if (pEvsConfig != NULL)
                    {
                        nBw = pEvsConfig->GetBwList();

                        if (nBw == (1 << CodecEVSConfig::EVS_BANDWIDTH_NB))
                        {
                            eCodecType = CODEC_EVS_NB;
                        }
                        else if (nBw == (1 << CodecEVSConfig::EVS_BANDWIDTH_WB))
                        {
                            eCodecType = CODEC_EVS_WB;
                        }
                        else if (nBw == (1 << CodecEVSConfig::EVS_BANDWIDTH_SWB))
                        {
                            eCodecType = CODEC_EVS_SWB;
                        }
                        else if (nBw == (1 << CodecEVSConfig::EVS_BANDWIDTH_FB))
                        {
                            eCodecType = CODEC_EVS_FB;
                        }
                    }
                    break;
                }
                default:
                {
                    //Telephone Event, PCM-A, PCM-U
                    eCodecType = CODEC_NONE;
                    break;
                }
            }

            if (eCodecType < CODEC_NONE)
            {
                for (IMS_UINT32 i = 0; i < listCodecPriority.GetSize(); i++)
                {
                    if (listCodecPriority.GetAt(i) == eCodecType)
                    {
                        IMS_TRACE_D("CreateCodecConfig SetPriority pPayloadNum[%d], Priority[%d]",
                                pCodecConfig->GetPayloadType(), i, 0);
                        pCodecConfig->SetPriority((IMS_UINT32) i);
                        break;
                    }
                    else
                    {
                        IMS_TRACE_D("CreateCodecConfig Skip pPayloadNum[%d], \
                                listCodecPriority.GetAt[%d]: [%d]",
                                pCodecConfig->GetPayloadType(), i, listCodecPriority.GetAt(i));
                    }
                }
            }
        }

        IMS_UINT32 nCodecIndex = 0;

        for (nCodecIndex = 0; nCodecIndex < objCodecConfigs.GetSize(); nCodecIndex++)
        {
            CodecConfig* tempConfig = objCodecConfigs.GetAt(nCodecIndex);
            if (tempConfig->GetPriority() > pCodecConfig->GetPriority())
            {
                break;
            }
        }

        if (objCodecConfigs.InsertAt(pCodecConfig,nCodecIndex))
        {
            IMS_TRACE_D("CreateCodecConfig Codec Added pPayloadNum[%d], Index[%d], Priority[%d]",
                    pCodecConfig->GetPayloadType(),nCodecIndex,pCodecConfig->GetPriority());
        }
        else
        {
            IMS_TRACE_E(0, "CreateCodecConfigs fail, InsertAt failure pPayloadNum[%d] Index[%d]",
                    pCodecConfig->GetPayloadType(),nCodecIndex, 0);
            piBuffer->Destroy();
            Clear();

            return IMS_FALSE;
        }
    }

    piBuffer->Destroy();

    return IMS_TRUE;
}
