package com.android.imsstack.core.agents.agentif;

public interface IWakeLock extends IAgent {
    /**
     * Set the device stay on until timer expired
     */
    void acquire(int timeout);
    /**
     * Set the device stay on until timer expired when screen is off
     */
    void acquire(int timeout, boolean screenOffOnly);

}
