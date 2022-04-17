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
#include "AStringBuffer.h"
#include "IConfigBuffer.h"
#include "config/IMSCodec.h"
#include "config/MediaSessionConfig.h"
#include "config/CodecT140Config.h"

__IMS_TRACE_TAG_USER_DECL__("CONF");

PRIVATE GLOBAL
const IMS_CHAR CodecT140Config::KEY_REDUNDANCY[] = "redundancy";
PRIVATE GLOBAL
const IMS_CHAR CodecT140Config::KEY_RED_SUBTYPE[] = "red_subtype";
PRIVATE GLOBAL
const IMS_CHAR CodecT140Config::KEY_SAMPLING_RATE[] = "sampling_rate";

PUBLIC
CodecT140Config::CodecT140Config(IN IMS_SINT32 nType_, IN IMS_SINT32 nProfielNum_) :
        CodecConfig(nType_, IMSCodec::TtoString(nType_), nProfielNum_),
        nRedLevel(-1),
        nRedPayload(-1),
        nSamplingRate(1000)
{
}

PUBLIC VIRTUAL
CodecT140Config::~CodecT140Config()
{
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL CodecT140Config::Create(IN IConfigBuffer *piBuffer)
{
    if (piBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    // Set payload type & network parameter
    CodecConfig::Create(piBuffer);

    // textcodec parameters - text
    if (!piBuffer->CaptureSection(MediaSessionConfig::KEY_TEXT_CODEC, GetProfileNum()))
    {
        IMS_TRACE_E(0, "Create-strSection (%d), profileNum (%s)",
                MediaSessionConfig::KEY_TEXT_CODEC,GetProfileNum(),0);
        return IMS_FALSE;
    }

    // Sampling Rate parameter
    nSamplingRate = piBuffer->ReadValueInt(KEY_SAMPLING_RATE);

    // T140/RED parameters
    AString strSection;
    GetSection(strSection);
    if (!piBuffer->CaptureSection(strSection.GetStr(), GetProfileNum()))
    {
        IMS_TRACE_E(0, "Create-strSection (%s), profileNum (%d)",
                strSection.GetStr(),GetProfileNum(),0);
        return IMS_FALSE;
    }

    // Redundancy parameter
    nRedLevel= piBuffer->ReadValueInt(KEY_REDUNDANCY);

    // SubPayloadType for Redundancy
    nRedPayload = piBuffer->ReadValueInt(KEY_RED_SUBTYPE);

    IMS_TRACE_D("Create-strSection (%s), profileNum (%d)", strSection.GetStr(),GetProfileNum(),0);
    IMS_TRACE_D("nRedLevel (%d), nRedPayload (%d)", nRedLevel, nRedPayload, 0);

    piBuffer->ReleaseSection();

    if (GetCodecName().Equals("red") && nRedLevel <= 1)
    {
        IMS_TRACE_E(0, "'red' attribute needs more than 1 of redundancy (%d>1)", nRedLevel , 0, 0);
        //return IMS_FALSE;
    }
    else if (GetCodecName().Equals("red") && (nRedPayload < 0))
    {
        IMS_TRACE_E(0, "'red' attribute needs SubPT (%d)", nRedPayload, 0, 0);
        return IMS_FALSE;
    }
    else if (GetCodecName().Equals("t140"))
    {
        nRedLevel = 1;
        nRedPayload = -1;
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL
AString CodecT140Config::GetFmtp() const
{
    AStringBuffer objFMTP(64);

    if (GetCodecType() == IMSCodec::TEXT_RED)
    {
        IMS_SINT32 nTempRed = nRedLevel;

        while(nTempRed-->0)
        {
            if (objFMTP.GetLength() > 0)
            {
                objFMTP.Append("/");
            }
            objFMTP.Append(nRedPayload);
        }
    }

    return static_cast<const AStringBuffer&>(objFMTP).GetString();
}

/*

Remarks

*/
PUBLIC VIRTUAL
AString CodecT140Config::GetRtpMap() const
{
    AString strRTPMAP;

    strRTPMAP.Sprintf("%d/%d", GetCodecName().GetStr(), GetSamplingRate());

    return strRTPMAP;
}

/*

Remarks

*/
PUBLIC VIRTUAL
void CodecT140Config::ToDebugString() const
{
    CodecConfig::ToDebugString();

    IMS_TRACE_D("nRedLevel (%d), nRedPayload (%d), nSamplingRate (%d)",
            nRedLevel, nRedPayload, nSamplingRate);
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecT140Config::GetRedLevel() const
{
    return nRedLevel;
}

PUBLIC
IMS_SINT32 CodecT140Config::GetRedPayload() const
{
    return nRedPayload;
}

PUBLIC
IMS_SINT32 CodecT140Config::GetSamplingRate() const
{
    return nSamplingRate;
}
