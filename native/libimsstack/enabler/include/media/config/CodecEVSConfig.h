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

#ifndef _CODEC_EVS_CONFIG_H_
#define _CODEC_EVS_CONFIG_H_

#include "IMSList.h"
#include "config/CodecConfig.h"

class IConfigBuffer;

class CodecEVSConfig :
        public CodecConfig
{
public:
    // EVS default mode-set/Bitrate/Bandwidth
    enum
    {
        DEFAULT_MODE_SET_AMR_WB_IO = 8      // 8 == 23.85kBPS
    };
    enum
    {
        DEFAULT_BITRATE = 6                 // 6 == 24.4Kbps
    };
    enum
    {
        DEFAULT_BANDWIDTH = 2               // 2 == SWB
    };

private:
    // EVS parameters
    static const IMS_CHAR KEY_CHANNEL[];
    static const IMS_CHAR KEY_PTIME[];
    static const IMS_CHAR KEY_MAX_PTIME[];
    static const IMS_CHAR KEY_DTX[];
    static const IMS_CHAR KEY_HF_ONLY[];
    static const IMS_CHAR KEY_EVS_MODE_SWITCH[];
    static const IMS_CHAR KEY_MAX_RED[];
    static const IMS_CHAR KEY_BITRATE[];
    static const IMS_CHAR KEY_BANDWIDTH[];
    static const IMS_CHAR KEY_CMR[];
    static const IMS_CHAR KEY_CHANNEL_AWARE_MODE[];
    static const IMS_CHAR KEY_MODE_SET[];
    static const IMS_CHAR KEY_DEFAULT_MODE_SET[];
    static const IMS_CHAR KEY_DEFAULT_BANDWIDTH[];
    static const IMS_CHAR KEY_DEFAULT_BITRATE[];
    static const IMS_CHAR KEY_SEND_CMR[];

    // EVS FrameType
    enum
    {
        EVS_PRIMARY_5900 = 0,
        EVS_PRIMARY_7200 = 1,
        EVS_PRIMARY_8000 = 2,
        EVS_PRIMARY_9600 = 3,
        EVS_PRIMARY_13200 = 4,
        EVS_PRIMARY_16400 = 5,
        EVS_PRIMARY_24400 = 6,
        EVS_PRIMARY_32000 = 7,
        EVS_PRIMARY_48000 = 8,
        EVS_PRIMARY_64000 = 9,
        EVS_PRIMARY_96000 = 10,
        EVS_PRIMARY_128000 = 11
    };

public:
    enum
    {
        EVS_BANDWIDTH_NB = 0,
        EVS_BANDWIDTH_WB = 1,
        EVS_BANDWIDTH_SWB = 2,
        EVS_BANDWIDTH_FB = 3
    };

public:
    CodecEVSConfig(IN IMS_SINT32 nType_, IN IMS_SINT32 nProfielNum_);
    virtual ~CodecEVSConfig();

private:
    CodecEVSConfig();
    CodecEVSConfig(IN CONST CodecEVSConfig &objRHS);
    CodecEVSConfig& operator=(IN CONST CodecEVSConfig &objRHS);

public:
    // CodecConfig class
    virtual IMS_BOOL Create(IN IConfigBuffer *piBuffer);
    virtual AString GetFmtp() const;
    virtual AString GetRtpMap() const;
    virtual void ToDebugString() const;
    IMS_SINT32 GetChannel() const;
    IMS_SINT32 GetPtime() const;
    IMS_SINT32 GetMaxPtime() const;
    IMS_SINT32 GetDtx() const;
    IMS_SINT32 GetHfOnly() const;
    IMS_SINT32 GetEvsModeSwitch() const;
    IMS_SINT32 GetMaxRed() const;
    IMS_UINT32 GetBrList() const;
    IMS_SINT32 GetBr() const;
    IMS_UINT32 GetBwList() const;
    IMS_SINT32 GetBw() const;
    IMS_SINT32 GetCmr() const;
    IMS_SINT32 GetChAwareRecv() const;
    IMS_UINT32 GetModeSetList() const;
    IMS_UINT32 GetModeSet() const;
    IMS_UINT32 GetDefaultRptModeSet() const;
    IMS_UINT32 GetDefaultBandwidth() const;
    IMS_UINT32 GetDefaultBitrate() const;
    IMS_BOOL GetSendCmr() const;

private:
    IMS_UINT32 ConvertEvsBitrateToList(IN AString strBitrate) const;
    IMS_UINT32 ConvertEvsBandwidthToList(IN AString strBandwidth) const;
    IMS_UINT32 ConvertEvsModeSetToList(IN AString strModeSet) const;
    IMS_SINT32 GetEvsBandwidthFromList(IN IMS_UINT32 nBandwidthList) const;
    IMS_SINT32 GetEvsBitrateFromList(IN IMS_UINT32 nBitrateList) const;
    IMS_SINT32 GetAmrIoModeSetFromList(IN IMS_UINT32 nAmrIoModeSet) const;

private:
    IMS_SINT32 nChannel;
    IMS_SINT32 nPtime;
    IMS_SINT32 nMaxPtime;
    IMS_SINT32 nDtx;                // 1(default) is turn on DTX
    //IMS_UINT32 nDtx_Recv;         // 1(default) is turn on DTX
    IMS_SINT32 nHfOnly;             // 0(default) is compact and hf format used,
                                    //  other is only hf format used
    IMS_SINT32 nEvsModeSwitch;      // 0(default) is "primary mode start"
    IMS_SINT32 nMaxRed;

    // Primary parameter
    IMS_UINT32 nBrList;             // EVS primary mode bitrate range (kbps)
    //IMS_SINT32 nBrSend;           // EVS primary mode bitrate range (kbps)
                                    // - only send direction (used at sendrecv/sendonly direction)
    //IMS_SINT32 nBrRecv;           // EVS primary mode bitrate range (kbps)
                                    // - only recv direction (used at sendrecv/recvonly direction)
    IMS_UINT32 nBwList;             // bw has a value from the set:
                                    // nb, wb, swb, fb, nb-wb, nb-swb, and nb-fb. nb, wb, swb, fb
    //IMS_SINT32 nBwSend;
    //IMS_SINT32 nBwRecv;
    IMS_SINT32 nCmr;
    // multiple mono ch is not supported yet..
    //IMS_SINT32 nCh_Send;          // the number of send direction audio channel
    //IMS_SINT32 nCh_Recv;          // the number of recv direction audio channel
    IMS_SINT32 nChAwRecv;           // -1 is channel aware mode disable,
                                    // 0(default) is not used at the start of the session,
                                    // but it'll be changed using CMR or RTCP app.
    // AMR-WB IO parameter
    IMS_UINT32 nModeSetList;
    IMS_UINT32 nDefaultRtpModeSet;   // default mode-set

    // default bandwidth & default bitrate
    IMS_UINT32 nDefaultBandwidth;   // default mode-set
    IMS_UINT32 nDefaultBitrate;     // default mode-set
    IMS_UINT32 nSendCmr;            // send CMR
};
#endif                                              // _CODEC_EVS_CONFIG_H_
