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

package com.android.imsstack.enabler.media.audio;

import android.telephony.ims.RtpHeaderExtension;
import android.telephony.imsmedia.AudioConfig;
import android.telephony.imsmedia.MediaQualityThreshold;
import com.android.imsstack.enabler.mtc.MtcMediaSession;
import com.android.imsstack.util.ImsLog;
import java.util.List;

/**
 * This is a Listener to handle requests from ImsStack (Media Enabler Native)
 * to ImsMedia
 */
public class MediaAudioListener {
    /**
     * Called when Open session request is received
     */
    public void onAudioOpenSession(String localIpAddress,
        int localPortNumber, AudioConfig audioConfig) {
        // no-op
    }

    /**
     * Called when Close session request is received
     */
    public void onAudioCloseSession() {
        // no-op
    }

    /**
     * Called when Modify session request is received
     */
    public void onAudioModifySession(AudioConfig audioConfig) {
        // no-op
    }

    /**
     * Called when Add a new remote Configuration request is received
     */
    public void onAudioAddConfig(AudioConfig audioConfig) {
        // no-op
    }

    /**
     * Called when Delete remote Configuration request is received
     */
    public void onAudioDeleteConfig(AudioConfig audioConfig) {
        // no-op
    }

    /**
     * Called when Confirm remote Configuration request is received
     */
    public void onAudioConfirmConfig(AudioConfig audioConfig) {
        // no-op
    }

    /**
     * Called when Send DTMF request is received
     */
    public void onAudioSendDtmf(char dtmfDigit, int duration) {
        // no-op
    }

    /**
     * Called when Close session request is received
     */
    public void onAudioSetMediaQualityThreshold(MediaQualityThreshold threshold) {
        // no-op
    }

    /**
     * Called when Send RTP header extension request is received
     */
    public void onAudioSendHeaderExtension(List<RtpHeaderExtension> extensions) {
        // no-op
    }
}
