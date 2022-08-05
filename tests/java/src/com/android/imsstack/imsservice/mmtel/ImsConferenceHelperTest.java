/*
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

package com.android.imsstack.imsservice.mmtel;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.mockito.Mockito.any;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import android.telephony.ims.stub.ImsCallSessionImplBase;

import com.android.imsstack.enabler.mtc.MtcCall;
import com.android.imsstack.imsservice.mmtel.base.ICallContext;
import com.android.imsstack.imsservice.mmtel.internal.ConferenceProxy;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;
import org.mockito.Mock;
import org.mockito.Mockito;

@RunWith(JUnit4.class)
public class ImsConferenceHelperTest {
    private static final String TAG = ImsConferenceHelper.class.getName();

    @Mock private ICallContext mICallContext;
    @Mock private ImsCallApp mImsCallApp;
    @Mock private ImsCallManager mImsCallManager;
    @Mock private ImsCallSessionImpl mFgImsCallSession, mBgImsCallSession;
    @Mock private MtcCall mFgMtcCall, mBgMtcCall;

    private ImsConferenceHelper mConfHelper;
    private String[] mParticipants = {"1234", "5678"};

    @Before
    public void setUp() {
        mConfHelper = ImsConferenceHelper.getInstance();
        mICallContext = Mockito.mock(ICallContext.class);
        mImsCallApp = Mockito.mock(ImsCallApp.class);
        mImsCallManager = Mockito.mock(ImsCallManager.class);
        mFgImsCallSession = Mockito.mock(ImsCallSessionImpl.class);
        mBgImsCallSession = Mockito.mock(ImsCallSessionImpl.class);
        mFgMtcCall = Mockito.mock(MtcCall.class);
        mBgMtcCall = Mockito.mock(MtcCall.class);
    }

    @Test
    public void test_getInstance() {
        assertNotNull(mConfHelper);
    }

    @Test
    public void test_onConferenceProxyDisposed() {
        ConferenceProxy mConfProxy = Mockito.mock(ConferenceProxy.class);
        mConfHelper.onConferenceProxyDisposed(mConfProxy);
        verify(mConfProxy, Mockito.times(1)).dispose();
    }

    @Test
    public void test_setBackgroundSession() {
        ImsCallSessionImplBase mSession = Mockito.mock(ImsCallSessionImplBase.class);
        mConfHelper.setBackgroundSession(mSession);
        assertEquals(mConfHelper.getBackgroundSession(), mSession);
    }

    @Test
    public void test_setTransientConferenceSession() {
        ImsCallSessionImplBase mConfSession = Mockito.mock(ImsCallSessionImplBase.class);
        mConfHelper.setTransientConferenceSession(mConfSession);
        assertEquals(mConfHelper.getTransientConferenceSession(), mConfSession);
    }

    @Test
    public void test_extendToConference() {
        when(mICallContext.getApp()).thenReturn(mImsCallApp);
        when(mImsCallApp.getCallManager()).thenReturn(mImsCallManager);
        when(mImsCallManager.getActiveSession()).thenReturn(mFgImsCallSession);
        when(mFgImsCallSession.getMtcCall()).thenReturn(mFgMtcCall);

        assertEquals(false, mConfHelper.extendToConference(mICallContext, mParticipants));
        verify(mFgImsCallSession, Mockito.times(1)).setConferenceProxy(any(ConferenceProxy.class));

        mFgMtcCall = null;
        when(mFgImsCallSession.getMtcCall()).thenReturn(mFgMtcCall);
        assertNull(mFgMtcCall);

        mFgImsCallSession = null;
        when(mImsCallManager.getActiveSession()).thenReturn(mFgImsCallSession);
        assertNull(mFgImsCallSession);
    }

    @Test
    public void test_merge() {
        when(mICallContext.getApp()).thenReturn(mImsCallApp);
        when(mImsCallApp.getCallManager()).thenReturn(mImsCallManager);
        when(mImsCallManager.getActiveSession()).thenReturn(mFgImsCallSession);
        when(mImsCallManager.getHoldSession()).thenReturn(mBgImsCallSession);
        when(mFgImsCallSession.getMtcCall()).thenReturn(mFgMtcCall);
        when(mBgImsCallSession.getMtcCall()).thenReturn(mBgMtcCall);

        assertEquals(false, mConfHelper.merge(mICallContext));
        verify(mBgImsCallSession, Mockito.times(1)).setConferenceProxy(any(ConferenceProxy.class));
    }

    @After
    public void tearDown() {
        mConfHelper = null;
        mICallContext = null;
        mImsCallApp = null;
        mImsCallManager = null;
    }
}
