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
#include "IConfigBuffer.h"
#include "config/IMSCodec.h"
#include "config/CodecTelephoneEventConfig.h"

__IMS_TRACE_TAG_USER_DECL__("CONF");

PRIVATE GLOBAL
const IMS_CHAR CodecTelephoneEventConfig::KEY_EVENTS[] = "events";
PRIVATE GLOBAL
const IMS_CHAR CodecTelephoneEventConfig::KEY_REDUNDANCY_COUNT[] = "redundancy_count";

PUBLIC
CodecTelephoneEventConfig::CodecTelephoneEventConfig() :
        CodecConfig(IMSCodec::AUDIO_TELEPHONE_EVENT,
                IMSCodec::AtoString(IMSCodec::AUDIO_TELEPHONE_EVENT)),
        strEvents(AString::ConstNull()),
        nRedundancyCount(5)
{
}

PUBLIC VIRTUAL
CodecTelephoneEventConfig::~CodecTelephoneEventConfig()
{
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL CodecTelephoneEventConfig::Create(IN IConfigBuffer *piBuffer)
{
    if (piBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    AString strSection;

    GetSection(strSection);

    if (!piBuffer->CaptureSection(strSection.GetStr()))
    {
        return IMS_FALSE;
    }

    // Set payload type parameter
    CodecConfig::Create(piBuffer);

    // Events parameter
    strEvents = piBuffer->ReadValue(KEY_EVENTS);

    // Redundancy count parameter
    nRedundancyCount = piBuffer->ReadValueInt(KEY_REDUNDANCY_COUNT);

    piBuffer->ReleaseSection();

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL
AString CodecTelephoneEventConfig::GetFmtp() const
{
    if (strEvents.GetLength() > 0)
    {
        return strEvents;
    }

    return AString::ConstNull();
}

/*

Remarks

*/
PUBLIC VIRTUAL
AString CodecTelephoneEventConfig::GetRtpMap() const
{
    AString strRTPMAP;

    strRTPMAP.Sprintf("%s/8000", GetCodecName().GetStr());

    return strRTPMAP;
}

/*

Remarks

*/
PUBLIC VIRTUAL
void CodecTelephoneEventConfig::ToDebugString() const
{
    CodecConfig::ToDebugString();

    IMS_TRACE_D("events (%s), redundancy_count (%d)", strEvents.GetStr(), nRedundancyCount, 0);
}

/*

Remarks

*/
PUBLIC
const AString& CodecTelephoneEventConfig::GetEvents() const
{
    return strEvents;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecTelephoneEventConfig::GetRedundancyCount() const
{
    return nRedundancyCount;
}
