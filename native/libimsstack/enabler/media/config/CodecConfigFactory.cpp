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
#include "config/IMSCodec.h"
#include "config/CodecAMRConfig.h"
#include "config/CodecAMRConfigEx.h"
#include "config/CodecEVRCConfig.h"
#include "config/CodecH263Config.h"
#include "config/CodecH264Config.h"
#include "config/CodecH265Config.h"
#include "config/CodecMPEG4Config.h"
#include "config/CodecPCMConfig.h"
#include "config/CodecT140Config.h"
#include "config/CodecTelephoneEventConfig.h"
#include "config/CodecTelephoneEventConfigEx.h"
#include "config/CodecConfigFactory.h"
#include "config/CodecEVSConfig.h"

/*

Remarks

*/
PUBLIC GLOBAL
CodecConfig* CodecConfigFactory::CreateACodecConfig(IN CONST AString &strName,
        IN CONST IMS_SINT32 nIndex)
{
    IMS_SINT32 nACodec = IMSCodec::AtoType(strName);

    if (nACodec == IMSCodec::AUDIO_NONE)
    {
        return IMS_NULL;
    }

    CodecConfig *pCodecConfig = IMS_NULL;

    switch (nACodec)
    {
        case IMSCodec::AUDIO_AMR:
        case IMSCodec::AUDIO_AMR_WB:
            // pCodecConfig = new CodecAMRConfig(nACodec);
            pCodecConfig = new CodecAMRConfigEx(nACodec, nIndex);
            break;
        case IMSCodec::AUDIO_EVRC:
        case IMSCodec::AUDIO_EVRC0:
        case IMSCodec::AUDIO_EVRCB:
            pCodecConfig = new CodecEVRCConfig(nACodec, nIndex);
            break;
        case IMSCodec::AUDIO_PCMA:
        case IMSCodec::AUDIO_PCMU:
            pCodecConfig = new CodecPCMConfig(nACodec, nIndex);
            break;
        case IMSCodec::AUDIO_TELEPHONE_EVENT:
            // pCodecConfig = new CodecTelephoneEventConfig();
            pCodecConfig = new CodecTelephoneEventConfigEx(nIndex);
            break;
        case IMSCodec::AUDIO_EVS:
            pCodecConfig = new CodecEVSConfig(nACodec, nIndex);
            break;
    }

    return pCodecConfig;
}

PUBLIC GLOBAL
CodecConfig* CodecConfigFactory::CreateVCodecConfig(IN CONST AString &strName,
        IN CONST IMS_SINT32 nIndex)
{
    IMS_SINT32 nVCodec = IMSCodec::VtoType(strName);

    if (nVCodec == IMSCodec::VIDEO_NONE)
    {
        return IMS_NULL;
    }

    CodecConfig *pCodecConfig = IMS_NULL;

    switch (nVCodec)
    {
        case IMSCodec::VIDEO_H264:
            pCodecConfig = new CodecH264Config(nIndex);
            break;

        case IMSCodec::VIDEO_H263:
        case IMSCodec::VIDEO_H263_1998:
        case IMSCodec::VIDEO_H263_2000:
            pCodecConfig = new CodecH263Config(nVCodec, nIndex);
            break;

        case IMSCodec::VIDEO_MPEG4:
            pCodecConfig = new CodecMPEG4Config();
            break;
        case IMSCodec::VIDEO_H265:
            pCodecConfig = new CodecH265Config(nIndex);
            break;
    }
    return pCodecConfig;
}

PUBLIC GLOBAL CodecConfig* CodecConfigFactory::CreateTCodecConfig(
        IN CONST AString &strName, IN CONST IMS_SINT32 nIndex)
{
    IMS_SINT32 nTCodec = IMSCodec::TtoType(strName);

    if (nTCodec == IMSCodec::TEXT_NONE)
    {
        return IMS_NULL;
    }

    CodecConfig *pCodecConfig = IMS_NULL;

    switch (nTCodec)
    {
        case IMSCodec::TEXT_T140:                  // FALL_THROUGH
        case IMSCodec::TEXT_RED:
            pCodecConfig = new CodecT140Config(nTCodec, nIndex);
            break;
        default:
            break;
    }

    return pCodecConfig;
}
