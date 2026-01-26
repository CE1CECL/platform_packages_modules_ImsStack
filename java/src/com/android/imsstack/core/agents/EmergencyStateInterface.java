/*
 * Copyright (C) 2026 The Android Open Source Project
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
package com.android.imsstack.core.agents;

import static android.telephony.TelephonyManager.DomainSelectionEmergencyType;

import androidx.annotation.NonNull;

/**
 * An interface for monitoring the emergency state such as emergency mode entered/exited.
 */
public interface EmergencyStateInterface extends IAgent {
    /**
     * An interface to monitor the emergency state events.
     */
    interface EmergencyStateListener {
        /**
         * Called when the emergency mode is changed.
         *
         * @param type The emergency mode type.
         * @param entered {@code true} if the emergency mode is entered, {@code false} otherwise.
         */
        void onEmergencyModeChanged(@DomainSelectionEmergencyType int type, boolean entered);
    }

    /**
     * Adds the listener to monitor the emergency state.
     *
     * @param listener A {@link EmergencyStateListener} instance to be added.
     */
    void addListener(@NonNull EmergencyStateListener listener);

    /**
     * Removes the listener to monitor the emergency state.
     *
     * @param listener A {@link EmergencyStateListener} instance to be removed.
     */
    void removeListener(@NonNull EmergencyStateListener listener);

    /**
     * Checks if the emergency mode is entered for the specified type.
     *
     * @param type The emergency mode type.
     * @return {@code true} if the emergency mode is entered, {@code false} otherwise.
     */
    boolean isInEmergencyMode(@DomainSelectionEmergencyType int type);
}
