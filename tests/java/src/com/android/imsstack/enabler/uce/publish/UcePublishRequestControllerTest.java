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
package com.android.imsstack.enabler.uce.publish;

import android.os.Handler;
import android.os.Looper;
import android.os.Parcel;

import androidx.test.filters.SmallTest;

import com.android.imsstack.enabler.uce.impl.define.UceMessage;
import com.android.imsstack.enabler.uce.impl.define.UceServiceIds;
import com.android.imsstack.enabler.uce.impl.jni.IUceJNIListener;
import com.android.imsstack.enabler.uce.impl.jni.UceJNI;
import com.android.imsstack.enabler.uce.impl.publish.UcePublishRequest;
import com.android.imsstack.enabler.uce.impl.publish.UcePublishRequestController;
import com.android.imsstack.enabler.uce.interf.PublishResponse;
import com.android.imsstack.enabler.uce.interf.UceApiConstant;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

import static org.mockito.ArgumentMatchers.anyBoolean;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.doReturn;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.verifyNoMoreInteractions;

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

@RunWith(JUnit4.class)
public class UcePublishRequestControllerTest {
    private static final int MAX_WAIT_TIME = 1000;
    private static final int SLOT_ID = 0;
    @Mock PublishResponse publishCb;
    @Mock UcePublishRequest active;
    @Mock UcePublishRequest pending;

    private final TestUceJni mUceJni = new TestUceJni();

    private class TestUceJni extends UceJNI {
        public IUceJNIListener mUceJniListener;
        public TestUceJni() {
            super();
            mUceJniListener = null;
        }

        @Override
        public void init(int nSimSlot) {};

        @Override
        public void release(int nSimSlot) {};

        @Override
        public void addListener(int nSimSlot, IUceJNIListener mListener, int nMsgType) {
            mUceJniListener = mListener;
        }
    }

    @Before
    public void setUp(){
        MockitoAnnotations.initMocks(this);
    }

    @After
    public void cleanUp(){}

    @Test
    @SmallTest
    public void test_publishCapabilitiesWithoutImsRegistrationAndEmptyPidf() throws Exception {
        UcePublishRequestController controller = createUcePublishRequestController();

        // Check whether a command error is transmitted when a request is received in the state
        // that ims is not registered.
        controller.setImsRegistrationStatus(false);
        controller.publishCapabilities("", publishCb, active);

        verify(publishCb).onCommandError(eq(UceApiConstant.COMMAND_CODE_SERVICE_UNAVAILABLE));

        // Check whether a command error is transmitted when a request is received with
        // empty pidfxml.
        controller.setImsRegistrationStatus(true);
        controller.publishCapabilities("", publishCb, active);

        verify(publishCb).onCommandError(eq(UceApiConstant.COMMAND_CODE_NO_CHANGE));
        verifyNoMoreInteractions(publishCb);
    }

    @Test
    @SmallTest
    public void test_publishCapabilities() throws Exception {
        String pidfxml = UceServiceIds.ServiceIds.SERVICE_ID_PRESENCE.getId();

        UcePublishRequestController controller = createUcePublishRequestController();

        // Check whether a command error is transmitted when a request is received with
        // pidfxml same as the latest capability.
        controller.setImsRegistrationStatus(true);
        controller.setActiveCapability((long)UceServiceIds.SERVICE_ID_PRESENCE);
        controller.publishCapabilities(pidfxml, publishCb, active);

        verify(publishCb).onCommandError(eq(UceApiConstant.COMMAND_CODE_NO_CHANGE));

        controller.setActiveCapability((long)0);
        controller.publishCapabilities(pidfxml, publishCb, active);
        verify(active).setRequestInfo(eq(pidfxml), anyBoolean(),
                eq(UceServiceIds.SERVICE_ID_PRESENCE));
        verify(active).sendRequest();
        verifyNoMoreInteractions(active);
    }

    @Test
    @SmallTest
    public void test_publishCapabilitiesWithLatestCapabilityPidf() throws Exception {
        String pidfxml = getPidfxml();

        UcePublishRequestController controller = createUcePublishRequestController();

        // Check whether a command error is transmitted when a request is received with
        // pidfxml same as the latest capability.
        controller.setImsRegistrationStatus(true);
        controller.setActiveCapability((long)(UceServiceIds.SERVICE_ID_IPCALL_VOICE |
                UceServiceIds.SERVICE_ID_STANDALONE_MESSAGING));
        controller.publishCapabilities(pidfxml, publishCb, active);

        verify(publishCb).onCommandError(eq(UceApiConstant.COMMAND_CODE_NO_CHANGE));
    }

    @Test
    @SmallTest
    public void test_deletePendingRequest() throws Exception {

        UcePublishRequestController controller = createUcePublishRequestController();
        controller.setPendingRequest(pending);

        controller.deletePendingRequest();

        verify(pending).informCommandError(eq(UceApiConstant.COMMAND_CODE_SERVICE_UNAVAILABLE));
        verifyNoMoreInteractions(pending);
    }

    @Test
    @SmallTest
    public void test_handlePendingRequest() throws Exception {

        UcePublishRequestController controller = createUcePublishRequestController();
        controller.setPendingRequest(pending);

        controller.handlePendingRequest();

        verify(pending).sendRequest();
        verifyNoMoreInteractions(pending);
    }

    @Test
    @SmallTest
    public void test_receivedResponse() throws Exception {
        int key = 10;
        doReturn(key).when(active).getKey();

        long capability = UceServiceIds.SERVICE_ID_PRESENCE;
        int responseCode = 400;
        String reason = "test";
        int reasonCause = 5;
        String reasonCauseText = "reasonCauseText";
        String eTag = "fadsfasdgfdagd";

        UcePublishRequestController controller = createUcePublishRequestController();

        controller.setActiveRequest(active);
        controller.setPendingRequest(pending);

        Parcel parcel = Parcel.obtain();
        parcel.writeInt(UceMessage.UCE_PUBLISH_RESPONSE_IND);
        parcel.writeInt(key); // key
        parcel.writeLong(capability); // capability
        parcel.writeInt(responseCode); // responseCode
        parcel.writeString(reason); // reason
        parcel.writeInt(reasonCause); // reasonHeaderCause
        parcel.writeString(reasonCauseText); // reasonHeaderText
        parcel.writeString(eTag); // etag
        parcel.writeInt(0); // need to retry
        parcel.setDataPosition(0);

        mUceJni.mUceJniListener.onPublishResponseMessage(parcel);

        Handler handler = controller.getHandler();
        waitForHandlerActionDelayed(handler, MAX_WAIT_TIME, 0);

        verify(active, times(1)).informNetworkResponse(eq(responseCode),
                eq(reason), eq(reasonCause), eq(reasonCauseText), eq(eTag));

        verify(pending, times(1)).sendRequest();
    }

    @Test
    @SmallTest
    public void test_receivedError() throws Exception {
        int key = 11;
        doReturn(key).when(active).getKey();

        int commandCode = 50;

        UcePublishRequestController controller = createUcePublishRequestController();
        controller.setActiveRequest(active);
        controller.setPendingRequest(pending);

        Parcel parcel = Parcel.obtain();
        parcel.writeInt(UceMessage.UCE_PUBLISH_CMD_ERROR_IND);
        parcel.writeInt(key); // key
        parcel.writeLong(commandCode); // commandErrorCode
        parcel.setDataPosition(0);

        mUceJni.mUceJniListener.onPublishResponseMessage(parcel);

        Handler handler = controller.getHandler();
        waitForHandlerActionDelayed(handler, MAX_WAIT_TIME, 0);

        verify(active, times(1)).informCommandError(eq(commandCode));

        verify(pending, times(1)).sendRequest();
    }

    private UcePublishRequestController createUcePublishRequestController() {
        UcePublishRequestController controller = new UcePublishRequestController(SLOT_ID,
                mUceJni, Looper.getMainLooper());
        return controller;
    }

    private void waitForHandlerActionDelayed(Handler h, long timeoutMillis, long delayMs) {
        final CountDownLatch lock = new CountDownLatch(1);
        h.postDelayed(lock::countDown, delayMs);
        while (lock.getCount() > 0) {
            try {
                lock.await(timeoutMillis, TimeUnit.MILLISECONDS);
            } catch (InterruptedException e) {
                // do nothing
            }
        }
    }

    private String getPidfxml() {
        StringBuilder pidfBuilder = new StringBuilder();
        pidfBuilder.append("<?xml version='1.0' encoding='utf-8' standalone='yes' ?>")
                .append("<presence entity=\"").append("123456").append("\"")
                .append(" xmlns=\"urn:ietf:params:xml:ns:pidf\"")
                .append(" xmlns:op=\"urn:oma:xml:prs:pidf:oma-pres\"")
                .append(" xmlns:caps=\"urn:ietf:params:xml:ns:pidf:caps\">")
                .append("<tuple id=\"tid0\"><status><basic>open</basic></status>")
                .append("<op:service-description>")
                .append("<op:service-id>org.openmobilealliance:StandaloneMsg</op:service-id>")
                .append("<op:version>1.0</op:version>")
                .append("<op:description>description_test1</op:description>")
                .append("</op:service-description>")
                .append("<contact>").append("123456").append("</contact>")
                .append("</tuple>")
                .append("<tuple id=\"tid1\"><status><basic>open</basic></status>")
                .append("<op:service-description>")
                .append("<op:service-id>org.3gpp.urn:urn-7:3gpp-service.ims.icsi.mmtel</op:service-id>")
                .append("<op:version>1.0</op:version>")
                .append("<op:description>description_test2</op:description>")
                .append("</op:service-description>")
                .append("<contact>").append("123456").append("</contact>")
                .append("</tuple></presence>");
        return pidfBuilder.toString();
    }
}
