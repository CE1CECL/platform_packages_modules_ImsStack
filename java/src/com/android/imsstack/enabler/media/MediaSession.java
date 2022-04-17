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

package com.android.imsstack.enabler.media;

import android.annotation.NonNull;
import android.annotation.Nullable;
import android.telephony.imsmedia.ImsMediaManager;
import android.telephony.imsmedia.ImsMediaSession;
import android.telephony.imsmedia.RtpConfig;
import com.android.imsstack.enabler.IBaseContext;
import com.android.imsstack.enabler.media.audio.MediaAudioSession;
import com.android.imsstack.enabler.media.base.MediaSessionBase;
import com.android.imsstack.enabler.mtc.MtcMediaSession;
import com.android.imsstack.util.ImsLog;
import com.android.internal.annotations.VisibleForTesting;
import java.net.DatagramSocket;
import java.util.concurrent.Executor;

/**
 * This manages Media Sessions
 */
public class MediaSession  extends MediaSessionBase {

    @VisibleForTesting
    ImsMediaManager mImsMediaManager;

    public MediaSession(IBaseContext context, MtcMediaSession mtcMediaSession){
        super(context, mtcMediaSession);
        ImsLog.v("MediaSession created");
    }

    @VisibleForTesting
    public MediaSession(IBaseContext context, MtcMediaSession mtcMediaSession,
        ImsMediaManager imsMediaManager, Executor executor){
        super(context, mtcMediaSession, imsMediaManager, executor);
        ImsLog.v("MediaSession created");
    }

    protected void openSession(@NonNull final DatagramSocket rtpSocket,
            @NonNull final DatagramSocket rtcpSocket,
            @NonNull final @ImsMediaSession.SessionType int sessionType,
            @Nullable final RtpConfig rtpConfig,
            @NonNull final ImsMediaManager.SessionCallback callback) {
        getMediaManagerInstance().openSession(rtpSocket,
                    rtcpSocket, sessionType, rtpConfig,
                    getExecutor(), callback);
    }

    protected void closeSession(@NonNull final ImsMediaSession session) {
        getMediaManagerInstance().closeSession(session);
    }

}
