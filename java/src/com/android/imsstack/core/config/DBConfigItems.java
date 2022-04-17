package com.android.imsstack.core.config;

import android.net.Uri;

import java.util.Objects;

/**
 * This class is a helper class to access the configuration.
 * It contains key values to access the database.
 */
public class DBConfigItems extends ConfigItems {
    private final String mName;
    private final String mTable;

    public DBConfigItems(int dbType, String name, String table) {
        super(dbType);

        mName = name;
        mTable = table;
    }

    @Override
    public boolean equals(Object o) {
        if (!(o instanceof DBConfigItems)) {
            return false;
        }

        DBConfigItems dcis = (DBConfigItems)o;

        return (getDBType() == dcis.getDBType())
                && Objects.equals(mName, dcis.mName)
                && Objects.equals(mTable, dcis.mTable);
    }

    @Override
    public int hashCode() {
        int code = 17;

        code = 31 * code + Objects.hashCode(mName);
        code = 31 * code + Objects.hashCode(mTable);
        code = 31 * code + getDBType();

        return code;
    }

    /** For ContentProvider access */
    @Override
    public Uri getContentUri() {
        return null;
    }

    /** For DB access */
    @Override
    public String getDBName() {
        return mName;
    }

    @Override
    public String getDBTable() {
        return mTable;
    }
}
