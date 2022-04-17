package com.android.imsstack.core.agents.agentif;

public class SubscriptionListener {
    /**
     * Invokes if SIM load is completed.
     */
    public void onSimLoadCompleted(int slotId) {
    }

    /**
     * Invokes when default subscription is changed.
     */
    public void onDefaultSubscriptionChanged(int subId) {
    }

    /**
     * Invokes when default data subscription is changed.
     */
    public void onDefaultDataSubscriptionChanged(int subId) {
    }

    /**
     * Invokes when carrier configuration is changed.
     */
    public void onCarrierConfigChanged(int phoneId, int subId) {
    }
}
