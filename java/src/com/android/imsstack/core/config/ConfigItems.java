package com.android.imsstack.core.config;

import android.net.Uri;

import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;

/**
 * This class is a helper class to access the configuration.
 * It contains key values to access the database or content provider.
 */
public abstract class ConfigItems {
    /** Column name is a key */
    public static final int DB_TYPE_COLUMN_KEY_BASED = 0;
    /**
     * Value of "name" field is a key
     * The structure of database can have these column fields:
     *      columns : "name" | "type" | "value_int" | "value_string"
     */
    public static final int DB_TYPE_NAME_KEY_BASED = 1;

    private final int mDBType;
    private Set<ConfigItem> mItems = new HashSet<ConfigItem>();

    public ConfigItems(int dbType) {
        mDBType = dbType;
    }

    /** For ContentProvider access */
    public abstract Uri getContentUri();

    /** For DB access */
    public abstract String getDBName();
    public abstract String getDBTable();

    public final int getDBType() {
        return mDBType;
    }

    public final Set<ConfigItem> getItems() {
        synchronized (mItems) {
            return mItems;
        }
    }

    public ConfigItem getItem(String name) {
        if (name == null) {
            return null;
        }

        if (mDBType == DB_TYPE_COLUMN_KEY_BASED) {
            synchronized (mItems) {
                for (Iterator<ConfigItem> it = mItems.iterator(); it.hasNext(); ) {
                    ConfigItem item = it.next();

                    if (name.equals(item.getName())) {
                        return item;
                    }
                }
            }
        } else {
            String selection = null;

            try {
                selection = String.format("%s='%s'", ConfigItem.COLUMN_NAME, name);
            } catch (Throwable t) {
                t.printStackTrace();
            }

            if (selection == null) {
                return null;
            }

            synchronized (mItems) {
                for (Iterator<ConfigItem> it = mItems.iterator(); it.hasNext(); ) {
                    ConfigItem item = it.next();

                    if (selection.equals(item.getSelection())) {
                        return item;
                    }
                }
            }
        }

        return null;
    }

    public final boolean add(ConfigItem item) {
        synchronized (mItems) {
            return mItems.add(item);
        }
    }

    public final boolean contains(ConfigItem item) {
        synchronized (mItems) {
            return mItems.contains(item);
        }
    }

    public final boolean remove(ConfigItem item) {
        synchronized (mItems) {
            return mItems.remove(item);
        }
    }
}
