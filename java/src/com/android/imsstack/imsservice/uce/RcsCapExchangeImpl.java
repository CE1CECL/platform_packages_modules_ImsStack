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
package com.android.imsstack.imsservice.uce;

import android.content.Context;
import android.net.Uri;
import android.telephony.ims.stub.CapabilityExchangeEventListener;
import android.telephony.ims.stub.RcsCapabilityExchangeImplBase;
import java.util.Collection;
import java.util.Set;
import com.android.imsstack.enabler.uce.impl.RcsCapEventListenerCallBack;
import com.android.imsstack.enabler.uce.impl.RcsCapOptionsResponseCallBack;
import com.android.imsstack.enabler.uce.impl.RcsCapPublishResponseCallBack;
import com.android.imsstack.enabler.uce.impl.RcsCapSubscribeResponseCallBack;
import com.android.imsstack.enabler.uce.impl.UceImpl;
import com.android.imsstack.enabler.uce.interf.IUceApi;
import com.android.imsstack.enabler.uce.interf.UceManager;
import com.android.imsstack.util.ImsLog;
import com.android.imsstack.util.MessageExecutor;

public class RcsCapExchangeImpl extends RcsCapabilityExchangeImplBase {
    private CapabilityExchangeEventListener mCapabilityExchangeEventListener;
    private int mSlotId = -1;
    private final MessageExecutor mCapExchangeExecutor =
        new MessageExecutor("RcsCapExchangeExecutor");
    private final MessageExecutor mCallBackExecutor =
            new MessageExecutor("CallBackExecutor");
    private UceImpl mUceImpl = null;
    private IUceApi mUceApi = null;
    private Context mContext = null;
    /**
     * Create a new RcsCapabilityExchangeImplBase instance.
     * @param listener used by the framework to listen to events from the vendor RCS stack
     * @param slotId  The slot ID associated with the RcsFeature.
     * @param context The context that is used in the ImsService.
     */
    public RcsCapExchangeImpl(CapabilityExchangeEventListener listener, int slotId,
        Context context) {
        mCapabilityExchangeEventListener = listener;
        mSlotId = slotId;
        mContext = context;
        mUceApi = UceManager.create(mContext, mSlotId);
        RcsCapEventListenerCallBack event = new RcsCapEventListenerCallBack(
                mCapabilityExchangeEventListener, mCallBackExecutor, mCapExchangeExecutor);
        if (mUceApi != null)
            mUceApi.setListener(event);
    }


    /**
     * The user capabilities of one or multiple contacts have been requested by the framework.
     * <p>
     * The implementer must follow up this call with an
     * {@link SubscribeResponseCallback#onCommandError} call to indicate this operation has failed
     * The response from the network to the SUBSCRIBE request must be sent back to the framework
     * using {@link SubscribeResponseCallback#onNetworkResponse(int, String)}.
     * As NOTIFY requests come in from the network, the requested contact’s capabilities should be
     * sent back to the framework using
     * {@link SubscribeResponseCallback#onNotifyCapabilitiesUpdate(List<String>}) and
     * {@link SubscribeResponseCallback#onResourceTerminated(List<Pair< Uri , String>>)}
     * should be called with the presence information for the contacts specified.
     * <p>
     * Once the subscription is terminated,
     * {@link SubscribeResponseCallback#onTerminated(String, long)} must be called for the
     * framework to finish listening for NOTIFY responses.
     *
     * @param uris A {@link Collection } of the {@link Uri}s that the framework is requesting the
     *             UCE capabilities for.
     * @param cb   The callback of the subscribe request.
     */
    @Override
    public void subscribeForCapabilities(Collection<Uri> uris, SubscribeResponseCallback cb) {
        if (mUceApi != null) {
            postAndRunTask(() -> {
                RcsCapSubscribeResponseCallBack callback =
                        new RcsCapSubscribeResponseCallBack(cb, mCallBackExecutor);
                mUceApi.subscribeCapabilities(uris, callback);
                });
        } else {
            ImsLog.i("mUceApi is null for slot "+ mSlotId);
        }
    }

    /**
     * The capabilities of this device have been updated and should be published to the network.
     * <p>
     * If this operation succeeds, network response updates should be sent to the framework using
     * {@link PublishResponseCallback#onNetworkResponse(int, String)}.
     *
     * @param pidfXml The XML PIDF document containing the capabilities of this device to be sent
     *                to the carrier’s presence server.
     * @param cb      The callback of the publish request
     */
    @Override
    public void publishCapabilities(String pidfXml, PublishResponseCallback cb) {
        if(mUceApi != null) {
            postAndRunTask(()-> {
            RcsCapPublishResponseCallBack rcsCapPublishResponseCallBack =
                        new RcsCapPublishResponseCallBack(cb, mCallBackExecutor);
            mUceApi.publishCapabilities(pidfXml, rcsCapPublishResponseCallBack);
            ImsLog.d("publishCapabilities pidfXml:"+ pidfXml);
            });
        } else {
            ImsLog.i("mUceApi is null for slot "+ mSlotId);
        }
    }

    /**
     * Push one's own capabilities to a remote user via the SIP OPTIONS presence exchange mechanism
     * in order to receive the capabilities of the remote user in response.
     * <p>
     * The implementer must use {@link OptionsResponseCallback} to send the response of
     * this query from the network back to the framework.
     *
     * @param contactUri     The URI of the remote user that we wish to get the capabilities of.
     * @param myCapabilities The capabilities of this device to send to the remote user.
     * @param callback       The callback of this request which is sent from the remote user.
     */
    @Override
    public void sendOptionsCapabilityRequest(Uri contactUri, Set<String> myCapabilities,
            OptionsResponseCallback callback) {
        if(mUceApi != null) {
            postAndRunTask(()-> {
                RcsCapOptionsResponseCallBack rcsCapOptionsResponseCallBack =
                        new RcsCapOptionsResponseCallBack(callback, mCallBackExecutor);
                mUceApi.sendOptionsCapabilityRequest(contactUri, myCapabilities,
                        rcsCapOptionsResponseCallBack);
            });
        } else {
            ImsLog.i("mUceApi is null for slot "+ mSlotId);
        }
    }

   private void postAndRunTask(Runnable task) {
       if (mCapExchangeExecutor != null) {
           mCapExchangeExecutor.execute(task);
       } else {
           ImsLog.d("MessageExecutor object is null");
       }
   }
}
