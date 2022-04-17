/*
    Author
    <table>
    date        author                  description
    --------    --------------          ----------
    20150521    hwangoo.park@           Created
    </table>

    Description
*/

package com.android.imsstack.enabler.mtc;

/**
 * Listener for emergency callback mode.
 */
public interface EcbmListener {
    /**
     * This is invoked if emergency callback mode is entered.
     */
    public void onEcbmEntered();

    /**
     * This is invoked if emergency callback mode is exited.
     */
    public void onEcbmExited();
}
