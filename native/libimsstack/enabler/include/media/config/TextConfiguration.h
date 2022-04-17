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

#ifndef _TEXT_CONFIG_H_
#define _TEXT_CONFIG_H_

#include "config/CodecConfig.h"
#include "config/MediaConfiguration.h"

class IConfigBuffer;

class TextConfiguration :
        public MediaConfiguration
{
public:
    TextConfiguration(MEDIA_CONTENT_TYPE eSessionType);
    virtual ~TextConfiguration();

private:
    TextConfiguration(IN CONST TextConfiguration &objRHS);
    TextConfiguration& operator=(IN CONST TextConfiguration &objRHS);

public:
    virtual IMS_BOOL Create(IN IConfigBuffer *piBuffer, IN IMS_UINT32 nIndex,
            IN IMS_SINT32 nSlotID);
    virtual void ToDebugString() const;

    IMS_SINT32 GetPdpProfileNum() const;
    IMS_BOOL GetKeepRedLevel() const;

    const CodecConfig* GetCodecConfig() const;
    const IMSList<CodecConfig*>& GetCodecConfigs() const;

    IMS_BOOL Update(IN IConfigBuffer *piBuffer, IN IMS_UINT32 nIndex);

protected:
    void Clear();
    IMS_BOOL CreateCodecConfigs(IN CONST AString &strRef, IN CONST IMS_SINT32 nCodecListSize,
            IN IMS_SINT32 nSlotID = 0);

private:

    // PDP Profile number for CP audio
    IMS_SINT32 nPdpProfileNum;
    IMS_SINT32 nPdpProfileNumOf3G;
    IMS_SINT32 nKeepRedlevel;

    static const IMS_CHAR KEY_KEEP_REDLEVEL[];

    // Provisioned codecs
    IMSList<CodecConfig*> objCodecConfigs;
};
#endif                                              // _TEXT_CONFIG_H_
