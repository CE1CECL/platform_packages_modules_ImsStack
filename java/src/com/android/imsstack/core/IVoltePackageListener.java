package com.android.imsstack.core;

public interface IVoltePackageListener {
    /**
     * Notifies the application that volte package is started and it's ready to use.
     */
    public void onVoltePackageReady(int slotId);
}
