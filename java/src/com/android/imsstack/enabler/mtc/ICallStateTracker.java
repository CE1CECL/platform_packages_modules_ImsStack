package com.android.imsstack.enabler.mtc;

public interface ICallStateTracker {
    public void addListener(CallStateListener listener);
    public void removeListener(CallStateListener listener);

    /**
     * Checks if call (active or pending session) exists.
     */
    public boolean hasCall();

    /**
     * Checks if established call (in OFFHOOK state) exists.
     */
    public boolean hasEstablishedCall();
}
