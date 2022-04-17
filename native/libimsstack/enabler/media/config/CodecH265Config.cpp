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
#include "TextParser.h"
#include "config/IMSCodec.h"
#include "config/MediaSessionConfig.h"
#include "config/CodecH265Config.h"

__IMS_TRACE_TAG_USER_DECL__("CONF");

const IMS_CHAR CodecH265Config::KEY_RESOLUTION[] = "resolution";
const IMS_CHAR CodecH265Config::KEY_FRAMERATE[] = "framerate";
const IMS_CHAR CodecH265Config::KEY_BITRATE[] = "bitrate";
const IMS_CHAR CodecH265Config::KEY_AS[] = "AS";
const IMS_CHAR CodecH265Config::KEY_FRAMESIZE[] = "framesize";
const IMS_CHAR CodecH265Config::KEY_IMAGEATTR[] = "imageattr";

const IMS_CHAR CodecH265Config::KEY_PROFILE[] = "profile";
const IMS_CHAR CodecH265Config::KEY_LEVEL[] = "level";
const IMS_CHAR CodecH265Config::KEY_PACKETIZATION_MODE[] = "packetization_mode";
const IMS_CHAR CodecH265Config::KEY_SPROP_PARAMETER_SETS[] = "sprop_parameter_sets";
const IMS_CHAR CodecH265Config::KEY_MAX_MBPS[] = "max_mbps";
const IMS_CHAR CodecH265Config::KEY_MAX_FS[] = "max_fs";
const IMS_CHAR CodecH265Config::KEY_MAX_CBP[] = "max_cpb";
const IMS_CHAR CodecH265Config::KEY_MAX_DPB[] = "max_dpb";
const IMS_CHAR CodecH265Config::KEY_MAX_BR[] = "max_br";

PUBLIC
CodecH265Config::CodecH265Config(IN IMS_SINT32 nProfielNum_) :
        CodecConfig(IMSCodec::VIDEO_H265, IMSCodec::VtoString(IMSCodec::VIDEO_H265), nProfielNum_),
        nResolutionId(4),
        nFramerate(30),
        nBitrate(1000),
        nProfile(0),
        nLevel(0),
        bIncludeFrameSize(IMS_FALSE),
        bIncludeImageAttr(IMS_FALSE),
        nPacketizationMode(1),
        bIncludeSpropParameterSets(IMS_TRUE),
        nMaxMbps(-1),
        nMaxFs(-1),
        nMaxCbp(-1),
        nMaxDpb(-1),
        nMaxBr(-1)
{
}

PUBLIC VIRTUAL
CodecH265Config::~CodecH265Config()
{
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL CodecH265Config::Create(IN IConfigBuffer *piBuffer)
{
    if (piBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    // videocodec parameters - audio/video common ( payload type & network)
    CodecConfig::Create(piBuffer);

    // videocodec parameters - video
    if (!piBuffer->CaptureSection(MediaSessionConfig::KEY_VIDEO_CODEC, GetProfileNum()))
    {
        IMS_TRACE_E(0, "Create-strSection (%d), profileNum (%s)",
                MediaSessionConfig::KEY_VIDEO_CODEC,GetProfileNum(),0);
        return IMS_FALSE;
    }

    IMSList<AString> objTokens;
    nResolutionId           = piBuffer->ReadValueInt(KEY_RESOLUTION);
    nFramerate              = piBuffer->ReadValueInt(KEY_FRAMERATE);
    nBitrate                = piBuffer->ReadValueInt(KEY_BITRATE);
    bIncludeFrameSize       = piBuffer->ReadValueBoolean(KEY_FRAMESIZE);
    bIncludeImageAttr       = piBuffer->ReadValueBoolean(KEY_IMAGEATTR);

    // get IPv4, IPv6 AS - 2014.1.21
    const AString &strBandwidthAS = piBuffer->ReadValue(KEY_AS);
    objTokens = strBandwidthAS.Split(TextParser::CHAR_COMMA);
    if (objTokens.GetSize() == 1)
    {
        nAs[0] = objTokens.GetAt(0).ToInt32();    // IPv4
        nAs[1] = objTokens.GetAt(0).ToInt32();    // IPv6
    }
    else if (objTokens.GetSize() == 2)
    {
        nAs[0] = objTokens.GetAt(0).ToInt32();    // IPv4
        nAs[1] = objTokens.GetAt(1).ToInt32();    // IPv6
    }
    else
    {
        nAs[0] = 0;
        nAs[1] = 0;
    }

    // h265 parameters
    AString strSection;
    GetSection(strSection);
    if (!piBuffer->CaptureSection(strSection.GetStr(), GetProfileNum()))
    {
        return IMS_FALSE;
    }

    nProfile                        = piBuffer->ReadValueInt(KEY_PROFILE);
    nLevel                          = piBuffer->ReadValueInt(KEY_LEVEL);
    nPacketizationMode              = piBuffer->ReadValueInt(KEY_PACKETIZATION_MODE);
    bIncludeSpropParameterSets      = piBuffer->ReadValueBoolean(KEY_SPROP_PARAMETER_SETS);
    nMaxMbps                        = piBuffer->ReadValueInt(KEY_MAX_MBPS);
    nMaxFs                          = piBuffer->ReadValueInt(KEY_MAX_FS);
    nMaxCbp                         = piBuffer->ReadValueInt(KEY_MAX_CBP);
    nMaxDpb                         = piBuffer->ReadValueInt(KEY_MAX_DPB);
    nMaxBr                          = piBuffer->ReadValueInt(KEY_MAX_BR);

    piBuffer->ReleaseSection();

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL
void CodecH265Config::ToDebugString() const
{
    CodecConfig::ToDebugString();

    IMS_TRACE_D("nResolutionId (%d), bIncludeImageAttr(%d), bIncludeFrameSize (%d)",
            nResolutionId, bIncludeImageAttr, bIncludeFrameSize);
    IMS_TRACE_D("nFramerate (%d), nBitrate (%d), nPacketizationMode(%d)",
            nFramerate, nBitrate, nPacketizationMode);
    IMS_TRACE_D("Profile (%d), Level (%d), bIncludeSpropParameterSets (%d)",
            nProfile, nLevel, bIncludeSpropParameterSets);
    IMS_TRACE_D("nAs IPv4(%d) IPv6(%d)", nAs[0], nAs[1], 0);
}

PUBLIC
IMS_SINT32 CodecH265Config::GetResolutionId() const
{
    return nResolutionId;
}

PUBLIC
IMS_SINT32 CodecH265Config::GetBitrate() const
{
    return nBitrate;
}

PUBLIC
IMS_SINT32 CodecH265Config::GetAs(IMS_BOOL bIsIPv6) const
{
    if (IMS_TRUE == bIsIPv6)
    {
        return nAs[1];
    }
    else
    {
        return nAs[0];
    }
}

PUBLIC
IMS_SINT32 CodecH265Config::GetProfile() const
{
    if (nProfile < 0)
    {
        return 0;
    }
    else
    {
        return (IMS_UINT32)nProfile;
    }
}

PUBLIC
IMS_SINT32 CodecH265Config::GetLevel() const
{
    return nLevel;
}

PUBLIC
IMS_SINT32 CodecH265Config::GetFramerate() const
{
    return nFramerate;
}

PUBLIC
IMS_BOOL CodecH265Config::GetIncludeFrameSize() const
{
    return bIncludeFrameSize;
}

PUBLIC
IMS_BOOL CodecH265Config::GetIncludeImageAttr() const
{
    return bIncludeImageAttr;
}

IMS_SINT32 CodecH265Config::GetPacketizationMode() const
{
    return nPacketizationMode;
}

PUBLIC
IMS_BOOL CodecH265Config::GetIncludeSpropParameterSets() const
{
    return bIncludeSpropParameterSets;
}

PUBLIC
IMS_SINT32 CodecH265Config::GetMaxMbps() const
{
    return nMaxMbps;
}

PUBLIC
IMS_SINT32 CodecH265Config::GetMaxFs() const
{
    return nMaxFs;
}

PUBLIC
IMS_SINT32 CodecH265Config::GetMaxCbp() const
{
    return nMaxCbp;
}

PUBLIC
IMS_SINT32 CodecH265Config::GetMaxDpb() const
{
    return nMaxDpb;
}

PUBLIC
IMS_SINT32 CodecH265Config::GetMaxBr() const
{
    return nMaxBr;
}
