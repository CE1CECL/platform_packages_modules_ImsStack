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

#include "config/CodecAMRConfig.h"
#include "IConfigBuffer.h"
#include "config/IMSCodec.h"

__IMS_TRACE_TAG_USER_DECL__("CONF");

PRIVATE GLOBAL
const IMS_CHAR CodecAMRConfig::KEY_MODE_CHANGE_CAPABILITY[] = "mode_change_capability";
PRIVATE GLOBAL
const IMS_CHAR CodecAMRConfig::KEY_MODE_SET[] = "mode_set";
PRIVATE GLOBAL
const IMS_CHAR CodecAMRConfig::KEY_OCTET_ALIGN[] = "octet_align";

/*

Remarks

*/
PUBLIC
CodecAMRConfig::CodecAMRConfig(IN IMS_SINT32 nType_) :
        CodecConfig(nType_, IMSCodec::AtoString(nType_)),
        nModeSetList(0),
        nModeChangeCapability(2),
        nOctetAlign(MODE_OCTET_ALIGN)
{
}

/*

Remarks

*/
PUBLIC VIRTUAL
CodecAMRConfig::~CodecAMRConfig()
{
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL CodecAMRConfig::Create(IN IConfigBuffer *piBuffer)
{
    if (piBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    AString strSection = AString::ConstNull();
    GetSection(strSection);

    if (!piBuffer->CaptureSection(strSection.GetStr()))
    {
        return IMS_FALSE;
    }

    // Set payload type parameter
    CodecConfig::Create(piBuffer);

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
            IMS_TRACE_E(0, "mode-set (%s) is lower than 0. No mode-set speficied",
                    strModeSet.GetStr(), 0, 0);
            break;
        }

        nModeSetList = ( nModeSetList | ( 1 << nModeSet));
    }

    // Mode-change-capability parameter
    nModeChangeCapability = piBuffer->ReadValueInt(KEY_MODE_CHANGE_CAPABILITY);

    // Octet-align parameter
    nOctetAlign = piBuffer->ReadValueInt(KEY_OCTET_ALIGN);

    piBuffer->ReleaseSection();

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL
AString CodecAMRConfig::GetFmtp() const
{
    AStringBuffer objFMTP(64);
    if ( nModeSetList != 0)
    {
        IMS_UINT32 nFirstModeSet = 0;
        if (objFMTP.GetLength() > 0)
        {
            objFMTP.Append(';');
            objFMTP.Append(' ');
        }

        objFMTP.Append("mode-set=");
        for (IMS_UINT32 nModeSet = 0; nModeSet <= DEFAULT_MODE_SET_AMR_WB; nModeSet++)
        {
            IMS_UINT32 nMatch = nModeSetList & ( 1 << nModeSet);
            if (nMatch)
            {
                objFMTP.Append(nModeSet);
                nFirstModeSet = nModeSet;
                break;
            }
        }

        for (IMS_UINT32 nModeSet = nFirstModeSet; nModeSet < DEFAULT_MODE_SET_AMR_WB; ++nModeSet)
        {
            IMS_UINT32 nMatch = nModeSetList & ( 1 << nModeSet);
            if (nMatch)
            {
                objFMTP.Append(',');
                objFMTP.Append(nModeSet);
            }
        }
    }

    if (nOctetAlign != MODE_NONE)
    {
        if (objFMTP.GetLength() > 0)
        {
            objFMTP.Append(';');
            objFMTP.Append(' ');
        }

        objFMTP.Append("octet-align=");
        objFMTP.Append(nOctetAlign);
    }

    if (nModeChangeCapability >= 0)
    {
        if (objFMTP.GetLength() > 0)
        {
            objFMTP.Append(';');
            objFMTP.Append(' ');
        }

        objFMTP.Append("mode-change-capability=");
        objFMTP.Append(nModeChangeCapability);
    }

    return static_cast<const AStringBuffer&>(objFMTP).GetString();
}

/*

Remarks

*/
PUBLIC VIRTUAL
AString CodecAMRConfig::GetRtpMap() const
{
    AString strRtpMap = AString::ConstNull();

    if (GetCodecType() == IMSCodec::AUDIO_AMR_WB)
    {
        strRtpMap.Sprintf("%s/16000/1", GetCodecName().GetStr());
    }
    else
    {
        strRtpMap.Sprintf("%s/8000/1", GetCodecName().GetStr());
    }

    return strRtpMap;
}

/*

Remarks

*/
PUBLIC VIRTUAL
void CodecAMRConfig::ToDebugString() const
{
    CodecConfig::ToDebugString();

    AString strModeSets = AString::ConstNull();

    IMS_TRACE_D("mode-set (0x%04x), octet-align (%d), mode-change-capability (%d)",
            nModeSetList, nOctetAlign, nModeChangeCapability);
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecAMRConfig::GetModeChangeCapability() const
{
    return nModeChangeCapability;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecAMRConfig::GetModeSet() const
{
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

    for (IMS_SINT32 nModeSet = DEFAULT_MODE_SET_AMR_WB; nModeSet >= 0; nModeSet--)
    {
        IMS_UINT32 nMatch = nModeSetList & ( 1 << nModeSet);
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
IMS_UINT32  CodecAMRConfig::GetModeSets() const
{
    return nModeSetList;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 CodecAMRConfig::GetOctetAlign() const
{
    return nOctetAlign;
}
