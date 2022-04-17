package com.android.imsstack.core.agents.dcmif;

/**
 * This class provides constant values for Data Connection.
 */
public final class DCConstants {
    public static final int CAPABILITY_NONE = -1;

    // Native: NetworkPolicy#APN_XXX
    public static final int TYPE_NONE = -1;
    public static final int TYPE_IMS = 1;
    public static final int TYPE_INTERNET = 2;
    public static final int TYPE_XCAP = 3;
    public static final int TYPE_EMERGENCY = 9;
    public static final int TYPE_WIFI = 21;

    // failCause
    public static final String DATA_FAILURE_CAUSE_KEY = "failCause"; // PhoneConstants.DATA_FAILURE_CAUSE_KEY;
}
