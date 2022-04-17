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
#include "config/CodecH263Config.h"

__IMS_TRACE_TAG_USER_DECL__("CONF");

const IMS_CHAR CodecH263Config::KEY_RESOLUTION[] = "resolution";
const IMS_CHAR CodecH263Config::KEY_FRAMERATE[] = "framerate";
const IMS_CHAR CodecH263Config::KEY_BITRATE[] = "bitrate";
const IMS_CHAR CodecH263Config::KEY_AS[] = "AS";
const IMS_CHAR CodecH263Config::KEY_PROFILE_LEVEL_ID[] = "profile_level_id";
const IMS_CHAR CodecH263Config::KEY_FRAMESIZE[] = "framesize";
const IMS_CHAR CodecH263Config::KEY_IMAGEATTR[] = "imageattr";
const IMS_CHAR CodecH263Config::KEY_PROFILE[] = "profile";
const IMS_CHAR CodecH263Config::KEY_LEVEL[] = "level";
const IMS_CHAR CodecH263Config::KEY_QCIF[] = "QCIF";

PUBLIC
CodecH263Config::CodecH263Config(IN IMS_SINT32 nType_, IN IMS_SINT32 nProfielNum_) :
        CodecConfig(nType_, IMSCodec::VtoString(nType_), nProfielNum_),
        nResolutionId(0),
        nFramerate(10),
        nBitrate(48),
        strProfileLevelId(AString::ConstNull()),
        bIncludeFrameSize(IMS_FALSE),
        bIncludeImageAttr(IMS_FALSE),
        nProfile(0),
        nLevel(45),
        bIncludeQcif(IMS_FALSE)
{
}

PUBLIC VIRTUAL
CodecH263Config::~CodecH263Config()
{
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL CodecH263Config::Create(IN IConfigBuffer *piBuffer)
{


    if (piBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    // videocodec parameters - audio/video common ( payload type & network)
    CodecConfig::Create(piBuffer);

    IMSList<AString> objTokens;

    // videocodec parameters - video
    if (!piBuffer->CaptureSection(MediaSessionConfig::KEY_VIDEO_CODEC, GetProfileNum()))
    {
        IMS_TRACE_E(0, "Create-strSection (%d), profileNum (%s)",
                MediaSessionConfig::KEY_VIDEO_CODEC,GetProfileNum(),0);
        return IMS_FALSE;
    }

    nResolutionId           = piBuffer->ReadValueInt(KEY_RESOLUTION);
    nFramerate              = piBuffer->ReadValueInt(KEY_FRAMERATE);
    nBitrate                = piBuffer->ReadValueInt(KEY_BITRATE);
    strProfileLevelId       = piBuffer->ReadValue(KEY_PROFILE_LEVEL_ID);
    bIncludeFrameSize       = piBuffer->ReadValueBoolean(KEY_FRAMESIZE);
    bIncludeImageAttr       = piBuffer->ReadValueBoolean(KEY_IMAGEATTR);

    //nAs                     = piBuffer->ReadValueInt(KEY_AS);
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

    // h263 parameters
    AString strSection;
    GetSection(strSection);

    if (!piBuffer->CaptureSection(strSection.GetStr(), GetProfileNum()))
    {
        return IMS_FALSE;
    }

    nProfile = piBuffer->ReadValueInt(KEY_PROFILE);
    nLevel = piBuffer->ReadValueInt(KEY_LEVEL);
    bIncludeQcif = piBuffer->ReadValueBoolean(KEY_QCIF);

    piBuffer->ReleaseSection();

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL
AString CodecH263Config::GetFmtp() const
{
    AString strFMTP;

    strFMTP.Sprintf("profile=%d; level=%d", nProfile, nLevel);

    return strFMTP;
}

/*

Remarks

*/
PUBLIC VIRTUAL
AString CodecH263Config::GetRtpMap() const
{
    AString strRTPMAP;

    strRTPMAP.Sprintf("%s/90000", GetCodecName().GetStr());

    return strRTPMAP;
}

/*

Remarks

*/
PUBLIC VIRTUAL
void CodecH263Config::ToDebugString() const
{
    CodecConfig::ToDebugString();

    IMS_TRACE_D("nResolutionId (%d), bIncludeImageAttr (%s), bIncludeFrameSize (%d)",
            nResolutionId, bIncludeImageAttr, bIncludeFrameSize);
    IMS_TRACE_D("nFramerate (%d), nBitrate (%d), strProfileLevelId (%s)",
            nFramerate, nBitrate, strProfileLevelId.GetStr());
    IMS_TRACE_D("profile (%d), level (%d), bIncludeQcif (%d)", nProfile, nLevel, bIncludeQcif);
    IMS_TRACE_D("AS IPv4(%d), IPv6(%d)", nAs[0], nAs[1], 0);
}

PUBLIC
IMS_SINT32 CodecH263Config::GetResolutionId() const
{
    return nResolutionId;
}

PUBLIC
IMS_SINT32 CodecH263Config::GetFramerate() const
{
    return nFramerate;
}

PUBLIC
IMS_SINT32 CodecH263Config::GetBitrate() const
{
    return nBitrate;
}

PUBLIC
IMS_SINT32 CodecH263Config::GetAs(IMS_BOOL bIsIPv6) const
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
const AString& CodecH263Config::GetProfileLevelId() const
{
    return strProfileLevelId;
}

PUBLIC
IMS_BOOL CodecH263Config::GetIncludeImageAttr() const
{
    return bIncludeImageAttr;
}

PUBLIC
IMS_BOOL CodecH263Config::GetIncludeFrameSize() const
{
    return bIncludeFrameSize;
}

PUBLIC
IMS_SINT32 CodecH263Config::GetProfile() const
{
    return nProfile;
}

PUBLIC
IMS_SINT32 CodecH263Config::GetLevel() const
{
    return nLevel;
}

PUBLIC
IMS_BOOL CodecH263Config::GetIncludeQcif() const
{
    return bIncludeQcif;
}
