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

#include "config/CodecH264Config.h"
#include "IConfigBuffer.h"
#include "TextParser.h"
#include "config/IMSCodec.h"
#include "config/MediaSessionConfig.h"

__IMS_TRACE_TAG_USER_DECL__("CONF");

PRIVATE GLOBAL const IMS_CHAR CodecH264Config::KEY_RESOLUTION[] = "resolution";
PRIVATE GLOBAL const IMS_CHAR CodecH264Config::KEY_FRAMERATE[] = "framerate";
PRIVATE GLOBAL const IMS_CHAR CodecH264Config::KEY_BITRATE[] = "bitrate";
PRIVATE GLOBAL const IMS_CHAR CodecH264Config::KEY_AS[] = "AS";
PRIVATE GLOBAL const IMS_CHAR CodecH264Config::KEY_PROFILE_LEVEL_ID[] = "profile_level_id";
PRIVATE GLOBAL const IMS_CHAR CodecH264Config::KEY_CHP_PROFILE_LEVEL_ID[] = "chp_profile_level_id";
PRIVATE GLOBAL const IMS_CHAR CodecH264Config::KEY_FRAMESIZE[] = "framesize";
PRIVATE GLOBAL const IMS_CHAR CodecH264Config::KEY_IMAGEATTR[] = "imageattr";
PRIVATE GLOBAL const IMS_CHAR CodecH264Config::KEY_PACKETIZATION_MODE[] = "packetization_mode";
PRIVATE GLOBAL const IMS_CHAR CodecH264Config::KEY_SPROP_PARAMETER_SETS[] = "sprop_parameter_sets";
PRIVATE GLOBAL const IMS_CHAR CodecH264Config::KEY_MAX_MBPS[] = "max_mbps";
PRIVATE GLOBAL const IMS_CHAR CodecH264Config::KEY_MAX_FS[] = "max_fs";
PRIVATE GLOBAL const IMS_CHAR CodecH264Config::KEY_MAX_CBP[] = "max_cpb";
PRIVATE GLOBAL const IMS_CHAR CodecH264Config::KEY_MAX_DPB[] = "max_dpb";
PRIVATE GLOBAL const IMS_CHAR CodecH264Config::KEY_MAX_BR[] = "max_br";

/*

Remarks

*/
PUBLIC
CodecH264Config::CodecH264Config(IN IMS_SINT32 nProfielNum_) :
        CodecConfig(IMSCodec::VIDEO_H264, IMSCodec::VtoString(IMSCodec::VIDEO_H264), nProfielNum_),
        nResolutionId(4),
        nFramerate(10),
        nBitrate(512),
        strProfileLevelId(AString::ConstNull()),
        strChpProfileLevelId(AString::ConstNull()),
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

/*

Remarks

*/
PUBLIC VIRTUAL
CodecH264Config::~CodecH264Config()
{
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL CodecH264Config::Create(IN IConfigBuffer *piBuffer)
{

    if (piBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    // videocodec parameters - audio/video common (payload type & network)
    CodecConfig::Create(piBuffer);

    // videocodec parameters - video
    if (!piBuffer->CaptureSection(MediaSessionConfig::KEY_VIDEO_CODEC, GetProfileNum()))
    {
        IMS_TRACE_E(0, "Create-strSection (%d), profileNum (%s)",
                MediaSessionConfig::KEY_VIDEO_CODEC,GetProfileNum(),0);
        return IMS_FALSE;
    }

    IMSList<AString> objTokens;
    nResolutionId             = piBuffer->ReadValueInt(KEY_RESOLUTION);
    nFramerate             = piBuffer->ReadValueInt(KEY_FRAMERATE);
    nBitrate                 = piBuffer->ReadValueInt(KEY_BITRATE);
    strProfileLevelId        = piBuffer->ReadValue(KEY_PROFILE_LEVEL_ID);
    strChpProfileLevelId     = piBuffer->ReadValue(KEY_CHP_PROFILE_LEVEL_ID);
    bIncludeFrameSize         = piBuffer->ReadValueBoolean(KEY_FRAMESIZE);
    bIncludeImageAttr        = piBuffer->ReadValueBoolean(KEY_IMAGEATTR);

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

    // h264 parameters
    AString strSection;
    GetSection(strSection);
    if (!piBuffer->CaptureSection(strSection.GetStr(), GetProfileNum()))
    {
        return IMS_FALSE;
    }

    nPacketizationMode          = piBuffer->ReadValueInt(KEY_PACKETIZATION_MODE);
    bIncludeSpropParameterSets  = piBuffer->ReadValueBoolean(KEY_SPROP_PARAMETER_SETS);
    nMaxMbps                    = piBuffer->ReadValueInt(KEY_MAX_MBPS);
    nMaxFs                      = piBuffer->ReadValueInt(KEY_MAX_FS);
    nMaxCbp                     = piBuffer->ReadValueInt(KEY_MAX_CBP);
    nMaxDpb                     = piBuffer->ReadValueInt(KEY_MAX_DPB);
    nMaxBr                      = piBuffer->ReadValueInt(KEY_MAX_BR);

    piBuffer->ReleaseSection();

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL
void CodecH264Config::ToDebugString() const
{

    CodecConfig::ToDebugString();

    IMS_TRACE_D("nResolutionId (%d), bIncludeImageAttr(%d), bIncludeFrameSize (%d)",
            nResolutionId, bIncludeImageAttr, bIncludeFrameSize);
    IMS_TRACE_D("nFramerate (%d), nBitrate (%d), strProfileLevelId (%s)",
            nFramerate, nBitrate, strProfileLevelId.GetStr());
    IMS_TRACE_D("nPacketizationMode (%d), bIncludeSpropParameterSets (%d), nMaxMbps (%d)",
            nPacketizationMode, bIncludeSpropParameterSets, nMaxMbps);
    IMS_TRACE_D("nMaxFs (%d), nMaxCbp (%d), nMaxDpb (%d)", nMaxFs, nMaxCbp, nMaxDpb);
    IMS_TRACE_D("nMaxBr (%d), nAs IPv4(%d) IPv6(%d)", nMaxBr, nAs[0], nAs[1]);
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecH264Config::GetResolutionId() const
{
    return nResolutionId;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecH264Config::GetFramerate() const
{
    return nFramerate;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecH264Config::GetBitrate() const
{
    return nBitrate;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecH264Config::GetAs(IMS_BOOL bIsIPv6) const
{
    // return (if bIsIPv6 == IMS_TRUE ? nAs[1] : nAs[0]);
    if (IMS_TRUE == bIsIPv6)
    {
        return nAs[1];
    }
    else
    {
        return nAs[0];
    }
}

/*

Remarks

*/
PUBLIC
const AString& CodecH264Config::GetProfileLevelId() const
{
    return strProfileLevelId;
}

/*

Remarks

*/
PUBLIC
const AString& CodecH264Config::GetChpProfileLevelId() const
{
    if (strChpProfileLevelId == AString::ConstEmpty())
    {
        return strProfileLevelId;
    }

    return strChpProfileLevelId;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL CodecH264Config::GetIncludeImageAttr() const
{
    return bIncludeImageAttr;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL CodecH264Config::GetIncludeFrameSize() const
{
    return bIncludeFrameSize;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecH264Config::GetPacketizationMode() const
{
    return nPacketizationMode;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL CodecH264Config::GetIncludeSpropParameterSets() const
{
    return bIncludeSpropParameterSets;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecH264Config::GetMaxMbps() const
{
    return nMaxMbps;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecH264Config::GetMaxFs() const
{
    return nMaxFs;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecH264Config::GetMaxCbp() const
{
    return nMaxCbp;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecH264Config::GetMaxDpb() const
{
    return nMaxDpb;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecH264Config::GetMaxBr() const
{
    return nMaxBr;
}
