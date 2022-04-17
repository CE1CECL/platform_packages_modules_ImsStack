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

#ifndef _CODEC_PCM_CONFIG_H_
#define _CODEC_PCM_CONFIG_H_

#include "IMSList.h"
#include "config/CodecConfig.h"

class CodecPCMConfig :
        public CodecConfig
{
public:
    CodecPCMConfig(IN IMS_SINT32 nType_, IN IMS_SINT32 nProfielNum_);
    virtual ~CodecPCMConfig();

private:
    CodecPCMConfig();
    CodecPCMConfig(IN CONST CodecPCMConfig &objRHS);
    CodecPCMConfig& operator=(IN CONST CodecPCMConfig &objRHS);

public:
    // CodecConfig class
    virtual IMS_BOOL Create(IN IConfigBuffer *piBuffer);
    virtual AString GetRtpMap() const;
    virtual void ToDebugString() const;

private:
};
#endif                                              // _CODEC_PCM_CONFIG_H_
