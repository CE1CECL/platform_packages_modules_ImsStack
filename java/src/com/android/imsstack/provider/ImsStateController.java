package com.android.imsstack.provider;

import android.content.ContentResolver;
import android.content.ContentValues;
import android.database.ContentObserver;
import android.database.Cursor;
import android.net.Uri;
import android.text.TextUtils;

import com.android.imsstack.util.DBUtils;
import com.android.imsstack.util.ImsLog;
import com.android.imsstack.util.MSimUtils;

import java.util.Locale;

public final class ImsStateController {
    public static final int DEFAULT_PHONE_ID = MSimUtils.DEFAULT_PHONE_ID;
    public static final int DEFAULT_SUB_ID = MSimUtils.DEFAULT_SUB_ID;

    public static final int STATE_INACTIVE = 0;
    public static final int STATE_ACTIVE = 1;

    private static final int INVALID_VALUE = -1;

    /**
     * The original phone id is zero-based,
     * so it should be formed by this method to access the ImsStateProvider.
     */

    public static int getPrimaryKey(int phoneId) {
        // We assume that phone id is zero-based identifier.
        int key = (phoneId < 0) ? DEFAULT_PHONE_ID : phoneId;
        return (key + ImsState.Columns.PRIMARY_KEY_BASE);
    }

    public static int getRegState(ContentResolver cr,
            String selection, String key, int defaultValue) {
        return DBUtils.CP.getInt(cr, ImsState.RegState.CONTENT_URI,
                selection, key, defaultValue);
    }

    public static int putRegState(ContentResolver cr,
            String selection, String key, int value) {
        ContentValues values = new ContentValues();

        values.put(key, value);

        return DBUtils.CP.update(cr, ImsState.RegState.CONTENT_URI,
                values, selection, null);
    }

    public static int getVoLteState(ContentResolver cr,
            String selection, String key, int defaultValue) {
        return DBUtils.CP.getInt(cr, ImsState.VoLteState.CONTENT_URI,
                selection, key, defaultValue);
    }

    public static int putVoLteState(ContentResolver cr,
            String selection, String key, int value) {
        ContentValues values = new ContentValues();

        values.put(key, value);

        return DBUtils.CP.update(cr, ImsState.VoLteState.CONTENT_URI,
                values, selection, null);
    }

    public static int getCallState(ContentResolver cr,
            String selection, String key, int defaultValue) {
        return DBUtils.CP.getInt(cr, ImsState.CallState.CONTENT_URI,
                selection, key, defaultValue);
    }

    public static int putCallState(ContentResolver cr,
            String selection, String key, int value) {
        ContentValues values = new ContentValues();

        values.put(key, value);

        return DBUtils.CP.update(cr, ImsState.CallState.CONTENT_URI,
                values, selection, null);
    }

    public static int getRoamingState(ContentResolver cr,
            String selection, String key, int defaultValue) {
        return DBUtils.CP.getInt(cr, ImsState.RoamingState.CONTENT_URI,
                selection, key, defaultValue);
    }

    public static int putRoamingState(ContentResolver cr,
            String selection, String key, int value) {
        ContentValues values = new ContentValues();

        values.put(key, value);

        return DBUtils.CP.update(cr, ImsState.RoamingState.CONTENT_URI,
                values, selection, null);
    }

    public static void initCallState(ContentResolver cr, int phoneId) {
        VoLteState.putCallStateForPhoneId(cr, STATE_INACTIVE, phoneId);
        CallState.putStateForPhoneId(cr, STATE_INACTIVE, phoneId);
        CallState.putConnectedCallOnWifiForPhoneId(cr, STATE_INACTIVE, phoneId);
    }

    public static void initRegState(ContentResolver cr, int phoneId) {
        RegState.putStateForPhoneId(cr, STATE_INACTIVE, phoneId);
        RegState.putServiceStatusForPhoneId(cr, STATE_INACTIVE, phoneId);
        VoLteState.putServiceStatusForPhoneId(cr, STATE_INACTIVE, phoneId);
    }

    public static String selectForPhoneId(int phoneId) {
        return String.format(Locale.US, "%s='%d'", ImsState.Columns._ID, getPrimaryKey(phoneId));
    }

    public static String selectForSubId(int subId) {
        return String.format(Locale.US, "%s='%d'", ImsState.Columns.SUB_ID, subId);
    }

    public static final class RegState {
        public static void init(ContentResolver cr, int phoneId, int subId) {
            initTable(cr, phoneId, subId,
                    ImsState.RegState.CONTENT_URI,
                    ImsState.RegState.TABLE_NAME);
        }

        public static int getRowCount(ContentResolver cr) {
            return getRowCountForTable(cr, ImsState.RegState.CONTENT_URI);
        }

        public static int getSubId(ContentResolver cr) {
            return getSubId(cr, DEFAULT_PHONE_ID);
        }

        public static int getSubId(ContentResolver cr, int phoneId) {
            return DBUtils.CP.getInt(cr, ImsState.RegState.CONTENT_URI,
                    selectForPhoneId(phoneId),
                    ImsState.Columns.SUB_ID, DEFAULT_SUB_ID);
        }

        public static int getState(ContentResolver cr) {
            return getState(cr, getDefaultSubId(cr, ImsState.RegState.CONTENT_URI));
        }

        public static int getState(ContentResolver cr, int subId) {
            int result = getRegState(cr, selectForSubId(subId),
                    ImsState.Columns.STATE, INVALID_VALUE);

            if (result == INVALID_VALUE) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return getStateForPhoneId(cr, phoneId);
                }

                return STATE_INACTIVE;
            }

            return result;
        }

        public static int getStateForPhoneId(ContentResolver cr, int phoneId) {
            return getRegState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.STATE, STATE_INACTIVE);
        }

        public static int getServiceStatus(ContentResolver cr) {
            return getServiceStatus(cr, getDefaultSubId(cr, ImsState.RegState.CONTENT_URI));
        }

        public static int getServiceStatus(ContentResolver cr, int subId) {
            int result = getRegState(cr, selectForSubId(subId),
                    ImsState.Columns.SERVICE_STATUS, INVALID_VALUE);

            if (result == INVALID_VALUE) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return getServiceStatusForPhoneId(cr, phoneId);
                }

                return STATE_INACTIVE;
            }

            return result;
        }

        public static int getServiceStatusForPhoneId(ContentResolver cr, int phoneId) {
            return getRegState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.SERVICE_STATUS, STATE_INACTIVE);
        }

        public static int getNetworkType(ContentResolver cr) {
            return getNetworkType(cr, getDefaultSubId(cr, ImsState.RegState.CONTENT_URI));
        }

        public static int getNetworkType(ContentResolver cr, int subId) {
            int result = getRegState(cr, selectForSubId(subId),
                    ImsState.Columns.NETWORK_TYPE, INVALID_VALUE);

            if (result == INVALID_VALUE) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return getNetworkTypeForPhoneId(cr, phoneId);
                }

                return STATE_INACTIVE;
            }

            return result;
        }

        public static int getNetworkTypeForPhoneId(ContentResolver cr, int phoneId) {
            return getRegState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.NETWORK_TYPE, STATE_INACTIVE);
        }

        public static int putSubId(ContentResolver cr, int subId) {
            return putSubId(cr, DEFAULT_PHONE_ID, subId);
        }

        public static int putSubId(ContentResolver cr, int phoneId, int subId) {
            return putSubIdForPhoneId(cr, ImsState.RegState.CONTENT_URI, phoneId, subId);
        }

        public static int putState(ContentResolver cr, int state) {
            return putState(cr, state, getDefaultSubId(cr, ImsState.RegState.CONTENT_URI));
        }

        public static int putState(ContentResolver cr, int state, int subId) {
            int result = putRegState(cr, selectForSubId(subId), ImsState.Columns.STATE, state);
            if (result <= 0) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return putStateForPhoneId(cr, state, phoneId);
                }
            }

            return result;
        }

        public static int putStateForPhoneId(ContentResolver cr, int state, int phoneId) {
            return putRegState(cr, selectForPhoneId(phoneId), ImsState.Columns.STATE, state);
        }

        public static int putServiceStatus(ContentResolver cr, int status) {
            return putServiceStatus(cr, status,
                    getDefaultSubId(cr, ImsState.RegState.CONTENT_URI));
        }

        public static int putServiceStatus(ContentResolver cr, int status, int subId) {
            int result = putRegState(cr, selectForSubId(subId),
                    ImsState.Columns.SERVICE_STATUS, status);

            if (result <= 0) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return putServiceStatusForPhoneId(cr, status, phoneId);
                }
            }

            return result;
        }

        public static int putServiceStatusForPhoneId(ContentResolver cr,
                int status, int phoneId) {
            return putRegState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.SERVICE_STATUS, status);
        }

        public static int putNetworkType(ContentResolver cr, int networkType) {
            return putNetworkType(cr, networkType,
                    getDefaultSubId(cr, ImsState.RegState.CONTENT_URI));
        }

        public static int putNetworkType(ContentResolver cr, int networkType, int subId) {
            int result = putRegState(cr, selectForSubId(subId),
                    ImsState.Columns.NETWORK_TYPE, networkType);

            if (result <= 0) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return putNetworkTypeForPhoneId(cr, networkType, phoneId);
                }
            }

            return result;
        }

        public static int putNetworkTypeForPhoneId(ContentResolver cr,
                int networkType, int phoneId) {
            return putRegState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.NETWORK_TYPE, networkType);
        }

        /** ContentObserver for RegState */
        public static void registerObserver(ContentResolver cr, ContentObserver observer) {
            cr.registerContentObserver(ImsState.RegState.CONTENT_URI, true, observer);
        }

        public static void unregisterObserver(ContentResolver cr, ContentObserver observer) {
            cr.unregisterContentObserver(observer);
        }
    }

    public static final class VoLteState {
        public static void init(ContentResolver cr, int phoneId, int subId) {
            initTable(cr, phoneId, subId,
                    ImsState.VoLteState.CONTENT_URI,
                    ImsState.VoLteState.TABLE_NAME);
        }

        public static int getRowCount(ContentResolver cr) {
            return getRowCountForTable(cr, ImsState.VoLteState.CONTENT_URI);
        }

        public static int getSubId(ContentResolver cr) {
            return getSubId(cr, DEFAULT_PHONE_ID);
        }

        public static int getSubId(ContentResolver cr, int phoneId) {
            return DBUtils.CP.getInt(cr, ImsState.VoLteState.CONTENT_URI,
                    selectForPhoneId(phoneId),
                    ImsState.Columns.SUB_ID, DEFAULT_SUB_ID);
        }

        public static int getCallState(ContentResolver cr) {
            return getCallState(cr, getDefaultSubId(cr, ImsState.VoLteState.CONTENT_URI));
        }

        public static int getCallState(ContentResolver cr, int subId) {
            int result = getVoLteState(cr, selectForSubId(subId),
                    ImsState.Columns.CALL_STATE, INVALID_VALUE);

            if (result == INVALID_VALUE) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return getCallStateForPhoneId(cr, phoneId);
                }

                return STATE_INACTIVE;
            }

            return result;
        }

        public static int getCallStateForPhoneId(ContentResolver cr, int phoneId) {
            return getVoLteState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.CALL_STATE, STATE_INACTIVE);
        }

        public static int getDeviceVoLteAvailable(ContentResolver cr) {
            return getDeviceVoLteAvailable(cr,
                    getDefaultSubId(cr, ImsState.VoLteState.CONTENT_URI));
        }

        public static int getDeviceVoLteAvailable(ContentResolver cr, int subId) {
            int result = getVoLteState(cr, selectForSubId(subId),
                    ImsState.Columns.DEVICE_VOLTE_AVAILABLE, INVALID_VALUE);

            if (result == INVALID_VALUE) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return getDeviceVoLteAvailableForPhoneId(cr, phoneId);
                }

                return STATE_INACTIVE;
            }

            return result;
        }

        public static int getDeviceVoLteAvailableForPhoneId(ContentResolver cr, int phoneId) {
            return getVoLteState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.DEVICE_VOLTE_AVAILABLE, STATE_INACTIVE);
        }

        public static int getDeviceVtAvailable(ContentResolver cr) {
            return getDeviceVtAvailable(cr, getDefaultSubId(cr, ImsState.VoLteState.CONTENT_URI));
        }

        public static int getDeviceVtAvailable(ContentResolver cr, int subId) {
            int result = getVoLteState(cr, selectForSubId(subId),
                    ImsState.Columns.DEVICE_VT_AVAILABLE, INVALID_VALUE);

            if (result == INVALID_VALUE) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return getDeviceVtAvailableForPhoneId(cr, phoneId);
                }

                return STATE_INACTIVE;
            }

            return result;
        }

        public static int getDeviceVtAvailableForPhoneId(ContentResolver cr, int phoneId) {
            return getVoLteState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.DEVICE_VT_AVAILABLE, STATE_INACTIVE);
        }

        public static int getDeviceWfcAvailable(ContentResolver cr) {
            return getDeviceWfcAvailable(cr, getDefaultSubId(cr, ImsState.VoLteState.CONTENT_URI));
        }

        public static int getDeviceWfcAvailable(ContentResolver cr, int subId) {
            int result = getVoLteState(cr, selectForSubId(subId),
                    ImsState.Columns.DEVICE_WFC_AVAILABLE, INVALID_VALUE);

            if (result == INVALID_VALUE) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return getDeviceWfcAvailableForPhoneId(cr, phoneId);
                }

                return STATE_INACTIVE;
            }

            return result;
        }

        public static int getDeviceWfcAvailableForPhoneId(ContentResolver cr, int phoneId) {
            return getVoLteState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.DEVICE_WFC_AVAILABLE, STATE_INACTIVE);
        }

        public static int getServiceStatus(ContentResolver cr) {
            return getServiceStatus(cr, getDefaultSubId(cr, ImsState.VoLteState.CONTENT_URI));
        }

        public static int getServiceStatus(ContentResolver cr, int subId) {
            int result = getVoLteState(cr, selectForSubId(subId),
                    ImsState.Columns.SERVICE_STATUS, INVALID_VALUE);

            if (result == INVALID_VALUE) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return getServiceStatusForPhoneId(cr, phoneId);
                }

                return STATE_INACTIVE;
            }

            return result;
        }

        public static int getServiceStatusForPhoneId(ContentResolver cr, int phoneId) {
            return getVoLteState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.SERVICE_STATUS, STATE_INACTIVE);
        }

        public static int getVoLteProvisioned(ContentResolver cr) {
            return getVoLteProvisioned(cr, getDefaultSubId(cr, ImsState.VoLteState.CONTENT_URI));
        }

        public static int getVoLteProvisioned(ContentResolver cr, int subId) {
            int result = getVoLteState(cr, selectForSubId(subId),
                    ImsState.Columns.VOLTE_PROVISIONED, INVALID_VALUE);

            if (result == INVALID_VALUE) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return getVoLteProvisionedForPhoneId(cr, phoneId);
                }

                return STATE_INACTIVE;
            }

            return result;
        }

        public static int getVoLteProvisionedForPhoneId(ContentResolver cr, int phoneId) {
            return getVoLteState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.VOLTE_PROVISIONED, STATE_INACTIVE);
        }

        public static int getVtProvisioned(ContentResolver cr) {
            return getVtProvisioned(cr, getDefaultSubId(cr, ImsState.VoLteState.CONTENT_URI));
        }

        public static int getVtProvisioned(ContentResolver cr, int subId) {
            int result = getVoLteState(cr, selectForSubId(subId),
                    ImsState.Columns.VT_PROVISIONED, INVALID_VALUE);

            if (result == INVALID_VALUE) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return getVtProvisionedForPhoneId(cr, phoneId);
                }

                return STATE_INACTIVE;
            }

            return result;
        }

        public static int getVtProvisionedForPhoneId(ContentResolver cr, int phoneId) {
            return getVoLteState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.VT_PROVISIONED, STATE_INACTIVE);
        }

        public static int getWfcProvisioned(ContentResolver cr) {
            return getWfcProvisioned(cr, getDefaultSubId(cr, ImsState.VoLteState.CONTENT_URI));
        }

        public static int getWfcProvisioned(ContentResolver cr, int subId) {
            int result = getVoLteState(cr, selectForSubId(subId),
                    ImsState.Columns.WFC_PROVISIONED, INVALID_VALUE);

            if (result == INVALID_VALUE) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return getWfcProvisionedForPhoneId(cr, phoneId);
                }

                return STATE_INACTIVE;
            }

            return result;
        }

        public static int getWfcProvisionedForPhoneId(ContentResolver cr, int phoneId) {
            return getVoLteState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.WFC_PROVISIONED, STATE_INACTIVE);
        }

        public static int putSubId(ContentResolver cr, int subId) {
            return putSubId(cr, DEFAULT_PHONE_ID, subId);
        }

        public static int putSubId(ContentResolver cr, int phoneId, int subId) {
            return putSubIdForPhoneId(cr, ImsState.VoLteState.CONTENT_URI, phoneId, subId);
        }

        public static int putCallState(ContentResolver cr, int state) {
            return putCallState(cr, state, getDefaultSubId(cr, ImsState.VoLteState.CONTENT_URI));
        }

        public static int putCallState(ContentResolver cr, int state, int subId) {
            int result = putVoLteState(cr, selectForSubId(subId),
                    ImsState.Columns.CALL_STATE, state);

            if (result <= 0) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return putCallStateForPhoneId(cr, state, phoneId);
                }
            }

            return result;
        }

        public static int putCallStateForPhoneId(ContentResolver cr, int state, int phoneId) {
            return putVoLteState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.CALL_STATE, state);
        }

        public static int putDeviceVoLteAvailable(ContentResolver cr, int available) {
            return putDeviceVoLteAvailable(cr, available,
                    getDefaultSubId(cr, ImsState.VoLteState.CONTENT_URI));
        }

        public static int putDeviceVoLteAvailable(ContentResolver cr, int available, int subId) {
            int result = putVoLteState(cr, selectForSubId(subId),
                    ImsState.Columns.DEVICE_VOLTE_AVAILABLE, available);

            if (result <= 0) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return putDeviceVoLteAvailableForPhoneId(cr, available, phoneId);
                }
            }

            return result;
        }

        public static int putDeviceVoLteAvailableForPhoneId(ContentResolver cr,
                int available, int phoneId) {
            return putVoLteState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.DEVICE_VOLTE_AVAILABLE, available);
        }

        public static int putDeviceVtAvailable(ContentResolver cr, int available) {
            return putDeviceVtAvailable(cr, available,
                    getDefaultSubId(cr, ImsState.VoLteState.CONTENT_URI));
        }

        public static int putDeviceVtAvailable(ContentResolver cr, int available, int subId) {
            int result = putVoLteState(cr, selectForSubId(subId),
                    ImsState.Columns.DEVICE_VT_AVAILABLE, available);

            if (result <= 0) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return putDeviceVtAvailableForPhoneId(cr, available, phoneId);
                }
            }

            return result;
        }

        public static int putDeviceVtAvailableForPhoneId(ContentResolver cr,
                int available, int phoneId) {
            return putVoLteState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.DEVICE_VT_AVAILABLE, available);
        }

        public static int putDeviceWfcAvailable(ContentResolver cr, int available) {
            return putDeviceWfcAvailable(cr, available,
                    getDefaultSubId(cr, ImsState.VoLteState.CONTENT_URI));
        }

        public static int putDeviceWfcAvailable(ContentResolver cr, int available, int subId) {
            int result = putVoLteState(cr, selectForSubId(subId),
                    ImsState.Columns.DEVICE_WFC_AVAILABLE, available);

            if (result <= 0) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return putDeviceWfcAvailableForPhoneId(cr, available, phoneId);
                }
            }

            return result;
        }

        public static int putDeviceWfcAvailableForPhoneId(ContentResolver cr,
                int available, int phoneId) {
            return putVoLteState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.DEVICE_WFC_AVAILABLE, available);
        }

        public static int putServiceStatus(ContentResolver cr, int status) {
            return putServiceStatus(cr, status,
                    getDefaultSubId(cr, ImsState.VoLteState.CONTENT_URI));
        }

        public static int putServiceStatus(ContentResolver cr, int status, int subId) {
            int result = putVoLteState(cr, selectForSubId(subId),
                    ImsState.Columns.SERVICE_STATUS, status);

            if (result <= 0) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return putServiceStatusForPhoneId(cr, status, phoneId);
                }
            }

            return result;
        }

        public static int putServiceStatusForPhoneId(ContentResolver cr,
                int status, int phoneId) {
            return putVoLteState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.SERVICE_STATUS, status);
        }

        public static int putVoLteProvisioned(ContentResolver cr, int provisioned) {
            return putVoLteProvisioned(cr, provisioned,
                    getDefaultSubId(cr, ImsState.VoLteState.CONTENT_URI));
        }

        public static int putVoLteProvisioned(ContentResolver cr, int provisioned, int subId) {
            int result = putVoLteState(cr, selectForSubId(subId),
                    ImsState.Columns.VOLTE_PROVISIONED, provisioned);

            if (result <= 0) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return putVoLteProvisionedForPhoneId(cr, provisioned, phoneId);
                }
            }

            return result;
        }

        public static int putVoLteProvisionedForPhoneId(ContentResolver cr,
                int provisioned, int phoneId) {
            return putVoLteState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.VOLTE_PROVISIONED, provisioned);
        }

        public static int putVtProvisioned(ContentResolver cr, int provisioned) {
            return putVtProvisioned(cr, provisioned,
                    getDefaultSubId(cr, ImsState.VoLteState.CONTENT_URI));
        }

        public static int putVtProvisioned(ContentResolver cr, int provisioned, int subId) {
            int result = putVoLteState(cr, selectForSubId(subId),
                    ImsState.Columns.VT_PROVISIONED, provisioned);

            if (result <= 0) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return putVtProvisionedForPhoneId(cr, provisioned, phoneId);
                }
            }

            return result;
        }

        public static int putVtProvisionedForPhoneId(ContentResolver cr,
                int provisioned, int phoneId) {
            return putVoLteState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.VT_PROVISIONED, provisioned);
        }

        public static int putWfcProvisioned(ContentResolver cr, int provisioned) {
            return putWfcProvisioned(cr, provisioned,
                    getDefaultSubId(cr, ImsState.VoLteState.CONTENT_URI));
        }

        public static int putWfcProvisioned(ContentResolver cr, int provisioned, int subId) {
            int result = putVoLteState(cr, selectForSubId(subId),
                    ImsState.Columns.WFC_PROVISIONED, provisioned);

            if (result <= 0) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return putWfcProvisionedForPhoneId(cr, provisioned, phoneId);
                }
            }

            return result;
        }

        public static int putWfcProvisionedForPhoneId(ContentResolver cr,
                int provisioned, int phoneId) {
            return putVoLteState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.WFC_PROVISIONED, provisioned);
        }

        /** ContentObserver **/
        public static void registerObserver(ContentResolver cr, ContentObserver observer) {
            cr.registerContentObserver(ImsState.VoLteState.CONTENT_URI, true, observer);
        }

        public static void unregisterObserver(ContentResolver cr, ContentObserver observer) {
            cr.unregisterContentObserver(observer);
        }
    }

    public static final class CallState {
        public static void init(ContentResolver cr, int phoneId, int subId) {
            initTable(cr, phoneId, subId,
                    ImsState.CallState.CONTENT_URI,
                    ImsState.CallState.TABLE_NAME);
        }

        public static int getRowCount(ContentResolver cr) {
            return getRowCountForTable(cr, ImsState.CallState.CONTENT_URI);
        }

        public static int getSubId(ContentResolver cr) {
            return getSubId(cr, DEFAULT_PHONE_ID);
        }

        public static int getSubId(ContentResolver cr, int phoneId) {
            return DBUtils.CP.getInt(cr, ImsState.CallState.CONTENT_URI,
                    selectForPhoneId(phoneId),
                    ImsState.Columns.SUB_ID, DEFAULT_SUB_ID);
        }

        public static int getState(ContentResolver cr) {
            return getState(cr, getDefaultSubId(cr, ImsState.CallState.CONTENT_URI));
        }

        public static int getState(ContentResolver cr, int subId) {
            int result = getCallState(cr, selectForSubId(subId),
                    ImsState.Columns.STATE, INVALID_VALUE);

            if (result == INVALID_VALUE) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return getStateForPhoneId(cr, phoneId);
                }

                return STATE_INACTIVE;
            }

            return result;
        }

        public static int getStateForPhoneId(ContentResolver cr, int phoneId) {
            return getCallState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.STATE, STATE_INACTIVE);
        }

        public static int getConnectedCallOnWifi(ContentResolver cr) {
            return getConnectedCallOnWifi(cr, getDefaultSubId(cr, ImsState.CallState.CONTENT_URI));
        }

        public static int getConnectedCallOnWifi(ContentResolver cr, int subId) {
            int result = getCallState(cr, selectForSubId(subId),
                    ImsState.Columns.CONNECTED_CALL_ON_WIFI, INVALID_VALUE);

            if (result == INVALID_VALUE) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return getConnectedCallOnWifiForPhoneId(cr, phoneId);
                }

                return STATE_INACTIVE;
            }

            return result;
        }

        public static int getConnectedCallOnWifiForPhoneId(ContentResolver cr, int phoneId) {
            return getCallState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.CONNECTED_CALL_ON_WIFI, STATE_INACTIVE);
        }

        public static int putSubId(ContentResolver cr, int subId) {
            return putSubId(cr, DEFAULT_PHONE_ID, subId);
        }

        public static int putSubId(ContentResolver cr, int phoneId, int subId) {
            return putSubIdForPhoneId(cr, ImsState.CallState.CONTENT_URI, phoneId, subId);
        }

        public static int putState(ContentResolver cr, int state) {
            return putState(cr, state, getDefaultSubId(cr, ImsState.CallState.CONTENT_URI));
        }

        public static int putState(ContentResolver cr, int state, int subId) {
            int result = putCallState(cr, selectForSubId(subId), ImsState.Columns.STATE, state);

            if (result <= 0) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return putStateForPhoneId(cr, state, phoneId);
                }
            }

            return result;
        }

        public static int putStateForPhoneId(ContentResolver cr, int state, int phoneId) {
            return putCallState(cr, selectForPhoneId(phoneId), ImsState.Columns.STATE, state);
        }

        public static int putConnectedCallOnWifi(ContentResolver cr, int state) {
            return putConnectedCallOnWifi(cr, state,
                    getDefaultSubId(cr, ImsState.CallState.CONTENT_URI));
        }

        public static int putConnectedCallOnWifi(ContentResolver cr, int state, int subId) {
            int result = putCallState(cr, selectForSubId(subId),
                    ImsState.Columns.CONNECTED_CALL_ON_WIFI, state);

            if (result <= 0) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return putConnectedCallOnWifiForPhoneId(cr, state, phoneId);
                }
            }

            return result;
        }

        public static int putConnectedCallOnWifiForPhoneId(ContentResolver cr,
                int state, int phoneId) {
            return putCallState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.CONNECTED_CALL_ON_WIFI, state);
        }

        /** ContentObserver for CallState */
        public static void registerObserver(ContentResolver cr, ContentObserver observer) {
            cr.registerContentObserver(ImsState.CallState.CONTENT_URI, true, observer);
        }

        public static void unregisterObserver(ContentResolver cr, ContentObserver observer) {
            cr.unregisterContentObserver(observer);
        }
    }

    public static final class RoamingState {
        public static void init(ContentResolver cr, int phoneId, int subId) {
            initTable(cr, phoneId, subId,
                    ImsState.RoamingState.CONTENT_URI,
                    ImsState.RoamingState.TABLE_NAME);
        }

        public static int getRowCount(ContentResolver cr) {
            return getRowCountForTable(cr, ImsState.RoamingState.CONTENT_URI);
        }

        public static int getSubId(ContentResolver cr) {
            return getSubId(cr, DEFAULT_PHONE_ID);
        }

        public static int getSubId(ContentResolver cr, int phoneId) {
            return DBUtils.CP.getInt(cr, ImsState.RoamingState.CONTENT_URI,
                    selectForPhoneId(phoneId),
                    ImsState.Columns.SUB_ID, DEFAULT_SUB_ID);
        }

        public static int getVoLteRoaming(ContentResolver cr) {
            return getVoLteRoaming(cr, getDefaultSubId(cr, ImsState.RoamingState.CONTENT_URI));
        }

        public static int getVoLteRoaming(ContentResolver cr, int subId) {
            int result = getRoamingState(cr, selectForSubId(subId),
                    ImsState.Columns.VOLTE_ROAMING, INVALID_VALUE);

            if (result == INVALID_VALUE) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return getVoLteRoamingForPhoneId(cr, phoneId);
                }

                return STATE_INACTIVE;
            }

            return result;
        }

        public static int getVoLteRoamingForPhoneId(ContentResolver cr, int phoneId) {
            return getRoamingState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.VOLTE_ROAMING, STATE_INACTIVE);
        }

        public static int getVtRoaming(ContentResolver cr) {
            return getVtRoaming(cr, getDefaultSubId(cr, ImsState.RoamingState.CONTENT_URI));
        }

        public static int getVtRoaming(ContentResolver cr, int subId) {
            int result = getRoamingState(cr, selectForSubId(subId),
                    ImsState.Columns.VT_ROAMING, INVALID_VALUE);

            if (result == INVALID_VALUE) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return getVtRoamingForPhoneId(cr, phoneId);
                }

                return STATE_INACTIVE;
            }

            return result;
        }

        public static int getVtRoamingForPhoneId(ContentResolver cr, int phoneId) {
            return getRoamingState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.VT_ROAMING, STATE_INACTIVE);
        }

        public static int putSubId(ContentResolver cr, int subId) {
            return putSubId(cr, DEFAULT_PHONE_ID, subId);
        }

        public static int putSubId(ContentResolver cr, int phoneId, int subId) {
            return putSubIdForPhoneId(cr, ImsState.RoamingState.CONTENT_URI, phoneId, subId);
        }

        public static int putVoLteRoaming(ContentResolver cr, int state) {
            return putVoLteRoaming(cr, state, getDefaultSubId(cr, ImsState.RoamingState.CONTENT_URI));
        }

        public static int putVoLteRoaming(ContentResolver cr, int state, int subId) {
            int result = putRoamingState(cr, selectForSubId(subId),
                    ImsState.Columns.VOLTE_ROAMING, state);

            if (result <= 0) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return putVoLteRoamingForPhoneId(cr, state, phoneId);
                }
            }

            return result;
        }

        public static int putVoLteRoamingForPhoneId(ContentResolver cr, int state, int phoneId) {
            return putRoamingState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.VOLTE_ROAMING, state);
        }

        public static int putVtRoaming(ContentResolver cr, int state) {
            return putVtRoaming(cr, state,
                    getDefaultSubId(cr, ImsState.RoamingState.CONTENT_URI));
        }

        public static int putVtRoaming(ContentResolver cr, int state, int subId) {
            int result = putRoamingState(cr, selectForSubId(subId),
                    ImsState.Columns.VT_ROAMING, state);

            if (result <= 0) {
                int phoneId = getPhoneIdForSubId(subId);

                if (phoneId >= DEFAULT_PHONE_ID) {
                    logi("FailOver :: phoneId=" + phoneId);
                    return putVtRoamingForPhoneId(cr, state, phoneId);
                }
            }

            return result;
        }

        public static int putVtRoamingForPhoneId(ContentResolver cr, int state, int phoneId) {
            return putRoamingState(cr, selectForPhoneId(phoneId),
                    ImsState.Columns.VT_ROAMING, state);
        }
    }

    public static final class VoWiFiActivationState {
        public static final int STATE_NO_EXIST = -1;

        public static boolean createState(ContentResolver cr,
                int slotId, String operator, String country, int state) {
            ContentValues values = new ContentValues();

            values.put(ImsState.Columns.SLOT_ID, slotId);
            values.put(ImsState.Columns.STATE, state);
            values.put(ImsState.Columns.OPERATOR, getOperator(operator, country));

            return DBUtils.CP.insert(cr,
                    ImsState.VoWiFiActivationState.CONTENT_URI, values) != null;
        }

        public static int getState(ContentResolver cr,
                int slotId, String operator, String country) {
            String where = getWhereClause(slotId, operator, country);
            return DBUtils.CP.getInt(cr, ImsState.VoWiFiActivationState.CONTENT_URI,
                    where, ImsState.Columns.STATE, STATE_NO_EXIST);
        }

        public static boolean putState(ContentResolver cr,
                int slotId, String operator, String country, int state) {
            String where = getWhereClause(slotId, operator, country);
            return DBUtils.CP.putInt(cr, ImsState.VoWiFiActivationState.CONTENT_URI,
                    ImsState.Columns.STATE, state, where);
        }

        public static boolean putOrCreateState(ContentResolver cr,
                int slotId, String operator, String country, int state) {
            String where = getWhereClause(slotId, operator, country);
            Cursor c = DBUtils.CP.getFirstCursor(cr,
                    ImsState.VoWiFiActivationState.CONTENT_URI, where);
            boolean exist = (c != null);
            DBUtils.closeCursor(c);

            if (!exist) {
                return createState(cr, slotId, operator, country, state);
            }

            return DBUtils.CP.putInt(cr, ImsState.VoWiFiActivationState.CONTENT_URI,
                    ImsState.Columns.STATE, state, where);
        }

        private static String getWhereClause(int slotId, String operator, String country) {
            return String.format(Locale.US, "%s='%d' AND %s='%s'",
                    ImsState.Columns.SLOT_ID, slotId,
                    ImsState.Columns.OPERATOR, getOperator(operator, country));
        }

        private static String getOperator(String operator, String country) {
            if (TextUtils.isEmpty(country)) {
                return operator;
            }

            return String.format(Locale.US, "%s-%s", operator, country);
        }
    }

    private static boolean hasToken(String[] tokens, String token) {
        if (tokens == null || token == null) {
            return false;
        }

        for (int i = 0; i < tokens.length; ++i) {
            if (token.equals(tokens[i])) {
                return true;
            }
        }

        return false;
    }

    private static int getDefaultSubId(ContentResolver cr, Uri contentUri) {
        if (MSimUtils.isMultiSimEnabled()) {
            return MSimUtils.getDefaultDataSubId();
        }

        int subId = MSimUtils.getSubId(DEFAULT_PHONE_ID);

        if (!MSimUtils.isValidSubId(subId)) {
            subId = getSubIdForPhoneId(cr, contentUri, DEFAULT_PHONE_ID);
        } else if (MSimUtils.isDummySubId(subId)) {
            subId = getSubIdForPhoneId(cr, contentUri, DEFAULT_PHONE_ID);
        }

        return subId;
    }

    private static int getPhoneIdForSubId(int subId) {
        return MSimUtils.getPhoneIdForDummySubId(subId);
    }

    private static int getSubIdForPhoneId(ContentResolver cr, Uri contentUri, int phoneId) {
        return DBUtils.CP.getInt(cr, contentUri,
                selectForPhoneId(phoneId), ImsState.Columns.SUB_ID, -1);
    }

    private static int putSubIdForPhoneId(ContentResolver cr, Uri uri, int phoneId, int subId) {
        ContentValues values = new ContentValues();

        values.put(ImsState.Columns.SUB_ID, subId);

        return DBUtils.CP.update(cr, uri, values, selectForPhoneId(phoneId), null);
    }

    private static int getRowCountForTable(ContentResolver cr, Uri uri) {
        Cursor c = null;

        try {
            c = DBUtils.CP.getCursor(cr, uri, null);
            return (c != null) ? c.getCount() : 0;
        } finally {
            DBUtils.closeCursor(c);
        }
    }

    private static void initTable(ContentResolver cr, int phoneId, int subId,
            Uri contentUri, String tableName) {
        String[][] contents = ImsState.getTableContents(tableName);

        if (contents == null) {
            return;
        }

        String[] persistentColumns = ImsState.getPersistentColumns(tableName);
        ContentValues values = new ContentValues();

        for (int i = ImsState.Columns.I_VALUE_BASE; i < contents.length; ++i) {
            if ((persistentColumns != null)
                    && hasToken(persistentColumns,
                        contents[i][ImsState.Columns.I_NAME])) {
                continue;
            }

            values.put(contents[i][ImsState.Columns.I_NAME],
                    contents[i][ImsState.Columns.I_VALUE]);
        }

        values.put(ImsState.Columns.SUB_ID, subId);

        DBUtils.CP.update(cr, contentUri, values, selectForPhoneId(phoneId), null);
    }

    private static void logi(String s) {
        ImsLog.i("[ISC] " + s);
    }
}
