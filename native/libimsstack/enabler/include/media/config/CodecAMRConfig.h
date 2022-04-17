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

#ifndef _CODEC_AMR_CONFIG_H_
#define _CODEC_AMR_CONFIG_H_

#include "IMSList.h"
#include "config/CodecConfig.h"

class IConfigBuffer;

class CodecAMRConfig :
        public CodecConfig
{
public:
    // AMR default mode-set
    enum
    {
        DEFAULT_MODE_SET_AMR = 7
    };
    enum {
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
    static const IMS_CHAR KEY_MODE_CHANGE_CAPABILITY[];
    static const IMS_CHAR KEY_MODE_SET[];
    static const IMS_CHAR KEY_OCTET_ALIGN[];

public:
    CodecAMRConfig(IN IMS_SINT32 nType_);
    virtual ~CodecAMRConfig();

private:
    CodecAMRConfig();
    CodecAMRConfig(IN CONST CodecAMRConfig &objRHS);
    CodecAMRConfig& operator=(IN CONST CodecAMRConfig &objRHS);

public:
    // CodecConfig class
    virtual IMS_BOOL Create(IN IConfigBuffer *piBuffer);
    virtual AString GetFmtp() const;
    virtual AString GetRtpMap() const;
    virtual void ToDebugString() const;

    IMS_SINT32 GetModeChangeCapability() const;
    IMS_SINT32 GetModeSet() const;
    IMS_UINT32 GetModeSets() const;
    IMS_SINT32 GetOctetAlign() const;

private:
    // AMR
    //    4.75 (0)
    //    5.15 (1)
    //    5.9 (2)
    //    6.7 (3)
    //    7.4 (4)
    //    7.95 (5)
    //    10.2 (6)
    //    12.2 (7) : default
    //
    // AMR-WB
    //    6.6 (0)
    //    8.85 (1)
    //    12.65 (2) : default
    //    14.25 (3)
    //    15.85 (4)
    //    18.25 (5)
    //    19.85 (6)
    //    23.05 (7)
    //    23.85 (8)
    IMS_UINT32 nModeSetList;
    IMS_SINT32 nModeChangeCapability;
    // octet-align (0), bandwidth-efficient (1)
    IMS_SINT32 nOctetAlign;
};
#endif                                              // _CODEC_AMR_CONFIG_H_
