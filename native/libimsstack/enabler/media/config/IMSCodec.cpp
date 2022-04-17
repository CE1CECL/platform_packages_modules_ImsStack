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

/*

Remarks

*/
PUBLIC GLOBAL
const IMS_CHAR* IMSCodec::AtoString(IN IMS_SINT32 nType)
{
    switch (nType)
    {
        case AUDIO_AMR:
            return "AMR";

        case AUDIO_AMR_WB:
            return "AMR-WB";

        case AUDIO_EVRC:
            return "EVRC";

        case AUDIO_EVRC0:
            return "EVRC0";

        case AUDIO_EVRCB:
            return "EVRCB";

        case AUDIO_PCMA:
            return "PCMA";

        case AUDIO_PCMU:
            return "PCMU";

        case AUDIO_TELEPHONE_EVENT:
            return "telephone-event";

        case AUDIO_EVS:
            return "EVS";

        default:
            return "";
    }
}

/*

Remarks

*/
PUBLIC GLOBAL
const IMS_CHAR* IMSCodec::VtoString(IN IMS_SINT32 nType)
{
    switch (nType)
    {
        case VIDEO_H264:
            return "H264";

        case VIDEO_H263:
            return "H263";

        case VIDEO_H263_1998:
            return "H263-1998";

        case VIDEO_H263_2000:
            return "H263-2000";

        case VIDEO_MPEG4:
            return "MP4V-ES";

        case VIDEO_H265:
            return "H265";

        default:
            return "";
    }
}

/*

Remarks

*/
PUBLIC GLOBAL
const IMS_CHAR* IMSCodec::TtoString(IN IMS_SINT32 nType)
{
    switch (nType)
    {
        case TEXT_T140:
            return "t140";
        case TEXT_RED:
            return "red";

        default:
            return "";
    }
}

/*

Remarks

*/
PUBLIC GLOBAL
IMS_SINT32 IMSCodec::AtoType(IN CONST AString &strName)
{
    if (strName.GetLength() == 0)
    {
        return AUDIO_NONE;
    }

    switch (strName[0])
    {
        case 'A':
            if (strName.Equals("AMR"))
            {
                return AUDIO_AMR;
            }
            else if (strName.Equals("AMR-WB"))
            {
                return AUDIO_AMR_WB;
            }

            break;

        case 'E':
            if (strName.Equals("EVRC"))
            {
                return AUDIO_EVRC;
            }
            else if (strName.Equals("EVRC0"))
            {
                return AUDIO_EVRC0;
            }
            else if (strName.Equals("EVRCB"))
            {
                return AUDIO_EVRCB;
            }
            else if (strName.Equals("EVS"))
            {
                return AUDIO_EVS;
            }

            break;

        case 'P':
            if (strName.Equals("PCMA"))
            {
                return AUDIO_PCMA;
            }
            else if (strName.Equals("PCMU"))
            {
                return AUDIO_PCMU;
            }

            break;

        case 't':
            if (strName.Equals("telephone-event"))
            {
                return AUDIO_TELEPHONE_EVENT;
            }

            break;

        default:
            break;
    }

    return AUDIO_NONE;
}

/*

Remarks

*/
PUBLIC GLOBAL
IMS_SINT32 IMSCodec::VtoType(IN CONST AString &strName)
{
    if (strName.GetLength() == 0)
        return VIDEO_NONE;

    switch (strName[0])
    {
        case 'H':
            if (strName.Equals("H264"))
            {
                return VIDEO_H264;
            }
            else if (strName.Equals("H263"))
            {
                return VIDEO_H263;
            }
            else if (strName.Equals("H263-1998"))
            {
                return VIDEO_H263_1998;
            }
            else if (strName.Equals("H263-2000"))
            {
                return VIDEO_H263_2000;
            }
            else if (strName.Equals("H265"))
            {
                return VIDEO_H265;
            }

            break;

        case 'M':
            if (strName.Equals("MP4V-ES"))
            {
                return VIDEO_MPEG4;
            }

            break;

        default:
            break;
    }

    return VIDEO_NONE;
}

/*

Remarks

*/
PUBLIC GLOBAL
IMS_SINT32 IMSCodec::TtoType(IN CONST AString &strName)
{
    if (strName.GetLength() == 0)
    {
        return TEXT_NONE;
    }

    if (strName.Equals("t140"))
    {
        return TEXT_T140;
    }

    if (strName.Equals("red"))
    {
        return TEXT_RED;
    }

    return TEXT_NONE;
}
