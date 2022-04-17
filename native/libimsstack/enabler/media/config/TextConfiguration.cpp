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
#include "TextParser.h"
#include "IConfigBuffer.h"
#include "Configuration.h"
#include "config/MediaSessionConfig.h"
#include "config/CodecConfigFactory.h"
#include "config/TextConfiguration.h"

__IMS_TRACE_TAG_USER_DECL__("CONF");

PRIVATE GLOBAL
const IMS_CHAR TextConfiguration::KEY_KEEP_REDLEVEL[] = "keep_redlevel";

PUBLIC
TextConfiguration::TextConfiguration(MEDIA_CONTENT_TYPE eSessionType) :
        MediaConfiguration(eSessionType),
        nPdpProfileNum(-1),
        nPdpProfileNumOf3G(-1),
        nKeepRedlevel(-1)
{
}

PUBLIC
TextConfiguration::~TextConfiguration()
{
    Clear();
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL TextConfiguration::Create(IN IConfigBuffer *piBuffer, IN IMS_UINT32 nIndex,
        IN IMS_SINT32 nSlotID)
{
    if (piBuffer == IMS_NULL)
    {
        IMS_TRACE_D("piBuffer == IMS_NULL", 0, 0, 0);
        return IMS_FALSE;
    }

    if (!piBuffer->CaptureSection(MediaSessionConfig::KEY_TEXT, nIndex))
    {
        IMS_TRACE_D("CaptureSection", 0, 0, 0);
        return IMS_FALSE;
    }

    if (!MediaConfiguration::Create(piBuffer, nIndex))
    {
        IMS_TRACE_D("MediaConfiguration::Create", 0, 0, 0);
        piBuffer->ReleaseSection();
        return IMS_FALSE;
    }

    // mmpf configuration
    nKeepRedlevel = piBuffer->ReadValueInt(KEY_KEEP_REDLEVEL);

    // Codec information
    const IMS_SINT32 nCodecListSize =
            piBuffer->ReadValueInt(MediaSessionConfig::KEY_CODEC_LIST_SIZE);
    const AString &strCodecRef = piBuffer->ReadValue(MediaSessionConfig::KEY_CODEC_REF);

    Clear();

    // Creates a codec configuration
    if (!CreateCodecConfigs(strCodecRef, nCodecListSize, nSlotID))
    {
        IMS_TRACE_E(0, "Create - CreateCodecConfigs failure [%s]", strCodecRef.GetStr(), 0, 0);
        piBuffer->ReleaseSection();
        return IMS_FALSE;
    }

    piBuffer->ReleaseSection();

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL
void TextConfiguration::ToDebugString() const
{


    MediaConfiguration::ToDebugString();

    IMS_TRACE_D("nPdpProfileNum (%d)", nPdpProfileNum, 0, 0);

    for (IMS_UINT32 i = 0; i < objCodecConfigs.GetSize(); ++i)
    {
        CodecConfig *pCodecConfig = objCodecConfigs.GetAt(i);

        if (pCodecConfig == IMS_NULL)
            continue;

        pCodecConfig->ToDebugString();
    }
}

PUBLIC
IMS_SINT32 TextConfiguration::GetPdpProfileNum() const
{
    return nPdpProfileNum;
}

/*!
 * details :    configuration for mmpf to distingish with VZW and TMO requirement
 *              VZW requires primary only data should sent by T.140 payload not redundant payload
 */
PUBLIC
IMS_BOOL TextConfiguration::GetKeepRedLevel() const
{
    return (nKeepRedlevel > 0);
}

PUBLIC
const CodecConfig* TextConfiguration::GetCodecConfig() const
{
    if (objCodecConfigs.IsEmpty())
        return IMS_NULL;

    return objCodecConfigs.GetAt(0);
}

/*

Remarks

*/
PUBLIC
const IMSList<CodecConfig*>& TextConfiguration::GetCodecConfigs() const
{
    return objCodecConfigs;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL TextConfiguration::Update(IN IConfigBuffer *piBuffer, IN IMS_UINT32 nIndex)
{
    if (piBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    if (!piBuffer->CaptureSection(MediaSessionConfig::KEY_TEXT, nIndex))
    {
        return IMS_FALSE;
    }

    // Remove writing code to DB
    /*
    // RTP / RTCP port numbers
    //    piBuffer->WriteValueInt(MediaSessionConfig::KEY_PORT_RTP, nPortRtp);
    //    piBuffer->WriteValueInt(MediaSessionConfig::KEY_PORT_RTCP, nPortRtcp);

    // Codec information
    //3 Codec Update ?

    piBuffer->WriteToMedium();
    */

    piBuffer->ReleaseSection();

    return IMS_TRUE;
}

/*

Remarks

*/
PROTECTED
void TextConfiguration::Clear()
{
    for (IMS_UINT32 i = 0; i < objCodecConfigs.GetSize(); ++i)
    {
        CodecConfig *pCodecConfig = objCodecConfigs.GetAt(i);

        if (pCodecConfig == IMS_NULL)
        {
            continue;
        }

        delete pCodecConfig;
    }

    objCodecConfigs.Clear();
}

/*

Remarks

*/
PROTECTED
IMS_BOOL TextConfiguration::CreateCodecConfigs(IN CONST AString &strRef,
        IN CONST IMS_SINT32 nCodecListSize, IN IMS_SINT32 nSlotID)
{
    IConfigBuffer *piBuffer = Configuration::GetInstance()->CreateConfig(strRef, nSlotID);

    if (piBuffer == IMS_NULL)
    {
        return IMS_FALSE;
    }

    for (IMS_SINT32 nIndex = 0; nIndex < nCodecListSize; ++nIndex)
    {
        if (!piBuffer->CaptureSection(MediaSessionConfig::KEY_TEXT_CODEC, nIndex))
        {
            piBuffer->Destroy();
            return IMS_FALSE;
        }

        const AString &strCodecName = piBuffer->ReadValue(MediaSessionConfig::KEY_CODEC_TYPE);

        IMS_TRACE_D("CodecConfig text - session_type[%d], index[%d], codec[%s]",
                GetSessionType(), nIndex, strCodecName.GetStr());

        if (strCodecName.EqualsIgnoreCase("None") || strCodecName.Equals(""))
        {
            continue;
        }

        CodecConfig *pCodecConfig = CodecConfigFactory::CreateTCodecConfig(strCodecName, nIndex);

        if (pCodecConfig == IMS_NULL)
        {
            IMS_TRACE_E(0, "CreateCodecConfigs fail, can't create a codec config(%s)",
                    strCodecName.GetStr(), 0, 0);

            piBuffer->Destroy();
            return IMS_FALSE;
        }

        if (!pCodecConfig->Create(piBuffer))
        {
            IMS_TRACE_E(0, "CreateCodecConfigs fail, Create failure", 0, 0, 0);

            piBuffer->Destroy();
            delete pCodecConfig;
            return IMS_FALSE;
        }

        if (!objCodecConfigs.Append(pCodecConfig))
        {
            IMS_TRACE_E(0, "CreateCodecConfigs fail, Append failure", 0, 0, 0);

            piBuffer->Destroy();
            return IMS_FALSE;
        }
    }

    piBuffer->Destroy();

    return IMS_TRUE;
}
