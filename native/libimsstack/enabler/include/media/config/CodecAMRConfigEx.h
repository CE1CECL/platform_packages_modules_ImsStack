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

#ifndef _CODEC_AMR_CONFIG_EX_H_
#define _CODEC_AMR_CONFIG_EX_H_

#include "IMSList.h"
#include "config/CodecConfig.h"

class CodecAMRConfigEx :
        public CodecConfig
{
public:
    CodecAMRConfigEx(IN IMS_SINT32 nType_, IN IMS_SINT32 nProfielNum_);
    virtual ~CodecAMRConfigEx();

private:
    CodecAMRConfigEx();
    CodecAMRConfigEx(IN CONST CodecAMRConfigEx &objRHS);
    CodecAMRConfigEx& operator=(IN CONST CodecAMRConfigEx &objRHS);

public:
    // CodecConfig class
    virtual IMS_BOOL Create(IN IConfigBuffer *piBuffer);
    virtual AString GetFmtp() const;
    virtual AString GetRtpMap() const;
    virtual void ToDebugString() const;

    IMS_SINT32 GetSamplingRate() const;
    IMS_SINT32 GetChannel() const;
    IMS_UINT32 GetDefaultRtpModeSet() const;
    IMS_SINT32 GetModeSet() const;
    IMS_UINT32 GetModeSets() const;
    IMS_SINT32 GetOctetAlign() const;

    IMS_SINT32 GetModeChangeCapability() const;
    IMS_SINT32 GetModeChangePeriod() const;
    IMS_SINT32 GetModeChangeNeighbor() const;
    IMS_SINT32 GetMaxRed() const;
    IMS_SINT32 GetRobustSorting() const;
    IMS_SINT32 GetPtime() const;
    IMS_SINT32 GetMaxPtime() const;
    IMS_BOOL GetScr() const;

public:
    // AMR default mode-set
    enum
    {
        DEFAULT_MODE_SET_AMR = 7
    };
    enum
    {
        DEFAULT_MODE_SET_AMR_WB = 8
    };

    // Packetization modes
    enum
    {
        MODE_NONE = (-1),
        MODE_OCTET_ALIGN = 0,
        MODE_BANDWIDTH_EFFICIENT
    };

private:
    // "AMR" / "AMR-WB" parameters
    static const IMS_CHAR KEY_CHANNEL[];
    static const IMS_CHAR KEY_MODE_SET[];
    static const IMS_CHAR KEY_DEFAULT_RTP_MODESET[];
    static const IMS_CHAR KEY_OCTET_ALIGN[];
    static const IMS_CHAR KEY_MODE_CHANGE_CAPABILITY[];
    static const IMS_CHAR KEY_MODE_CHANGE_PERIOD[];
    static const IMS_CHAR KEY_MODE_CHANGE_NEIGHBOR[];
    static const IMS_CHAR KEY_MAX_RED[];
    static const IMS_CHAR KEY_ROBUST_SORTING[];
    static const IMS_CHAR KEY_PTIME[];
    static const IMS_CHAR KEY_MAX_PTIME[];
    static const IMS_CHAR KEY_SAMPLING_RATE[];
    static const IMS_CHAR KEY_SCR[];

private:

    //
    // Payload Num
    //    RTP Payload Number
    //    -1 means default value should be used.
    //    otherwise the value will be used for the rtp payload number
    //

    //
    // Mode Set
    //
    //    If there are no value in objModeSets, it means 'mode-set' should be hided.
    //    Otherwise 'mode-set' will be presented in SDP
    //    and the values in objModeSets will be listed as 'mode-set' values.
    //
    //     AMR
    //        4.75 (0)
    //        5.15 (1)
    //        5.9 (2)
    //        6.7 (3)
    //        7.4 (4)
    //        7.95 (5)
    //        10.2 (6)
    //        12.2 (7)
    //
    //     AMR-WB
    //        6.6 (0)
    //        8.85 (1)
    //        12.65 (2)
    //        14.25 (3)
    //        15.85 (4)
    //        18.25 (5)
    //        19.85 (6)
    //        23.05 (7)
    //        23.85 (8)

    //
    // Other items
    //    -1 means that this item shold NOT be presented in SDP.
    //    If each item has other value, the item will be presented in SDP with the value.
    //

    IMS_SINT32 nChannel;
    IMS_UINT32 nModeSetList;    // 0 means support all mode set
    IMS_UINT32 nDefaultRtpModeSet;    // default mode-set support

    IMS_SINT32 nOctetAlign;
    IMS_SINT32 nSamplingRate;
    IMS_SINT32 nModeChangeCapability;
    IMS_SINT32 nModeChangePeriod;
    IMS_SINT32 nModeChangeNeighbor;
    IMS_SINT32 nMaxRed;
    IMS_SINT32 nRobustSorting;
    IMS_SINT32 nPtime;
    IMS_SINT32 nMaxPtime;
    IMS_SINT32 nScr;
};
#endif                                              // _CODEC_AMR_CONFIG_EX_H_
