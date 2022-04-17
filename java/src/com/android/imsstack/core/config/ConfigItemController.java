package com.android.imsstack.core.config;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;

import com.android.imsstack.util.DBUtils;

/**
 * This class is a helper class to access the configuration.
 */
public final class ConfigItemController {

    public static int getValueInt(Context c, String name,
            ConfigItems items, int defaultValue) {
        if ((c == null) || (name == null) || (items == null)) {
            return defaultValue;
        }

        ConfigItem item = items.getItem(name);

        if (item == null) {
            return defaultValue;
        }

        if (items instanceof CPConfigItems) {
            return DBUtils.CP.getInt(c.getContentResolver(), items.getContentUri(),
                    item.getSelection(), item.getName(), defaultValue);
        } else if (items instanceof DBConfigItems) {
            SQLiteDatabase db = DBUtils.DB.open(items.getDBName(), SQLiteDatabase.OPEN_READONLY);

            if (db == null) {
                return defaultValue;
            }

            int value = DBUtils.DB.getInt(db, items.getDBTable(),
                    item.getSelection(), item.getName(), defaultValue);

            DBUtils.DB.close(db);
            return value;
        }

        return defaultValue;
    }

    public static String getValueString(Context c, String name,
            ConfigItems items, String defaultValue) {
        if ((c == null) || (name == null) || (items == null)) {
            return defaultValue;
        }

        ConfigItem item = items.getItem(name);

        if (item == null) {
            return defaultValue;
        }

        if (items instanceof CPConfigItems) {
            return DBUtils.CP.getString(c.getContentResolver(), items.getContentUri(),
                    item.getSelection(), item.getName(), defaultValue);
        } else if (items instanceof DBConfigItems) {
            SQLiteDatabase db = DBUtils.DB.open(items.getDBName(), SQLiteDatabase.OPEN_READONLY);

            if (db == null) {
                return defaultValue;
            }

            String value = DBUtils.DB.getString(db, items.getDBTable(),
                    item.getSelection(), item.getName(), defaultValue);

            DBUtils.DB.close(db);
            return value;
        }

        return defaultValue;
    }

    public static boolean putValueInt(Context c, String name,
            ConfigItems items, int value) {
        if ((c == null) || (name == null) || (items == null)) {
            return false;
        }

        ConfigItem item = items.getItem(name);

        if (item == null) {
            return false;
        }

        if (items instanceof CPConfigItems) {
            return DBUtils.CP.putInt(c.getContentResolver(), items.getContentUri(),
                    item.getName(), value, item.getSelection());
        } else if (items instanceof DBConfigItems) {
            SQLiteDatabase db = DBUtils.DB.open(items.getDBName(), SQLiteDatabase.OPEN_READWRITE);

            if (db == null) {
                return false;
            }

            boolean result = DBUtils.DB.ImmediateMode.putInt(db, items.getDBTable(),
                    item.getName(), value, item.getSelection());

            DBUtils.DB.close(db);
            return result;
        }

        return false;
    }

    public static boolean putValueString(Context c, String name,
            ConfigItems items, String value) {
        if ((c == null) || (name == null) || (items == null)) {
            return false;
        }

        ConfigItem item = items.getItem(name);

        if (item == null) {
            return false;
        }

        if (items instanceof CPConfigItems) {
            return DBUtils.CP.putString(c.getContentResolver(), items.getContentUri(),
                    item.getName(), value, item.getSelection());
        } else if (items instanceof DBConfigItems) {
            SQLiteDatabase db = DBUtils.DB.open(items.getDBName(), SQLiteDatabase.OPEN_READWRITE);

            if (db == null) {
                return false;
            }

            boolean result = DBUtils.DB.ImmediateMode.putString(db, items.getDBTable(),
                    item.getName(), value, item.getSelection());

            DBUtils.DB.close(db);
            return result;
        }

        return false;
    }
}
