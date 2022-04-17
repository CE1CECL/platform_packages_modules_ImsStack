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
#include "ServiceUtil.h"
#include "MediaDef.h"
#include "config/CodecConfig.h"
#include "IConfigBuffer.h"
#include "config/IMSCodec.h"
#include "config/MediaSessionConfig.h"

#include "mmpf/MMPFBoardConfigInfo.h"

__IMS_TRACE_TAG_USER_DECL__("CONF");

PRIVATE GLOBAL
const IMS_CHAR CodecConfig::KEY_PAYLOAD_TYPE[] = "payload_type";
PRIVATE GLOBAL
const IMS_CHAR CodecConfig::KEY_NETWORK_TYPE[] = "network_type";
PRIVATE GLOBAL
const IMS_CHAR CodecConfig::KEY_PRIORITY[] = "priority";
PRIVATE GLOBAL
const IMS_CHAR CodecConfig::KEY_BANDWIDTH[] = "bandwidth";

PUBLIC
CodecConfig::CodecConfig(IN IMS_SINT32 nCodecType_, IN CONST AString &strCodecName_,
        IN CONST IMS_SINT32 nProfileNum_) :
        strSectionName(strCodecName_),
        nProfileNum(nProfileNum_),
        nPayloadType(0),
        nNetworkType(0),
        nCodecType(nCodecType_),
        strCodecName(strCodecName_),
        nPriority(0)
{
}

PUBLIC VIRTUAL
CodecConfig::~CodecConfig()
{
}

PUBLIC VIRTUAL
IMS_BOOL CodecConfig::Create(IN IConfigBuffer *piBuffer)
{


    if (piBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    AString strSectioName = AString::ConstNull();

    if (nCodecType > IMSCodec::AUDIO_NONE && nCodecType < IMSCodec::AUDIO_MAX)
    {
        strSectioName = MediaSessionConfig::KEY_AUDIO_CODEC;
    }
    else if (nCodecType > IMSCodec::VIDEO_NONE && nCodecType < IMSCodec::VIDEO_MAX)
    {
        strSectioName = MediaSessionConfig::KEY_VIDEO_CODEC;
    }
    else if (nCodecType > IMSCodec::TEXT_NONE && nCodecType < IMSCodec::TEXT_MAX)
    {
        strSectioName = MediaSessionConfig::KEY_TEXT_CODEC;
    }
    else
    {
        IMS_TRACE_E(0, "Invalid Codec Type[%d]", nCodecType,0,0);
    }

    if (!piBuffer->CaptureSection(strSectioName.GetStr(), nProfileNum))
    {
        IMS_TRACE_E(0, "Create-strSection (%s), profileNum (%d)",
            strSectioName.GetStr(), nProfileNum, 0);
        return IMS_FALSE;
    }

    // payload_type parameter
    nPayloadType = piBuffer->ReadValueInt(KEY_PAYLOAD_TYPE);

    const AString &strNetworks = piBuffer->ReadValue(KEY_NETWORK_TYPE);
    IMS_TRACE_D("Create-strSection (%s), profileNum (%d)", strSectioName.GetStr(),nProfileNum,0);
    IMS_TRACE_D("nPayloadType (%d), strNetworks (%s)", nPayloadType,strNetworks.GetStr(),0);

    IMSList<AString> objTokens = strNetworks.Split(',');

    nNetworkType = 0;
    for (IMS_UINT32 i = 0 ; i < objTokens.GetSize(); i ++)
    {
        const AString &strNetwork = objTokens.GetAt(i);

        if (strNetwork.EqualsIgnoreCase("3g") == IMS_TRUE)
        {
            nNetworkType += (IMS_UINT32)MEDIA_NETWORK_EHRPD;
            nNetworkType += (IMS_UINT32)MEDIA_NETWORK_WCDMA;
        }
        else if (strNetwork.EqualsIgnoreCase("hspa") == IMS_TRUE)
        {
            nNetworkType += (IMS_UINT32)MEDIA_NETWORK_HSPA;
            nNetworkType += (IMS_UINT32)MEDIA_NETWORK_HSPA_PLUS;
        }
        else if (strNetwork.EqualsIgnoreCase("lte") == IMS_TRUE)
        {
            nNetworkType += (IMS_UINT32)MEDIA_NETWORK_LTE;
        }
        else if (strNetwork.EqualsIgnoreCase("wifi") == IMS_TRUE)
        {
            nNetworkType += (IMS_UINT32)MEDIA_NETWORK_WIFI;
        }
        else
        {
            IMS_TRACE_D("Create, Invalid network index[%d], type[%s]", i, strNetwork.GetStr(), 0);
        }
    }

    nPriority = piBuffer->ReadValueInt(KEY_PRIORITY);

    {
        if (nCodecType == IMSCodec::AUDIO_EVS)
        {
            // remove EVS SWB only payload if device does not support EVS SWB codec
            if (!piBuffer->CaptureSection(IMSCodec::AtoString(IMSCodec::AUDIO_EVS), nProfileNum))
            {
                IMS_TRACE_E(0, "EVS , profileNum (%d)", nProfileNum, 0, 0);
                return IMS_FALSE;
            }

            // TODO - 20220415 - Need to implement this requirement later
            /*AString strBandwidth = piBuffer->ReadValue(KEY_BANDWIDTH);
            if ((!IMS_OPERATOR(VZW, GetSlotId())) &&
                (strBandwidth.EqualsIgnoreCase("SWB") && !MMPFBoardConfigInfo::IsEVSSWBSupport()))
            {
                IMS_TRACE_E(0, "remove evs swb only payload - EVS SWB does not support case",
                        0, 0, 0);
                nNetworkType = 0;
            }*/
        }
    }

    IMS_TRACE_D("Create, [%s][%d][%08x]",strCodecName.GetStr(), nPayloadType, nNetworkType);

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL
AString CodecConfig::GetFmtp() const
{
    return AString::ConstNull();
}

/*

Remarks

*/
PUBLIC VIRTUAL
AString CodecConfig::GetRtpMap() const
{
    return AString::ConstNull();
}

/*

Remarks

*/
PUBLIC VIRTUAL
void CodecConfig::ToDebugString() const
{
    IMS_TRACE_D("codec (%d, %s), payload (%d)",
            GetCodecType(), GetCodecName().GetStr(), GetPayloadType());
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecConfig::GetCodecType() const
{
    return nCodecType;
}

/*

Remarks

*/
PUBLIC
const AString& CodecConfig::GetCodecName() const
{
    return strCodecName;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecConfig::GetProfileNum() const
{
    return nProfileNum;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecConfig::GetPayloadType() const
{
    return nPayloadType;
}

/*

Remarks

*/
PUBLIC
IMS_UINT32 CodecConfig::GetAvailableNetworkType() const
{
    return nNetworkType;
}

/*

Remarks

*/
PUBLIC
IMS_UINT32 CodecConfig::GetPriority() const
{
    return nPriority;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL CodecConfig::SetPriority(IMS_UINT32 priority)
{
    nPriority = priority;
    return IMS_TRUE;
}
/*

Remarks

*/
PUBLIC
void CodecConfig::GetSection(OUT AString &strSection) const
{
    strSection.Sprintf("%s", strSectionName.GetStr());
    strSection.Replace('-', '_');
}
