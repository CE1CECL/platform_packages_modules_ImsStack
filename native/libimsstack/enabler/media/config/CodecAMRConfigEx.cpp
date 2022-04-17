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
#include "config/CodecAMRConfigEx.h"

__IMS_TRACE_TAG_USER_DECL__("CONF");

PRIVATE GLOBAL const IMS_CHAR CodecAMRConfigEx::KEY_CHANNEL[] = "channel";
PRIVATE GLOBAL const IMS_CHAR CodecAMRConfigEx::KEY_SAMPLING_RATE[] = "sampling_rate";
PRIVATE GLOBAL const IMS_CHAR CodecAMRConfigEx::KEY_MODE_SET[] = "mode_set";
PRIVATE GLOBAL const IMS_CHAR CodecAMRConfigEx::KEY_DEFAULT_RTP_MODESET[] = "default_rtp_modeset";
PRIVATE GLOBAL const IMS_CHAR CodecAMRConfigEx::KEY_OCTET_ALIGN[] = "octet_align";
PRIVATE GLOBAL const IMS_CHAR CodecAMRConfigEx::KEY_MODE_CHANGE_CAPABILITY[] =
        "mode_change_capability";
PRIVATE GLOBAL const IMS_CHAR CodecAMRConfigEx::KEY_MODE_CHANGE_PERIOD[] = "mode_change_period";
PRIVATE GLOBAL const IMS_CHAR CodecAMRConfigEx::KEY_MODE_CHANGE_NEIGHBOR[] = "mode_change_neighbor";
PRIVATE GLOBAL const IMS_CHAR CodecAMRConfigEx::KEY_MAX_RED[] = "max_red";
PRIVATE GLOBAL const IMS_CHAR CodecAMRConfigEx::KEY_ROBUST_SORTING[] = "robust_sorting";
PRIVATE GLOBAL const IMS_CHAR CodecAMRConfigEx::KEY_PTIME[] = "ptime";
PRIVATE GLOBAL const IMS_CHAR CodecAMRConfigEx::KEY_MAX_PTIME[] = "max_ptime";
PRIVATE GLOBAL const IMS_CHAR CodecAMRConfigEx::KEY_SCR[] = "scr";

PUBLIC
CodecAMRConfigEx::CodecAMRConfigEx(IN IMS_SINT32 nType_, IN IMS_SINT32 nProfielNum_) :
        CodecConfig(nType_, IMSCodec::AtoString(nType_), nProfielNum_),
        nChannel(-1),
        nModeSetList(0),
        nDefaultRtpModeSet(0),
        nOctetAlign(-1),
        nSamplingRate(16000),
        nModeChangeCapability(-1),
        nModeChangePeriod(-1),
        nModeChangeNeighbor(-1),
        nMaxRed(-1),
        nRobustSorting(-1),
        nPtime(-1),
        nMaxPtime(-1),
        nScr(-1)
{
}

PUBLIC VIRTUAL
CodecAMRConfigEx::~CodecAMRConfigEx()
{
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL CodecAMRConfigEx::Create(IN IConfigBuffer *piBuffer)
{


    if (piBuffer == IMS_NULL)
    {
        IMS_TRACE_E(0, "CodecAMRConfigEx - piBuffer is NULL", 0, 0, 0);
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
    nOctetAlign = piBuffer->ReadValueInt(KEY_OCTET_ALIGN);

    // Default Mode-set parameter
    const AString &strDefaultModeSet = piBuffer->ReadValue(KEY_DEFAULT_RTP_MODESET);
    IMSList<AString> objDefaultModeSet = strDefaultModeSet.Split(',');

    if ( objDefaultModeSet.GetSize() == 0)
    {
        IMS_SINT32 nDefaultMode = DEFAULT_MODE_SET_AMR_WB;

        if (GetCodecType() == IMSCodec::AUDIO_AMR)
        {
            nDefaultMode = DEFAULT_MODE_SET_AMR;
        }
        else if (GetCodecType() == IMSCodec::AUDIO_AMR_WB)
        {
            nDefaultMode = DEFAULT_MODE_SET_AMR_WB;
        }

        for (IMS_SINT32 i = 0; i <= nDefaultMode; i++)
        {
            nDefaultRtpModeSet = (nDefaultRtpModeSet | (1 << i));
        }
    }
    else if (objDefaultModeSet.GetSize() > 0)
    {
        for (IMS_UINT32 i = 0; i < objDefaultModeSet.GetSize(); ++i)
        {
            const AString &strDefaultMode = objDefaultModeSet.GetAt(i);

            if (strDefaultMode.GetLength() == 0)
            {
                continue;
            }

            IMS_BOOL bOK = IMS_FALSE;
            IMS_SINT32 nDefaultMode = strDefaultMode.ToInt32(&bOK);

            if (!bOK)
            {
                IMS_TRACE_E(0, "defaultmode(%s) is not number", strDefaultMode.GetStr(), 0, 0);
                return IMS_FALSE;
            }

            nDefaultRtpModeSet = (nDefaultRtpModeSet | (1 << nDefaultMode));
        }
    }
    // Mode-set parameter
    nModeSetList = 0;

    const AString &strModeSets = piBuffer->ReadValue(KEY_MODE_SET);
    IMSList<AString> objTokens = strModeSets.Split(',');

    for (IMS_UINT32 i = 0; i < objTokens.GetSize(); ++i)
    {
        const AString &strModeSet = objTokens.GetAt(i);

        if (strModeSet.GetLength() == 0)
        {
            continue;
        }

        IMS_BOOL bOK = IMS_FALSE;
        IMS_SINT32 nModeSet = strModeSet.ToInt32(&bOK);

        if (!bOK)
        {
            IMS_TRACE_E(0, "mode-set (%s) is not number", strModeSet.GetStr(), 0, 0);
            return IMS_FALSE;
        }
        else if (nModeSet < 0)
        {
            //IMS_TRACE_E(0, "mode-set (%s) is lower than 0. No mode-set speficied",
            //      strModeSet.GetStr(), 0, 0);
            break;
        }

        nModeSetList = (nModeSetList | (1 << nModeSet));
    }

    nModeChangeCapability = piBuffer->ReadValueInt(KEY_MODE_CHANGE_CAPABILITY);
    nModeChangePeriod = piBuffer->ReadValueInt(KEY_MODE_CHANGE_PERIOD);
    nModeChangeNeighbor = piBuffer->ReadValueInt(KEY_MODE_CHANGE_NEIGHBOR);
    nMaxRed = piBuffer->ReadValueInt(KEY_MAX_RED);
    nRobustSorting = piBuffer->ReadValueInt(KEY_ROBUST_SORTING);
    nPtime = piBuffer->ReadValueInt(KEY_PTIME);
    nMaxPtime = piBuffer->ReadValueInt(KEY_MAX_PTIME);
    nScr = piBuffer->ReadValueInt(KEY_SCR);

    piBuffer->ReleaseSection();

    if (GetCodecName().Equals("AMR") && nSamplingRate == 16000)
    {
        strCodecName.Sprintf("%s", "AMR-WB");
        nCodecType = IMSCodec::AUDIO_AMR_WB;
    }

    return IMS_TRUE;
}

PUBLIC VIRTUAL
AString CodecAMRConfigEx::GetFmtp() const
{
    return AString::ConstNull();
}

PUBLIC VIRTUAL
AString CodecAMRConfigEx::GetRtpMap() const
{
    return AString::ConstNull();
}

PUBLIC VIRTUAL
void CodecAMRConfigEx::ToDebugString() const
{
    CodecConfig::ToDebugString();
    IMS_TRACE_D("sampling-rate (%d), channel (%d), mode-set (0x%04x)",
            nSamplingRate, nChannel, nModeSetList);
    IMS_TRACE_D("octet-align (%d), mode-change-capability (%d), mode-change-neighbor (%d)",
            nOctetAlign, nModeChangeCapability, nModeChangeNeighbor);
    IMS_TRACE_D("mode-change-period(%d), max-red (%d), robust-sorting (%d)",
            nModeChangePeriod, nMaxRed, nRobustSorting);
    IMS_TRACE_D("ptime (%d), max-ptime (%d), scr (%d)", nPtime, nMaxPtime, nScr);
    IMS_TRACE_D("Default Rtp Mode-Set (%d)", nDefaultRtpModeSet, 0, 0);
}

PUBLIC
IMS_SINT32 CodecAMRConfigEx::GetSamplingRate() const
{
    return nSamplingRate;
}

/*

Remarks

*/

PUBLIC
IMS_SINT32 CodecAMRConfigEx::GetChannel() const
{
    return nChannel;
}

/*
Remarks

*/
PUBLIC
IMS_UINT32 CodecAMRConfigEx::GetDefaultRtpModeSet() const
{
    return nDefaultRtpModeSet;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecAMRConfigEx::GetModeSet() const
{
    IMS_SINT32 nModeSet;

    if (nModeSetList == 0)
    {
        if (GetCodecType() == IMSCodec::AUDIO_AMR)
        {
            return DEFAULT_MODE_SET_AMR;
        }
        else if (GetCodecType() == IMSCodec::AUDIO_AMR_WB)
        {
            return DEFAULT_MODE_SET_AMR_WB;
        }

        return (-1);
    }

    for (nModeSet = DEFAULT_MODE_SET_AMR_WB; nModeSet >=0 ; nModeSet--)
    {
        IMS_UINT32 nMatch = nModeSetList & (1 << nModeSet);
        if (nMatch)
        {
            return nModeSet;
        }
    }
    return 0;
}

/*

Remarks

*/
PUBLIC
IMS_UINT32 CodecAMRConfigEx::GetModeSets() const
{
    return nModeSetList;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecAMRConfigEx::GetOctetAlign() const
{
    return nOctetAlign;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecAMRConfigEx::GetModeChangeCapability() const
{
    return nModeChangeCapability;
}

PUBLIC
IMS_SINT32 CodecAMRConfigEx::GetModeChangePeriod() const
{
    return nModeChangePeriod;
}

PUBLIC
IMS_SINT32 CodecAMRConfigEx::GetModeChangeNeighbor() const
{
    return nModeChangeNeighbor;
}

PUBLIC
IMS_SINT32 CodecAMRConfigEx::GetMaxRed() const
{
    return nMaxRed;
}

PUBLIC
IMS_SINT32 CodecAMRConfigEx::GetRobustSorting() const
{
    return nRobustSorting;
}

PUBLIC
IMS_SINT32 CodecAMRConfigEx::GetPtime() const
{
    return nPtime;
}

PUBLIC
IMS_SINT32 CodecAMRConfigEx::GetMaxPtime() const
{
    return nMaxPtime;
}

PUBLIC
IMS_BOOL CodecAMRConfigEx::GetScr() const
{
    if (nScr == 0)
    {
        return IMS_FALSE;
    }
    else
    {
        return IMS_TRUE;
    }
}
