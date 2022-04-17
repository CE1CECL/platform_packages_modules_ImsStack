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

#ifndef _CODEC_TELEPHONE_EVENT_CONFIG_H_
#define _CODEC_TELEPHONE_EVENT_CONFIG_H_

#include "IMSList.h"
#include "config/CodecConfig.h"

class CodecTelephoneEventConfig :
        public CodecConfig
{
public:
    CodecTelephoneEventConfig();
    virtual ~CodecTelephoneEventConfig();

private:
    CodecTelephoneEventConfig(IN CONST CodecTelephoneEventConfig &objRHS);
    CodecTelephoneEventConfig& operator=(IN CONST CodecTelephoneEventConfig &objRHS);

public:
    // CodecConfig class
    virtual IMS_BOOL Create(IN IConfigBuffer *piBuffer);
    virtual AString GetFmtp() const;
    virtual AString GetRtpMap() const;
    virtual void ToDebugString() const;

    const AString& GetEvents() const;
    IMS_SINT32 GetRedundancyCount() const;

private:
    static const IMS_CHAR KEY_EVENTS[];
    static const IMS_CHAR KEY_REDUNDANCY_COUNT[];

private:
    // Events (comma-separated value list)
    AString strEvents;
    // Redundancy for reliability
    IMS_SINT32 nRedundancyCount;
};
#endif                                              // _CODEC_TELEPHONE_EVENT_CONFIG_H_
