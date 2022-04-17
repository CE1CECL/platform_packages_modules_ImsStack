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
#include "config/VideoConfiguration.h"
#include "config/CodecConfig.h"
#include "config/IMSCodec.h"
#include "config/CodecH264Config.h"
#include "config/CodecH263Config.h"

__IMS_TRACE_TAG_USER_DECL__("CONF");

PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_FRAMERATE_MODE[] = "framerate_mode";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_FRAMERATE[] = "framerate";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_MAX_FRAGMENT_UNIT_SIZE[] = "max_fragment_unit_size";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_BITRATE_ADAPT_ENABLE[] = "bitrate_adapt_enable";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_SEND_PERIODIC_SPS_PPS[] = "send_periodic_sps_pps";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_CANDIDATE_PRIORITY[] = "candidate_priority";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_CONF_DISABLE_TX_VIEW[] = "conf_disable_tx_view";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_CONF_USE_LANDSCAPE[] = "conf_use_landscape";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_ENABLE_ONSCREEN_DEBUG_INFO_VIDEO[] =
        "enable_onscreen_debug_info_video";
//add CVO parameter
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_CVO_MODE[] = "cvo_mode";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_CVO_ID[] = "cvo_id";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_MULTITASKING_TYPE[] = "multitasking_type";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_HOLD_TYPE[] = "hold_type";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_PROFILE_LEVEL_NEGOTIATION[] = "profile_level_negotiation";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_CBP_PRIORITY[] = "cbp_priority";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_SUPPORT_LOW_FRAMERATE[] = "support_low_framerate";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_CONF_SURFACE_UPDATE[] = "conf_surface_update";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_AVP_RATE_ADAPTATION_ENABLE[] = "avp_video_adaptation";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_AVP_RATE_ADAPTATION_CYCLE[] = "avp_rate_adaptation_cycle";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_AVP_RATE_ADAPTATION_DEV_RATIO[] =
        "avp_rate_adaptation_dev_ratio";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_AVP_RATE_ADAPTATION_THRESHOLD[] =
        "avp_rate_adaptation_threshold";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_AVP_RATE_ADAPTATION_MIN_BITRATE_RATIO[] =
        "avp_rate_adaptation_min_bitrate_ratio";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_IFRAME_INTERVAL[] = "iframe_interval";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_AVPF_ENABLE[] = "avpf_enable";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_SDP_OFFER_CAPNEG_FOR_AVPF[] = "sdp_offer_capneg_for_avpf";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_AVPF_TRR[] = "avpf_trr";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_AVPF_NACK[] = "avpf_nack";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_AVPF_TMMBR[] = "avpf_tmmbr";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_AVPF_PLI[] = "avpf_pli";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_AVPF_FIR[] = "avpf_fir";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_AVPF_TMMBR_DOWN_INTERVAL[] = "avpf_tmmbr_down_interval";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_AVPF_TMMBR_UP_INTERVAL[] = "avpf_tmmbr_up_interval";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_AVPF_TMMBR_LOSS_THRESHOLD[] = "avpf_tmmbr_loss_threshold";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_AVPF_TMMBR_MIN_BITRATE_RATIO[] =
        "avpf_tmmbr_min_bitrate_ratio";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_AVPF_TMMBR_BITRATE_LEVEL[] = "avpf_tmmbr_bitrate_level";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_AVPF_TMMBR_UP_LEVEL[] = "avpf_tmmbr_up_level";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_AVPF_TMMBR_ENFORCE_VT_DOWNGRADE[] =
        "avpf_tmmbr_enforce_vt_downgrade";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_VIDEO_MARKER_OPTION[] = "video_marker_option";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_SEND_REDUNDANT_RTCP_SR[] = "send_redundant_rtcp_sr";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_TEST_MODE[] = "test_mode";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_VIDEO_FILE_PATH[] = "video_file_path";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_DUMP_CAMERA[] = "dump_camera";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_DUMP_ENCODER_IN[] = "dump_encoder_in";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_DUMP_ENCODER_OUT[] = "dump_encoder_out";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_DUMP_DECODER_IN[] = "dump_decoder_in";
PRIVATE GLOBAL
const IMS_CHAR VideoConfiguration::KEY_DUMP_DECODER_OUT[] = "dump_decoder_out";

/*!
 * @brief   Creator
 * @details
 */
PUBLIC
VideoConfiguration::VideoConfiguration(IN MEDIA_CONTENT_TYPE _nSessionType) :
        MediaConfiguration(_nSessionType),
        nFramerate_mode(FR_MODE_MANUAL),
        nFramerate(15),
        nMaxFragmentUnitSize(1300),
        nBitrateAdaptEnable(0),
        nEnableOnScreenDebugInfo(0),
        nSendPeriodic_SPS_PPS(0),
        nCandidatePriority(0),
        nConfDisableTxView(0),
        nConfUseLandscape(0),
        nConfSurfaceUpdate(0),
        nCVOmode(0),
        nCVOId(0),
        eMultitaskingType(VIDEO_MULTI_INVALID),
        eHoldType(VIDEO_HOLD_INVALID),
        nProfileLevelNegotiation(0),
        nCBPPriority(0),
        nSupportLowFramerate(0),
        nAVP_RateAdaptaion_Enable(0),
        nAVP_RateAdaptaion_Cycle(0),
        nAVP_RateAdaptaion_DevRatio(0),
        nAVP_RateAdaptaion_Threshold(0),
        nAVP_RateAdaptaion_MinBitrateRatio(0),
        nIframeInterval(1),
        nAVPFEnable(0),
        nSdpOfferCapNegForAVPF(0),
        nAVPF_TRR(0),
        nAVPF_NACK(0),
        nAVPF_TMMBR(0),
        nAVPF_PLI(0),
        nAVPF_FIR(0),
        nAVPF_TMMBR_DownInterval(5),
        nAVPF_TMMBR_UpInterval(10),
        nAVPF_TMMBR_LossThreshold(5),
        nAVPF_TMMBR_MinBitrateRatio(40),
        nAVPF_TMMBR_BitrateLevel(5),
        nAVPF_TMMBR_UpLevel(5),
        nAVPF_TMMBR_EnforceVTDowngrade(0),
        nVideoMarkerOption(0),
        nSendRedundantRTCPSR(0),
        nVideoTestMode(0),
        nVideoDumpCamera(0),
        nVideoDumpEncoderIn(0),
        nVideoDumpEncoderOut(0),
        nVideoDumpDecoderIn(0),
        nVideoDumpDecoderOut(0),
        nHEVCCodecEnable(0)
{
}

/*!
 * @brief   Destructor
 * @details
 */
PUBLIC VIRTUAL
VideoConfiguration::~VideoConfiguration()
{
    Clear();
}

/*!
 * @brief   Create
 * @details
 */
PUBLIC VIRTUAL
IMS_BOOL VideoConfiguration::Create(IN IConfigBuffer *piBuffer, IN IMS_UINT32 nIndex,
        IN IMS_SINT32 nSlotID)
{
    if (piBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    if (!piBuffer->CaptureSection(MediaSessionConfig::KEY_VIDEO, nIndex))
    {
        return IMS_FALSE;
    }

    if (!MediaConfiguration::Create(piBuffer, nIndex))
    {
        piBuffer->ReleaseSection();
        return IMS_FALSE;
    }

    // video attributes
    nFramerate_mode = piBuffer->ReadValueInt(KEY_FRAMERATE_MODE);
    nFramerate = piBuffer->ReadValueInt(KEY_FRAMERATE);

    if (piBuffer->ReadValueInt(KEY_MAX_FRAGMENT_UNIT_SIZE) > 0)
        nMaxFragmentUnitSize = piBuffer->ReadValueInt(KEY_MAX_FRAGMENT_UNIT_SIZE);

    nBitrateAdaptEnable = piBuffer->ReadValueInt(KEY_BITRATE_ADAPT_ENABLE);
    nEnableOnScreenDebugInfo = piBuffer->ReadValueInt(KEY_ENABLE_ONSCREEN_DEBUG_INFO_VIDEO);
    nSendPeriodic_SPS_PPS = piBuffer->ReadValueInt(KEY_SEND_PERIODIC_SPS_PPS);
    nCandidatePriority =  piBuffer->ReadValueInt(KEY_CANDIDATE_PRIORITY);
    //for conference
    nConfDisableTxView = piBuffer->ReadValueInt(KEY_CONF_DISABLE_TX_VIEW);
    nConfUseLandscape = piBuffer->ReadValueInt(KEY_CONF_USE_LANDSCAPE);
    nConfSurfaceUpdate = piBuffer->ReadValueInt(KEY_CONF_SURFACE_UPDATE);

    // get nProfileLevelNegotiation, cbp priority and support low framerate from buffer
    nProfileLevelNegotiation = piBuffer->ReadValueInt(KEY_PROFILE_LEVEL_NEGOTIATION);
    nCBPPriority = piBuffer->ReadValueInt(KEY_CBP_PRIORITY);
    nSupportLowFramerate = piBuffer->ReadValueInt(KEY_SUPPORT_LOW_FRAMERATE);
    // Codec information
    const IMS_SINT32 nCodecListSize = piBuffer->ReadValueInt(
                MediaSessionConfig::KEY_CODEC_LIST_SIZE);
    const AString &strCodecRef = piBuffer->ReadValue(MediaSessionConfig::KEY_CODEC_REF);
    //add CVO parameter
    nCVOmode = piBuffer->ReadValueInt(KEY_CVO_MODE);
    nCVOId = piBuffer->ReadValueInt(KEY_CVO_ID);
    eMultitaskingType = (VIDEO_MULTI_TYPE)piBuffer->ReadValueInt(KEY_MULTITASKING_TYPE);
    eHoldType = (VIDEO_HOLD_TYPE)piBuffer->ReadValueInt(KEY_HOLD_TYPE);

    // add AVP video adaptation check variable
    nAVP_RateAdaptaion_Enable = piBuffer->ReadValueInt(KEY_AVP_RATE_ADAPTATION_ENABLE);
    nAVP_RateAdaptaion_Cycle = piBuffer->ReadValueInt(KEY_AVP_RATE_ADAPTATION_CYCLE);
    nAVP_RateAdaptaion_DevRatio = piBuffer->ReadValueInt(KEY_AVP_RATE_ADAPTATION_DEV_RATIO);
    nAVP_RateAdaptaion_Threshold = piBuffer->ReadValueInt(KEY_AVP_RATE_ADAPTATION_THRESHOLD);
    nAVP_RateAdaptaion_MinBitrateRatio = piBuffer->ReadValueInt(
                KEY_AVP_RATE_ADAPTATION_MIN_BITRATE_RATIO);

    nIframeInterval = piBuffer->ReadValueInt(KEY_IFRAME_INTERVAL);

    // AVPF
    nAVPFEnable = piBuffer->ReadValueInt(KEY_AVPF_ENABLE);

    // add SDP CapNego for AVPF
    nSdpOfferCapNegForAVPF = piBuffer->ReadValueInt(KEY_SDP_OFFER_CAPNEG_FOR_AVPF);
    nAVPF_TRR  = piBuffer->ReadValueInt(KEY_AVPF_TRR);
    nAVPF_NACK = piBuffer->ReadValueInt(KEY_AVPF_NACK);
    nAVPF_TMMBR = piBuffer->ReadValueInt(KEY_AVPF_TMMBR);
    nAVPF_PLI = piBuffer->ReadValueInt(KEY_AVPF_PLI);
    nAVPF_FIR = piBuffer->ReadValueInt(KEY_AVPF_FIR);

    nAVPF_TMMBR_DownInterval = piBuffer->ReadValueInt(KEY_AVPF_TMMBR_DOWN_INTERVAL);
    nAVPF_TMMBR_UpInterval = piBuffer->ReadValueInt(KEY_AVPF_TMMBR_UP_INTERVAL);
    nAVPF_TMMBR_LossThreshold = piBuffer->ReadValueInt(KEY_AVPF_TMMBR_LOSS_THRESHOLD);
    nAVPF_TMMBR_MinBitrateRatio = piBuffer->ReadValueInt(KEY_AVPF_TMMBR_MIN_BITRATE_RATIO);
    nAVPF_TMMBR_BitrateLevel = piBuffer->ReadValueInt(KEY_AVPF_TMMBR_BITRATE_LEVEL);
    nAVPF_TMMBR_UpLevel = piBuffer->ReadValueInt(KEY_AVPF_TMMBR_UP_LEVEL);
    nAVPF_TMMBR_EnforceVTDowngrade = piBuffer->ReadValueInt(KEY_AVPF_TMMBR_ENFORCE_VT_DOWNGRADE);
    nVideoMarkerOption = piBuffer->ReadValueInt(KEY_VIDEO_MARKER_OPTION);

    if (nVideoMarkerOption < 0)
        nVideoMarkerOption = 0;

    nSendRedundantRTCPSR = piBuffer->ReadValueInt(KEY_SEND_REDUNDANT_RTCP_SR);

    //test video file
    nVideoTestMode = piBuffer->ReadValueInt(KEY_TEST_MODE);
    strVideoFilePath = piBuffer->ReadValue(KEY_VIDEO_FILE_PATH);

    nVideoDumpCamera = piBuffer->ReadValueInt(KEY_DUMP_CAMERA);
    nVideoDumpEncoderIn = piBuffer->ReadValueInt(KEY_DUMP_ENCODER_IN);
    nVideoDumpEncoderOut = piBuffer->ReadValueInt(KEY_DUMP_ENCODER_OUT);
    nVideoDumpDecoderIn = piBuffer->ReadValueInt(KEY_DUMP_DECODER_IN);
    nVideoDumpDecoderOut = piBuffer->ReadValueInt(KEY_DUMP_DECODER_OUT);

    Clear();

    // TODO: use carrier-config
    //IFeatureUtil* iFeatureUtil = UtilService::GetUtilService()->GetFeatureUtil();
    //nHEVCCodecEnable = iFeatureUtil->IsFeatureSupported(IFeatureUtil::FEATURE_MEDIA_HEVC);

    // Creates a codec configuration
    if (!CreateCodecConfigs(strCodecRef, nCodecListSize, nSlotID))
    {
        IMS_TRACE_E(0, "Create - CreateCodecConfigs failure [%s]", strCodecRef.GetStr(), 0, 0);
        piBuffer->ReleaseSection();
        return IMS_FALSE;
    }

    piBuffer->ReleaseSection();

    //ToDebugString();

    return IMS_TRUE;
}

/*!
 * @brief   ToDebugString
 * @details Print ToDebugString
 */
PUBLIC VIRTUAL
void VideoConfiguration::ToDebugString() const
{
    //MediaConfiguration::ToDebugString();

    IMS_TRACE_D("frame-rate (%d, %d), periodic sps/pps (%d)",
            nFramerate_mode, nFramerate, nSendPeriodic_SPS_PPS);
    IMS_TRACE_D("candidate (%d), enable DebugInfo On Screen (%d), video marker option(%d)",
            nCandidatePriority, nEnableOnScreenDebugInfo, nVideoMarkerOption);
    IMS_TRACE_D("conf disable tx view (%d), conf use landscape (%d), conf surface update (%d)",
            nConfDisableTxView, nConfUseLandscape, nConfSurfaceUpdate);
    IMS_TRACE_D("CVO mode (%d), CVO ID(%d), nAVP_RateAdaptaion_Enable(%d)",
            nCVOmode, nCVOId, nAVP_RateAdaptaion_Enable);
    IMS_TRACE_D("nAVP_RateAdaptaion_Cycle (%d), nAVP_RateAdaptaion_DevRatio (%d), \
            nAVP_RateAdaptaion_Threshold (%d)",
            nAVP_RateAdaptaion_Cycle, nAVP_RateAdaptaion_DevRatio, nAVP_RateAdaptaion_Threshold);
    IMS_TRACE_D("nAVP_RateAdaptaion_MinBitrateRatio (%d)",
            nAVP_RateAdaptaion_MinBitrateRatio, 0, 0);
    IMS_TRACE_D("Iframe Interval(%d), nBitrateAdaptEnable(%d)",
            nIframeInterval, nBitrateAdaptEnable, 0);
    IMS_TRACE_D("avpf_enable (%d), avpf_nack (%d), avpf_tmmbr (%d)",
            nAVPFEnable, nAVPF_NACK, nAVPF_TMMBR);
    IMS_TRACE_D("avpf_trr-int(%d)", nAVPF_TRR, 0, 0);
    IMS_TRACE_D("avpf_pli (%d), avpf_fir (%d), CapaNego[%d]",
            nAVPF_PLI, nAVPF_FIR, nSdpOfferCapNegForAVPF);
    IMS_TRACE_D("tmmbr_Tdown (%d), tmmbr_Tup (%d), tmmbr_loss (%d)",
            nAVPF_TMMBR_DownInterval, nAVPF_TMMBR_UpInterval, nAVPF_TMMBR_LossThreshold);
    IMS_TRACE_D("tmmbr_minrate (%d), tmmbr_level (%d), tmmbr_up_level (%d)",
            nAVPF_TMMBR_MinBitrateRatio, nAVPF_TMMBR_BitrateLevel, nAVPF_TMMBR_UpLevel);
    IMS_TRACE_D("nVideoMarkerOption (%d), nSendRedundantRTCPSR(%d), nVideoTestMode (%d)",
            nVideoMarkerOption, nSendRedundantRTCPSR, nVideoTestMode);
    IMS_TRACE_D("nVideoDumpCamera (%d), nVideoDumpEncoderIn (%d), nVideoDumpEncoderOut (%d)",
            nVideoDumpCamera, nVideoDumpEncoderIn, nVideoDumpEncoderOut);

    /*for (IMS_UINT32 i = 0; i < objCodecConfigs.GetSize(); ++i)
    {
        CodecConfig *pCodecConfig = objCodecConfigs.GetAt(i);

        if (pCodecConfig != IMS_NULL)
        {
            pCodecConfig->ToDebugString();
        }
    }*/
}

/*!
 * @brief   GetFramerate_Mode
 * @details
 */
IMS_SINT32 VideoConfiguration::GetFramerate_Mode() const
{
    // 0 : Hide, 1 : Auto, 2: Manual
    return nFramerate_mode;
}

/*!
 * @brief   GetFramerate
 * @details
 */
IMS_SINT32 VideoConfiguration::GetFramerate() const
{
    return nFramerate;
}

/*!
 * @brief   GetMaxFragmentUnitSize
 * @details
 */
IMS_SINT32 VideoConfiguration::GetMaxFragmentUnitSize() const
{
    return nMaxFragmentUnitSize;
}

/*!
 * @brief   GetBitrateAdaptEnable
 * @details
 */
IMS_BOOL VideoConfiguration::GetBitrateAdaptEnable() const
{
    return (nBitrateAdaptEnable == 1);
}

/*!
 * @brief   GetEnableOnScreenDebugInfo
 * @details
 */
IMS_SINT32 VideoConfiguration::GetEnableOnScreenDebugInfo() const
{
    return nEnableOnScreenDebugInfo;
}

/*!
 * @brief   GetSendPeriodicSPSPPS
 * @details
 */
IMS_SINT32 VideoConfiguration::GetSendPeriodicSPSPPS() const
{
    return nSendPeriodic_SPS_PPS;
}

/*!
 * @brief   GetCandidatePriority
 * @details
 */
IMS_SINT32 VideoConfiguration::GetCandidatePriority() const
{
    return nCandidatePriority;
}

/*!
 * @brief   GetConfDisableTxView
 * @details
 */
IMS_BOOL VideoConfiguration::GetConfDisableTxView() const
{
    return (nConfDisableTxView == 1);
}

/*!
 * @brief   GetConfUseLandscape
 * @details
 */
IMS_BOOL VideoConfiguration::GetConfUseLandscape() const
{
    return (nConfUseLandscape == 1);
}

/*!
 * @brief   GetConfSurfaceUpdate
 * @details
 */
IMS_BOOL VideoConfiguration::GetConfSurfaceUpdate() const
{
    return (nConfSurfaceUpdate == 1);
}

/*!
 * @brief   GetCodecConfig
 * @details Get GetCodecConfig
 */
PUBLIC
const CodecConfig* VideoConfiguration::GetCodecConfig() const
{
    if (objCodecConfigs.IsEmpty())
    {
        return IMS_NULL;
    }

    return objCodecConfigs.GetAt(0);
}

/*!
 * @brief   GetCodecConfigs
 * @details Get GetCodecConfigs
 */
PUBLIC
const IMSList<CodecConfig*>& VideoConfiguration::GetCodecConfigs() const
{
    return objCodecConfigs;
}

/*!
 * @brief   GetCVOMode
 * @details
 */
PUBLIC
IMS_SINT32 VideoConfiguration::GetCVOMode() const
{
    return nCVOmode;
}

/*!
 * @brief   GetCVOId
 * @details
 */
PUBLIC
IMS_SINT32 VideoConfiguration::GetCVOId() const
{
    return nCVOId;
}

/*!
 * @brief   GetMultitaskingType
 * @details
 */
PUBLIC
IMS_SINT32 VideoConfiguration::GetMultitaskingType() const
{
    return (IMS_SINT32)eMultitaskingType;
}

/*!
 * @brief   GetHoldType
 * @details
 */
PUBLIC
IMS_SINT32 VideoConfiguration::GetHoldType() const
{
    return (IMS_SINT32)eHoldType;
}

/*!
 * @brief   GetAVP_RateAdaptation_Enable
 * @details
 */
PUBLIC
IMS_BOOL VideoConfiguration::GetAVP_RateAdaptation_Enable() const
{
    return (nAVP_RateAdaptaion_Enable == 1);
}

/*!
 * @brief   GetAVP_RateAdaptation_Cycle
 * @details
 */
PUBLIC
IMS_SINT32 VideoConfiguration::GetAVP_RateAdaptation_Cycle() const
{
    return (nAVP_RateAdaptaion_Cycle >= 1 ? nAVP_RateAdaptaion_Cycle : 0);
}

/*!
 * @brief   GetAVP_RateAdaptation_DevRatio
 * @details
 */
PUBLIC
IMS_SINT32 VideoConfiguration::GetAVP_RateAdaptation_DevRatio() const
{
    return (nAVP_RateAdaptaion_DevRatio >= 1 ? nAVP_RateAdaptaion_DevRatio : 0);
}

/*!
 * @brief   GetAVP_RateAdaptation_Threshold
 * @details
 */
PUBLIC
IMS_SINT32 VideoConfiguration::GetAVP_RateAdaptation_Threshold() const
{
    return (nAVP_RateAdaptaion_Threshold >= 1 ? nAVP_RateAdaptaion_Threshold : 0);
}

/*!
* @brief   GetAVP_RateAdaptation_MinBitrateRatio
* @details
*/
PUBLIC
IMS_SINT32 VideoConfiguration::GetAVP_RateAdaptation_MinBitrateRatio() const
{
    return (nAVP_RateAdaptaion_MinBitrateRatio >= 1 ? nAVP_RateAdaptaion_MinBitrateRatio : 0);
}

/*!
 * @brief   GetIframeInterval
 * @details
 */
PUBLIC
IMS_SINT32 VideoConfiguration::GetIframeInterval() const
{
    return nIframeInterval;
}

/*!
 * @brief   GetAVPF
 * @details Get AVPF enable
 */
PUBLIC
IMS_BOOL VideoConfiguration::GetAVPF() const
{
    return (nAVPFEnable == 1);
}

/*!
 * @brief   GetSdpOfferCapNegForAVPF
 * @details Get SDPOfferCapNegForAVPF enable
 */
PUBLIC
IMS_SINT32 VideoConfiguration::GetSdpOfferCapNegForAVPF() const
{
    return nSdpOfferCapNegForAVPF;
}

/*!
 * @brief   GetAVPF_TRR
 * @details Get Get avpf trr-int enable
 */
PUBLIC
IMS_BOOL VideoConfiguration::GetAVPF_TRR() const
{
    return (nAVPF_TRR == 1);
}

/*!
 * @brief   GetAVPF_NACK
 * @details Get avpf NACK attribute enabled
 */
PUBLIC
IMS_BOOL VideoConfiguration::GetAVPF_NACK() const
{
    return (nAVPF_NACK == 1);
}

/*!
 * @brief   GetAVPF_TMMBR
 * @details Get avpf TMMBR attribute enabled
 */
PUBLIC
IMS_BOOL VideoConfiguration::GetAVPF_TMMBR() const
{
    return (nAVPF_TMMBR == 1);
}

/*!
 * @brief   GetAVPF_PLI
 * @details Get avpf PLI attribute enabled
 */
PUBLIC
IMS_BOOL VideoConfiguration::GetAVPF_PLI() const
{
    return (nAVPF_PLI == 1);
}

/*!
 * @brief   GetAVPF_FIR
 * @details Get avpf FIR attribute enabled
 */
PUBLIC
IMS_BOOL VideoConfiguration::GetAVPF_FIR() const
{
    return (nAVPF_FIR == 1);
}

/*!
 * @brief   GetAVPF_TMMBR_DownInterval
 * @details Get avpf TMMBR Time interval to determine downward
 */
PUBLIC
IMS_SINT32 VideoConfiguration::GetAVPF_TMMBR_DownInterval() const
{
    return nAVPF_TMMBR_DownInterval;
}

/*!
 * @brief   GetAVPF_TMMBR_UpInterval
 * @details Get avpf TMMBR Time interval to determine upward
 */
PUBLIC
IMS_SINT32 VideoConfiguration::GetAVPF_TMMBR_UpInterval() const
{
    return nAVPF_TMMBR_UpInterval;
}

/*!
 * @brief   GetAVPF_TMMBR_LossThreshold
 * @details Get avpf TMMBR Threshold of loss rate to cause TMMBR
 */
PUBLIC
IMS_SINT32 VideoConfiguration::GetAVPF_TMMBR_LossThreshold() const
{
    return nAVPF_TMMBR_LossThreshold;
}

/*!
 * @brief   GetAVPF_TMMBR_MinBitrateRatio
 * @details Get avpf TMMBR Minimum threshold of bitrate
 */
PUBLIC
IMS_SINT32 VideoConfiguration::GetAVPF_TMMBR_MinBitrateRatio() const
{
    return nAVPF_TMMBR_MinBitrateRatio;
}

/*!
 * @brief   GetAVPF_TMMBR_BitrateLevel
 * @details Get avpf TMMBR Level of bitrate change (-> n+1 step of bitrate in total)
 */
PUBLIC
IMS_SINT32 VideoConfiguration::GetAVPF_TMMBR_BitrateLevel() const
{
    return nAVPF_TMMBR_BitrateLevel;
}

/*!
 * @brief   GetAVPF_TMMBR_MinBitrateRatio
 * @details Get avpf TMMBR Minimum threshold of bitrate
 */
PUBLIC
IMS_SINT32 VideoConfiguration::GetAVPF_TMMBR_UpLevel() const
{
    return nAVPF_TMMBR_UpLevel;
}

/*!
 * @brief   GetAVPF_TMMBR_EnforceVTDowngrade
 * @details Get avpf TMMBR enforce vt downgrade
 */
PUBLIC
IMS_SINT32 VideoConfiguration::GetAVPF_TMMBR_EnforceVTDowngrade() const
{
    return nAVPF_TMMBR_EnforceVTDowngrade;
}

/*!
 * @brief   GetProfileLevelNegotiation
 * @details get profile level negotiation, adjust codec profile and level to dest payload
 */
IMS_BOOL VideoConfiguration::GetProfileLevelNegotiation() const
{
    return (nProfileLevelNegotiation == 1);
}

/*!
 * @brief   GetCBPPriority
 * @details
 */
IMS_BOOL VideoConfiguration::GetCBPPriority() const
{
    return (nCBPPriority == 1);
}

/*!
 * @brief   GetSupportLowFramerate
 * @details
 */
IMS_BOOL VideoConfiguration::GetSupportLowFramerate() const
{
    return (nSupportLowFramerate == 1);
}

/*!
 * @brief   GetVideoMarkerOption
 * @details
0: (Default)

1: [MMPFJitterBuffer_Video::Add] line 1510 - "else if (!(m_nVideoMarkerOption & 0x01))"
    Do not make currEntry.bHeader to FALSE by checking timestamp
    Enable this check as default, Disable only in case (m_nVideoMarkerOption & 0x01 == TRUE)

2: [MMPFNode_VideoDecoder::ProcessData] line 264 - "if ((m_Property.GetVideoMarkerOption()) & 0x02)"
    Do check "bMark" to separate SPS/PPS/I-frame in a packet
    Disable this check as default, Enable only in case (m_nVideoMarkerOption & 0x02 == TRUE)
    -- 2016.06.13. Not used anymore. Always 'break' when bMark is TRUE
       (with modification on SPS/PPS handling on RTP_PH_Decoder)

4: [MMPFNode_VideoDecoder::ProcessData] line 311 - "if (m_Property.GetVideoMarkerOption() & 0x04)"
    Do no process P-frame until next I-frame once there's one or more packet loss found(DCM Request)
 */
PUBLIC
IMS_SINT32 VideoConfiguration::GetVideoMarkerOption() const
{
    return nVideoMarkerOption;
}

/*!
 *  @brief      IsSendRedundantRTCPSR
 *  @details    RFC 3550 6.4 Sender and Receiver Reports
                     The SR is issued if a site has sent any data packets during the interval
                     since issuing the last report or the previous one,
                     otherwise the RR is issued.
                     If value is 1, send 1 more redundant SR.
                     If value is 0, send RR.
 */
PUBLIC
IMS_BOOL VideoConfiguration::IsSendRedundantRTCPSR() const
{
    return (nSendRedundantRTCPSR == 1);
}

/*!
 *  @brief      GetVideoFilePath
 *  @details    Getting video file path for testing video stream
 */
PUBLIC
IMS_BOOL VideoConfiguration::GetVideoTestMode() const
{
    return (nVideoTestMode == 1);
}

/*!
 *  @brief      GetVideoFilePath
 *  @details    Getting video file path for testing video stream
 */
PUBLIC
AString VideoConfiguration::GetVideoFilePath() const
{
    return strVideoFilePath;
}

/*!
 *  @brief      GetVideoDumpOption
 *  @details    Getting video dump option
 */
PUBLIC
IMS_UINT32 VideoConfiguration::GetVideoDumpOption() const
{
    IMS_UINT32 nDump = 0;
    nDump |= (nVideoDumpCamera == 1);
    nDump = (nDump << 1);
    nDump |= (nVideoDumpEncoderIn == 1);
    nDump = (nDump << 1);
    nDump |= (nVideoDumpEncoderOut == 1);
    nDump = (nDump << 1);
    nDump |= (nVideoDumpDecoderIn == 1);
    nDump = (nDump << 1);
    nDump |= (nVideoDumpDecoderOut == 1);

    return nDump;
}

/*!
 * @brief   Update
 * @details Update the audio/video configurations
 */
PUBLIC
IMS_BOOL VideoConfiguration::Update(IN IConfigBuffer *piBuffer, IN IMS_UINT32 nIndex)
{
    if (piBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    if (!piBuffer->CaptureSection(MediaSessionConfig::KEY_VIDEO, nIndex))
    {
        return IMS_FALSE;
    }

    // Remove writing code to DB
    /*
    // RTP / RTCP port numbers
    //    piBuffer->WriteValueInt(MediaSessionConfig::KEY_PORT_RTP, nPortRtp);
    //    piBuffer->WriteValueInt(MediaSessionConfig::KEY_PORT_RTCP, nPortRtcp);

    // Common attributes
    piBuffer->WriteValueInt(KEY_FRAMERATE, nFramerate);

    // Codec information
    //3 Codec Update ?

    piBuffer->WriteToMedium();
    */

    // AV Config update
    MediaConfiguration::Update(piBuffer, nIndex);

    piBuffer->ReleaseSection();

    return IMS_TRUE;
}

/*!
 * @brief   Clear
 * @details
 */
PROTECTED
void VideoConfiguration::Clear()
{
    for (IMS_UINT32 i = 0; i < objCodecConfigs.GetSize(); ++i)
    {
        CodecConfig *pCodecConfig = objCodecConfigs.GetAt(i);

        if (pCodecConfig != IMS_NULL)
        {
            delete pCodecConfig;
            pCodecConfig = IMS_NULL;
        }
    }

    objCodecConfigs.Clear();
}

/*!
 * @brief   CreateCodecConfigs
 * @details
 */

PROTECTED
IMS_BOOL VideoConfiguration::CreateCodecConfigs(IN CONST AString &strRef,
        IN CONST IMS_SINT32 nCodecListSize, IN IMS_SINT32 nSlotID)
{
    IConfigBuffer *piBuffer = IMS_NULL;

    if (Configuration::GetInstance() == IMS_NULL)
    {
        return IMS_FALSE;
    }

    piBuffer = Configuration::GetInstance()->CreateConfig(strRef, nSlotID);
    if (piBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    for (IMS_SINT32 nIndex = 0; nIndex < nCodecListSize; ++nIndex)
    {
        if (!piBuffer->CaptureSection(MediaSessionConfig::KEY_VIDEO_CODEC, nIndex))
        {
            piBuffer->Destroy();
            return IMS_FALSE;
        }

        const AString &strCodecName = piBuffer->ReadValue(MediaSessionConfig::KEY_CODEC_TYPE);

        IMS_TRACE_D("CodecConfig video - session_type[%d], index[%d], codec[%s]",
                GetSessionType(), nIndex, strCodecName.GetStr());

        if (strCodecName.EqualsIgnoreCase("None") || strCodecName.Equals(""))
        {
            continue;
        }

        CodecConfig *pCodecConfig = CodecConfigFactory::CreateVCodecConfig(strCodecName, nIndex);

        if (pCodecConfig == IMS_NULL)
        {
            IMS_TRACE_E(0, "CodecConfig video - pCodecConfig is NULL", 0, 0, 0);
            piBuffer->Destroy();
            return IMS_FALSE;
        }

        if (!pCodecConfig->Create(piBuffer))
        {
            IMS_TRACE_E(0, "CodecConfig video - Create failure", 0, 0, 0);
            piBuffer->Destroy();

            delete pCodecConfig;
            pCodecConfig = NULL;
            return IMS_FALSE;
        }

        // Chipset Feature
        if (nHEVCCodecEnable == IMS_FALSE)
        {
            IMS_SINT32 nCodecType = IMSCodec::VtoType(pCodecConfig->GetCodecName());
            if (nCodecType == IMSCodec::VIDEO_H265)
            {
                IMS_TRACE_D("CreateCodecConfigs - Remove[Payload Num : %d, Name : VIDEO_H265]",
                    pCodecConfig->GetProfileNum(), 0, 0);
                delete pCodecConfig;
                pCodecConfig = IMS_NULL;
                continue;
            }
        }

        //GPRI Codec Priority
        {
            IMS_SINT32 nCodecType = IMSCodec::VtoType(pCodecConfig->GetCodecName());
            IMS_SINT32 nResolution = 0;
            eCodecNameList eCodecType = CODEC_NONE;

            switch (nCodecType)
            {
                case IMSCodec::VIDEO_H264:
                {
                    CodecH264Config *pH264Config = IMS_NULL;
                    pH264Config = REINTERPRET_CAST(CodecH264Config* , pCodecConfig);

                    if (pH264Config != IMS_NULL)
                    {
                        nResolution = pH264Config->GetResolutionId();
                    }

                    IMS_TRACE_D("CreateCodecConfig H264 ResolutionId[%d]", nResolution,0,0);
                    if (nResolution == DB_RESOLUTION_QCIF_PR)
                    {
                        eCodecType = CODEC_H264_QCIF_PR;
                    }
                    else if (nResolution == DB_RESOLUTION_QCIF_LS)
                    {
                        eCodecType = CODEC_H264_QCIF_LS;
                    }
                    else if (nResolution == DB_RESOLUTION_QVGA_PR)
                    {
                        eCodecType = CODEC_H264_QVGA_PR;
                    }
                    else if (nResolution == DB_RESOLUTION_QVGA_LS)
                    {
                        eCodecType = CODEC_H264_QVGA_LS;
                    }
                    else if (nResolution == DB_RESOLUTION_VGA_LS)
                    {
                        eCodecType = CODEC_H264_VGA_LS;
                    }
                    else if (nResolution == DB_RESOLUTION_VGA_PR)
                    {
                        eCodecType = CODEC_H264_VGA_PR;
                    }
                    else if (nResolution == DB_RESOLUTION_HD_LS)
                    {
                        eCodecType = CODEC_H264_HD_LS;
                    }
                    else if (nResolution == DB_RESOLUTION_HD_PR)
                    {
                        eCodecType = CODEC_H264_HD_PR;
                    }
                    else
                    {
                        eCodecType = CODEC_NONE;
                    }

                    break;
                }
                case IMSCodec::VIDEO_H263:
                {
                    CodecH263Config *pH263Config = IMS_NULL;
                    pH263Config = REINTERPRET_CAST(CodecH263Config*, pCodecConfig);

                    if (pH263Config != IMS_NULL)
                    {
                        nResolution = pH263Config->GetResolutionId();
                    }

                    IMS_TRACE_D("CreateCodecConfig H263 ResolutionId[%d]", nResolution,0,0);

                    if (nResolution == DB_RESOLUTION_QCIF_PR)
                    {
                        eCodecType = CODEC_H263_QCIF_PR;
                    }
                    else if (nResolution == DB_RESOLUTION_QCIF_LS)
                    {
                        eCodecType = CODEC_H263_QCIF_LS;
                    }
                    else
                    {
                        eCodecType = CODEC_NONE;
                    }

                    break;
                }
                default:
                {
                    eCodecType = CODEC_NONE;
                    break;
                }
            }

            IMS_TRACE_D("CreateCodecConfig eCodecType[%d]", eCodecType,0,0);

            if (eCodecType < CODEC_NONE)
            {
                for (IMS_UINT32 i = 0; i < listCodecPriority.GetSize(); i++)
                {
                    if (listCodecPriority.GetAt(i) == eCodecType)
                    {
                        IMS_TRACE_D("CreateCodecConfig SetPriority pPayloadNum[%d], Priority[%d]",
                                pCodecConfig->GetPayloadType(),i,0);
                        pCodecConfig->SetPriority((IMS_UINT32) i);
                        break;
                    }
                    else
                    {
                        IMS_TRACE_D("CreateCodecConfig Skip pPayloadNum[%d], \
                                listCodecPriority.GetAt[%d]: [%d]",
                                pCodecConfig->GetPayloadType(),i,listCodecPriority.GetAt(i));
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
