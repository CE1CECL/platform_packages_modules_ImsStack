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

/**
 * Listener for Google(Android) IMS service state (turn on / off).
 */
public interface GImsServiceStateListener {
    /**
     * Invokes when Ims service is turned on.
     *
     * @param phoneId phone identifier (as a default, 0) to be turned on
     */
    public void onImsServiceOn(int phoneId);

    /**
     * Invokes when Ims service is turned off.
     *
     * @param phoneId phone identifier (as a default, 0) to be turned off
     */
    public void onImsServiceOff(int phoneId);
}
