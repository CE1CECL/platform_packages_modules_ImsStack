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

import android.os.Parcel;
import android.util.ArraySet;

import androidx.test.filters.SmallTest;

import com.android.imsstack.enabler.uce.impl.define.UceFeatureTags;
import com.android.imsstack.enabler.uce.impl.define.UceMessage;
import com.android.imsstack.enabler.uce.impl.jni.UceJNI;
import com.android.imsstack.enabler.uce.impl.options.UceOptionsRequest;
import com.android.imsstack.enabler.uce.impl.publish.UcePublishRequest;
import com.android.imsstack.enabler.uce.impl.utils.UceUtils;
import com.android.imsstack.enabler.uce.interf.OptionsResponse;
import com.android.imsstack.enabler.uce.interf.PublishResponse;
import com.android.imsstack.enabler.uce.interf.UceApiConstant;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;
import org.mockito.ArgumentCaptor;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.verifyNoMoreInteractions;
import static org.junit.Assert.assertEquals;

import java.util.List;
import java.util.Set;

@RunWith(JUnit4.class)
public class UcePublishRequestTest {
    private static final int SLOT_ID = 0;
    private static final int KEY = 10;
    private static final boolean EXPIRED_ETAG = false;
    @Mock PublishResponse publishCb;
    @Mock UceJNI jni;

    @Before
    public void setUp(){
        MockitoAnnotations.initMocks(this);
    }

    @After
    public void cleanUp(){}

    @Test
    @SmallTest
    public void test_sendRequestWithEmptyUri() throws Exception {
        String pidfXml = "";
        boolean bAvailability = false;
        long capability = 150;
        UcePublishRequest request = createUcePublishRequest();
        // verify that send command error if the pidfxml is empty
        request.setRequestInfo(pidfXml, bAvailability, capability);
        request.sendRequest();
        verify(publishCb).onCommandError(eq(UceApiConstant.COMMAND_CODE_INVALID_PARAM));


        // verify that the input data be passed to the JNI
        ArgumentCaptor<Parcel> captor = ArgumentCaptor.forClass(Parcel.class);

        pidfXml = "test pidf";
        request.setRequestInfo(pidfXml, bAvailability, capability);
        request.sendRequest();
        verify(jni, times(1)).sendMessage(eq(SLOT_ID), captor.capture());

        Parcel parcel = captor.getValue();
        parcel.setDataPosition(0);
        assertEquals(parcel.readInt(), UceMessage.UCE_SEND_PUBLISH_CMD);
        assertEquals(parcel.readInt(), KEY);
        assertEquals(parcel.readInt(), 0); // extend
        assertEquals(parcel.readLong(), capability); // capability
        assertEquals(parcel.readString(), pidfXml); // pidf xml
        assertEquals(parcel.readString(), ""); // etag

        verifyNoMoreInteractions(jni);
    }

    @Test
    @SmallTest
    public void test_informResponse() throws Exception {
        int responseCode = 200;
        String reason = "OK";
        int reasonHdrCause = 0;
        String reasonHdrText = "test reason text";
        String eTag = "";

        UcePublishRequest request = createUcePublishRequest();
        request.informNetworkResponse(responseCode, reason, reasonHdrCause, reasonHdrText, eTag);

        verify(publishCb, times(1)).onNetworkResponse(eq(responseCode), eq(reason));

        reasonHdrCause = 10;

        request.informNetworkResponse(responseCode, reason, reasonHdrCause, reasonHdrText, eTag);
        verify(publishCb, times(1)).onNetworkResponse(eq(responseCode), eq(reason),
                eq(reasonHdrCause), eq(reasonHdrText));
    }

    @Test
    @SmallTest
    public void test_informCommandCode() throws Exception {
        int commandCode = 200;

        UcePublishRequest request = createUcePublishRequest();
        request.informCommandError(commandCode);

        verify(publishCb, times(1)).onCommandError(eq(commandCode));
    }

    private UcePublishRequest createUcePublishRequest() {
        UcePublishRequest request = new UcePublishRequest(publishCb, SLOT_ID, KEY, EXPIRED_ETAG,
                jni);
        return request;
    }
}
