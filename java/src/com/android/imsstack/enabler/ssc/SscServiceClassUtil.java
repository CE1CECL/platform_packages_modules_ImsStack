package com.android.imsstack.enabler.ssc;

public class SscServiceClassUtil {
    // See 27.007 +CCFC or +CLCK
    public static final int SERVICE_CLASS_NONE          = 0; // no user input
    public static final int SERVICE_CLASS_VOICE         = (1 << 0);
    public static final int SERVICE_CLASS_DATA          = (1 << 1);
    public static final int SERVICE_CLASS_FAX           = (1 << 2);
    public static final int SERVICE_CLASS_SMS           = (1 << 3);
    public static final int SERVICE_CLASS_DATA_SYNC     = (1 << 4); // used for video
    public static final int SERVICE_CLASS_DATA_ASYNC    = (1 << 5);
    public static final int SERVICE_CLASS_PACKET        = (1 << 6);
    public static final int SERVICE_CLASS_PAD           = (1 << 7);
    public static final int SERVICE_CLASS_MAX           = (1 << 7); // Max SERVICE_CLASS value

    // IMS internal SC
    public static final int SERVICE_CLASS_CALL = SERVICE_CLASS_VOICE + SERVICE_CLASS_DATA_SYNC;

    public static boolean isValid(int serviceClass) {
        if (serviceClass == SERVICE_CLASS_NONE || isAudio(serviceClass) || isVideo(serviceClass)) {
            return true;
        }
        return false;
    }

    public static boolean isAudio(int serviceClass) {
        return isClassType(serviceClass, SERVICE_CLASS_VOICE);
    }

    public static boolean isVideo(int serviceClass) {
        return isClassType(serviceClass, SERVICE_CLASS_DATA_SYNC);
    }

    private static boolean isClassType(int serviceClass, int serviceClassType) {
        if ((serviceClass & serviceClassType) != 0) {
            return true;
        }
        return false;
    }

    public static int removeNotValidSC(int serviceClass) {
        return serviceClass & SERVICE_CLASS_CALL;
    }
}
