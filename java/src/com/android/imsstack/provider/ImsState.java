package com.android.imsstack.provider;

import android.content.ContentValues;
import android.net.Uri;
import android.provider.BaseColumns;

import com.android.imsstack.util.MSimUtils;

public class ImsState {
    public static final String AUTHORITY = "com.android.imsstack.provider.ims_state";
    public static final String DATABASE_NAME = "ims_state.db";
    public static final int DATABASE_VERSION = 7;
    /** In the moment, it's not used.
    public static final String DEFAULT_SORT_ORDER = "modified DESC";
    */

    public static final class Columns implements BaseColumns {
        public static final String SUB_ID = "sub_id";
        public static final String STATE = "state";
        public static final String NETWORK_TYPE = "network_type";
        public static final String SERVICE_STATUS = "service_status";
        public static final String CALL_STATE = "call_state";
        public static final String VOLTE_PROVISIONED = "volte_provisioned";
        public static final String VT_PROVISIONED = "vt_provisioned";
        public static final String WFC_PROVISIONED = "wfc_provisioned";
        public static final String DEVICE_VOLTE_AVAILABLE = "device_volte_available";
        public static final String DEVICE_VT_AVAILABLE = "device_vt_available";
        public static final String DEVICE_WFC_AVAILABLE = "device_wfc_available";
        public static final String CONNECTED_CALL_ON_WIFI = "connected_call_on_wifi";
        public static final String VOLTE_ROAMING = "volte_roaming";
        public static final String VT_ROAMING = "vt_roaming";
        public static final String SLOT_ID = "slot_id";
        public static final String OPERATOR = "operator";
        public static final String RESERVED = "reserved";

        public static final String TYPE_INT = "INTEGER";
        public static final String TYPE_INT_PRIMARY_KEY
                = "INTEGER PRIMARY KEY AUTOINCREMENT DEFAULT 1";
        public static final String TYPE_TEXT = "TEXT";

        public static final int PRIMARY_KEY_BASE = 1;
        /** To skip the "_id" field; automatic increment */
        public static final int I_VALUE_BASE = 1;
        public static final int I_NAME = 0;
        public static final int I_TYPE = 1;
        public static final int I_VALUE = 2;
    }

    /** Registration */
    public static final class RegState {
        public static final String TABLE_NAME = "reg_state";
        public static final Uri CONTENT_URI = Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);
        public static final String CONTENT_TYPE
            = "vnd.android.cursor.dir/" + AUTHORITY + "." + TABLE_NAME;
        public static final String CONTENT_ITEM_TYPE
            = "vnd.android.cursor.item/" + AUTHORITY + "." + TABLE_NAME + "/#";

        public static final String CONTENTS[][] = {
            { Columns._ID, Columns.TYPE_INT_PRIMARY_KEY, "1" },
            { Columns.SUB_ID, Columns.TYPE_INT, "-1" },
            { Columns.STATE, Columns.TYPE_INT, "0" },
            { Columns.SERVICE_STATUS, Columns.TYPE_INT, "0" },
            { Columns.NETWORK_TYPE, Columns.TYPE_INT, "0" }
        };
    }

    /** VoLTE/VT */
    public static final class VoLteState {
        public static final String TABLE_NAME = "volte_state";
        public static final Uri CONTENT_URI = Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);
        public static final String CONTENT_TYPE
            = "vnd.android.cursor.dir/" + AUTHORITY + "." + TABLE_NAME;
        public static final String CONTENT_ITEM_TYPE
            = "vnd.android.cursor.item/" + AUTHORITY + "." + TABLE_NAME + "/#";

        public static final String CONTENTS[][] = {
            { Columns._ID, Columns.TYPE_INT_PRIMARY_KEY, "1" },
            { Columns.SUB_ID, Columns.TYPE_INT, "-1" },
            // 1: VoIP, 2: VT, 3: UC
            { Columns.SERVICE_STATUS, Columns.TYPE_INT, "0" },
            { Columns.CALL_STATE, Columns.TYPE_INT, "0" },
            { Columns.VOLTE_PROVISIONED, Columns.TYPE_INT, "1" },
            { Columns.VT_PROVISIONED, Columns.TYPE_INT, "0" },
            { Columns.WFC_PROVISIONED, Columns.TYPE_INT, "0" },
            { Columns.DEVICE_VOLTE_AVAILABLE, Columns.TYPE_INT, "0" },
            { Columns.DEVICE_VT_AVAILABLE, Columns.TYPE_INT, "0" },
            { Columns.DEVICE_WFC_AVAILABLE, Columns.TYPE_INT, "0" }
        };

        /** Persistent items */
        private static final String PERSISTENT_COLUMNS[] = {
            Columns.VOLTE_PROVISIONED,
            Columns.VT_PROVISIONED,
            Columns.WFC_PROVISIONED,
            Columns.DEVICE_VOLTE_AVAILABLE,
            Columns.DEVICE_VT_AVAILABLE,
            Columns.DEVICE_WFC_AVAILABLE
        };
    }

    /** Call specific state */
    public static final class CallState {
        public static final String TABLE_NAME = "call_state";
        public static final Uri CONTENT_URI = Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);
        public static final String CONTENT_TYPE
            = "vnd.android.cursor.dir/" + AUTHORITY + "." + TABLE_NAME;
        public static final String CONTENT_ITEM_TYPE
            = "vnd.android.cursor.item/" + AUTHORITY + "." + TABLE_NAME + "/#";

        public static final String CONTENTS[][] = {
            { Columns._ID, Columns.TYPE_INT_PRIMARY_KEY, "1" },
            { Columns.SUB_ID, Columns.TYPE_INT, "-1" },
            { Columns.STATE, Columns.TYPE_INT, "0" },
            { Columns.CONNECTED_CALL_ON_WIFI, Columns.TYPE_INT, "0" }
        };
    }

    public static final class RoamingState {
        public static final String TABLE_NAME = "roaming_state";
        public static final Uri CONTENT_URI = Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);
        public static final String CONTENT_TYPE
            = "vnd.android.cursor.dir/" + AUTHORITY + "." + TABLE_NAME;
        public static final String CONTENT_ITEM_TYPE
            = "vnd.android.cursor.item/" + AUTHORITY + "." + TABLE_NAME + "/#";

        public static final String CONTENTS[][] = {
            { Columns._ID, Columns.TYPE_INT_PRIMARY_KEY, "1" },
            { Columns.SUB_ID, Columns.TYPE_INT, "-1" },
            { Columns.VOLTE_ROAMING, Columns.TYPE_INT, "0" },
            { Columns.VT_ROAMING, Columns.TYPE_INT, "0" }
        };

        /** Persistent items */
        private static final String PERSISTENT_COLUMNS[] = {
            Columns.VOLTE_ROAMING,
            Columns.VT_ROAMING
        };
    }

    /** Global: VoWiFi activation process */
    public static final class VoWiFiActivationState {
        public static final String TABLE_NAME = "vowifi_activation_state";
        public static final Uri CONTENT_URI = Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);
        public static final String CONTENT_TYPE
            = "vnd.android.cursor.dir/" + AUTHORITY + "." + TABLE_NAME;
        public static final String CONTENT_ITEM_TYPE
            = "vnd.android.cursor.item/" + AUTHORITY + "." + TABLE_NAME + "/#";

        public static final String CONTENTS[][] = {
            { Columns._ID, Columns.TYPE_INT_PRIMARY_KEY, "1" },
            { Columns.SUB_ID, Columns.TYPE_INT, "-1" },
            { Columns.SLOT_ID, Columns.TYPE_INT, "0" },
            { Columns.OPERATOR, Columns.TYPE_TEXT, "" },
            { Columns.STATE, Columns.TYPE_INT, "0" },
            { Columns.RESERVED, Columns.TYPE_TEXT, "" }
        };
    }

    /** Tables */
    private static final String TABLES[] = {
        RegState.TABLE_NAME,
        VoLteState.TABLE_NAME,
        CallState.TABLE_NAME,
        RoamingState.TABLE_NAME,
        VoWiFiActivationState.TABLE_NAME
    };

    public static final String[] getPersistentColumns(String tableName) {
        if (VoLteState.TABLE_NAME.equals(tableName)) {
            return VoLteState.PERSISTENT_COLUMNS;
        } else if (RoamingState.TABLE_NAME.equals(tableName)) {
            return RoamingState.PERSISTENT_COLUMNS;
        }

        return null;
    }

    public static final String[] getTables() {
        return TABLES;
    }

    public static final String[][] getTableContents(String tableName) {
        if (RegState.TABLE_NAME.equals(tableName)) {
            return RegState.CONTENTS;
        } else if (VoLteState.TABLE_NAME.equals(tableName)) {
            return VoLteState.CONTENTS;
        } else if (CallState.TABLE_NAME.equals(tableName)) {
            return CallState.CONTENTS;
        } else if (RoamingState.TABLE_NAME.equals(tableName)) {
            return RoamingState.CONTENTS;
        } else if (VoWiFiActivationState.TABLE_NAME.equals(tableName)) {
            return VoWiFiActivationState.CONTENTS;
        }

        return null;
    }

    public static final int getTableContentCount() {
        return MSimUtils.isMultiSimEnabled() ? 2 : 1;
    }

    public static final boolean hasDefaultValues(String tableName) {
        if (VoWiFiActivationState.TABLE_NAME.equals(tableName)) {
            return false;
        }

        return true;
    }

    /* package */ static final class PersistentValues {
        private String mTableName;
        private int mId;
        private ContentValues mValues;

        PersistentValues(String table, int id) {
            mTableName = table;
            mId = id;
            mValues = new ContentValues();
        }

        int getId() {
            return mId;
        }

        String getTableName() {
            return mTableName;
        }

        ContentValues getValues() {
            return mValues;
        }

        boolean isEmpty() {
            return mValues.size() == 0;
        }

        void putInt(String name, int value) {
            mValues.put(name, value);
        }

        void putString(String name, String value) {
            mValues.put(name, value);
        }

        @Override
        public String toString() {
            return "[ PersistentValues: " + mTableName + ", " + mId + ", "
                    + mValues.toString() + " ]";
        }
    }
}
