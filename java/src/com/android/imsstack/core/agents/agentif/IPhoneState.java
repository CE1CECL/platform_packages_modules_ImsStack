package com.android.imsstack.core.agents.agentif;

import android.os.Looper;

public interface IPhoneState extends IAgent {
    /**
     * Creates the phone state notifier without Handler.
     * Application SHOULD handle the event after posting the event on callback.
     */
    IPhoneStateNotifier createNotifier(ImsPhoneStateListener listener);

    /**
     * Creates the phone state notifier with Handler of the specified Looper.
     * Application can handle the events directly (on callback flow)
     * because event callback is invoked by its Handler.
     */
    IPhoneStateNotifier createNotifier(ImsPhoneStateListener listener, Looper looper);

    /**
     * Adds the notifier to monitor the phone state (call state, service state, ...).
     */
    void addNotifier(IPhoneStateNotifier notifier);

    /**
     * Removes the notifier to monitor the phone state (call state, service state, ...).
     */
    void removeNotifier(IPhoneStateNotifier notifier);

    /**
     * Get data RAT for cellular (TelephonyManager.NETWORK_TYPE_XXX)
     */
     int getCellularDataRAT();
}
