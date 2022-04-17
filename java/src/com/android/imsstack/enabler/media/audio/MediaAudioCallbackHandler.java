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

import android.annotation.NonNull;
import android.os.Parcel;
import android.os.Parcelable;
import android.telephony.CallQuality;
import android.telephony.ims.RtpHeaderExtension;
import android.telephony.imsmedia.AudioConfig;
import android.telephony.imsmedia.ImsMediaSession;
import android.telephony.imsmedia.MediaQualityThreshold;
import com.android.imsstack.enabler.media.MediaConstants;
import com.android.imsstack.enabler.mtc.MtcMediaSession;
import com.android.imsstack.util.ImsLog;
import java.util.List;

/**
 * This handles callbacks received from ImsMediaManager and sends to
 * ImsStack (Media Enabler Native)
 */
public class MediaAudioCallbackHandler {

    private final MtcMediaSession mMtcMediaSession;

    public MediaAudioCallbackHandler(@NonNull final MtcMediaSession mtcMediaSession){
        mMtcMediaSession = mtcMediaSession;
        ImsLog.v("Constructor - Exit");
    }

    private final MtcMediaSession getMtcMediaSession() {
        return mMtcMediaSession;
    }

    /**
     * Response for Open Session request.
     */
    public void openSessionResponse(@ImsMediaSession.SessionOperationResult int result) {
        ImsLog.d("openSession Result=" + result);

        Parcel parcel = Parcel.obtain();

        parcel.writeInt(MediaConstants.RESPONSE_OPEN_SESSION);
        parcel.writeInt(ImsMediaSession.SESSION_TYPE_AUDIO);
        parcel.writeInt(result);

        getMtcMediaSession().sendRequest(parcel);
    }

    /**
     * Called when any change occurs to the RTP session.
     */
    public void sessionChanged(int state) {
        ImsLog.v("sessionChanged");

        Parcel parcel = Parcel.obtain();

        parcel.writeInt(MediaConstants.RESPONSE_SESSION_CHANGED);
        parcel.writeInt(ImsMediaSession.SESSION_TYPE_AUDIO);
        parcel.writeInt(state);

        getMtcMediaSession().sendRequest(parcel);
    }

    /**
     * Response for Modify Session request.
     */
    public void modifySessionResponse(AudioConfig audioConfig, int result) {
        ImsLog.v("modifySessionResponse");

        Parcel parcel = Parcel.obtain();

        parcel.writeInt(MediaConstants.RESPONSE_MODIFY_SESSION);
        parcel.writeInt(ImsMediaSession.SESSION_TYPE_AUDIO);
        audioConfig.writeToParcel(parcel, Parcelable.PARCELABLE_WRITE_RETURN_VALUE);
        parcel.writeInt(result);

        getMtcMediaSession().sendRequest(parcel);
    }

    /**
     * Response for addConfig()
     */
    public void addConfigResponse(AudioConfig audioConfig, int result) {
        ImsLog.v("addConfigResponse");

        Parcel parcel = Parcel.obtain();

        parcel.writeInt(MediaConstants.RESPONSE_ADD_CONFIG);
        parcel.writeInt(ImsMediaSession.SESSION_TYPE_AUDIO);
        audioConfig.writeToParcel(parcel, Parcelable.PARCELABLE_WRITE_RETURN_VALUE);
        parcel.writeInt(result);

        getMtcMediaSession().sendRequest(parcel);
    }

    /**
     * Response for confirmConfig()
     */
    public void confirmConfigResponse(AudioConfig audioConfig, int result) {
        ImsLog.v("confirmConfigResponse");

        Parcel parcel = Parcel.obtain();

        parcel.writeInt(MediaConstants.RESPONSE_CONFIRM_CONFIG);
        parcel.writeInt(ImsMediaSession.SESSION_TYPE_AUDIO);
        audioConfig.writeToParcel(parcel, Parcelable.PARCELABLE_WRITE_RETURN_VALUE);
        parcel.writeInt(result);

        getMtcMediaSession().sendRequest(parcel);
    }

    /**
     * Called when the first Rtp media packet is received
     */
    public void firstMediaPacketReceived(AudioConfig audioConfig) {
        ImsLog.v("firstMediaPacketReceived");

        Parcel parcel = Parcel.obtain();

        parcel.writeInt(MediaConstants.NOTIFY_FIRST_PACKET);
        parcel.writeInt(ImsMediaSession.SESSION_TYPE_AUDIO);
        audioConfig.writeToParcel(parcel, Parcelable.PARCELABLE_WRITE_RETURN_VALUE);

        getMtcMediaSession().sendRequest(parcel);
    }

    /**
     * Called when RTP header extension received from the other party
     */
    public void headerExtensionReceived(List<RtpHeaderExtension> rtpExtensions) {
        ImsLog.v("headerExtensionReceived");

        Parcel parcel = Parcel.obtain();

        parcel.writeInt(MediaConstants.NOTIFY_HEADER_EXTENSION);
        parcel.writeInt(ImsMediaSession.SESSION_TYPE_AUDIO);
        parcel.writeInt(rtpExtensions.size());
        if (!rtpExtensions.isEmpty()) {
            for (int i = 0; i < rtpExtensions.size(); ++i) {
                rtpExtensions.get(i).writeToParcel(parcel,
                    Parcelable.PARCELABLE_WRITE_RETURN_VALUE);
            }
        }

        getMtcMediaSession().sendRequest(parcel);
    }

    /**
     * Called when media inactivity observed as per thresholds set by
     * setMediaQualityThreshold()
     */
    public void onNotifyMediaInactivity(int packetType) {
        ImsLog.v("onNotifyMediaInactivity");

        Parcel parcel = Parcel.obtain();

        parcel.writeInt(MediaConstants.NOTIFY_MEDIA_INACTIVITY);
        parcel.writeInt(ImsMediaSession.SESSION_TYPE_AUDIO);
        parcel.writeInt(packetType);

        getMtcMediaSession().sendRequest(parcel);
    }

    /**
     * Called when RTP packet loss observed as per thresholds set by
     * setMediaQualityThreshold()
     */
    public void onNotifyPacketLoss(int packetLossPercentage) {
        ImsLog.v("onNotifyPacketLoss");

        Parcel parcel = Parcel.obtain();

        parcel.writeInt(MediaConstants.NOTIFY_PACKET_LOSS);
        parcel.writeInt(ImsMediaSession.SESSION_TYPE_AUDIO);
        parcel.writeInt(packetLossPercentage);

        getMtcMediaSession().sendRequest(parcel);
    }

    /**
     * Called when RTP jitter observed as per thresholds set by
     * setMediaQualityThreshold()
     */
    public void onNotifyJitter(int jitter) {
        ImsLog.v("onNotifyJitter");

        Parcel parcel = Parcel.obtain();

        parcel.writeInt(MediaConstants.NOTIFY_JITTER);
        parcel.writeInt(ImsMediaSession.SESSION_TYPE_AUDIO);
        parcel.writeInt(jitter);

        getMtcMediaSession().sendRequest(parcel);
    }

    /**
     * Called when a change to media quality is occurred
     */
    public void mediaQualityChanged(CallQuality callQuality) {
        ImsLog.v("mediaQualityChanged");

        Parcel parcel = Parcel.obtain();

        parcel.writeInt(MediaConstants.NOTIFY_MEDIA_QUALITY_CHANGE);
        parcel.writeInt(ImsMediaSession.SESSION_TYPE_AUDIO);
        callQuality.writeToParcel(parcel,  Parcelable.PARCELABLE_WRITE_RETURN_VALUE);

        getMtcMediaSession().sendRequest(parcel);
    }

}
