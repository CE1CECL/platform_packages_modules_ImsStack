package com.android.imsstack.external.ims;

/**
 * Provides the conference information for the operator specific requirements.
 * @hide
 */
public class ImsConferenceStateEx {
    /**
     * conference-info : Disconnected cause (integer)
     */
    public static final String DISCONNECTED_CAUSE = "disconnected_cause";

    /**
     * Disconnected cause values.
     */
    public static final int DISCONNECTED_CAUSE_REJECT = 21;
    public static final int DISCONNECTED_CAUSE_BUSY = 22;
    public static final int DISCONNECTED_CAUSE_SERVERERROR = 23;
    public static final int DISCONNECTED_CAUSE_NOTSUPPORTED = 24;
    public static final int DISCONNECTED_CAUSE_NOTACCEPTABLE = 25;
    public static final int DISCONNECTED_CAUSE_NOANSWER = 26;
    public static final int DISCONNECTED_CAUSE_NOTREACHABLE = 27;
    public static final int DISCONNECTED_CAUSE_LOWBATTERY = 28;
    public static final int DISCONNECTED_CAUSE_FORBIDDEN = 29;
    public static final int DISCONNECTED_CAUSE_INTSERVERERROR = 30;
}
