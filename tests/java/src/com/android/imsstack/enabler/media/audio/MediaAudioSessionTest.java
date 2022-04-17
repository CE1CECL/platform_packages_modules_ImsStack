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

import static org.junit.Assert.assertEquals;
import static org.mockito.ArgumentMatchers.isNull;
import static org.mockito.Matchers.any;
import static org.mockito.Matchers.eq;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import android.content.Context;
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
import com.android.imsstack.enabler.media.MediaSocket;
import com.android.imsstack.enabler.media.audio.MediaAudioCallbackHandler;
import com.android.imsstack.enabler.media.audio.MediaAudioListener;
import com.android.imsstack.enabler.media.audio.MediaTestUtils;
import com.android.imsstack.enabler.mtc.MtcMediaSession;
import java.net.DatagramSocket;
import java.util.List;
import java.util.concurrent.Executor;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

@RunWith(JUnit4.class)
public class MediaAudioSessionTest {
    // open Session Parameters
    private static final String LOCAL_RTP_ADDRESS = "111.11.11.11";
    private static final int LOCAL_RTP_PORT = 50010;
    private static final int SESSION_ID = 439;

    // Session Operator Results
    private static final int RESULT_FAILURE = ImsMediaSession.RESULT_NO_RESOURCES;
    private static final int RESULT_SUCCESS = ImsMediaSession.RESULT_SUCCESS;

    // Packet Types
    private static final int RTP = ImsMediaSession.PACKET_TYPE_RTP;
    private static final int RTCP = ImsMediaSession.PACKET_TYPE_RTCP;

    // Dtmf settings
    private static final int DTMF_DURATION = 20;
    private static final char DTMF_DIGIT = '9';

    // Mock Objects
    @Mock Context context;
    @Mock IBaseContext mMockContext;
    @Mock MtcMediaSession mMockMtcMediaSession;
    @Mock MediaAudioCallbackHandler mMockAudioCallbackHandler;
    @Mock ImsAudioSession mMockAudioSession;
    @Mock ImsMediaManager mMockImsMediaManager;
    @Mock Executor mExecutor;

    private MediaAudioSession mMediaAudioSessionUT;
    private AudioSessionCallback mAudioSessionCallback;
    private MediaAudioListener mMediaAudioListener;

    @Before
    public void setUp() throws Exception {
        //Initialize Mock Objects
        MockitoAnnotations.initMocks(this);
        when(mMockContext.getContext()).thenReturn(context);

        //create the instance to test
        mMediaAudioSessionUT = new MediaAudioSession(mMockContext, mMockMtcMediaSession,
            mMockAudioCallbackHandler, mMockAudioSession, mMockImsMediaManager, mExecutor);
        mAudioSessionCallback = mMediaAudioSessionUT.getAudioSessionCallback();
        mMediaAudioListener = mMediaAudioSessionUT.getAudioListenerProxy();
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void testOpenSession() {

        AudioConfig audioConfig = MediaTestUtils.createAudioConfig();
        // OpenSession Received
        mMediaAudioSessionUT.setAudioSession(null);
        mMediaAudioSessionUT.setImsMediaConnected(true);
        mMediaAudioListener.onAudioOpenSession(LOCAL_RTP_ADDRESS, LOCAL_RTP_PORT, audioConfig);
        verify(mMockImsMediaManager).openSession(isNull(), isNull(),
            eq(ImsMediaSession.SESSION_TYPE_AUDIO),
            eq(audioConfig), eq(mExecutor), eq(mAudioSessionCallback));

        /**
         * AudioSession was opened already, but OpenAudioSession requested again
         * for same session
         */
        mMediaAudioSessionUT.setAudioSession(mMockAudioSession);
        mMediaAudioListener.onAudioOpenSession(LOCAL_RTP_ADDRESS, LOCAL_RTP_PORT, audioConfig);
        verify(mMockAudioCallbackHandler).openSessionResponse(
            eq(ImsMediaSession.RESULT_NOT_SUPPORTED));

        // ImsMediaManager is not connected
        mMediaAudioSessionUT.setAudioSession(null);
        mMediaAudioSessionUT.setImsMediaConnected(false);
        mMediaAudioListener.onAudioOpenSession(LOCAL_RTP_ADDRESS, LOCAL_RTP_PORT, audioConfig);
        verify(mMockAudioCallbackHandler).openSessionResponse(
            eq(ImsMediaSession.RESULT_NOT_READY));

        // OpenSession Success Received.
        when(mMockAudioSession.getSessionId()).thenReturn(SESSION_ID);
        mAudioSessionCallback.onOpenSessionSuccess(mMockAudioSession);
        verify(mMockAudioCallbackHandler).openSessionResponse(
            eq(ImsMediaSession.RESULT_SUCCESS));
        assertEquals(mMediaAudioSessionUT.getAudioSessionId(),SESSION_ID);

        // AudioSession null object received in OpenSession Success.
        mAudioSessionCallback.onOpenSessionSuccess(null);
        verify(mMockAudioCallbackHandler).openSessionResponse(
            eq(ImsMediaSession.RESULT_NO_MEMORY));

        // OpenSession Failure Received
        mAudioSessionCallback.onOpenSessionFailure(ImsMediaSession.RESULT_NO_RESOURCES);
        verify(mMockAudioCallbackHandler).openSessionResponse(
            eq(ImsMediaSession.RESULT_NO_RESOURCES));
    }

    @Test
    public void testCloseSession() {
        mMediaAudioListener.onAudioCloseSession();
        verify(mMockImsMediaManager).closeSession(eq(mMockAudioSession));
    }

    @Test
    public void testSessionChanged() {
        mAudioSessionCallback.onSessionChanged(ImsMediaSession.SESSION_STATE_ACTIVE);
        verify(mMockAudioCallbackHandler).sessionChanged(eq(ImsMediaSession.SESSION_STATE_ACTIVE));
    }

    @Test
    public void testModifySession() {
        // Modify Session Request
        AudioConfig audioConfig = MediaTestUtils.createAudioConfig();
        mMediaAudioListener.onAudioModifySession(audioConfig);
        verify(mMockAudioSession).modifySession(eq(audioConfig));

        // Modify Session Response - SUCCESS
        mAudioSessionCallback.onModifySessionResponse(audioConfig,
            RESULT_SUCCESS);
        verify(mMockAudioCallbackHandler).modifySessionResponse(eq(audioConfig),
            eq(RESULT_SUCCESS));

        // Modify Session Response - FAILURE
        mAudioSessionCallback.onModifySessionResponse(audioConfig,
            RESULT_FAILURE);
        verify(mMockAudioCallbackHandler).modifySessionResponse(eq(audioConfig),
            eq(RESULT_FAILURE));
    }

    @Test
    public void testAddConfig() {
        // Add Config Request
        AudioConfig audioConfig = MediaTestUtils.createAudioConfig();
        mMediaAudioListener.onAudioAddConfig(audioConfig);
        verify(mMockAudioSession).addConfig(eq(audioConfig));

        // Add Config Response - SUCCESS
        mAudioSessionCallback.onAddConfigResponse(audioConfig,
            RESULT_SUCCESS);
        verify(mMockAudioCallbackHandler).addConfigResponse(eq(audioConfig),
            eq(RESULT_SUCCESS));

        // Add Config Response - FAILURE
        mAudioSessionCallback.onAddConfigResponse(audioConfig,
            RESULT_FAILURE);
        verify(mMockAudioCallbackHandler).addConfigResponse(eq(audioConfig),
            eq(RESULT_FAILURE));
    }

    @Test
    public void testDeleteConfig() {
        // Delete Config Request
        AudioConfig audioConfig = MediaTestUtils.createAudioConfig();
        mMediaAudioListener.onAudioDeleteConfig(audioConfig);
        verify(mMockAudioSession).deleteConfig(eq(audioConfig));
    }

    @Test
    public void testConfirmConfig() {
        // Confirm Config Request
        AudioConfig audioConfig = MediaTestUtils.createAudioConfig();
        mMediaAudioListener.onAudioConfirmConfig(audioConfig);
        verify(mMockAudioSession).confirmConfig(eq(audioConfig));

        // Confirm Config Response - SUCCESS
        mAudioSessionCallback.onConfirmConfigResponse(audioConfig,
            RESULT_SUCCESS);
        verify(mMockAudioCallbackHandler).confirmConfigResponse(eq(audioConfig),
            eq(RESULT_SUCCESS));

        // Confirm Config Response - FAILURE
        mAudioSessionCallback.onConfirmConfigResponse(audioConfig,
            RESULT_FAILURE);
        verify(mMockAudioCallbackHandler).confirmConfigResponse(eq(audioConfig),
            eq(RESULT_FAILURE));
    }

    @Test
    public void testDtmf() {
        // Send Dtmf
        mMediaAudioListener.onAudioSendDtmf(DTMF_DIGIT, DTMF_DURATION);
        verify(mMockAudioSession).sendDtmf(eq(DTMF_DIGIT), eq(DTMF_DURATION));
    }

    @Test
    public void testSetMediaQualityThreshold() {
        // Set Media Quality Threshold
        MediaQualityThreshold threshold = MediaTestUtils.createMediaQualityThreshold();
        mMediaAudioListener.onAudioSetMediaQualityThreshold(threshold);
        verify(mMockAudioSession).setMediaQualityThreshold(eq(threshold));
    }

    @Test
    public void testRtpHeaderExtension() {
        // Send RtpHeaderExtensions
        List<RtpHeaderExtension> rtpExtensions = MediaTestUtils.createRtpExtensions();
        mMediaAudioListener.onAudioSendHeaderExtension(rtpExtensions);
        verify(mMockAudioSession).sendHeaderExtension(eq(rtpExtensions));

        // Receive RtpHeaderExtensions
        mAudioSessionCallback.onHeaderExtensionReceived(rtpExtensions);
        verify(mMockAudioCallbackHandler).headerExtensionReceived(eq(rtpExtensions));
    }

    @Test
    public void testMediaInactivityNotifications() {
        // Receive RTP Inactivity Notification
        mAudioSessionCallback.notifyMediaInactivity(RTP);
        verify(mMockAudioCallbackHandler).onNotifyMediaInactivity(eq(RTP));

        // Receive RTCP Inactivity Notification
        mAudioSessionCallback.notifyMediaInactivity(RTCP);
        verify(mMockAudioCallbackHandler).onNotifyMediaInactivity(eq(RTCP));
    }

    @Test
    public void testMediaQualityNotifications() {
        // Receive Packet Loss Notification
        mAudioSessionCallback.notifyPacketLoss(
            MediaTestUtils.PACKET_LOSS_PERCENT);
        verify(mMockAudioCallbackHandler).onNotifyPacketLoss(
            eq(MediaTestUtils.PACKET_LOSS_PERCENT));

        // Receive Packet Loss Notification
        mAudioSessionCallback.notifyJitter(MediaTestUtils.JITTER);
        verify(mMockAudioCallbackHandler).onNotifyJitter(eq(MediaTestUtils.JITTER));

        // Receive Media Quality Changed Notification
        CallQuality callQuality = MediaTestUtils.createCallQuality();
        mAudioSessionCallback.onMediaQualityChanged(callQuality);
        verify(mMockAudioCallbackHandler).mediaQualityChanged(eq(callQuality));
    }

}
