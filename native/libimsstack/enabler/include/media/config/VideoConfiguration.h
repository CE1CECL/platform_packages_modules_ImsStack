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

#ifndef _VIDEO_CONFIG_H_
#define _VIDEO_CONFIG_H_

#include "config/MediaConfiguration.h"
#include "video/VideoDef.h"
#include "IMSList.h"

class IConfigBuffer;
class CodecConfig;

/*!
 * @class   VideoConfiguration
 * @brief   Video Configuration class
 */
class VideoConfiguration :
        public MediaConfiguration
{
public:
    enum
    {
        FR_MODE_HIDE    = 0,
        FR_MDOE_AUTO    = 1,
        FR_MODE_MANUAL  = 2
    };
    enum
    {
        DB_RESOLUTION_QCIF_PR     = 0,
        DB_RESOLUTION_QVGA_LS     = 1,
        DB_RESOLUTION_QVGA_PR     = 2,
        DB_RESOLUTION_VGA_LS      = 3,
        DB_RESOLUTION_VGA_PR      = 4,
        DB_RESOLUTION_QCIF_LS     = 5,
        DB_RESOLUTION_CIF_LS      = 6,
        DB_RESOLUTION_CIF_PR      = 7,
        DB_RESOLUTION_SQCIF_LS    = 8,
        DB_RESOLUTION_SQCIF_PR    = 9,
        DB_RESOLUTION_SIF_LS      = 10,
        DB_RESOLUTION_SIF_PR      = 11,
        DB_RESOLUTION_HD_LS       = 12,
        DB_RESOLUTION_HD_PR       = 13,
        DB_RESOLUTION_FHD_LS      = 14,
        DB_RESOLUTION_FHD_PR      = 15,
        DB_RESOLUTION_MAX         = 99,
    };
private:
    static const IMS_CHAR KEY_FRAMERATE_MODE[];
    static const IMS_CHAR KEY_FRAMERATE[];
    static const IMS_CHAR KEY_MAX_FRAGMENT_UNIT_SIZE[];
    static const IMS_CHAR KEY_BITRATE_ADAPT_ENABLE[];
    static const IMS_CHAR KEY_SEND_PERIODIC_SPS_PPS[];
    static const IMS_CHAR KEY_CANDIDATE_PRIORITY[];
    static const IMS_CHAR KEY_CONF_DISABLE_TX_VIEW[];
    static const IMS_CHAR KEY_CONF_USE_LANDSCAPE[];
    static const IMS_CHAR KEY_ENABLE_ONSCREEN_DEBUG_INFO_VIDEO[];
    //add CVO parameter
    static const IMS_CHAR KEY_CVO_MODE[];
    static const IMS_CHAR KEY_CVO_ID[];
    static const IMS_CHAR KEY_MULTITASKING_TYPE[];
    static const IMS_CHAR KEY_HOLD_TYPE[];
    //add profile level negotiation,  cbp priority and support low framerate field
    static const IMS_CHAR KEY_PROFILE_LEVEL_NEGOTIATION[];
    static const IMS_CHAR KEY_CBP_PRIORITY[];
    static const IMS_CHAR KEY_SUPPORT_LOW_FRAMERATE[];
    static const IMS_CHAR KEY_CONF_SURFACE_UPDATE[];
    // add avp_video_adaptation check field
    static const IMS_CHAR KEY_AVP_RATE_ADAPTATION_ENABLE[];
    static const IMS_CHAR KEY_AVP_RATE_ADAPTATION_CYCLE[];
    static const IMS_CHAR KEY_AVP_RATE_ADAPTATION_DEV_RATIO[];
    static const IMS_CHAR KEY_AVP_RATE_ADAPTATION_THRESHOLD[];
    static const IMS_CHAR KEY_AVP_RATE_ADAPTATION_MIN_BITRATE_RATIO[];
    static const IMS_CHAR KEY_IFRAME_INTERVAL[];
    // AVPF
    static const IMS_CHAR KEY_AVPF_ENABLE[];
    static const IMS_CHAR KEY_SDP_OFFER_CAPNEG_FOR_AVPF[];
    static const IMS_CHAR KEY_AVPF_TRR[];
    static const IMS_CHAR KEY_AVPF_NACK[];
    static const IMS_CHAR KEY_AVPF_TMMBR[];
    static const IMS_CHAR KEY_AVPF_PLI[];
    static const IMS_CHAR KEY_AVPF_FIR[];
    static const IMS_CHAR KEY_AVPF_TMMBR_DOWN_INTERVAL[];
    static const IMS_CHAR KEY_AVPF_TMMBR_UP_INTERVAL[];
    static const IMS_CHAR KEY_AVPF_TMMBR_LOSS_THRESHOLD[];
    static const IMS_CHAR KEY_AVPF_TMMBR_MIN_BITRATE_RATIO[];
    static const IMS_CHAR KEY_AVPF_TMMBR_BITRATE_LEVEL[];
    static const IMS_CHAR KEY_AVPF_TMMBR_UP_LEVEL[];
    static const IMS_CHAR KEY_AVPF_TMMBR_ENFORCE_VT_DOWNGRADE[];
    static const IMS_CHAR KEY_VIDEO_MARKER_OPTION[];     // For Exception treatment for SKT VT
    static const IMS_CHAR KEY_SEND_REDUNDANT_RTCP_SR[];
    // video test mode
    static const IMS_CHAR KEY_TEST_MODE[];
    static const IMS_CHAR KEY_VIDEO_FILE_PATH[];
    //file dump
    static const IMS_CHAR KEY_DUMP_CAMERA[];
    static const IMS_CHAR KEY_DUMP_ENCODER_IN[];
    static const IMS_CHAR KEY_DUMP_ENCODER_OUT[];
    static const IMS_CHAR KEY_DUMP_DECODER_IN[];
    static const IMS_CHAR KEY_DUMP_DECODER_OUT[];

public:
    VideoConfiguration(IN MEDIA_CONTENT_TYPE _nSessionType);
    virtual ~VideoConfiguration();

private:
    VideoConfiguration(IN CONST VideoConfiguration &objRHS);
    VideoConfiguration& operator=(IN CONST VideoConfiguration &objRHS);

public:
    virtual IMS_BOOL Create(IN IConfigBuffer *piBuffer, IN IMS_UINT32 nIndex,
            IN IMS_SINT32 nSlotID);
    virtual void ToDebugString() const;
    IMS_SINT32 GetFramerate_Mode() const;
    IMS_SINT32 GetFramerate() const;
    IMS_SINT32 GetMaxFragmentUnitSize() const;
    IMS_BOOL GetBitrateAdaptEnable() const;
    IMS_SINT32 GetEnableOnScreenDebugInfo() const;
    IMS_SINT32 GetSendPeriodicSPSPPS() const;
    IMS_SINT32 GetCandidatePriority() const;
    IMS_BOOL GetConfDisableTxView() const;
    IMS_BOOL GetConfUseLandscape() const;
    IMS_BOOL GetConfSurfaceUpdate() const;
    const CodecConfig* GetCodecConfig() const;
    const IMSList<CodecConfig*>& GetCodecConfigs() const;
    //add CVO parameter
    IMS_SINT32 GetCVOMode() const;
    IMS_SINT32 GetCVOId() const;
    IMS_SINT32 GetMultitaskingType() const;
    IMS_SINT32 GetHoldType() const;
    // add avp video bitrate adaptation
    IMS_BOOL GetAVP_RateAdaptation_Enable() const;
    IMS_SINT32 GetAVP_RateAdaptation_Cycle() const;
    IMS_SINT32 GetAVP_RateAdaptation_DevRatio() const;
    IMS_SINT32 GetAVP_RateAdaptation_Threshold() const;
    IMS_SINT32 GetAVP_RateAdaptation_MinBitrateRatio() const;
    IMS_SINT32 GetIframeInterval() const;
    // get profile level negotiation ,cbp priorty and support low framerate value
    IMS_BOOL GetProfileLevelNegotiation() const;
    IMS_BOOL GetCBPPriority() const;
    IMS_BOOL GetSupportLowFramerate() const;
    // AVPF
    IMS_BOOL GetAVPF() const;
    IMS_SINT32 GetSdpOfferCapNegForAVPF() const;
    IMS_BOOL GetAVPF_TRR() const;
    IMS_BOOL GetAVPF_NACK() const;
    IMS_BOOL GetAVPF_TMMBR() const;
    IMS_BOOL GetAVPF_PLI() const;
    IMS_BOOL GetAVPF_FIR() const;
    IMS_SINT32 GetAVPF_TMMBR_DownInterval() const;
    IMS_SINT32 GetAVPF_TMMBR_UpInterval() const;
    IMS_SINT32 GetAVPF_TMMBR_LossThreshold() const;
    IMS_SINT32 GetAVPF_TMMBR_MinBitrateRatio() const;
    IMS_SINT32 GetAVPF_TMMBR_BitrateLevel() const;
    IMS_SINT32 GetAVPF_TMMBR_UpLevel() const;
    IMS_SINT32 GetAVPF_TMMBR_EnforceVTDowngrade() const;
    IMS_SINT32 GetVideoMarkerOption() const;
    IMS_BOOL IsSendRedundantRTCPSR() const;
    IMS_BOOL GetVideoTestMode() const;
    AString GetVideoFilePath() const;

    IMS_UINT32 GetVideoDumpOption() const;

    IMS_BOOL Update(IN IConfigBuffer *piBuffer, IN IMS_UINT32 nIndex);

protected:
    void Clear();
    IMS_BOOL CreateCodecConfigs(IN CONST AString &strRef, IN CONST IMS_SINT32 nCodecListSize,
        IN IMS_SINT32 nSlotID = 0);

private:
    // Frame rate
    IMS_SINT32  nFramerate_mode;            // hide / auto / manual
    IMS_SINT32  nFramerate;                 // used when nFramerate_mode == FR_MODE_MANUAL
    IMS_SINT32  nMaxFragmentUnitSize;
    IMS_SINT32  nBitrateAdaptEnable;        // bitrate aaptation by AS bandwidth
    IMS_SINT32  nEnableOnScreenDebugInfo;
    // sending SPS/PPS parameter
    IMS_SINT32  nSendPeriodic_SPS_PPS;
    // candidate priority
    IMS_SINT32  nCandidatePriority;
    // conference call priority
    IMS_SINT32  nConfDisableTxView;
    IMS_SINT32  nConfUseLandscape;
    IMS_SINT32  nConfSurfaceUpdate;         // used when updating surface for video conferece call
                                            //  before starting video conference call
    // Codec specific parameters
    IMSList<CodecConfig*> objCodecConfigs;
    //add CVO parameter
    IMS_SINT32 nCVOmode;
    IMS_SINT32 nCVOId;
    VIDEO_MULTI_TYPE eMultitaskingType;
    VIDEO_HOLD_TYPE eHoldType;
    //add profile level negotiation , cbp priority field and support low framerate
    IMS_SINT32  nProfileLevelNegotiation;
    IMS_SINT32  nCBPPriority;
    IMS_SINT32  nSupportLowFramerate;
    IMS_SINT32  nAVP_RateAdaptaion_Enable;
    IMS_SINT32  nAVP_RateAdaptaion_Cycle;
    IMS_SINT32  nAVP_RateAdaptaion_DevRatio;
    IMS_SINT32  nAVP_RateAdaptaion_Threshold;
    IMS_SINT32  nAVP_RateAdaptaion_MinBitrateRatio;
    IMS_SINT32  nIframeInterval;
    // AVPF
    IMS_SINT32 nAVPFEnable;
    IMS_SINT32 nSdpOfferCapNegForAVPF;
    IMS_SINT32 nAVPF_TRR;
    IMS_SINT32 nAVPF_NACK;
    IMS_SINT32 nAVPF_TMMBR;
    IMS_SINT32 nAVPF_PLI;
    IMS_SINT32 nAVPF_FIR;
    IMS_SINT32 nAVPF_TMMBR_DownInterval;
    IMS_SINT32 nAVPF_TMMBR_UpInterval;
    IMS_SINT32 nAVPF_TMMBR_LossThreshold;
    IMS_SINT32 nAVPF_TMMBR_MinBitrateRatio;
    IMS_SINT32 nAVPF_TMMBR_BitrateLevel;
    IMS_SINT32 nAVPF_TMMBR_UpLevel;
    IMS_SINT32 nAVPF_TMMBR_EnforceVTDowngrade;
    IMS_SINT32 nVideoMarkerOption;
    // RFC 3550 6.4 Sender and Receiver Reports
    // The SR is issued if a site has sent any data packets during the interval
    // since issuing the last report or the previous one, otherwise the RR is issued.
    IMS_SINT32 nSendRedundantRTCPSR;
    //video file path for test
    IMS_SINT32 nVideoTestMode;
    AString strVideoFilePath;
    //Video Dump option for debug
    IMS_SINT32 nVideoDumpCamera;
    IMS_SINT32 nVideoDumpEncoderIn;
    IMS_SINT32 nVideoDumpEncoderOut;
    IMS_SINT32 nVideoDumpDecoderIn;
    IMS_SINT32 nVideoDumpDecoderOut;
    IMS_SINT32 nHEVCCodecEnable;
};
#endif                                              // _VIDEO_CONFIG_H_
