package com.android.imsstack.core.agents.agentif;

public interface IBatteryState extends IAgent {
    /**
     * Returns the battery level.
     */
    int getBatteryLevel();

    /**
     * Checks if battery level is low or not.
     */
    boolean isLowBattery();

    /**
     * Checks if the device is connected by USB cable or charger.
     */
    boolean isPowerPlugged(boolean log);

    /**
     * Notify low battery state.
     */
    void notifyLowBatteryState(int slotId);
}
