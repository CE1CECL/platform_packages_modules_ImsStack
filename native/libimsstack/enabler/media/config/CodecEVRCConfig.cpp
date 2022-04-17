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
#include "config/CodecEVRCConfig.h"
#include "IConfigBuffer.h"
#include "config/IMSCodec.h"

//__IMS_TRACE_TAG_USER_DECL__("CONF");

/*

Remarks

*/
PUBLIC
CodecEVRCConfig::CodecEVRCConfig(IN IMS_SINT32 nType_, IN IMS_SINT32 nProfielNum_) :
        CodecConfig(nType_, IMSCodec::AtoString(nType_), nProfielNum_)
{
}

/*

Remarks

*/
PUBLIC VIRTUAL
CodecEVRCConfig::~CodecEVRCConfig()
{
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL CodecEVRCConfig::Create(IN IConfigBuffer *piBuffer)
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
AString CodecEVRCConfig::GetRtpMap() const
{
    AString strRtpMap = AString::ConstNull();

    strRtpMap.Sprintf("%s/8000", GetCodecName().GetStr());

    return strRtpMap;
}

/*

Remarks

*/
PUBLIC VIRTUAL
void CodecEVRCConfig::ToDebugString() const
{
    CodecConfig::ToDebugString();
}
