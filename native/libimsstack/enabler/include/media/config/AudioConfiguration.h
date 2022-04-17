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

#ifndef _AUDIO_CONFIG_H_
#define _AUDIO_CONFIG_H_

#include "config/CodecConfig.h"
#include "config/MediaConfiguration.h"

class IConfigBuffer;

class AudioConfiguration
        : public MediaConfiguration
{
    public:
        AudioConfiguration(MEDIA_CONTENT_TYPE eSessionType);
        virtual ~AudioConfiguration();

    private:
        AudioConfiguration(IN CONST AudioConfiguration &objRHS);
        AudioConfiguration& operator=(IN CONST AudioConfiguration &objRHS);

    public:
        virtual IMS_BOOL Create(IN IConfigBuffer *piBuffer, IN IMS_UINT32 nIndex,
                IN IMS_SINT32 nSlotID);
        virtual void ToDebugString() const;

        IMS_SINT32 GetPdpProfileNum() const;
        IMS_SINT32 GetPdpProfileNumOf3G() const;
        // Jitter buffer control
        IMS_SINT32 GetJitterBufferMaxSize() const;
        IMS_SINT32 GetJitterBufferMinSize() const;
        IMS_SINT32 GetJitterBufferSize() const;
        // Common attributes
        IMS_SINT32 GetPacketTime() const;
        IMS_SINT32 GetMaxPacketTime() const;
        IMS_SINT32 GetCandidatePriority() const;
        // DTMF Telephone Event
        IMS_SINT32 GetDTMFDuration() const;
        IMS_SINT32 GetDTMFRetransmitDuration() const;
        IMS_SINT32 GetDTMFInterval() const;
        IMS_BOOL GetRearrangeModeSetByAs() const;
        //[VOCODER_INTERFACE]
        IMS_SINT32 GetModeChangeCapaAlwaysAnswer() const;
        IMS_BOOL GetDmOperationPreferredMode() const;
        IMS_BOOL GetAudioHalfRateMode() const;

        const CodecConfig* GetCodecConfig() const;
        const IMSList<CodecConfig*>& GetCodecConfigs() const;

        IMS_BOOL Update(IN IConfigBuffer *piBuffer, IN IMS_UINT32 nIndex);

    protected:
        void Clear();
        IMS_BOOL CreateCodecConfigs(IN CONST AString &strRef, IN CONST IMS_SINT32 nCodecListSize,
            IN IMS_SINT32 nSlotID = 0);

    private:
        enum
        {
            DEFAULT_PTIME = 20
        };
        enum
        {
            DEFAULT_MAXPTIME = 60
        };
        enum
        {
            DEFAULT_DTMF_DURATION = 10
        };
        enum
        {
            DEFAULT_DTMF_RETRANSMIT_DURATION = 2
        };
        enum
        {
            DEFAULT_DTMF_INTERVAL = 3
        };
        enum
        {
            DEFAULT_JITTER_BUFFER_MAX_SIZE = 9,
            DEFAULT_JITTER_BUFFER_MIN_SIZE = 4,
            DEFAULT_JITTER_BUFFER_SIZE = 4
        };
        enum
        {
            AUDIO_HALFRATE_SETTING = 10
        };   // setting at "dm_operation_preferred_mode" Media DB table.

    public:
        enum
        {
            DEFAULT_VOCODER_INTERFACE = 2
        }; //[VOCODER_INTERFACE] DEFAULT_VOCODER_INTERFACE is CVD.

    private:
        // sdp offer/answer
        static const IMS_CHAR KEY_PTIME[];
        static const IMS_CHAR KEY_MAXPTIME[];
        static const IMS_CHAR KEY_CANDIDATE_PRIORITY[];
        static const IMS_CHAR KEY_REARRANGE_MODE_SET_BY_AS[];
        static const IMS_CHAR KEY_VOCODER_INTERFACE[];
        static const IMS_CHAR KEY_DM_OPERATION_PREFERRED_MODE[];
        static const IMS_CHAR KEY_MODE_CHANGE_CAPA_ALWAYS_ANSWER[];
        static const IMS_CHAR KEY_USE_SINGLE_IP[];

    private:
        // PDP Profile number for CP audio
        IMS_SINT32 nPdpProfileNum;
        IMS_SINT32 nPdpProfileNumOf3G;
        // Jitter buffer control
        IMS_SINT32 nJitterBufferMaxSize;
        IMS_SINT32 nJitterBufferMinSize;
        IMS_SINT32 nJitterBufferSize;
        // Common attributes
        IMS_SINT32 nPacketTime;
        IMS_SINT32 nMaxPacketTime;
        IMS_SINT32 nCandidatePriority;
        IMS_SINT32 nDtmfDuration;                  // duration / 20msec
        IMS_SINT32 nDtmfRetransmitDuration;
        IMS_SINT32 nDtmfInterval;
        IMS_SINT32 nRearrangeModeSetByAs;
        IMS_SINT32 nDmOperationPreferred_mode;
        IMS_SINT32 nEvsCodecEnable;
        IMS_SINT32 nModeChangeCapaAlwaysAnswer;
        IMS_SINT32 nAudioHalfRateMode;
        // Provisioned codecs
        IMSList<CodecConfig*> objCodecConfigs;
};
#endif                                              // _AUDIO_CONFIG_H_
