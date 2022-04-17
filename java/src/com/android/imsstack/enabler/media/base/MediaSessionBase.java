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

package com.android.imsstack.enabler.media.base;

import android.annotation.NonNull;
import android.content.Context;
import android.telephony.imsmedia.ImsMediaManager;
import com.android.imsstack.enabler.IBaseContext;
import com.android.imsstack.enabler.mtc.MtcMediaSession;
import com.android.imsstack.util.ImsLog;
import com.android.internal.annotations.VisibleForTesting;
import java.util.concurrent.Executor;
import java.util.concurrent.Executors;

/**
 * This instantiates and connects with ImsMediaManager
 */
public class MediaSessionBase {

    private final IBaseContext mContext;
    private MtcMediaSession mMtcMediaSession;
    private static boolean sIsImsMediaManagerReady;
    private static ImsMediaManager sImsMediaManager;
    private static Executor sExecutor;

    public MediaSessionBase(@NonNull IBaseContext context, MtcMediaSession mtcMediaSession) {

        mContext = context;
        mMtcMediaSession = mtcMediaSession;

        createMediaManagerInstance(context.getContext());
    }

    @VisibleForTesting
    public MediaSessionBase(IBaseContext context, MtcMediaSession mtcMediaSession,
        ImsMediaManager imsMediaManager, Executor executor) {

        mContext = context;
        mMtcMediaSession = mtcMediaSession;
        sImsMediaManager = imsMediaManager;
        sExecutor = executor;
    }

    private void createMediaManagerInstance(Context context){

        if (sImsMediaManager == null) {
            ImsLog.v("ImsMediaManager instance created");
            sExecutor = Executors.newSingleThreadExecutor();
            sImsMediaManager = new ImsMediaManager(context, sExecutor,
                                    new ImsMediaManagerCallback());
        }
    }

    /**
     * Returns ImsMediaManager instance
     */
    protected ImsMediaManager getMediaManagerInstance() {
        createMediaManagerInstance(mContext.getContext());
        return sImsMediaManager;
    }

    /**
     * Returns Executor instance
     */
    protected Executor getExecutor() {
        return sExecutor;
    }

    /**
     * Returns whether ImsMedia is connected or not
     */
    protected boolean isImsMediaConnected() {
        return sIsImsMediaManagerReady;
    }

    @VisibleForTesting(visibility = VisibleForTesting.Visibility.PROTECTED)
    public void setImsMediaConnected(boolean isConnected) {
        sIsImsMediaManagerReady = isConnected;
    }

    /**
     * Returns slotId
     */
    protected int getSlotId() {
        return mContext.getSlotId();
    }

    /**
     * clears the resources
     */
    public void close() {
        sImsMediaManager = null;
        sExecutor = null;
    }

    protected MtcMediaSession getMtcMediaSession() {
        return mMtcMediaSession;
    }

    /**
     * Implements Interface to receive call-back when the ImsMediaManager is connected
     * or disconnected.
     */
    private class ImsMediaManagerCallback implements ImsMediaManager.OnConnectedCallback {

        @Override
        public void onConnected() {
            ImsLog.i("ImsMediaManager - connected");
            sIsImsMediaManagerReady = true;
        }

        @Override
        public void onDisconnected() {
            ImsLog.i("ImsMediaManager - disconnected");
            // TODO: call has to be disconnected
            sIsImsMediaManagerReady = false;
            close();
        }
    }
}
