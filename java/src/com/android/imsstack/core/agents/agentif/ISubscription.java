package com.android.imsstack.core.agents.agentif;

public interface ISubscription extends IAgent {
    /** Adds listener for subscription */
    void addListener(SubscriptionListener listener);
    /** Removes listener for subscription */
    void removeListener(SubscriptionListener listener);

    /**
     * Returns an active subscription id for single SIM
     * or single IMS instance of multiple SIM environment.
     */
    int getActiveSubId();

    /**
     * Returns the IMSI from the default SIM.
     */
    String getImsi();

    /**
     * Returns the default phone id for Ims service.
     */
    int getPhoneId();

    /**
     * Returns a phone id for the specified SIM slot.
     */
    int getPhoneId(int slotId);

    /**
     * Returns the default slot id for Ims service.
     */
    int getSlotId();

    /**
     * Returns the default subscription id for Ims service.
     */
    int getSubId();

    /**
     * Returns a subscription id for the specified SIM slot.
     */
    int getSubId(int slotId);

    /**
     * Checks if all the SIMs are absent or not.
     */
    boolean isAllSimAbsent();

    /**
     * Checks if all the SIM are absent or locked.
     */
    boolean isAllSimAbsentOrLocked();

    /**
     * Checks if SIM is absent or not.
     */
    boolean isSimAbsent(int slotId);

    /**
     * Checks if SIM is loaded or not.
     */
    boolean isSimLoaded(int slotId);

    /**
     * Checks if SIM is locked or not.
     */
    boolean isSimLocked(int slotId);

    /**
     * Checks if SIM load is done or not for specified SIM slot.
     */
    boolean isSimLoadCompleted(int slotId);

    /**
     * Checks if SIM load is done or not.
     */
    boolean isSimLoadCompleted();

    /**
     * Returns the current SIM STATE
     */
    String getSimState();
}
