/*
    Author
    <table>
    date        author                  description
    --------    --------------          ----------
    20151115    hwangoo.park@           Created
    </table>

    Description
*/

package com.android.imsstack.internal.imsservice;

import android.os.Handler;

import com.android.imsstack.enabler.mtc.CallStateListener;
import com.android.imsstack.enabler.mtc.ICallStateTracker;
import com.android.imsstack.enabler.mtc.IServiceStateTracker;

/**
 * Base interface for Google(Android) IMS service registry.
 */
public interface GImsServiceRegistry {
    /**
     * Checks if Ims service is on or not.
     *
     * @param phoneId phone id to be checked
     */
    public boolean isImsOn(int phoneId);

    /**
     * Turns on Ims.
     *
     * @param phoneId phone id to be turned on
     */
    public void turnOnIms(int phoneId);

    /**
     * Turns off Ims.
     *
     * @param phoneId phone id to be turned off
     */
    public void turnOffIms(int phoneId);

    /**
     * Returns UC call state tracker.
     */
    public ICallStateTracker getCallStateTracker(int phoneId);

    /**
     * Return UC service state tracker.
     */
    public IServiceStateTracker getServiceStateTracker(int phoneId);

    /**
     * Add/remove call state listener.
     */
    public void addCallStateListener(int phoneId, CallStateListener listener);
    public void removeCallStateListener(int phoneId, CallStateListener listener);

    /**
     * Add/remove service state listener.
     */
    // The Message object contains AsyncResult object in "obj" field.
    // The AsyncResult object contains ImsServiceState object in "result" field.
    public void registerForEmergencyServiceStateChanged(
            int phoneId, Handler h, int what, Object obj);
    public void unregisterForEmergencyServiceStateChanged(
            int phoneId, Handler h);

    // The Message object contains AsyncResult object in "obj" field.
    // The AsyncResult object contains ImsServiceState object in "result" field.
    public void registerForServiceStateChanged(
            int phoneId, Handler h, int what, Object obj);
    public void unregisterForServiceStateChanged(
            int phoneId, Handler h);
}
