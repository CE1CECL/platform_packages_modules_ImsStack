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

#ifndef _CODEC_H264_CONFIG_H_
#define _CODEC_H264_CONFIG_H_

#include "config/CodecConfig.h"

class CodecH264Config :
        public CodecConfig
{
private:
    static const IMS_CHAR KEY_RESOLUTION[];
    static const IMS_CHAR KEY_FRAMERATE[];
    static const IMS_CHAR KEY_BITRATE[];
    static const IMS_CHAR KEY_AS[];
    static const IMS_CHAR KEY_PROFILE_LEVEL_ID[];
    static const IMS_CHAR KEY_CHP_PROFILE_LEVEL_ID[];
    static const IMS_CHAR KEY_FRAMESIZE[];
    static const IMS_CHAR KEY_IMAGEATTR[];
    static const IMS_CHAR KEY_PACKETIZATION_MODE[];
    static const IMS_CHAR KEY_SPROP_PARAMETER_SETS[];
    static const IMS_CHAR KEY_MAX_MBPS[];
    static const IMS_CHAR KEY_MAX_FS[];
    static const IMS_CHAR KEY_MAX_CBP[];
    static const IMS_CHAR KEY_MAX_DPB[];
    static const IMS_CHAR KEY_MAX_BR[];

public:
    CodecH264Config(IN IMS_SINT32 nProfielNum_);
    virtual ~CodecH264Config();

private:
    CodecH264Config(IN CONST CodecH264Config &objRHS);
    CodecH264Config& operator=(IN CONST CodecH264Config &objRHS);

public:
    // CodecConfig class
    virtual IMS_BOOL Create(IN IConfigBuffer *piBuffer);
    virtual void ToDebugString() const;
    IMS_SINT32 GetResolutionId() const ;
    IMS_SINT32 GetFramerate() const ;
    IMS_SINT32 GetBitrate() const ;
    IMS_SINT32 GetAs(IN IMS_BOOL bIsIPv6 = IMS_FALSE) const ;
    const AString& GetProfileLevelId() const ;
    const AString& GetChpProfileLevelId() const ;
    IMS_BOOL GetIncludeFrameSize() const ;
    IMS_BOOL GetIncludeImageAttr() const ;
    IMS_SINT32 GetPacketizationMode() const;
    IMS_BOOL GetIncludeSpropParameterSets() const;
    IMS_SINT32 GetMaxMbps() const;
    IMS_SINT32 GetMaxFs() const;
    IMS_SINT32 GetMaxCbp() const;
    IMS_SINT32 GetMaxDpb() const;
    IMS_SINT32 GetMaxBr() const;

private:
    // videocodec parameters
    IMS_SINT32 nResolutionId;              // DB index
    IMS_SINT32 nFramerate;
    IMS_SINT32 nBitrate;
    IMS_SINT32 nAs[2];              // AS List AS[0] IPv4, AS[1] IPv6 2014.1.21
    AString strProfileLevelId;
    AString strChpProfileLevelId;
    IMS_BOOL bIncludeFrameSize;
    IMS_BOOL bIncludeImageAttr;

    // h264 parameters
    IMS_SINT32 nPacketizationMode;
    IMS_BOOL bIncludeSpropParameterSets;
    IMS_SINT32 nMaxMbps;
    IMS_SINT32 nMaxFs;
    IMS_SINT32 nMaxCbp;
    IMS_SINT32 nMaxDpb;
    IMS_SINT32 nMaxBr;
};
#endif                                              // _CODEC_H264_CONFIG_H_
