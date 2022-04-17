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

#include "config/CodecTelephoneEventConfigEx.h"
#include "IConfigBuffer.h"
#include "config/IMSCodec.h"
#include "config/MediaSessionConfig.h"

__IMS_TRACE_TAG_USER_DECL__("CONF");

PRIVATE GLOBAL
const IMS_CHAR CodecTelephoneEventConfigEx::KEY_EVENTS[] = "events";
PRIVATE GLOBAL
const IMS_CHAR CodecTelephoneEventConfigEx::KEY_SAMPLING_RATE[] = "sampling_rate";

/*

Remarks

*/
PUBLIC
CodecTelephoneEventConfigEx::CodecTelephoneEventConfigEx(IN IMS_SINT32 nProfielNum_) :
        CodecConfig(IMSCodec::AUDIO_TELEPHONE_EVENT,
        IMSCodec::AtoString(IMSCodec::AUDIO_TELEPHONE_EVENT), nProfielNum_),
        strEvents(AString::ConstNull()),
        nRedundancyCount(-1),
        nSamplingRate(-1)
{
}

/*

Remarks

*/
PUBLIC VIRTUAL
CodecTelephoneEventConfigEx::~CodecTelephoneEventConfigEx()
{
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL CodecTelephoneEventConfigEx::Create(IN IConfigBuffer *piBuffer)
{
    if (piBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    // Set payload type & network parameter
    CodecConfig::Create(piBuffer);

    // audiocodec parameters - audio
    if (!piBuffer->CaptureSection(MediaSessionConfig::KEY_AUDIO_CODEC, GetProfileNum()))
    {
        IMS_TRACE_E(0, "Create-strSection (%d), profileNum (%s)",
                MediaSessionConfig::KEY_AUDIO_CODEC,GetProfileNum(),0);
        return IMS_FALSE;
    }

    nSamplingRate = piBuffer->ReadValueInt(KEY_SAMPLING_RATE);

    // Telephony Event parameters
    AString strSection = AString::ConstNull();
    GetSection(strSection);

    if (!piBuffer->CaptureSection(strSection.GetStr(), GetProfileNum()))
    {
        IMS_TRACE_E(0, "Create-strSection (%d), profileNum (%s)",
                strSection.GetStr(),GetProfileNum(),0);
        return IMS_FALSE;
    }

    // Events parameter
    strEvents = piBuffer->ReadValue(KEY_EVENTS);

    piBuffer->ReleaseSection();

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL
AString CodecTelephoneEventConfigEx::GetFmtp() const
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
AString CodecTelephoneEventConfigEx::GetRtpMap() const
{
    AString strRTPMAP = AString::ConstNull();

    strRTPMAP.Sprintf("%s/8000", GetCodecName().GetStr());

    return strRTPMAP;
}

/*

Remarks

*/
PUBLIC VIRTUAL
void CodecTelephoneEventConfigEx::ToDebugString() const
{
    CodecConfig::ToDebugString();
}

/*

Remarks

*/
PUBLIC
const AString& CodecTelephoneEventConfigEx::GetEvents() const
{
    return strEvents;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecTelephoneEventConfigEx::GetSamplingRate() const
{
    return nSamplingRate;
}
