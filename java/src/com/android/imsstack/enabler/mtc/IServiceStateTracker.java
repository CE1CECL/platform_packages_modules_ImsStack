package com.android.imsstack.enabler.mtc;

import android.os.Handler;

public interface IServiceStateTracker {
    /**
     * Checks if the specified service is registered.
     *
     * @param serviceType IUMtcService.SERVICE_XXX
     */
    public boolean isServiceRegistered(int serviceType);

    // The Message object contains AsyncResult object in "obj" field.
    // The AsyncResult object contains ImsServiceState object in "result" field.
    public void registerForEmergencyServiceStateChanged(Handler h, int what, Object obj);
    public void unregisterForEmergencyServiceStateChanged(Handler h);

    // The Message object contains AsyncResult object in "obj" field.
    // The AsyncResult object contains ImsServiceState object in "result" field.
    public void registerForServiceStateChanged(Handler h, int what, Object obj);
    public void unregisterForServiceStateChanged(Handler h);
}
