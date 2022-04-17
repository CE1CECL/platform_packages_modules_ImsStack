package com.android.imsstack.imsservice.mmtel.base;

public interface ISrvccStateTracker {
    /**
     * Adds a listener for SRVCC events.
     */
    public void addListener(ISrvccStateListener listener);

    /**
     * Removes a listener for SRVCC events.
     */
    public void removeListener(ISrvccStateListener listener);

    /**
     * Checks if SRVCC is successfully completed.
     */
    public boolean isSrvccCompleted();

    /**
     * Checks if SRVCC is ongoing.
     */
    public boolean isSrvccPending();
}
