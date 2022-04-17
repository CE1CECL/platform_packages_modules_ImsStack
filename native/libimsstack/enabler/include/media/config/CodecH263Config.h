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

#ifndef _CODEC_H263_CONFIG_H_
#define _CODEC_H263_CONFIG_H_

#include "config/CodecConfig.h"

class CodecH263Config :
        public CodecConfig
{
public:
    CodecH263Config(IN IMS_SINT32 nType_, IN IMS_SINT32 nProfielNum_);
    virtual ~CodecH263Config();

private:
    CodecH263Config();
    CodecH263Config(IN CONST CodecH263Config &objRHS);
    CodecH263Config& operator=(IN CONST CodecH263Config &objRHS);

public:
    // CodecConfig class
    virtual IMS_BOOL Create(IN IConfigBuffer *piBuffer);
    virtual AString GetFmtp() const;
    virtual AString GetRtpMap() const;
    virtual void ToDebugString() const;

    IMS_SINT32 GetResolutionId() const ;
    IMS_SINT32 GetFramerate() const ;
    IMS_SINT32 GetBitrate() const ;
    IMS_SINT32 GetAs(IMS_BOOL bIsIPv6 = IMS_FALSE) const ;
    const AString& GetProfileLevelId() const ;
    IMS_BOOL GetIncludeFrameSize() const ;
    IMS_BOOL GetIncludeImageAttr() const ;

    IMS_SINT32 GetProfile() const;
    IMS_SINT32 GetLevel() const;
    IMS_BOOL GetIncludeQcif() const;

private:
    static const IMS_CHAR KEY_RESOLUTION[];
    static const IMS_CHAR KEY_FRAMERATE[];
    static const IMS_CHAR KEY_BITRATE[];
    static const IMS_CHAR KEY_AS[];
    static const IMS_CHAR KEY_PROFILE_LEVEL_ID[];
    static const IMS_CHAR KEY_FRAMESIZE[];
    static const IMS_CHAR KEY_IMAGEATTR[];

    static const IMS_CHAR KEY_PROFILE[];
    static const IMS_CHAR KEY_LEVEL[];
    static const IMS_CHAR KEY_QCIF[];

private:
    // videocodec parameters
    IMS_SINT32 nResolutionId;
    IMS_SINT32 nFramerate;
    IMS_SINT32 nBitrate;
    IMS_SINT32 nAs[2];              // AS List AS[0] IPv4, AS[1] IPv6 2014.1.21
    AString strProfileLevelId;
    IMS_BOOL bIncludeFrameSize;
    IMS_BOOL bIncludeImageAttr;

    IMS_SINT32 nProfile;
    IMS_SINT32 nLevel;
    IMS_BOOL bIncludeQcif;

};
#endif                                              // _CODEC_H263_CONFIG_H_
