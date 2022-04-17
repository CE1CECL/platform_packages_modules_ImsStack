package com.android.imsstack.core;

public interface ICommonPackageListener {
    /**
     * Notifies the application that common package is started and it's ready to use.
     */
    public void onCommonPackageReady(int slotId);

    /**
     * Notifies the application that common package will be stopped after returning this method.
     */
    public void onCommonPackageStop(int slotId);
}
