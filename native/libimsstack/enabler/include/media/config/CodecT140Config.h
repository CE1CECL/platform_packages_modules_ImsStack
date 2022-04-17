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

#ifndef _CODEC_T140_CONFIG_H_
#define _CODEC_T140_CONFIG_H_

#include "config/CodecConfig.h"

class CodecT140Config :
        public CodecConfig
{
public:
    CodecT140Config(IN IMS_SINT32 nType_, IN IMS_SINT32 nProfielNum_);
    virtual ~CodecT140Config();

private:
    CodecT140Config();
    CodecT140Config(IN CONST CodecT140Config &objRHS);
    CodecT140Config& operator=(IN CONST CodecT140Config &objRHS);

public:
    // CodecConfig class
    virtual IMS_BOOL Create(IN IConfigBuffer *piBuffer);
    virtual AString GetFmtp() const;
    virtual AString GetRtpMap() const;
    virtual void ToDebugString() const;

    IMS_SINT32 GetRedLevel() const;
    IMS_SINT32 GetRedPayload() const;
    IMS_SINT32 GetSamplingRate() const;

private:
    // "T140" parameters
    static const IMS_CHAR KEY_REDUNDANCY[];
    static const IMS_CHAR KEY_RED_SUBTYPE[];
    static const IMS_CHAR KEY_SAMPLING_RATE[];

private:
    IMS_SINT32 nRedLevel;
    IMS_SINT32 nRedPayload;
    IMS_SINT32 nSamplingRate;
};
#endif                                              // _CODEC_T140_CONFIG_H_
