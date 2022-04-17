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

#ifndef _AV_CONFIG_H_
#define _AV_CONFIG_H_

#include "MediaDef.h"

class IConfigBuffer;

typedef enum
{
    CODEC_AMR_WB_BE     = 0,
    CODEC_AMR_WB_OA     = 1,
    CODEC_AMR_NB_BE     = 2,
    CODEC_AMR_NB_OA     = 3,
    CODEC_EVS           = 4,

    CODEC_H264_VGA_PR   = 5,
    CODEC_H264_VGA_LS   = 6,
    CODEC_H264_QVGA_PR  = 7,
    CODEC_H264_QVGA_LS  = 8,
    CODEC_H264_QCIF_PR  = 9,
    CODEC_H264_QCIF_LS  = 10,
    CODEC_H263_QCIF_PR  = 11,
    CODEC_H263_QCIF_LS  = 12,
    CODEC_H264_HD_PR    = 13,
    CODEC_H264_HD_LS    = 14,

    CODEC_EVS_NB        = 15,
    CODEC_EVS_WB        = 16,
    CODEC_EVS_SWB       = 17,
    CODEC_EVS_FB        = 18,
    CODEC_NONE          = 99,
} eCodecNameList;

/*!
 * @class   MediaConfiguration
 * @brief   MediaConfiguration class for android
 * @details Get audio/video common configurations from DB
 */
class MediaConfiguration
{
public:
    MediaConfiguration(MEDIA_CONTENT_TYPE _eSessionType);
    virtual ~MediaConfiguration();
private:
    MediaConfiguration(IN CONST MediaConfiguration &objRHS);
    MediaConfiguration& operator=(IN CONST MediaConfiguration &objRHS);
public:
    IMS_BOOL Create(IN IConfigBuffer *piBuffer, IN IMS_UINT32 nIndex);
    virtual void ToDebugString() const;
    MEDIA_CONTENT_TYPE GetSessionType();
    // negotation condition
    IMS_BOOL GetSdpAnswerFullcapa() const;
    IMS_BOOL GetSdpReofferFullcapa() const;
    IMS_BOOL GetSdpKeepMLine() const;
    const IMSList<IMS_SINT32>& GetHoldDirList() const;
    //SDP Version Update Reset Parameter
    IMS_BOOL GetSdpVersionUpdate() const;
    //SDP Direction Loose Checker Parameter
    IMS_SINT32 GetSdpDirLooseCheck() const;
    IMS_SINT32 GetVideoResolutionLooseCheck() const;
    // RTP/RTCP port numbers
    IMS_SINT32 GetPortRtp() const;
    IMS_SINT32 GetPortRtpEnd() const;
    IMS_SINT32 GetPortRtcp() const;
    IMS_SINT32 GetCommonRtpPortPerService() const;
    IMS_SINT32 GetRadioConnectionCheckTimer() const;
    // RTCP usage
    IMS_BOOL GetRtcpEnable() const;
    IMS_SINT32 GetRtcpLiveInterval() const;
    IMS_SINT32 GetRtcpInterval() const;
    IMS_UINT32 GetSendRtcpBye() const;
    //[2013/11/26]hyunho.shin : add get nat binding field api
    IMS_BOOL GetSetNatBinding() const;
    // RTP inactivity timer value
    IMS_SINT32 GetTvRtpInactivityInitial() const;
    IMS_SINT32 GetTvRtpInactivityRuntime() const;
    IMS_SINT32 GetTvRtpInactivityHold() const;
    // Bandwidth - AS, RR, RS
    IMS_SINT32 GetBandwidthMode(IN IMS_SINT32 nType) const;
    IMS_SINT32 GetBandwidth(IN IMS_SINT32 nType, IN IMS_BOOL bIpV6 = IMS_FALSE) const;
    IMS_SINT32 GetBandwidthNegoOption()const;
    // Socket Pos
    IMS_SINT32 GetSocketPos() const;
    IMS_SINT32 GetSocketTos() const;
    IMS_BOOL GetSocketTosEnableAll() const;
    // RTP Filtering
    IMS_BOOL GetRTPFiltering() const;
    //RTCP-XR
    IMS_BOOL GetRtcpXr() const;
    IMS_BOOL GetRtcpXrVoip() const;
    IMS_BOOL GetRtcpXrStatistics() const;
    IMS_BOOL GetRtcpXrPlr() const;
    IMS_BOOL GetRtcpXrPdr() const;
    void SetPortRtp(IN IMS_SINT32 nPort);
    void SetPortRtcp(IN IMS_SINT32 nPort);
    IMS_BOOL Update(IN IConfigBuffer *piBuffer, IN IMS_UINT32 nIndex);
    // SRTP
    IMS_SINT32 GetSrtp() const;
    eMMPFSrtpCryptoType GetSrtpCryptoType() const;
    IMS_SINT32 GetSrtpMasterKeyLifeTime() const;
    IMS_BOOL GetSrtpSupportCapaNego() const;
    //FMTP
    IMS_SINT32 IsEmptyFmtpAttr() const;
    IMS_BOOL IsRtcpDisableBeforeSetup() const;
    IMSList<eCodecNameList> GetCodecPriorityList() const;

public:

    // Bandwidth mode
    enum
    {
        BW_MODE_HIDE        = 0,
        BW_MODE_OPTIMAL     = 1,
        BW_MODE_MAX         = 2,
        BW_MODE_MANUAL      = 3,
        BW_MODE_NEGOTIABLE  = 4,
    };

    // Bandwidth RS/RR mode
    enum
    {
        BW_RS_RR_HIDE       = 0,
        BW_RS_RR_PERCENT    = 1,
        BW_RS_RR_MANUAL     = 2
    };

    // Bandwidth type
    enum
    {
        BW_AS = 1,
        BW_RR = 2,
        BW_RS = 3
    };

    enum
    {
        DEFAULT_PORT_RTP = 49152
    };

    /*
        Bandwidth_option_value
        BW_OPTION_SOURCE_VALUE : Use Source Profile's AS/RS/RR Value
        BW_OPTION_NEGOTIATED_VALUE : Use Negotiated AS/RS/RR Value
                                    (compare source profile and dest profile)
    */
    enum
    {
        BW_OPTION_SOURCE_VALUE = 0,
        BW_OPTION_NEGOTIATED_VALUE = 1
    };

    enum
    {
        SOCKET_POS_DEFAULT = 0,
        SOCKET_POS_CP = 1,
        SOCKET_POS_AP = 2
    };

    // check SDPOfferCapNegForAVPF option
    enum
    {
        CAPNEG_OFFER_NONE = 0,
        CAPNEG_OFFER_WITHOUT_ACAP = 1,
        CAPNEG_OFFER_WITH_ACAP = 2
    };
    // Video Resolution Loose Check mode
    enum
    {
        USE_SELF_RESOLUTION_STRICTLY    = 0,
        USE_PEER_RESOLUTION_RX_ONLY     = 1,
        USE_PEER_RESOLUTION_TRX         = 2,
    };

private:
    static const IMS_CHAR KEY_SDP_ANSWER_FULLCAPA[];
    static const IMS_CHAR KEY_SDP_REOFFER_FULLCAPA[];
    static const IMS_CHAR KEY_SDP_KEEP_M_LINE[];
    static const IMS_CHAR KEY_SDP_HOLD_DIR[];
    static const IMS_CHAR KEY_SDP_VERSION_UPDATE_RESET[];
    static const IMS_CHAR KEY_SDP_DIR_LOOSE_CHECK[];
    static const IMS_CHAR KEY_PORT_RTP_MODE[];
    static const IMS_CHAR KEY_PORT_RTP[];
    static const IMS_CHAR KEY_PORT_RTCP[];
    static const IMS_CHAR KEY_COMMON_RTP_PORT_PER_SERVICE[];
    static const IMS_CHAR KEY_RADIO_CONNECTION_CHECK_TIMER[];
    static const IMS_CHAR KEY_RTCP_ENABLE[];
    static const IMS_CHAR KEY_RTCP_LIVE_INTERVAL[];
    static const IMS_CHAR KEY_RTCP_INTERVAL[];
    static const IMS_CHAR KEY_SEND_RTCP_BYE[];
    //[2013/11/26]hyunho.shin : add set nat binding field
    static const IMS_CHAR KEY_SET_NAT_BINDING[];
    static const IMS_CHAR KEY_TV_RTP_INACTIVITY[];
    static const IMS_CHAR KEY_BW_AS_MODE[];
    static const IMS_CHAR KEY_BW_AS[];
    static const IMS_CHAR KEY_BW_RR_MODE[];
    static const IMS_CHAR KEY_BW_RR[];
    static const IMS_CHAR KEY_BW_RS_MODE[];
    static const IMS_CHAR KEY_BW_RS[];
    static const IMS_CHAR KEY_BW_NEGO_OPTION[];
    static const IMS_CHAR KEY_SOCKET_POS[];
    static const IMS_CHAR KEY_SOCKET_TOS[];
    static const IMS_CHAR KEY_SOCKET_TOS_ENABLE_ALL[];
    static const IMS_CHAR KEY_RTP_FILTERING[];
    static const IMS_CHAR KEY_SDP_RESOLUTION_LOOSE_CHECK[];
    // RTCP-XR
    static const IMS_CHAR KEY_RTCPXR_ENABLE[];
    static const IMS_CHAR KEY_RTCPXR_STATISTIC[];
    static const IMS_CHAR KEY_RTCPXR_VOIP[];
    static const IMS_CHAR KEY_RTCPXR_PLR[];
    static const IMS_CHAR KEY_RTCPXR_PDR[];
    // SRTP
    static const IMS_CHAR KEY_SRTP_ENABLE[];
    static const IMS_CHAR KEY_SRTP_CRYPTO_TYPE[];
    static const IMS_CHAR KEY_SRTP_MASTER_KEY_LIFE_TIME[];
    static const IMS_CHAR KEY_SRTP_SUPPORT_CAPA_NEGO[];
    //FMTP
    static const IMS_CHAR KEY_EMPTY_FMTPATTR[];
    // -- set RS/RR zero though Initial media direction is inactive
    static const IMS_CHAR KEY_DISABLE_RTCP_BEFORE_SETUP[];
    static const IMS_CHAR KEY_CODEC_PRIORITY_LIST[];

private:
    MEDIA_CONTENT_TYPE eSessionType;

    // SDP offer/answer parameter
    IMS_SINT32 nSdpAnswerFullcapa;
    IMS_SINT32 nSdpReofferFullcapa;
    IMS_SINT32 nSdpKeepMLine;
    IMSList<IMS_SINT32> listHoldDir;
    // SDP Version Update Reset Parameter
    IMS_SINT32 nSdpVersionUpdateReset;
    // SDP Direction Loose Checker Parameter
    IMS_SINT32 nSdpDirLooseCheck;
    // Video Resolution Loose Checker Parameter
    IMS_SINT32 nVideoResolutionLooseCheck;
    // RTP / RTCP port numbers
    IMS_SINT32 nPortRtp;
    IMS_SINT32 nPortRtpEnd;
    IMS_SINT32 nCommonRtpPortPerService;
    IMS_SINT32 nPortRtcp;
    IMS_SINT32 nRadioCcTimer;
    // RTCP activity
    IMS_SINT32 nRtcpEnable;                      // active call
    IMS_SINT32 nRtcpLiveInterval;
    IMS_SINT32 nRtcpInterval;
    IMS_SINT32 nSendRtcpBye;
    //[2013/11/26]hyunho.shin : add set nat binding field
    IMS_SINT32 nSetNatBinding;
    // RTP Timeout - 0 : disable, over 0 : enable(sec)
    IMS_SINT32 nTvRtpInactivityInitial;
    IMS_SINT32 nTvRtpInactivityRuntime;
    IMS_SINT32 nTvRtpInactivityHold;
    // Bandwidth - AS, RR, RS
    IMS_SINT32 nBwAsMode;                   // 0 : hide, 1 : optimal value(default),
                                            //      2 : maximum value of codec, 3 : manual input(AS)
    IMS_SINT32 nBwRrMode;                   // 0 : hide, 1 : percent * 1000 (default),
                                            //      2 : manual iinput(RR)
    IMS_SINT32 nBwRsMode;                   // 0 : hide, 1 : percent * 1000 (default),
                                            //      2 : manual input(RS)
    IMS_SINT32 nBwAs[2];                    // AS List AS[0] IPv4, AS[1] IPv6
    IMS_SINT32 nBwRr[2];
    IMS_SINT32 nBwRs[2];
    IMS_SINT32 nBwNegoOptionValue;
    IMS_SINT32 nSocketPos;
    IMS_SINT32 nSocketTos;
    IMS_SINT32 nSocketTosEnableAll;
    IMS_SINT32 nRtpFiltering;
    // RTCP-XR
    IMS_SINT32 nRtcpXrEnable;
    IMS_SINT32 nRtcpXrStatistic;
    IMS_SINT32 nRtcpXrVoip;
    IMS_SINT32 nRtcpXrPacketLossRle;
    IMS_SINT32 nRtcpXrPacketDuplicateRle;
    //SRTP
    IMS_SINT32 nSrtpEnable;
    IMS_SINT32 nSrtpCryptoType;
    IMS_SINT32 nMasterKeyLifeTime;
    IMS_SINT32 nSrtpSupportCapaNego;
    //FMTP
    IMS_SINT32 nEmptyFmtpAttr;
    // set RS/RR zero though Initial media direction is inactive
    IMS_SINT32 nRtcpDisableBeforeSetup;

protected:
    IMSList<eCodecNameList> listCodecPriority;
};
#endif                                              // _AV_CONFIG_H_
