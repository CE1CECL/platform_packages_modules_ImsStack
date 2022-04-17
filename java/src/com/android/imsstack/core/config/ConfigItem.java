package com.android.imsstack.core.config;

import java.util.Objects;

/**
 * This class is a helper class to access the configuration.
 * It contains key values to access the database or content provider.
 */
public final class ConfigItem {
    public static final int COLUMN_TYPE_STRING = 0;
    public static final int COLUMN_TYPE_INT = 1;

    /** This can be changed based on the design of database. */
    public static final String COLUMN_NAME = "name";
    public static final String COLUMN_VALUE_INT = "value_int";
    public static final String COLUMN_VALUE_STRING = "value_string";

    private final String mName;
    private final int mType;
    /** The selection statement for a query of DB or ContentProvider */
    private final String mSelection;

    public ConfigItem(String name, int type, String selection) {
        mName = name;
        mType = type;
        mSelection = selection;
    }

    @Override
    public boolean equals(Object o) {
        if (!(o instanceof ConfigItem)) {
            return false;
        }

        ConfigItem ci = (ConfigItem)o;

        return Objects.equals(mName, ci.mName)
                && (mType == ci.mType)
                && Objects.equals(mSelection, ci.mSelection);
    }

    @Override
    public int hashCode() {
        int code = 17;

        code = 31 * code + Objects.hashCode(mName);
        code = 31 * code + mType;
        code = 31 * code + Objects.hashCode(mSelection);

        return code;
    }

    public String getName() {
        return mName;
    }

    public int getType() {
        return mType;
    }

    public String getSelection() {
        return mSelection;
    }
}
