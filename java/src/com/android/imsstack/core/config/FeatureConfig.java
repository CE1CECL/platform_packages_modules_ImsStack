package com.android.imsstack.core.config;

import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

import com.android.imsstack.util.DBUtils;
import com.android.imsstack.util.ImsLog;

import java.util.HashMap;
import java.util.Locale;
import java.util.Map;

/**
 * This class provides the APIs to getting FEATURE_XXX in gims_feature table
 */
public final class FeatureConfig {
    /** Feature postfix */
    public static final String IPSEC = "IPSEC";
    public static final String TLS = "TLS";

    public static final String AUTH_SIP_DIGEST = "AUTH_SIP_DIGEST";
    public static final String SDP_PRECONDITION = "SDP_PRECONDITION";
    public static final String GRUU = "GRUU";
    public static final String MULTIPLE_REGISTRATION = "MULTIPLE_REGISTRATION";
    public static final String REQUEST_URI_VALIDATION_IN_MID_DIALOG
            = "REQUEST_URI_VALIDATION_IN_MID_DIALOG";
    public static final String NO_SESSION_REFRESH_BY_REINVITE
            = "NO_SESSION_REFRESH_BY_REINVITE";
    public static final String INVITE_TXN_HANDLING_CORRECTION
            = "INVITE_TXN_HANDLING_CORRECTION";
    public static final String GEOLOCATION = "GEOLOCATION";

    public static final String VOLTE = "VOLTE";
    public static final String VOWIFI = "VOWIFI";
    public static final String VT = "VT";
    public static final String SMS = "SMS";
    public static final String UCE = "UCE";

    public static final String VOLTE_IN_ROAMING = "VOLTE_IN_ROAMING";
    public static final String VT_IN_ROAMING = "VT_IN_ROAMING";

    /** Feature prefix */
    private static final String FEATURE_PREFIX = "FEATURE_";

    private static Map<Integer, Map<String, Integer>> sFeatures
            = new HashMap<Integer, Map<String, Integer>>();

    public static synchronized boolean isEnabled(int slotId, String feature) {
        Map<String, Integer> features = sFeatures.get(slotId);

        if (features != null) {
            Integer value = features.get(feature);
            return (value != null) && (value.intValue() == 1);
        }

        return false;
    }

    public static synchronized void init(int slotId) {
        SQLiteDatabase imsDB = DBUtils.DB.open(ProviderInterface.DBFP,
                SQLiteDatabase.OPEN_READONLY);

        if (imsDB == null) {
            ImsLog.e("IMS DB is null");
            return;
        }

        Cursor cursor = null;
        Map<String, Integer> features = null;

        try {
            cursor = DBUtils.DB.getFirstCursor(slotId, imsDB, ProviderInterface.FEATURE.TABLE_NAME);

            if (cursor != null) {
                features = new HashMap<String, Integer>();

                int columnCount = cursor.getColumnCount();

                // Skip column - id(0) / property(1)
                for (int columnIndex = 2; columnIndex < columnCount; columnIndex++) {
                    String feature = cursor.getColumnName(columnIndex);

                    if ((feature == null) || !feature.startsWith(FEATURE_PREFIX)) {
                        continue;
                    }

                    // "FEATURE_"
                    feature = feature.substring(FEATURE_PREFIX.length());

                    if ((feature != null) && !feature.isEmpty()) {
                        features.put(feature, cursor.getInt(columnIndex));
                    }
                }
            }
        } catch (Exception e) {
            ImsLog.e(e.toString());
            e.printStackTrace();
        } finally {
            DBUtils.closeCursor(cursor);
            DBUtils.DB.close(imsDB);
        }

        if (features != null) {
            sFeatures.put(slotId, features);
        } else {
            sFeatures.remove(slotId);
        }
    }
}
