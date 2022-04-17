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

package com.android.imsstack.imsservice.sipcontroller;

import android.util.Log;

public class SipTransportBaseRegistrationHandler implements ISipTransportBaseRegistrationListener{
    private final int mSlotId;

    public SipTransportBaseRegistrationHandler(int slotId) {
        mSlotId = slotId;
    }
    /**
     * Called when feature tag registration is required and also when feature tag configuration
     * is changed.
     */
    @Override
    public void triggerSipTransportDelegateRegistration() {
        Log.i(ImsSipTransport.LOG_TAG, "triggerSipTransportNetworkRegistration");
        //TODO call the native updateRegistration with the list of feature tags supported by
        // SipTransport
    }

    @Override
    public void triggerSipTransportDelegateDeregistration() {
        Log.i(ImsSipTransport.LOG_TAG, "triggerSipTransportNetworkDeregistration");
        //TODO Get all the sip delegates from the sip delegate manager and then  change the status
        // to de-registering.
    }
}
