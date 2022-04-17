package com.android.imsstack.imsservice.mmtel.base;

/**
 * Listener for SRVCC state.
 */
public interface ISrvccStateListener {
    /**
     * This is invoked if SRVCC handover is started.
     */
    public void onHandoverStarted();

    /**
     * This is invoked if SRVCC handover is completed.
     */
    public void onHandoverCompleted();

    /**
     * This is invoked if SRVCC handover is canceled.
     */
    public void onHandoverCanceled();

    /**
     * This is invoked if SRVCC handover is failed.
     */
    public void onHandoverFailed();
}
