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
import android.annotation.Nullable;
import android.os.ParcelFileDescriptor;
import android.telephony.CallQuality;
import android.telephony.ims.RtpHeaderExtension;
import android.telephony.imsmedia.AudioConfig;
import android.telephony.imsmedia.AudioSessionCallback;
import android.telephony.imsmedia.ImsAudioSession;
import android.telephony.imsmedia.ImsMediaManager;
import android.telephony.imsmedia.ImsMediaSession;
import android.telephony.imsmedia.MediaQualityThreshold;
import com.android.imsstack.enabler.IBaseContext;
import com.android.imsstack.enabler.media.MediaConstants;
import com.android.imsstack.enabler.media.MediaSession;
import com.android.imsstack.enabler.media.MediaSocket;
import com.android.imsstack.enabler.media.audio.MediaAudioCallbackHandler;
import com.android.imsstack.enabler.media.audio.MediaAudioListener;
import com.android.imsstack.enabler.media.base.MediaSessionBase;
import com.android.imsstack.enabler.mtc.MtcMediaSession;
import com.android.imsstack.util.ImsLog;
import com.android.internal.annotations.VisibleForTesting;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.util.List;
import java.util.concurrent.Executor;

/**
 * This manages Audio session communications between ImsStack and ImsMedia
 */
public class MediaAudioSession  extends MediaSession {

    private MtcMediaSession mMtcMediaSession;
    private MediaAudioSessionCallback mAudioSessionCallback;
    private ImsAudioSession mAudioSession;
    private int mAudioSessionId;
    private DatagramSocket mRtpSocket, mRtcpSocket;
    private MediaAudioListenerProxy mAudioListenerProxy;
    private MediaAudioCallbackHandler mAudioCallbackHandler;
    //private ImsMediaManager mImsMediaManager;

    public MediaAudioSession(IBaseContext context, @NonNull MtcMediaSession mtcMediaSession) {
        super(context, mtcMediaSession);
        mAudioCallbackHandler = new MediaAudioCallbackHandler(mtcMediaSession);
        mAudioSessionCallback = new MediaAudioSessionCallback();
        mAudioListenerProxy   = new MediaAudioListenerProxy();
        initMtcMediaSession(mtcMediaSession);
        ImsLog.d("MediaAudioSession created");
    }

    @VisibleForTesting
    public MediaAudioSession(@NonNull IBaseContext context,
        @NonNull MtcMediaSession mtcMediaSession,
        @NonNull MediaAudioCallbackHandler audioCallbackHandler,
        @NonNull ImsAudioSession audioSession,
        @NonNull ImsMediaManager imsMediaManager,
        @NonNull Executor executor) {
        super(context, mtcMediaSession, imsMediaManager, executor);
        mAudioCallbackHandler = audioCallbackHandler;
        mAudioSession = audioSession;
        mAudioSessionCallback = new MediaAudioSessionCallback();
        mAudioListenerProxy   = new MediaAudioListenerProxy();
        initMtcMediaSession(mtcMediaSession);
        ImsLog.d("MediaAudioSession created");
    }

    @VisibleForTesting
    void setAudioSession(@Nullable ImsAudioSession audioSession) {
        mAudioSession = audioSession;
    }

    @VisibleForTesting
    AudioSessionCallback getAudioSessionCallback() {
        return mAudioSessionCallback;
    }

    @VisibleForTesting
    MediaAudioListener getAudioListenerProxy() {
        return mAudioListenerProxy;
    }

    @VisibleForTesting
    int getAudioSessionId() {
        return mAudioSessionId;
    }

    private void initMtcMediaSession(MtcMediaSession mtcMediaSession) {
        mMtcMediaSession = mtcMediaSession;
        if (mtcMediaSession != null) {
            mtcMediaSession.setMediaAudioListener(mAudioListenerProxy);
        }
    }

    private ImsAudioSession getAudioSession() {
        return mAudioSession;
    }

    public void close() {
        ImsLog.d("clear the listener which was set");
        if (mMtcMediaSession != null) {
            mMtcMediaSession.setMediaAudioListener(null);
            mMtcMediaSession = null;
        }
    }

    private void handleAudioOpenSession(String localIpAddress, int localPortNumber,
        AudioConfig audioConfig) {
        if(mAudioSession == null) {
            if (isImsMediaConnected()) {
                //TODO: ImsQOSManager to be used
                mRtpSocket = MediaSocket.createDatagramSocket(localIpAddress,
                    localPortNumber);
                mRtcpSocket = MediaSocket.createDatagramSocket(localIpAddress,
                    (localPortNumber+1));

                if (mRtpSocket == null || mRtcpSocket == null) {
                    ImsLog.e("socket creation failed");
                    if (mAudioCallbackHandler != null) {
                        mAudioCallbackHandler.openSessionResponse(
                            ImsMediaSession.RESULT_PORT_UNAVAILABLE);
                    }
                    return;
                }

                openSession(mRtpSocket, mRtcpSocket, ImsMediaSession.SESSION_TYPE_AUDIO,
                    audioConfig, mAudioSessionCallback);
            }
            else {
                ImsLog.d("ImsMediaManager is not ready");
                if (mAudioCallbackHandler != null) {
                    mAudioCallbackHandler.openSessionResponse(ImsMediaSession.RESULT_NOT_READY);
                }
            }
        }
        else {
            ImsLog.d("Audio Session is already created: SessionId="
                + mAudioSession.getSessionId());
            if (mAudioCallbackHandler != null) {
                mAudioCallbackHandler.openSessionResponse(
                    ImsMediaSession.RESULT_NOT_SUPPORTED);
            }
        }
    }

    private void handleAudioCloseSession() {

        if (mAudioSession != null) {
            closeSession(mAudioSession);
            mAudioSession = null;
        }
        //TODO: ImsQOSManager to be used
        MediaSocket.closeDatagramSocket(mRtpSocket);
        MediaSocket.closeDatagramSocket(mRtcpSocket);
    }

    private void handleAudioModifySession(AudioConfig audioConfig) {
        if (mAudioSession != null) {
            mAudioSession.modifySession(audioConfig);
        }
    }

    private void handleAudioAddConfig(AudioConfig audioConfig) {
        if (mAudioSession != null) {
            mAudioSession.addConfig(audioConfig);
        }
    }

    private void handleAudioDeleteConfig(AudioConfig audioConfig) {
        if (mAudioSession != null) {
            mAudioSession.deleteConfig(audioConfig);
        }
    }

    private void handleAudioConfirmConfig(AudioConfig audioConfig) {
        if (mAudioSession != null) {
            mAudioSession.confirmConfig(audioConfig);
        }
    }

    private void handleAudioSendDtmf(char dtmfDigit, int duration) {
        if (mAudioSession != null) {
            mAudioSession.sendDtmf(dtmfDigit, duration);
        }
    }

    private void handleAudioSetMediaQualityThreshold(MediaQualityThreshold threshold) {
        if (mAudioSession != null) {
            mAudioSession.setMediaQualityThreshold(threshold);
        }
    }

    private void handleAudioSendHeaderExtension(List<RtpHeaderExtension> extensions) {
        if (mAudioSession != null) {
            mAudioSession.sendHeaderExtension(extensions);
        }
    }

    private class MediaAudioSessionCallback extends AudioSessionCallback {

        @Override
        public void onOpenSessionSuccess(ImsMediaSession session) {

            if (session == null) {
                ImsLog.e("Audio Session is not created");
                if (mAudioCallbackHandler != null) {
                    mAudioCallbackHandler.openSessionResponse(ImsMediaSession.RESULT_NO_MEMORY);
                }
                return ;
            }

            mAudioSession = (ImsAudioSession) session;
            mAudioSessionId = mAudioSession.getSessionId();
            ImsLog.d("Audio Session created: SessionId=" + mAudioSessionId);

            if (mAudioCallbackHandler != null) {
                mAudioCallbackHandler.openSessionResponse(ImsMediaSession.RESULT_SUCCESS);
            }
        }

        @Override
        public void onOpenSessionFailure(final @ImsMediaSession.SessionOperationResult int error) {
            ImsLog.d("error=" + error);

            if (mAudioCallbackHandler != null) {
                mAudioCallbackHandler.openSessionResponse(error);
            }
        }

        @Override
        public void onSessionChanged(final @ImsMediaSession.SessionState int state) {
            ImsLog.d("state=" + state);

            if (mAudioCallbackHandler != null) {
                mAudioCallbackHandler.sessionChanged(state);
            }
        }

        @Override
        public void onModifySessionResponse(final AudioConfig audioConfig,
                final @ImsMediaSession.SessionOperationResult int result) {
            ImsLog.d("result=" + result);

            if (mAudioCallbackHandler != null) {
                mAudioCallbackHandler.modifySessionResponse(audioConfig, result);
            }
        }

        @Override
        public void onAddConfigResponse(final AudioConfig audioConfig,
                final @ImsMediaSession.SessionOperationResult int result) {
            ImsLog.d("result=" + result);

            if (mAudioCallbackHandler != null) {
                mAudioCallbackHandler.addConfigResponse(audioConfig, result);
            }
        }

        @Override
        public void onConfirmConfigResponse(final AudioConfig audioConfig,
                final @ImsMediaSession.SessionOperationResult int result) {
            ImsLog.d("result=" + result);

            if (mAudioCallbackHandler != null) {
                mAudioCallbackHandler.confirmConfigResponse(audioConfig, result);
            }
        }

        @Override
        public void onFirstMediaPacketReceived(final AudioConfig audioConfig) {
            ImsLog.d("FirstMediaPacketReceived for SessionId[" + getAudioSessionId() + "]");

            if (mAudioCallbackHandler != null) {
                mAudioCallbackHandler.firstMediaPacketReceived(audioConfig);
            }
        }

        @Override
        public void onHeaderExtensionReceived(final List<RtpHeaderExtension> extensions) {
            ImsLog.d("onHeaderExtensionReceived");

            if (mAudioCallbackHandler != null) {
                mAudioCallbackHandler.headerExtensionReceived(extensions);
            }
        }

        @Override
        public void notifyMediaInactivity(final @ImsMediaSession.PacketType int packetType) {
            ImsLog.d("packetType=" + packetType);

            if (mAudioCallbackHandler != null) {
                mAudioCallbackHandler.onNotifyMediaInactivity(packetType);
            }
        }

        @Override
        public void notifyPacketLoss(final int packetLossPercentage) {
            ImsLog.d("packetLossPercentage=" + packetLossPercentage);

            if (mAudioCallbackHandler != null) {
                mAudioCallbackHandler.onNotifyPacketLoss(packetLossPercentage);
            }
        }

        @Override
        public void notifyJitter(final int jitter) {
            ImsLog.d("jitter=" + jitter);

            if (mAudioCallbackHandler != null) {
                mAudioCallbackHandler.onNotifyJitter(jitter);
            }
        }

        @Override
        public void onMediaQualityChanged(@NonNull final CallQuality callQuality) {
            ImsLog.v("Media Quality Changed: " + callQuality.toString());

            if (mAudioCallbackHandler != null) {
                mAudioCallbackHandler.mediaQualityChanged(callQuality);
            }
        }

    }

    private class MediaAudioListenerProxy extends MediaAudioListener {

        @Override
        public void onAudioOpenSession(@NonNull final String localIpAddress,
            final int localPortNumber, @Nullable final AudioConfig audioConfig) {
            ImsLog.v("localPortNumber= " + localPortNumber + " localIpAddress= " + localIpAddress);
            handleAudioOpenSession(localIpAddress, localPortNumber, audioConfig);
        }

        @Override
        public void onAudioCloseSession() {
            ImsLog.v("onAudioCloseSession");
            handleAudioCloseSession();
        }

        @Override
        public void onAudioModifySession(@NonNull final AudioConfig audioConfig) {
            ImsLog.v("onAudioModifySession");
            handleAudioModifySession(audioConfig);
        }

        @Override
        public void onAudioAddConfig(@NonNull final AudioConfig audioConfig) {
            ImsLog.v("onIMMediaAddConfig");
            handleAudioAddConfig(audioConfig);
        }

        @Override
        public void onAudioDeleteConfig(@NonNull final AudioConfig audioConfig) {
            ImsLog.v("onAudioDeleteConfig");
            handleAudioDeleteConfig(audioConfig);
        }

        @Override
        public void onAudioConfirmConfig(@NonNull final AudioConfig audioConfig) {
            ImsLog.v("onAudioConfirmConfig");
            handleAudioConfirmConfig(audioConfig);
        }

        @Override
        public void onAudioSendDtmf(final char dtmfDigit, final int duration) {
            ImsLog.v("dtmfDigit=" + dtmfDigit + ", duration=" +duration);
            handleAudioSendDtmf(dtmfDigit, duration);
        }

        @Override
        public void onAudioSetMediaQualityThreshold(
            @NonNull final MediaQualityThreshold threshold) {
            ImsLog.v("onAudioSetMediaQualityThreshold: " + threshold.toString());
            handleAudioSetMediaQualityThreshold(threshold);
        }

        @Override
        public void onAudioSendHeaderExtension(
            @NonNull final List<RtpHeaderExtension> extensions) {
            ImsLog.v("onAudioSendHeaderExtension");
            handleAudioSendHeaderExtension(extensions);
        }
    };
}
