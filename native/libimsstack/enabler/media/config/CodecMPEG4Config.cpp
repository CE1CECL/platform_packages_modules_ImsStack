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

#include "config/CodecMPEG4Config.h"
#include "IConfigBuffer.h"
#include "config/IMSCodec.h"

//__IMS_TRACE_TAG_USER_DECL__("CONF");

PUBLIC
CodecMPEG4Config::CodecMPEG4Config() :
        CodecConfig(IMSCodec::VIDEO_MPEG4, IMSCodec::VtoString(IMSCodec::VIDEO_MPEG4))
{
}

PUBLIC VIRTUAL
CodecMPEG4Config::~CodecMPEG4Config()
{
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL CodecMPEG4Config::Create(IN IConfigBuffer *piBuffer)
{

    if (piBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    // Set payload type parameter
    CodecConfig::Create(piBuffer);

    AString strSection = AString::ConstNull();

    GetSection(strSection);

    if (!piBuffer->CaptureSection(strSection.GetStr()))
    {
        return IMS_FALSE;
    }

    piBuffer->ReleaseSection();

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL
void CodecMPEG4Config::ToDebugString() const
{
    CodecConfig::ToDebugString();
}
