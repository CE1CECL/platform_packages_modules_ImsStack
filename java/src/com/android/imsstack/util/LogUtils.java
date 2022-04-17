package com.android.imsstack.util;

import android.net.Uri;

public final class LogUtils {
    public static final int TRACE_OPTION_D = 0x00000001;
    public static final int TRACE_OPTION_E = 0x00000002;
    public static final int TRACE_OPTION_I = 0x00000004;

    /** gims_subscriber */
    private static final Uri IMS_SUBSCRIBER
            = Uri.parse("content://com.android.imsstack.provider.gims/gims_subscriber");
    private static final String ADMIN_FEATURES = "admin_features";
    private static final int ADMIN_DEBUG = 0x40000000;
    /** gims_engine */
    private static final Uri IMS_ENGINE
            = Uri.parse("content://com.android.imsstack.provider.gims/gims_engine");
    private static final String TRACE_OPTION = "trace_option";
    private static final int DEFAULT_TRACE_OPTION = 0x0001000F;

    // Log configurations {
    public static int getLogOption(int slotId) {
        return DBUtils.CP.getInt(slotId, AppContext.get().getContentResolver(),
                IMS_ENGINE, TRACE_OPTION, DEFAULT_TRACE_OPTION);
    }

    public static boolean isAdminDebugOn(int slotId) {
        int adminFeatures = DBUtils.CP.getInt(slotId, AppContext.get().getContentResolver(),
                IMS_SUBSCRIBER, ADMIN_FEATURES, 0);
        return (adminFeatures & ADMIN_DEBUG) != 0;
    }

    public static int updateLogOption(String option, int slotId) {
        if ((option == null) || (option.length() != 10)) {
            return -1;
        }

        Integer optionInt;

        try {
            optionInt = Integer.decode(option);
        } catch (NumberFormatException e) {
            optionInt = null;
        }

        if (optionInt == null) {
            return -1;
        }

        DBUtils.CP.putString(slotId, AppContext.get().getContentResolver(),
                IMS_ENGINE, TRACE_OPTION, option);

        long optionLong = optionInt.longValue();
        return (int)(optionLong & 0xFFFFFFFF);
    }
    // }
}
