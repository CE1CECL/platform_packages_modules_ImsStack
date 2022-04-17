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
#include "IConfigBuffer.h"
#include "TextParser.h"
#include "mmpf/MMPFBoardConfigInfo.h"
#include "config/MediaConfiguration.h"

__IMS_TRACE_TAG_USER_DECL__("CONF");

PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_SDP_ANSWER_FULLCAPA[] = "sdp_answer_fullcapa";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_SDP_REOFFER_FULLCAPA[] = "sdp_reoffer_fullcapa";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_SDP_KEEP_M_LINE[] = "sdp_keep_m_line";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_SDP_HOLD_DIR[] = "sdp_hold_dir";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_SDP_VERSION_UPDATE_RESET[] = "sdp_version_update_reset";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_SDP_DIR_LOOSE_CHECK[] = "sdp_dir_loose_check";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_PORT_RTP_MODE[] = "port_rtp_mode";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_COMMON_RTP_PORT_PER_SERVICE[] =
        "common_rtp_port_per_service";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_PORT_RTP[] = "port_rtp";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_PORT_RTCP[] = "port_rtcp";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_RADIO_CONNECTION_CHECK_TIMER[] =
        "radio_connection_check_timer";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_RTCP_ENABLE[] = "rtcp_enable";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_RTCP_LIVE_INTERVAL[] = "rtcp_live_interval";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_RTCP_INTERVAL[] = "rtcp_interval";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_SEND_RTCP_BYE[] = "send_rtcp_bye";
//[2013/11/26]hyunho.shin : add set nat binding field
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_SET_NAT_BINDING[] = "set_nat_binding";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_TV_RTP_INACTIVITY[] = "tv_rtp_inactivity";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_BW_AS_MODE[] = "bw_as_mode";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_BW_AS[] = "bw_as";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_BW_RR_MODE[] = "bw_rr_mode";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_BW_RR[] = "bw_rr";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_BW_RS_MODE[] = "bw_rs_mode";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_BW_RS[] = "bw_rs";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_BW_NEGO_OPTION[] = "bw_nego_option";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_SOCKET_POS[] = "socket_pos";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_SOCKET_TOS[] = "socket_tos";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_SOCKET_TOS_ENABLE_ALL[] = "socket_tos_enable_all";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_RTP_FILTERING[] = "rtp_filtering";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_SRTP_ENABLE[] = "srtp_enable";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_SRTP_CRYPTO_TYPE[] = "srtp_crypto_type";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_SRTP_MASTER_KEY_LIFE_TIME[] = "srtp_master_key_life_time";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_SRTP_SUPPORT_CAPA_NEGO[] = "srtp_support_capa_nego";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_SDP_RESOLUTION_LOOSE_CHECK[] = "resolution_loose_check";

//rtcp-xr
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_RTCPXR_ENABLE[] = "rtcpxr_enable";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_RTCPXR_STATISTIC[] = "rtcpxr_statistics";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_RTCPXR_VOIP[] = "rtcpxr_voip_metrics";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_RTCPXR_PLR[] = "rtcpxr_pkt_loss_rle";
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_RTCPXR_PDR[] = "rtcpxr_pkt_dup_rle";

//FMTP
PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_EMPTY_FMTPATTR[] = "empty_fmtp_attr";

PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_DISABLE_RTCP_BEFORE_SETUP[] = "disable_rtcp_before_setup";

PRIVATE GLOBAL
const IMS_CHAR MediaConfiguration::KEY_CODEC_PRIORITY_LIST[] = "priority_list";
PUBLIC
MediaConfiguration::MediaConfiguration(MEDIA_CONTENT_TYPE _eSessionType) :
        eSessionType(_eSessionType),
        nSdpAnswerFullcapa(0),
        nSdpReofferFullcapa(0),
        nSdpKeepMLine(0),
        nSdpVersionUpdateReset(0),
        nSdpDirLooseCheck(0),
        nVideoResolutionLooseCheck(0),
        nPortRtp(DEFAULT_PORT_RTP),
        nPortRtpEnd(DEFAULT_PORT_RTP + 8),
        nCommonRtpPortPerService(0),
        nPortRtcp(DEFAULT_PORT_RTP + 1),
        nRadioCcTimer(0),
        nRtcpEnable(0),
        nRtcpLiveInterval(0),
        nRtcpInterval(0),
        nSendRtcpBye(0),
        nSetNatBinding(0),
        nTvRtpInactivityInitial(10),
        nTvRtpInactivityRuntime(10),
        nTvRtpInactivityHold(10),
        nBwAsMode(BW_MODE_OPTIMAL),
        nBwRrMode(BW_RS_RR_PERCENT),
        nBwRsMode(BW_RS_RR_PERCENT),
        nBwNegoOptionValue(0),
        nSocketPos(SOCKET_POS_DEFAULT),
        nSocketTos(0),
        nSocketTosEnableAll(0),
        nRtpFiltering(0),
        //rtcp-xr
        nRtcpXrEnable(0),
        nRtcpXrStatistic(0),
        nRtcpXrVoip(0),
        nRtcpXrPacketLossRle(0),
        nRtcpXrPacketDuplicateRle(0),
        //SRTP
        nSrtpEnable(0),
        nSrtpCryptoType(0),
        nMasterKeyLifeTime(0),
        nSrtpSupportCapaNego(0),
        nEmptyFmtpAttr(0), //FMTP
        nRtcpDisableBeforeSetup(0)
{
}

PUBLIC VIRTUAL
MediaConfiguration::~MediaConfiguration()
{
}

PUBLIC VIRTUAL
IMS_BOOL MediaConfiguration::Create(IN IConfigBuffer *piBuffer, IN IMS_UINT32 /* nIndex */)
{
    if (piBuffer == IMS_NULL)
    {
        return 0;
    }

    // Section MUST be captured by the derived class.
    // So, this method just read the parameter directly.
    listHoldDir.Clear();

    // SDP Offer/Answer
    nSdpAnswerFullcapa = piBuffer->ReadValueInt(KEY_SDP_ANSWER_FULLCAPA);
    nSdpReofferFullcapa = piBuffer->ReadValueInt(KEY_SDP_REOFFER_FULLCAPA);
    nSdpKeepMLine = piBuffer->ReadValueInt(KEY_SDP_KEEP_M_LINE);

    // SDP Version Update Reset
    nSdpVersionUpdateReset = piBuffer->ReadValueInt(KEY_SDP_VERSION_UPDATE_RESET);

    // SDP Direction Loose Check
    nSdpDirLooseCheck = piBuffer->ReadValueInt(KEY_SDP_DIR_LOOSE_CHECK);

    // SDP Direction Loose Check
    nVideoResolutionLooseCheck = piBuffer->ReadValueInt(KEY_SDP_RESOLUTION_LOOSE_CHECK);

    if (nVideoResolutionLooseCheck <= 0)
    {
        nVideoResolutionLooseCheck = 0;
    }

    // RTP timeout value
    const AString &strHoldDir = piBuffer->ReadValue(KEY_SDP_HOLD_DIR);
    IMSList<AString> objTokens = strHoldDir.Split(TextParser::CHAR_COMMA);

    for (IMS_UINT32 i=0; i<objTokens.GetSize(); i++)
    {
        if (objTokens.GetAt(i).EqualsIgnoreCase("ia"))
        {
            listHoldDir.Append(MEDIA_DIRECTION_INACTIVE);
        }

        if (objTokens.GetAt(i).EqualsIgnoreCase("so"))
        {
            listHoldDir.Append(MEDIA_DIRECTION_SEND);
        }

        if (objTokens.GetAt(i).EqualsIgnoreCase("ro"))
        {
            listHoldDir.Append(MEDIA_DIRECTION_RECEIVE);
        }
    }

    // RTP / RTCP port numbers
    const AString &strPortRTP = piBuffer->ReadValue(KEY_PORT_RTP);
    objTokens = strPortRTP.Split(TextParser::CHAR_COMMA);

    // Share RTP Port number between VoLTE and VT
    nCommonRtpPortPerService= piBuffer->ReadValueInt(KEY_COMMON_RTP_PORT_PER_SERVICE);
    if (nCommonRtpPortPerService == -1)
    {
        nCommonRtpPortPerService = 0;
    }

    nRadioCcTimer = piBuffer->ReadValueInt(KEY_RADIO_CONNECTION_CHECK_TIMER);

    // RTCP Usage
    nRtcpEnable = piBuffer->ReadValueInt(KEY_RTCP_ENABLE);
    nRtcpLiveInterval = piBuffer->ReadValueInt(KEY_RTCP_LIVE_INTERVAL);
    nRtcpInterval = piBuffer->ReadValueInt(KEY_RTCP_INTERVAL);
    nSendRtcpBye = piBuffer->ReadValueInt(KEY_SEND_RTCP_BYE);

    //[2013/11/26]hyunho.shin : set nat binding field
    nSetNatBinding = piBuffer->ReadValueInt(KEY_SET_NAT_BINDING);

    // RTP timeout value
    const AString &strRTPInActivity = piBuffer->ReadValue(KEY_TV_RTP_INACTIVITY);
    objTokens = strRTPInActivity.Split(TextParser::CHAR_COMMA);

    if (objTokens.GetSize() == 1)
    {
        nTvRtpInactivityInitial = objTokens.GetAt(0).ToInt32();
        nTvRtpInactivityRuntime = nTvRtpInactivityInitial;
        nTvRtpInactivityHold = nTvRtpInactivityInitial;
    }
    else if (objTokens.GetSize() ==2)
    {
        nTvRtpInactivityInitial = objTokens.GetAt(0).ToInt32();
        nTvRtpInactivityRuntime = objTokens.GetAt(1).ToInt32();
        nTvRtpInactivityHold = nTvRtpInactivityRuntime;
    }
    else if (objTokens.GetSize() > 2)
    {
        nTvRtpInactivityInitial = objTokens.GetAt(0).ToInt32();
        nTvRtpInactivityRuntime = objTokens.GetAt(1).ToInt32();
        nTvRtpInactivityHold = objTokens.GetAt(2).ToInt32();
    }

    // bw_as_mode
    nBwAsMode = piBuffer->ReadValueInt(KEY_BW_AS_MODE);

    if (nBwAsMode == -1)
    {
        nBwAsMode = BW_MODE_OPTIMAL;
    }

    // bw_as parameter
    const AString &strBandwidthAS = piBuffer->ReadValue(KEY_BW_AS);
    objTokens = strBandwidthAS.Split(TextParser::CHAR_COMMA);

    if (objTokens.GetSize() == 1)
    {
        nBwAs[0] = objTokens.GetAt(0).ToInt32();    // IPv4
        nBwAs[1] = objTokens.GetAt(0).ToInt32();    // IPv6
    }
    else if (objTokens.GetSize() == 2)
    {
        nBwAs[0] = objTokens.GetAt(0).ToInt32();    // IPv4
        nBwAs[1] = objTokens.GetAt(1).ToInt32();    // IPv6
    }
    else
    {
        nBwAs[0] = 0;
        nBwAs[1] = 0;
    }

    // bw_rr_mode
    nBwRrMode = piBuffer->ReadValueInt(KEY_BW_RR_MODE);

    if (nBwRrMode == -1)
    {
        nBwRrMode = BW_RS_RR_PERCENT;
    }

    // bw_rr parameter
    const AString &strBandwidthRR = piBuffer->ReadValue(KEY_BW_RR);
    objTokens = strBandwidthRR.Split(TextParser::CHAR_COMMA);

    if (objTokens.GetSize() == 1)
    {
        nBwRr[0] = objTokens.GetAt(0).ToInt32();    // IPv4
        nBwRr[1] = objTokens.GetAt(0).ToInt32();    // IPv6
    }
    else if (objTokens.GetSize() == 2)
    {
        nBwRr[0] = objTokens.GetAt(0).ToInt32();    // IPv4
        nBwRr[1] = objTokens.GetAt(1).ToInt32();    // IPv6
    }
    else
    {
        nBwRr[0] = 0;
        nBwRr[1] = 0;
    }

    // bw_rs_mode
    nBwRsMode = piBuffer->ReadValueInt(KEY_BW_RS_MODE);
    if (nBwRsMode == -1) nBwRsMode = BW_RS_RR_PERCENT;

    // bw_rs parameter
    const AString &strBandwidthRS = piBuffer->ReadValue(KEY_BW_RS);
    objTokens = strBandwidthRS.Split(TextParser::CHAR_COMMA);
    if (objTokens.GetSize() == 1)
    {
        nBwRs[0] = objTokens.GetAt(0).ToInt32();    // IPv4
        nBwRs[1] = objTokens.GetAt(0).ToInt32();    // IPv6
    }
    else if (objTokens.GetSize() == 2)
    {
        nBwRs[0] = objTokens.GetAt(0).ToInt32();    // IPv4
        nBwRs[1] = objTokens.GetAt(1).ToInt32();    // IPv6
    }
    else
    {
        nBwRs[0] = 0;
        nBwRs[1] = 0;
    }
    // bw answer option value check parameter
    nBwNegoOptionValue = piBuffer->ReadValueInt(KEY_BW_NEGO_OPTION);

    // socket position
    const AString &strSocketPos = piBuffer->ReadValue(KEY_SOCKET_POS);
    if (strSocketPos.EqualsIgnoreCase("cp"))
    {
        nSocketPos = SOCKET_POS_CP;
    }
    else if (strSocketPos.EqualsIgnoreCase("ap"))
    {
        nSocketPos = SOCKET_POS_AP;
    }
    else if (strSocketPos.EqualsIgnoreCase("apif2ip"))
    {
        // Whether chipset support Single IP or Dual IP
        // QCT : Dual IP
        // MTK : Single IP
        if (MMPFBoardConfigInfo::GetChipset() == MMPFBoardConfigInfo::MMPF_BC_CHIPSET_MTK)
        {
            nSocketPos = SOCKET_POS_CP;
        }
        else
        {
            nSocketPos = SOCKET_POS_AP;
        }
    }
    else
    {
        nSocketPos = SOCKET_POS_DEFAULT;
    }

    //socket ToS
    nSocketTos = piBuffer->ReadValueInt(KEY_SOCKET_TOS);
    nSocketTosEnableAll = piBuffer->ReadValueInt(KEY_SOCKET_TOS_ENABLE_ALL);

    IMS_TRACE_D("Socket ToS[%d], Socket ToS Enable All[%d] ", nSocketTos, nSocketTosEnableAll, 0);

    nRtpFiltering= piBuffer->ReadValueInt(KEY_RTP_FILTERING);

    //rtcp-xr
    nRtcpXrEnable = piBuffer->ReadValueInt(KEY_RTCPXR_ENABLE);
    nRtcpXrStatistic = piBuffer->ReadValueInt(KEY_RTCPXR_STATISTIC);
    nRtcpXrVoip = piBuffer->ReadValueInt(KEY_RTCPXR_VOIP);
    nRtcpXrPacketLossRle = piBuffer->ReadValueInt(KEY_RTCPXR_PLR);
    nRtcpXrPacketDuplicateRle = piBuffer->ReadValueInt(KEY_RTCPXR_PDR);

    //SRTP
    nSrtpEnable = piBuffer->ReadValueInt(KEY_SRTP_ENABLE);
    nSrtpCryptoType = piBuffer->ReadValueInt(KEY_SRTP_CRYPTO_TYPE);

    if (nSrtpEnable == -1 || nSrtpCryptoType < 2)
    {
        nSrtpEnable = 0;
        nSrtpCryptoType = MMPF_SRTP_CRYPTO_TYPE_NONE;
    }

    nMasterKeyLifeTime = piBuffer->ReadValueInt(KEY_SRTP_MASTER_KEY_LIFE_TIME);
    nSrtpSupportCapaNego = piBuffer->ReadValueInt(KEY_SRTP_SUPPORT_CAPA_NEGO);

    //FMTP - Disabled by default
    nEmptyFmtpAttr = piBuffer->ReadValueInt(KEY_EMPTY_FMTPATTR);

    if (nEmptyFmtpAttr == -1)//If not present in config
    {
        nEmptyFmtpAttr = 0;
    }

    nRtcpDisableBeforeSetup = piBuffer->ReadValueInt(KEY_DISABLE_RTCP_BEFORE_SETUP);

    listCodecPriority.Clear();
    const AString &strCodecList = piBuffer->ReadValue(KEY_CODEC_PRIORITY_LIST);
    objTokens = strCodecList.Split(TextParser::CHAR_COMMA);

    for (IMS_UINT32 i = 0; i < objTokens.GetSize(); i++)
    {
        AString strCodec = objTokens.GetAt(i);

        if (strCodec.EqualsIgnoreCase("AMR_WB_BE"))
        {
            listCodecPriority.Append(CODEC_AMR_WB_BE);
        }
        else if (strCodec.EqualsIgnoreCase("AMR_WB_OA"))
        {
            listCodecPriority.Append(CODEC_AMR_WB_OA);
        }
        else if (strCodec.EqualsIgnoreCase("AMR_NB_BE") || strCodec.EqualsIgnoreCase("AMR_BE"))
        {
            listCodecPriority.Append(CODEC_AMR_NB_BE);
        }
        else if (strCodec.EqualsIgnoreCase("AMR_NB_OA") || strCodec.EqualsIgnoreCase("AMR_OA"))
        {
            listCodecPriority.Append(CODEC_AMR_NB_OA);
        }
        else if (strCodec.EqualsIgnoreCase("EVS"))
        {
            listCodecPriority.Append(CODEC_EVS);
        }
        else if (strCodec.EqualsIgnoreCase("H264_VGA_PR"))
        {
            listCodecPriority.Append(CODEC_H264_VGA_PR);
        }
        else if (strCodec.EqualsIgnoreCase("H264_VGA_LS"))
        {
            listCodecPriority.Append(CODEC_H264_VGA_LS);
        }
        else if (strCodec.EqualsIgnoreCase("H264_QVGA_PR"))
        {
            listCodecPriority.Append(CODEC_H264_QVGA_PR);
        }
        else if (strCodec.EqualsIgnoreCase("H264_QVGA_LS"))
        {
            listCodecPriority.Append(CODEC_H264_QVGA_LS);
        }
        else if (strCodec.EqualsIgnoreCase("H264_QCIF_PR"))
        {
            listCodecPriority.Append(CODEC_H264_QCIF_PR);
        }
        else if (strCodec.EqualsIgnoreCase("H264_QCIF_LS"))
        {
            listCodecPriority.Append(CODEC_H264_QCIF_LS);
        }
        else if (strCodec.EqualsIgnoreCase("H263_QCIF_PR"))
        {
            listCodecPriority.Append(CODEC_H263_QCIF_PR);
        }
        else if (strCodec.EqualsIgnoreCase("H263_QCIF_LS"))
        {
            listCodecPriority.Append(CODEC_H263_QCIF_LS);
        }
        else if (strCodec.EqualsIgnoreCase("H264_HD_PR"))
        {
            listCodecPriority.Append(CODEC_H264_HD_PR);
        }
        else if (strCodec.EqualsIgnoreCase("H264_HD_LS"))
        {
            listCodecPriority.Append(CODEC_H264_HD_LS);
        }
        else if (strCodec.EqualsIgnoreCase("EVS_NB"))
        {
            listCodecPriority.Append(CODEC_EVS_NB);
        }
        else if (strCodec.EqualsIgnoreCase("EVS_WB"))
        {
            listCodecPriority.Append(CODEC_EVS_WB);
        }
        else if (strCodec.EqualsIgnoreCase("EVS_SWB"))
        {
            listCodecPriority.Append(CODEC_EVS_SWB);
        }
        else if (strCodec.EqualsIgnoreCase("EVS_FB"))
        {
            listCodecPriority.Append(CODEC_EVS_FB);
        }
        else
        {
            continue;
        }
    }
    return IMS_TRUE;
}

PUBLIC VIRTUAL
void MediaConfiguration::ToDebugString() const
{
    IMS_TRACE_D("session_type[%d], nSdpAnswerFullcapa[%d], nSdpReofferFullcapa[%d]",
            (IMS_SINT32)eSessionType, nSdpAnswerFullcapa, nSdpReofferFullcapa);
    IMS_TRACE_D("nSdpKeepMLine[%d]", nSdpKeepMLine, 0, 0);
    IMS_TRACE_D("port_rtp(%d, %d)", nPortRtp, nPortRtpEnd, 0);
    IMS_TRACE_D("RadioConnectionCheckTimer[%d]", nRadioCcTimer, 0, 0);
    IMS_TRACE_D("nPortRtpDenpendency[%d], nRtcpEnable[%d], nSendRtcpBye[%d]",
            nCommonRtpPortPerService, nRtcpEnable, nSendRtcpBye);
    IMS_TRACE_D("nTV_RTPInactivity(%d, %d, %d)",
            nTvRtpInactivityInitial, nTvRtpInactivityRuntime, nTvRtpInactivityHold);
    IMS_TRACE_D("bw_as IPv4[%d], IPv6[%d]", nBwAs[0], nBwAs[1], 0);
    IMS_TRACE_D("bw_rr IPv4[%d], IPv6[%d]", nBwRr[0], nBwRr[1], 0);
    IMS_TRACE_D("bw_rs IPv4[%d], IPv6[%d]", nBwRs[0], nBwRs[1], 0);
    IMS_TRACE_D("BW_Nego Option[%d], socket_pos[%d], socket_tos[%d]",
            nBwNegoOptionValue, nSocketPos, nSocketTos);
    IMS_TRACE_D("rtcpxr[%d], rtcpxr_stat_summery[%d], rtcpxr_voip_metrics[%d]",
            nRtcpXrEnable, nRtcpXrStatistic, nRtcpXrVoip);
    IMS_TRACE_D("rtcpxr_pkt_loss_rle[%d], rtcpxr_pkt_dup_rle[%d]",
            nRtcpXrPacketLossRle, nRtcpXrPacketDuplicateRle, 0);
    IMS_TRACE_D("IsEmptyFMTPAttr[%d]", nEmptyFmtpAttr, 0, 0);
}

PUBLIC
MEDIA_CONTENT_TYPE MediaConfiguration::GetSessionType()
{
    return eSessionType;
}

PUBLIC
IMS_BOOL MediaConfiguration::GetSdpAnswerFullcapa() const
{
    return (nSdpAnswerFullcapa == 1);
}

PUBLIC
IMS_BOOL MediaConfiguration::GetSdpReofferFullcapa() const
{
    return (nSdpReofferFullcapa == 1);
}

PUBLIC
IMS_BOOL MediaConfiguration::GetSdpKeepMLine() const
{
    return (nSdpKeepMLine == 1);
}

PUBLIC
const IMSList<IMS_SINT32>& MediaConfiguration::GetHoldDirList() const
{
    return listHoldDir;
}

PUBLIC
IMS_BOOL MediaConfiguration::GetSdpVersionUpdate() const
{
    IMS_TRACE_D("GetSdpVersionUpdate() - SDPVersionUpdatedReset[%d]",
            nSdpVersionUpdateReset, 0, 0);
    return (nSdpVersionUpdateReset == 1);
}

PUBLIC
IMS_SINT32 MediaConfiguration::GetSdpDirLooseCheck() const
{
    IMS_TRACE_D("GetSdpDirLooseCheck() - GetSdpDirLooseCheck[%d]",
            nSdpDirLooseCheck, 0, 0);
    return nSdpDirLooseCheck;
}

PUBLIC
IMS_SINT32 MediaConfiguration::GetVideoResolutionLooseCheck() const
{
    IMS_TRACE_D("GetVideoResolutionLooseCheck() - GetVideoResolutionLooseCheck[%d]",
            nVideoResolutionLooseCheck, 0, 0);
    return nVideoResolutionLooseCheck;
}

PUBLIC
IMS_SINT32 MediaConfiguration::GetPortRtp() const
{
    return nPortRtp;
}

PUBLIC
IMS_SINT32 MediaConfiguration::GetPortRtpEnd() const
{
    return nPortRtpEnd;
}

PUBLIC
IMS_SINT32 MediaConfiguration::GetPortRtcp() const
{
    return nPortRtcp;
}

PUBLIC
IMS_SINT32 MediaConfiguration::GetCommonRtpPortPerService() const
{
    return nCommonRtpPortPerService;
}

PUBLIC
IMS_SINT32 MediaConfiguration::GetRadioConnectionCheckTimer() const
{
    return (nRadioCcTimer == 1) ? nRadioCcTimer : 0;
}

PUBLIC
IMS_BOOL MediaConfiguration::GetRtcpEnable() const
{
    return (nRtcpEnable == 1);
}

/*!
 *  @brief  GetRtcpLiveInterval
 */
PUBLIC
IMS_SINT32 MediaConfiguration::GetRtcpLiveInterval() const
{
    return nRtcpLiveInterval;
}

/*!
 *  @brief  GetRtcpLiveInterval
 */
PUBLIC
IMS_SINT32 MediaConfiguration::GetRtcpInterval() const
{
    return nRtcpInterval;
}

/*!
 *  @brief      GetSendRtcpBye
 *  @details    0 : not sending, 1 : always sending, 2 : send in confirmed session only
 */
PUBLIC IMS_UINT32 MediaConfiguration::GetSendRtcpBye() const
{
    return (IMS_UINT32)nSendRtcpBye;
}

PUBLIC IMS_BOOL MediaConfiguration::GetSetNatBinding() const
{
    return (nSetNatBinding == 1);
}

PUBLIC
IMS_SINT32 MediaConfiguration::GetTvRtpInactivityInitial() const
{
    return nTvRtpInactivityInitial;
}

PUBLIC
IMS_SINT32 MediaConfiguration::GetTvRtpInactivityRuntime() const
{
    return nTvRtpInactivityRuntime;
}

PUBLIC
IMS_SINT32 MediaConfiguration::GetTvRtpInactivityHold() const
{
    return nTvRtpInactivityHold;
}

IMS_SINT32 MediaConfiguration::GetBandwidthMode(IN IMS_SINT32 nType) const
{
    switch (nType)
    {
        case BW_AS:
            return nBwAsMode;
        case BW_RR:
            return nBwRrMode;
        case BW_RS:
            return nBwRsMode;
        default:
            return (-1);
    }
}

PUBLIC
IMS_SINT32 MediaConfiguration::GetBandwidth(IN IMS_SINT32 nType, IN IMS_BOOL bIpV6) const
{
    IMS_SINT32 index;

    if (bIpV6)
    {
        index = 1;
    }
    else
    {
        index = 0;
    }

    switch (nType)
    {
        case BW_AS:
            return nBwAs[index];
        case BW_RR:
            return nBwRr[index];
        case BW_RS:
            return nBwRs[index];
        default:
            return (-1);
    }
}

PUBLIC
IMS_SINT32 MediaConfiguration::GetBandwidthNegoOption() const
{
    return nBwNegoOptionValue;
}

PUBLIC
IMS_SINT32 MediaConfiguration::GetSocketPos() const
{
    return nSocketPos;
}

PUBLIC
IMS_SINT32 MediaConfiguration::GetSocketTos() const
{
    return ((nSocketTos >= 0)? nSocketTos : 0);
}

PUBLIC
IMS_BOOL MediaConfiguration::GetSocketTosEnableAll() const
{
    return (nSocketTosEnableAll == 1);
}

PUBLIC
IMS_BOOL MediaConfiguration::GetRTPFiltering() const
{
    return (nRtpFiltering == 1);
}

/*!
 * @brief   GetRtcpXr
 * @details Get RTCP-XR enable
 */
IMS_BOOL MediaConfiguration::GetRtcpXr() const
{
    return (nRtcpXrEnable == 1);
}

/*!
 * @brief   GetRtcpXrVoip
 * @details Get RTCP-XR Voip metrics enable
 */
IMS_BOOL MediaConfiguration::GetRtcpXrVoip() const
{
    return (nRtcpXrVoip == 1);
}

/*!
 * @brief   GetRtcpXrStatistics
 * @details Get RTCP-XR statistic metrics enable
 */
IMS_BOOL MediaConfiguration::GetRtcpXrStatistics() const
{
    return (nRtcpXrStatistic == 1);
}

/*!
 * @brief   GetRtcpXrPlr
 * @details Get RTCP-XR packet loss rle enable
 */
IMS_BOOL MediaConfiguration::GetRtcpXrPlr() const
{
    return (nRtcpXrPacketLossRle == 1);
}

/*!
 * @brief   GetRtcpXrPdr
 * @details Get RTCP-XR packet duplicate rle enable
 */
IMS_BOOL MediaConfiguration::GetRtcpXrPdr() const
{
    return (nRtcpXrPacketDuplicateRle == 1);
}

PUBLIC
void MediaConfiguration::SetPortRtp(IN IMS_SINT32 nPort)
{
    IMS_TRACE_D("SetPortRtp :: [%d] -> [%d])", nPortRtp, nPort, 0);

    nPortRtp = nPort;
}

PUBLIC
void MediaConfiguration::SetPortRtcp(IN IMS_SINT32 nPort)
{
    IMS_TRACE_D("SetPortRtp :: [%d] -> [%d])", nPortRtcp, nPort, 0);

    nPortRtcp = nPort;
}

PUBLIC VIRTUAL
IMS_BOOL MediaConfiguration::Update(IN IConfigBuffer *piBuffer, IN IMS_UINT32 /* nIndex */)
{
    if (piBuffer == IMS_NULL)
    {
        return 0;
    }

    IMS_TRACE_D("Update :: Entered", 0, 0, 0);

    // RTP timeout value
    const AString &strRTPInActivity = piBuffer->ReadValue(KEY_TV_RTP_INACTIVITY);
    IMSList<AString> objTokens = strRTPInActivity.Split(TextParser::CHAR_COMMA);

    if (objTokens.GetSize() == 1)
    {
        nTvRtpInactivityInitial = objTokens.GetAt(0).ToInt32();
        nTvRtpInactivityRuntime = nTvRtpInactivityInitial;
        nTvRtpInactivityHold = nTvRtpInactivityInitial;
    }
    else if (objTokens.GetSize() == 2)
    {
        nTvRtpInactivityInitial = objTokens.GetAt(0).ToInt32();
        nTvRtpInactivityRuntime = objTokens.GetAt(1).ToInt32();
        nTvRtpInactivityHold = nTvRtpInactivityRuntime;
    }
    else if (objTokens.GetSize() > 2)
    {
        nTvRtpInactivityInitial = objTokens.GetAt(0).ToInt32();
        nTvRtpInactivityRuntime = objTokens.GetAt(1).ToInt32();
        nTvRtpInactivityHold = objTokens.GetAt(2).ToInt32();
    }

    return IMS_TRUE;
}

PUBLIC
IMS_SINT32 MediaConfiguration::GetSrtp() const
{
    return nSrtpEnable;
}

PUBLIC
eMMPFSrtpCryptoType MediaConfiguration::GetSrtpCryptoType() const
{
    eMMPFSrtpCryptoType eCryptoType = MMPF_SRTP_CRYPTO_TYPE_NONE;
    switch(nSrtpCryptoType)
    {
        case 2:
            eCryptoType = MMPF_SRTP_CRYPTO_TYPE_AES128_CM_SHA1_80;
            break;
        case 3:
            eCryptoType = MMPF_SRTP_CRYPTO_TYPE_AES128_CM_SHA1_32;
            break;
        case 4:
            eCryptoType = MMPF_SRTP_CRYPTO_TYPE_AES256_CM_SHA1_80;
            break;
        case 5:
            eCryptoType = MMPF_SRTP_CRYPTO_TYPE_AES256_CM_SHA1_32;
            break;
        case 6:
            eCryptoType = MMPF_SRTP_CRYPTO_TYPE_NULL_SHA1_80;
            break;
        case 7:
            eCryptoType = MMPF_SRTP_CRYPTO_TYPE_NULL_SHA1_32;
            break;
        default :
            eCryptoType = MMPF_SRTP_CRYPTO_TYPE_NONE;
            break;
    }
    return eCryptoType;
}

PUBLIC
IMS_SINT32 MediaConfiguration::GetSrtpMasterKeyLifeTime() const
{
    return nMasterKeyLifeTime;
}

PUBLIC
IMS_SINT32 MediaConfiguration::IsEmptyFmtpAttr() const
{
    return nEmptyFmtpAttr;
}

PUBLIC
IMS_BOOL MediaConfiguration::GetSrtpSupportCapaNego() const
{
    return (nSrtpSupportCapaNego == 1);
}

PUBLIC
IMSList<eCodecNameList> MediaConfiguration::GetCodecPriorityList() const
{
    return listCodecPriority;
}

/*!
 *  @brief  Get property for setting RS/RR zero though Initial media direction is inactive
 */
IMS_BOOL MediaConfiguration::IsRtcpDisableBeforeSetup() const
{
    return (nRtcpDisableBeforeSetup == 1);
}
