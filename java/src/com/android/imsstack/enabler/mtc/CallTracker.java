/*
    Author
    <table>
    date        author                  description
    --------    --------------          ----------
    20131015    hwangoo.park@           Created
    </table>

    Description
*/

package com.android.imsstack.enabler.mtc;

public interface CallTracker {
    /**
     * VoLTE/VT call state.
     */
    public static final int CALL_STATE_IDLE = 0;
    public static final int CALL_STATE_RINGING = 1;
    public static final int CALL_STATE_OFFHOOK = 2;
    public static final int CALL_STATE_RINGBACK = 3;

    /**
     * It will be used in {@code event} when calling {@link updateCallState}.
     * The event name in {@link IUMtcCall} can be also used in {@code event}.
     */
    public static final int E_CALL_EVENT_NONE = 91;
    public static final int E_CALL_EVENT_ORIGINATING = 92;
    public static final int E_CALL_EVENT_CONNECTED = 93;
    public static final int E_CALL_EVENT_END = 94;
    public static final int E_CALL_EVENT_SRVCC = 95;

    /**
     * Call related event for creation / destruction
     */
    public static final int CALL_EVENT_CREATE = 1;
    public static final int CALL_EVENT_ESTABLISHING = 2;
    public static final int CALL_EVENT_RINGING = 3;
    public static final int CALL_EVENT_ACCEPT = 4;
    public static final int CALL_EVENT_ESTABLISHED = 5;
    public static final int CALL_EVENT_UPDATED = 6;
    public static final int CALL_EVENT_TERMINATING = 7;
    public static final int CALL_EVENT_TERMINATED = 8;
    public static final int CALL_EVENT_DESTROY = 9;

    /** Creates new call identifier */
    default public String createCallId() {
        return null;
    }

    /** Returns the count of active(OFFHOOK) call. */
    public int getActiveCalls();
    public void updateCallState(Object call, int event, Object extraInfo);
}
