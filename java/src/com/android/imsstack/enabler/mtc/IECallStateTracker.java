package com.android.imsstack.enabler.mtc;

public interface IECallStateTracker {
    /**
     * Adds a listener for ECBM (Emergency CallBack Mode).
     */
    public void addEcbmListener(EcbmListener listener);

    /**
     * Removes a listener for ECBM (Emergency CallBack Mode).
     */
    public void removeEcbmListener(EcbmListener listener);

    /**
     * Exits emergency callback mode.
     */
    public void exitEmergencyCallbackMode(boolean bExitedByNewCall);

    /**
     * Checks if ECBM is entered.
     */
    public boolean isEcbmEntered();
}
