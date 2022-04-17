/*
    Author
    <table>
    date        author                  description
    --------    --------------          ----------
    20150514    hwangoo.park@           Created
    </table>

    Description
*/

package com.android.imsstack.enabler.mtc;

public class CallStateListener {
    /**
     * Invokes when the call is created.
     */
    public void onCallCreated(Call call) {
        // no-op
    }

    /**
     * Invokes when a call is destroyed.
     */
    public void onCallDestroyed(Call call) {
        // no-op
    }

    /**
     * Invokes when MO call is establishing.
     */
    public void onCallEstablishing(Call call) {
        // no-op
    }

    /**
     * Invokes when MT call is alerted to user.
     */
    public void onCallRinging(Call call) {
        // no-op
    }

    /**
     * Invokes when MT call is accepted.
     */
    public void onCallAccepted(Call call) {
        // no-op
    }

    /**
     * Invokes when a call is established.
     */
    public void onCallEstablished(Call call) {
        // no-op
    }

    /**
     * Invokes when a call is updated.
     */
    public void onCallUpdated(Call call) {
        // no-op
    }

    /**
     * Invokes when a call is terminating.
     */
    public void onCallTerminating(Call call) {
        // no-op
    }

    /**
     * Invokes when a call is start-failed or terminated.
     */
    public void onCallTerminated(Call call) {
        // no-op
    }
}
