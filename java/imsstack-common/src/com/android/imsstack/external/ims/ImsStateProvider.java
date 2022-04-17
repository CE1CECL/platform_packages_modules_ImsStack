package com.android.imsstack.external.ims;

import java.util.Locale;

import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.database.ContentObserver;
import android.database.Cursor;
import android.net.Uri;
import android.provider.BaseColumns;
import android.sysprop.TelephonyProperties;
import android.telephony.SubscriptionManager;
import android.text.TextUtils;
import android.util.Log;

/**
 * This class provides the utility methods to get the Ims state.
 * Ims state may be followings:
 *   - network_type
 *   - volte_provisioned
 *   - vt_provisioned
 *   - call_state
 *   - service_status
 *
 * @hide
 */
public final class ImsStateProvider {
    public static final String AUTHORITY = "com.android.imsstack.provider.ims_state";
    public static final String COLUMN_SUB_ID = "sub_id";
    public static final String COLUMN_SERVICE_STATUS = "service_status";
    /** RegState */
    public static final String COLUMN_STATE = "state";
    public static final String COLUMN_NETWORK_TYPE = "network_type";
    /** VoLteState */
    public static final String COLUMN_CALL_STATE = "call_state";
    public static final String COLUMN_VOLTE_PROVISIONED = "volte_provisioned";
    public static final String COLUMN_VT_PROVISIONED = "vt_provisioned";
    public static final String COLUMN_WFC_PROVISIONED = "wfc_provisioned";
    /** CallState */
    public static final String COLUMN_CONNECTED_CALL_ON_WIFI = "connected_call_on_wifi";

    public static final int STATE_INACTIVE = 0;
    public static final int STATE_ACTIVE = 1;

    /** RoamingState */
    public static final String COLUMN_VOLTE_ROAMING = "volte_roaming";
    public static final String COLUMN_VT_ROAMING = "vt_roaming";
    /** VoWiFiActivationState */
    public static final String COLUMN_SLOT_ID = "slot_id";
    public static final String COLUMN_OPERATOR = "operator";

    private static final String MSIM_CONFIG
            = TelephonyProperties.multi_sim_config().orElse("");
    private static final boolean MSIM = MSIM_CONFIG.equals("dsds") ? true :
            MSIM_CONFIG.equals("dsda") ? true :
            MSIM_CONFIG.equals("tsts") ? true : false;

    private static final boolean DBG = !android.os.Build.TYPE.equals("user");
    private static final int DEFAULT_PHONE_ID = 0;
    private static final int DEFAULT_SUB_ID = SubscriptionManager.DEFAULT_SUBSCRIPTION_ID;
    private static final int PRIMARY_KEY_BASE = 1;
    private static final int INVALID_VALUE = -1;

    public static final class RegState {
        public static final String TABLE_NAME = "reg_state";
        public static final Uri CONTENT_URI
                = Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        /**
         * @return Ims registration state
         *      {@link ImsStateProvider#STATE_ACTIVE}
         *      {@link ImsStateProvider#STATE_INACTIVE}
         */
        public static int getState(Context c) {
            return getState(c.getContentResolver(), getDefaultSubId(c, CONTENT_URI));
        }

        public static int getState(ContentResolver cr, int subId) {
            return getRegState(cr, selectForSubId(subId), COLUMN_STATE, 0);
        }

        /**
         * @return a network type
         *      {@link TelephonyManager#NETWORK_TYPE_IWLAN}
         *      {@link TelephonyManager#NETWORK_TYPE_LTE}
         */
        public static int getNetworkType(Context c) {
            return getNetworkType(c.getContentResolver(), getDefaultSubId(c, CONTENT_URI));
        }

        public static int getNetworkType(ContentResolver cr, int subId) {
            return getRegState(cr, selectForSubId(subId), COLUMN_NETWORK_TYPE, 0);
        }

        /** ContentObserver for RegState */
        public static boolean registerObserver(ContentResolver cr, ContentObserver observer) {
            return registerContentObserver(cr, observer, CONTENT_URI);
        }

        public static void unregisterObserver(ContentResolver cr, ContentObserver observer) {
            unregisterContentObserver(cr, observer);
        }
    }

    public static final class VoLteState {
        public static final String TABLE_NAME = "volte_state";
        public static final Uri CONTENT_URI
                = Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final int SERVICE_NONE = 0;
        public static final int SERVICE_VOIP = 1;
        public static final int SERVICE_VT = 2;
        public static final int SERVICE_UC = 3;

        /**
         * @return a call state
         *      {@link ImsStateProvider#STATE_ACTIVE}
         *      {@link ImsStateProvider#STATE_INACTIVE}
         */
        public static int getCallState(Context c) {
            return getCallState(c.getContentResolver(), getDefaultSubId(c, CONTENT_URI));
        }

        public static int getCallState(ContentResolver cr, int subId) {
            return getVoLteState(cr, selectForSubId(subId), COLUMN_CALL_STATE, 0);
        }

        /**
         * @return a service status
         *      {@link ImsStateProvider#VoLteState#SERVICE_NONE}
         *      {@link ImsStateProvider#VoLteState#SERVICE_VOIP}
         *      {@link ImsStateProvider#VoLteState#SERVICE_VT}
         *      {@link ImsStateProvider#VoLteState#SERVICE_UC}
         */
        public static int getServiceStatus(Context c) {
            return getServiceStatus(c.getContentResolver(), getDefaultSubId(c, CONTENT_URI));
        }

        public static int getServiceStatus(ContentResolver cr, int subId) {
            return getVoLteState(cr, selectForSubId(subId), COLUMN_SERVICE_STATUS, 0);
        }

        /**
         * @return a VoLTE provisioned state
         *      {@link ImsStateProvider#STATE_ACTIVE}
         *      {@link ImsStateProvider#STATE_INACTIVE}
         */
        public static int getVoLteProvisioned(Context c) {
            return getVoLteProvisioned(c.getContentResolver(), getDefaultSubId(c, CONTENT_URI));
        }

        public static int getVoLteProvisioned(ContentResolver cr, int subId) {
            int result = getVoLteState(cr, selectForSubId(subId),
                    COLUMN_VOLTE_PROVISIONED, INVALID_VALUE);

            if (result == INVALID_VALUE) {
                int phoneId = getPhoneId(subId, -1);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    log("FailOver :: phoneId=" + phoneId);
                    return getVoLteProvisionedForPhoneId(cr, phoneId);
                }

                return STATE_INACTIVE;
            }

            return result;
        }

        public static int getVoLteProvisionedForPhoneId(ContentResolver cr, int phoneId) {
            return getVoLteState(cr, selectForPhoneId(phoneId),
                    COLUMN_VOLTE_PROVISIONED, STATE_INACTIVE);
        }

        /**
         * @return a VT provisioned state
         *      {@link ImsStateProvider#STATE_ACTIVE}
         *      {@link ImsStateProvider#STATE_INACTIVE}
         */
        public static int getVtProvisioned(Context c) {
            return getVtProvisioned(c.getContentResolver(), getDefaultSubId(c, CONTENT_URI));
        }

        public static int getVtProvisioned(ContentResolver cr, int subId) {
            int result = getVoLteState(cr, selectForSubId(subId),
                    COLUMN_VT_PROVISIONED, INVALID_VALUE);

            if (result == INVALID_VALUE) {
                int phoneId = getPhoneId(subId, -1);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    log("FailOver :: phoneId=" + phoneId);
                    return getVtProvisionedForPhoneId(cr, phoneId);
                }

                return STATE_INACTIVE;
            }

            return result;
        }

        public static int getVtProvisionedForPhoneId(ContentResolver cr, int phoneId) {
            return getVoLteState(cr, selectForPhoneId(phoneId),
                    COLUMN_VT_PROVISIONED, STATE_INACTIVE);
        }

        /**
         * @return a WFC provisioned state
         *      {@link ImsStateProvider#STATE_ACTIVE}
         *      {@link ImsStateProvider#STATE_INACTIVE}
         */
        public static int getWfcProvisioned(Context c) {
            return getWfcProvisioned(c.getContentResolver(), getDefaultSubId(c, CONTENT_URI));
        }

        public static int getWfcProvisioned(ContentResolver cr, int subId) {
            int result = getVoLteState(cr, selectForSubId(subId),
                    COLUMN_WFC_PROVISIONED, INVALID_VALUE);

            if (result == INVALID_VALUE) {
                int phoneId = getPhoneId(subId, -1);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    log("FailOver :: phoneId=" + phoneId);
                    return getWfcProvisionedForPhoneId(cr, phoneId);
                }

                return STATE_INACTIVE;
            }

            return result;
        }

        public static int getWfcProvisionedForPhoneId(ContentResolver cr, int phoneId) {
            return getVoLteState(cr, selectForPhoneId(phoneId),
                    COLUMN_WFC_PROVISIONED, STATE_INACTIVE);
        }

        /** ContentObserver for VoLteState */
        public static boolean registerObserver(ContentResolver cr, ContentObserver observer) {
            return registerContentObserver(cr, observer, CONTENT_URI);
        }

        public static void unregisterObserver(ContentResolver cr, ContentObserver observer) {
            unregisterContentObserver(cr, observer);
        }
    }

    public static final class CallState {
        public static final String TABLE_NAME = "call_state";
        public static final Uri CONTENT_URI
                = Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        /**
         * @return a call state
         *      {@link ImsStateProvider#STATE_ACTIVE}
         *      {@link ImsStateProvider#STATE_INACTIVE}
         */
        public static int getState(Context c) {
            return getState(c.getContentResolver(), getDefaultSubId(c, CONTENT_URI));
        }

        public static int getState(ContentResolver cr, int subId) {
            return getCallState(cr, selectForSubId(subId), COLUMN_STATE, 0);
        }

        /**
         * @return a state for connected call on WiFi
         *      {@link ImsStateProvider#STATE_ACTIVE}
         *      {@link ImsStateProvider#STATE_INACTIVE}
         */
        public static int getConnectedCallOnWifi(Context c) {
            return getConnectedCallOnWifi(c.getContentResolver(), getDefaultSubId(c, CONTENT_URI));
        }

        public static int getConnectedCallOnWifi(ContentResolver cr, int subId) {
            return getCallState(cr, selectForSubId(subId), COLUMN_CONNECTED_CALL_ON_WIFI, 0);
        }

        /** ContentObserver for CallState */
        public static boolean registerObserver(ContentResolver cr, ContentObserver observer) {
            return registerContentObserver(cr, observer, CONTENT_URI);
        }

        public static void unregisterObserver(ContentResolver cr, ContentObserver observer) {
            unregisterContentObserver(cr, observer);
        }
    }

    public static final class RoamingState {
        public static final String TABLE_NAME = "roaming_state";
        public static final Uri CONTENT_URI
                = Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        /**
         * @return a state for VoLTE Roaming
         *      {@link ImsStateProvider#STATE_ACTIVE}
         *      {@link ImsStateProvider#STATE_INACTIVE}
         */
        public static int getVoLteRoaming(Context c) {
            return getVoLteRoaming(c.getContentResolver(), getDefaultSubId(c, CONTENT_URI));
        }

        public static int getVoLteRoaming(ContentResolver cr, int subId) {
            int result = getRoamingState(cr, selectForSubId(subId),
                    COLUMN_VOLTE_ROAMING, INVALID_VALUE);

            if (result == INVALID_VALUE) {
                int phoneId = getPhoneId(subId, -1);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    log("FailOver :: phoneId=" + phoneId);
                    return getVoLteRoamingForPhoneId(cr, phoneId);
                }

                return STATE_INACTIVE;
            }

            return result;
        }

        public static int getVoLteRoamingForPhoneId(ContentResolver cr, int phoneId) {
            return getRoamingState(cr, selectForPhoneId(phoneId),
                    COLUMN_VOLTE_ROAMING, STATE_INACTIVE);
        }

        /**
         * @return a state for VT Roaming
         *      {@link ImsStateProvider#STATE_ACTIVE}
         *      {@link ImsStateProvider#STATE_INACTIVE}
         */
        public static int getVtRoaming(Context c) {
            return getVtRoaming(c.getContentResolver(), getDefaultSubId(c, CONTENT_URI));
        }

        public static int getVtRoaming(ContentResolver cr, int subId) {
            int result = getRoamingState(cr, selectForSubId(subId),
                    COLUMN_VT_ROAMING, INVALID_VALUE);

            if (result == INVALID_VALUE) {
                int phoneId = getPhoneId(subId, -1);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    log("FailOver :: phoneId=" + phoneId);
                    return getVtRoamingForPhoneId(cr, phoneId);
                }

                return STATE_INACTIVE;
            }

            return result;
        }

        public static int getVtRoamingForPhoneId(ContentResolver cr, int phoneId) {
            return getRoamingState(cr, selectForPhoneId(phoneId),
                    COLUMN_VT_ROAMING, STATE_INACTIVE);
        }

        /** ContentObserver for RoamingState */
        public static boolean registerObserver(ContentResolver cr, ContentObserver observer) {
            return registerContentObserver(cr, observer, CONTENT_URI);
        }

        public static void unregisterObserver(ContentResolver cr, ContentObserver observer) {
            unregisterContentObserver(cr, observer);
        }
    }

    public static final class VoWiFiActivationState {
        public static final String TABLE_NAME = "vowifi_activation_state";
        public static final Uri CONTENT_URI
                = Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        /**
         * @param operator operator string (uppercase characters)
         * @param country country string (uppercase characters)
         * @return a state for VoWiFi activation result
         *      {@link ImsStateProvider#STATE_ACTIVE}
         *      {@link ImsStateProvider#STATE_INACTIVE}
         */
        public static int getState(ContentResolver cr,
                int slotId, String operator, String country) {
            String where = getWhereClause(slotId, operator, country);
            return getInt(cr, CONTENT_URI, where, COLUMN_STATE, STATE_INACTIVE);
        }

        /** ContentObserver for VoWiFiActivationState */
        public static boolean registerObserver(ContentResolver cr, ContentObserver observer) {
            return registerContentObserver(cr, observer, CONTENT_URI);
        }

        public static void unregisterObserver(ContentResolver cr, ContentObserver observer) {
            unregisterContentObserver(cr, observer);
        }

        private static String getWhereClause(int slotId, String operator, String country) {
            return String.format(Locale.US, "%s='%d' AND %s='%s'",
                    COLUMN_SLOT_ID, slotId,
                    COLUMN_OPERATOR, getOperator(operator, country));
        }

        private static String getOperator(String operator, String country) {
            if (TextUtils.isEmpty(country)) {
                return operator;
            }

            String op = operator;

            if ("NJU".equals(op) && "PL".equals(country)) {
                op = "ORG";
            }

            return String.format(Locale.US, "%s-%s", op, country);
        }
    }

    public static int getRegState(ContentResolver cr,
            String selection, String key, int defaultValue) {
        return getInt(cr, RegState.CONTENT_URI,
                selection, key, defaultValue);
    }

    public static int getVoLteState(ContentResolver cr,
            String selection, String key, int defaultValue) {
        return getInt(cr, VoLteState.CONTENT_URI,
                selection, key, defaultValue);
    }

    public static int getCallState(ContentResolver cr,
            String selection, String key, int defaultValue) {
        return getInt(cr, CallState.CONTENT_URI,
                selection, key, defaultValue);
    }

    public static int getRoamingState(ContentResolver cr,
            String selection, String key, int defaultValue) {
        return getInt(cr, RoamingState.CONTENT_URI,
                selection, key, defaultValue);
    }

    private static int getDefaultSubId(Context c, Uri contentUri) {
        if (isMultiSimEnabled()) {
            return SubscriptionManager.getDefaultDataSubscriptionId();
        }

        int subId = getSubId(c, DEFAULT_PHONE_ID);

        if (!isValidSubId(subId)) {
            log("Invalid default subscription; SUB" + subId);
            subId = getSubId(c.getContentResolver(), contentUri, DEFAULT_PHONE_ID);
        }

        return subId;
    }

    private static int getPhoneCount() {
        if (!isMultiSimEnabled()) {
            return 1;
        }

        return MSIM_CONFIG.equals("tsts") ? 3 : 2;
    }

    private static int getPhoneId(int subId, int defaultPhoneId) {
        int phoneId = SubscriptionManager.getSlotIndex(subId);

        if (phoneId < DEFAULT_PHONE_ID || phoneId >= getPhoneCount()) {
            // Set the phoneId as a default
            phoneId = defaultPhoneId;

            if ((defaultPhoneId < DEFAULT_PHONE_ID) && !isMultiSimEnabled()) {
                phoneId = DEFAULT_PHONE_ID;
            }
        }

        return phoneId;
    }

    private static int getSubId(Context c, int phoneId) {
        SubscriptionManager sm = c.getSystemService(SubscriptionManager.class);
        int[] subIds = (sm != null) ? sm.getSubscriptionIds(phoneId) : null;
        if ((subIds != null) && (subIds.length > 0)) {
            return subIds[0];
        }

        return SubscriptionManager.INVALID_SUBSCRIPTION_ID;
    }

    private static int getInt(ContentResolver cr, Uri contentUri, String selection,
            String columnName, int defaultValue) {
        if ((cr == null) || (contentUri == null)) {
            loge("getInt :: cr=" + cr + ", uri=" + contentUri);
            return defaultValue;
        }

        Cursor c = null;

        try {
            c = cr.query(contentUri, null, selection, null, null);

            if ((c != null) && c.moveToFirst()) {
                int index = c.getColumnIndex(columnName);
                int value = (index < 0) ? defaultValue : c.getInt(index);

                if (DBG) {
                    log("getInt :: column=" + columnName
                        + ", value=" + value + " at " + index);
                }

                return value;
            }
        } catch (Throwable t) {
            loge(t.toString());
            t.printStackTrace();
        } finally {
            if (c != null) {
                c.close();
            }
        }

        return defaultValue;
    }

    private static boolean registerContentObserver(ContentResolver cr,
            ContentObserver observer, Uri uri) {
        try {
            cr.registerContentObserver(uri, true, observer);
        } catch (Throwable t) {
            if (DBG) {
                log(t.toString());
            }
            return false;
        }

        return true;
    }

    private static void unregisterContentObserver(ContentResolver cr,
            ContentObserver observer) {
        try {
            cr.unregisterContentObserver(observer);
        } catch (Throwable t) {
            if (DBG) {
                log(t.toString());
            }
        }
    }

    /**
     * The original phone id is zero-based,
     * so it should be formed by this method to access the ImsStateProvider.
     */
    private static int getPrimaryKey(int phoneId) {
        // We assume that phone id is zero-based identifier.
        int key = (phoneId < 0) ? DEFAULT_PHONE_ID : phoneId;
        return (key + PRIMARY_KEY_BASE);
    }

    private static int getSubId(ContentResolver cr, Uri contentUri, int phoneId) {
        return getInt(cr, contentUri,
                selectForPrimaryKey(getPrimaryKey(phoneId)), COLUMN_SUB_ID, -1);
    }

    private static boolean isMultiSimEnabled() {
        return MSIM;
    }

    private static boolean isValidSubId(int subId) {
        return SubscriptionManager.isUsableSubscriptionId(subId);
    }

    private static String selectForPrimaryKey(int key) {
        return String.format(Locale.US, "%s='%d'", BaseColumns._ID, key);
    }

    private static String selectForPhoneId(int phoneId) {
        return selectForPrimaryKey(getPrimaryKey(phoneId));
    }

    private static String selectForSubId(int subId) {
        return String.format(Locale.US, "%s='%d'", COLUMN_SUB_ID, subId);
    }

    private static void log(String s) {
        Log.d("IMS", "[ImsStateProvider] " + s);
    }

    private static void loge(String s) {
        Log.e("IMS", "[ImsStateProvider] " + s);
    }
}
