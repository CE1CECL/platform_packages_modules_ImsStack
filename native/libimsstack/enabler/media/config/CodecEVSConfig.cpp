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

#include "config/CodecEVSConfig.h"
#include "ServiceMemory.h"
#include "ServiceTrace.h"
#include "AStringBuffer.h"
#include "IConfigBuffer.h"
#include "config/IMSCodec.h"
#include "mmpf/MMPFBoardConfigInfo.h"

__IMS_TRACE_TAG_USER_DECL__("CONF");

PRIVATE GLOBAL const IMS_CHAR CodecEVSConfig::KEY_CHANNEL[]           = "channel";
PRIVATE GLOBAL const IMS_CHAR CodecEVSConfig::KEY_PTIME[]             = "ptime";
PRIVATE GLOBAL const IMS_CHAR CodecEVSConfig::KEY_MAX_PTIME[]         = "max_ptime";
PRIVATE GLOBAL const IMS_CHAR CodecEVSConfig::KEY_DTX[]               = "dtx";
PRIVATE GLOBAL const IMS_CHAR CodecEVSConfig::KEY_HF_ONLY[]           = "header_full_only";
PRIVATE GLOBAL const IMS_CHAR CodecEVSConfig::KEY_EVS_MODE_SWITCH[]   = "mode_switch";
PRIVATE GLOBAL const IMS_CHAR CodecEVSConfig::KEY_MAX_RED[]           = "max_red";
PRIVATE GLOBAL const IMS_CHAR CodecEVSConfig::KEY_BITRATE[]           = "bitrate";
PRIVATE GLOBAL const IMS_CHAR CodecEVSConfig::KEY_BANDWIDTH[]         = "bandwidth";
PRIVATE GLOBAL const IMS_CHAR CodecEVSConfig::KEY_CMR[]               = "cmr";
PRIVATE GLOBAL const IMS_CHAR CodecEVSConfig::KEY_CHANNEL_AWARE_MODE[]= "channel_aware_mode";
PRIVATE GLOBAL const IMS_CHAR CodecEVSConfig::KEY_MODE_SET[]          = "mode_set";
PRIVATE GLOBAL const IMS_CHAR CodecEVSConfig::KEY_DEFAULT_MODE_SET[]  = "default_mode_set";
PRIVATE GLOBAL const IMS_CHAR CodecEVSConfig::KEY_DEFAULT_BANDWIDTH[] = "default_bandwidth";
PRIVATE GLOBAL const IMS_CHAR CodecEVSConfig::KEY_DEFAULT_BITRATE[]   = "default_bitrate";
PRIVATE GLOBAL const IMS_CHAR CodecEVSConfig::KEY_SEND_CMR[]          = "send_cmr";

/*

Remarks

*/
PUBLIC
CodecEVSConfig::CodecEVSConfig(IN IMS_SINT32 nType_, IN IMS_SINT32 nProfielNum_) :
        CodecConfig(nType_, IMSCodec::AtoString(nType_), nProfielNum_),
        nChannel(-1),
        nPtime(-1),
        nMaxPtime(-1),
        nDtx(-1),
        nHfOnly(-1),
        nEvsModeSwitch(-1),
        nMaxRed(-1),
        nBrList(0),
        nBwList(0),
        nCmr(-1),
        nChAwRecv(-1),
        nModeSetList(0),
        nDefaultRtpModeSet(0),
        nDefaultBandwidth(0),
        nDefaultBitrate(0),
        nSendCmr(0)
{
}

/*

Remarks

*/
PUBLIC VIRTUAL
CodecEVSConfig::~CodecEVSConfig()
{
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL CodecEVSConfig::Create(IN IConfigBuffer *piBuffer)
{

    // create EVS Config...
    if (piBuffer == IMS_NULL)
    {
        IMS_TRACE_E(0, "CodecEVSConfig - piBuffer is NULL", 0, 0, 0);
        return IMS_FALSE;
    }

    // Set payload type & network parameter
    CodecConfig::Create(piBuffer);

    // AMR/AMR-WB parameters
    AString strSection;
    GetSection(strSection);
    if (!piBuffer->CaptureSection(strSection.GetStr(), GetProfileNum()))
    {
        IMS_TRACE_E(0, "Create-strSection (%s), profileNum (%d)",
                strSection.GetStr(),GetProfileNum(),0);
        return IMS_FALSE;
    }

    nChannel = piBuffer->ReadValueInt(KEY_CHANNEL);
    nPtime = piBuffer->ReadValueInt(KEY_PTIME);
    nMaxPtime = piBuffer->ReadValueInt(KEY_MAX_PTIME);
    nDtx = piBuffer->ReadValueInt(KEY_DTX);
    nHfOnly = piBuffer->ReadValueInt(KEY_HF_ONLY);
    nEvsModeSwitch = piBuffer->ReadValueInt(KEY_EVS_MODE_SWITCH);
    nMaxRed = piBuffer->ReadValueInt(KEY_MAX_RED);
    nCmr = piBuffer->ReadValueInt(KEY_CMR);
    nChAwRecv = piBuffer->ReadValueInt(KEY_CHANNEL_AWARE_MODE);

    // [1]make br/bw/modeset
    //make bitrate set
    nBrList = ConvertEvsBitrateToList(piBuffer->ReadValue(KEY_BITRATE));
    //make bandwidth set
    nBwList = ConvertEvsBandwidthToList(piBuffer->ReadValue(KEY_BANDWIDTH));
    //make modeset list
    nModeSetList = ConvertEvsModeSetToList(piBuffer->ReadValue(KEY_MODE_SET));

    // [2] make default setting
    // make default bitrate set
    nDefaultBitrate = ConvertEvsBitrateToList(piBuffer->ReadValue(KEY_DEFAULT_BITRATE));
    if (nDefaultBitrate == 0)
    {
        nDefaultBitrate = 0x7f; // 5.9 ~ 24.4 kbps.
    }
    // make default bandwidth set
    nDefaultBandwidth = ConvertEvsBandwidthToList(piBuffer->ReadValue(KEY_DEFAULT_BANDWIDTH));
    if (nDefaultBandwidth == 0)
    {
        nDefaultBandwidth = 0x07; // NB ~ SWB bandwidth
    }
    // make default modeset list
    nDefaultRtpModeSet = ConvertEvsModeSetToList(piBuffer->ReadValue(KEY_DEFAULT_MODE_SET));
    if (nDefaultRtpModeSet == 0)
    {
        nDefaultBandwidth = 0x01ff; // 6.6~23.85 kbps
    }

    nSendCmr = piBuffer->ReadValueInt(KEY_SEND_CMR);    //true == 1

    piBuffer->ReleaseSection();

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL
AString CodecEVSConfig::GetFmtp() const
{
    return AString::ConstNull();
}

/*

Remarks

*/
PUBLIC VIRTUAL
AString CodecEVSConfig::GetRtpMap() const
{
    return AString::ConstNull();
}

/*

Remarks

*/
PUBLIC VIRTUAL
void CodecEVSConfig::ToDebugString() const
{
    CodecConfig::ToDebugString();

    IMS_TRACE_D("nPtime (%d), nMaxPtime (%d), nDtx (0x%04x)", nPtime, nMaxPtime, nDtx);
    IMS_TRACE_D("nHfOnly (%d), nEvsModeSwitch (%d), nMaxRed (%d)",
            nHfOnly, nEvsModeSwitch, nMaxRed);
    IMS_TRACE_D("nBr(0x%04x), nBw (0x%04x), nCmr (%d)", nBrList, nBwList, nCmr);
    IMS_TRACE_D("nChAwRecv (%d), nModeSetList (0x%04x), nDefaultRtpModeSet (0x%04x)",
            nChAwRecv, nModeSetList, nDefaultRtpModeSet);
    IMS_TRACE_D("nDefaultBandwidth (0x%04x), nDefaultBitrate(0x%04x)",
            nDefaultBandwidth, nDefaultBitrate, 0);
}

/*

Remarks

*/

PUBLIC
IMS_SINT32 CodecEVSConfig::GetChannel() const
{
    return nChannel;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecEVSConfig::GetPtime() const
{
    return nPtime;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecEVSConfig::GetMaxPtime() const
{
    return nMaxPtime;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecEVSConfig::GetDtx() const
{
    return nDtx;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecEVSConfig::GetHfOnly() const
{
    return nHfOnly;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecEVSConfig::GetEvsModeSwitch() const
{
    return nEvsModeSwitch;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecEVSConfig::GetMaxRed() const
{
    return nMaxRed;
}

/*

Remarks

*/
PUBLIC
IMS_UINT32 CodecEVSConfig::GetBrList() const
{
    return nBrList;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecEVSConfig::GetBr() const
{
    return GetEvsBitrateFromList(nBrList);
}

/*

Remarks

*/
PUBLIC
IMS_UINT32 CodecEVSConfig::GetBwList() const
{
    return nBwList;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecEVSConfig::GetBw() const
{
    return GetEvsBandwidthFromList(nBwList);
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecEVSConfig::GetCmr() const
{
    return nCmr;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecEVSConfig::GetChAwareRecv() const
{
    return nChAwRecv;
}

/*

Remarks

*/
PUBLIC
IMS_UINT32 CodecEVSConfig::GetModeSetList() const
{
    return nModeSetList;
}

/*

Remarks

*/
PUBLIC
IMS_UINT32 CodecEVSConfig::GetModeSet() const
{
    return GetAmrIoModeSetFromList(nModeSetList);
}

/*

Remarks

*/
PUBLIC
IMS_UINT32 CodecEVSConfig::GetDefaultRptModeSet() const
{
    return nDefaultRtpModeSet;
}

/*

Remarks

*/
PUBLIC
IMS_UINT32 CodecEVSConfig::GetDefaultBandwidth() const
{
    return nDefaultBandwidth;
}

/*

Remarks

*/
PUBLIC
IMS_UINT32 CodecEVSConfig::GetDefaultBitrate () const
{
    return nDefaultBitrate;
}

PUBLIC
IMS_BOOL CodecEVSConfig::GetSendCmr() const
{
    return (nSendCmr == 1);
}
/*

Remarks

*/
PRIVATE
IMS_UINT32 CodecEVSConfig::ConvertEvsBitrateToList(IN AString strBitrate) const
{
    AString strEVSBitrate = strBitrate;
    AString strTemp = AString::ConstNull();
    IMS_BOOL bOK = IMS_FALSE;
    IMS_SINT32 nBitrate = 0;
    IMS_SINT32 nBitrateOffset = 0;
    IMS_UINT32 nBitrateSet = 0;

    IMSList<AString> objbitrate = strEVSBitrate.Split(',');
    for (IMS_UINT32 i = 0; i < objbitrate.GetSize(); ++i)
    {
        strTemp = objbitrate.GetAt(i);

        if (strTemp.GetLength() == 0)
        {
            continue;
        }

        bOK = IMS_FALSE;
        nBitrateOffset = 0;
        nBitrate = strTemp.ToInt32(&bOK);

        if (!bOK)
        {
            IMS_TRACE_E(0, "nBitrate (%s) is not number", strTemp.GetStr(), 0, 0);
            return 0;
        }
        else if (nBitrate < 0)
        {
            break;
        }

        switch (nBitrate)
        {
            case 59:
                nBitrateOffset = EVS_PRIMARY_5900;
                break;
            case 72:
                nBitrateOffset = EVS_PRIMARY_7200;
                break;
            case 80:
                nBitrateOffset = EVS_PRIMARY_8000;
                break;
            case 96:
                nBitrateOffset = EVS_PRIMARY_9600;
                break;
            case 132:
                nBitrateOffset = EVS_PRIMARY_13200;
                break;
            case 164:
                nBitrateOffset = EVS_PRIMARY_16400;
                break;
            case 244:
                nBitrateOffset = EVS_PRIMARY_24400;
                break;
            case 320:
                nBitrateOffset = EVS_PRIMARY_32000;
                break;
            case 480:
                nBitrateOffset = EVS_PRIMARY_48000;
                break;
            case 640:
                nBitrateOffset = EVS_PRIMARY_64000;
                break;
            case 960:
                nBitrateOffset = EVS_PRIMARY_96000;
                break;
            case 1280:
                nBitrateOffset = EVS_PRIMARY_128000;
                break;
            default :
            {
                IMS_TRACE_E(0, "nBitrate (%s) is not matched", strTemp.GetStr(), 0, 0);
                return 0;
            }
        }

        nBitrateSet = ( nBitrateSet | ( 1 << nBitrateOffset));
    }

    return (IMS_UINT32)nBitrateSet;
}

/*

Remarks

*/
PRIVATE
IMS_UINT32 CodecEVSConfig::ConvertEvsBandwidthToList(IN AString strBandwidth) const
{
    AString strEVSBandwidth = strBandwidth;
    AString strTemp = AString::ConstNull();
    IMS_SINT32 nBandwidthOffset = 0;
    IMS_UINT32 nBandwidthSet = 0;

    IMSList<AString> objbandwidth = strEVSBandwidth.Split(',');
    for (IMS_UINT32 i = 0; i < objbandwidth.GetSize(); ++i)
    {
        strTemp = objbandwidth.GetAt(i);

        if (strTemp.GetLength() == 0)
        {
            continue;
        }

        nBandwidthOffset = 0;

        if (strTemp.EqualsIgnoreCase("NB"))
        {
            nBandwidthOffset = EVS_BANDWIDTH_NB;
        }
        else if (strTemp.EqualsIgnoreCase("WB"))
        {
            nBandwidthOffset = EVS_BANDWIDTH_WB;
        }
        else if (strTemp.EqualsIgnoreCase("SWB"))
        {
            // TODO - 20220415 - Need to implement this requirement later
            /*if ((!IMS_OPERATOR(VZW, GetSlotId())) && (!MMPFBoardConfigInfo::IsEVSSWBSupport()))
            {
                continue;
            }*/
            nBandwidthOffset = EVS_BANDWIDTH_SWB;
        }
        else if (strTemp.EqualsIgnoreCase("FB"))
        {
            nBandwidthOffset = EVS_BANDWIDTH_FB;
        }
        else
        {
            return 0;
        }

        nBandwidthSet = (nBandwidthSet | ( 1 << nBandwidthOffset));
    }

    return (IMS_UINT32)nBandwidthSet;
}

/*

Remarks

*/
PRIVATE
IMS_UINT32 CodecEVSConfig::ConvertEvsModeSetToList(IN AString strModeSet) const
{
    AString strEVSModeSets = strModeSet;
    AString strTemp = AString::ConstNull();
    IMS_SINT32 nModeSetOffset = 0;
//    IMS_UINT32 nEVSModeSet = 0;
    IMS_BOOL bOK = IMS_FALSE;
    IMS_SINT32 nCodecMode = 0;

    IMSList<AString> objEVSModeSet = strEVSModeSets.Split(',');

    for (IMS_UINT32 i = 0; i < objEVSModeSet.GetSize(); ++i)
    {
        strTemp = objEVSModeSet.GetAt(i);

        if (strTemp.GetLength() == 0)
        {
            continue;
        }

        bOK = IMS_FALSE;
        nCodecMode = strTemp.ToInt32(&bOK);

        if (!bOK)
        {
            IMS_TRACE_E(0, "CodecMode (%s) is not number", strTemp.GetStr(), 0, 0);
            return 0;
        }
        else if (nCodecMode < 0)
        {
            //IMS_TRACE_E(0, "mode-set (%s) is lower than 0. No mode-set speficied", strModeSet.GetStr(), 0, 0);
            break;
        }

        nModeSetOffset = (nModeSetOffset | (1 << nCodecMode));
    }
    return (IMS_UINT32)nModeSetOffset;
}

/*

Remarks

*/
PRIVATE
IMS_SINT32 CodecEVSConfig::GetEvsBandwidthFromList(IN IMS_UINT32 nBandwidthList) const
{
    if (nBandwidthList == 0)
    {
        return EVS_BANDWIDTH_SWB;
    }

    for (IMS_SINT32 nFindBandwidth = EVS_BANDWIDTH_FB; nFindBandwidth >= 0; nFindBandwidth--)
    {
        IMS_UINT32 nMatch = nBandwidthList & (1 << nFindBandwidth);

        if (nMatch)
        {
            return nFindBandwidth;
        }
    }

    return EVS_BANDWIDTH_SWB;
}

/*

Remarks

*/
PRIVATE
IMS_SINT32 CodecEVSConfig::GetEvsBitrateFromList(IN IMS_UINT32 nBitrateList) const
{
    if (nBitrateList == 0)
    {
        return EVS_PRIMARY_24400;
    }

    for (IMS_SINT32 nFindBitrate = EVS_PRIMARY_128000; nFindBitrate >= 0; nFindBitrate--)
    {
        IMS_UINT32 nMatch = nBitrateList & (1 << nFindBitrate);

        if (nMatch)
        {
            return nFindBitrate;
        }
    }

    return EVS_PRIMARY_24400;
}

/*

Remarks

*/
PRIVATE
IMS_SINT32 CodecEVSConfig::GetAmrIoModeSetFromList(IN IMS_UINT32 nAmrIoModeSet) const
{
    if (nAmrIoModeSet == 0)
    {
        return DEFAULT_MODE_SET_AMR_WB_IO;
    }

    for (IMS_SINT32 nFindModeSet = DEFAULT_MODE_SET_AMR_WB_IO; nFindModeSet >= 0; nFindModeSet--)
    {
        IMS_UINT32 nMatch = nAmrIoModeSet & (1 << nFindModeSet);

        if (nMatch)
        {
            return nFindModeSet;
        }
    }

    return DEFAULT_MODE_SET_AMR_WB_IO;
}
