package com.android.imsstack.core.config;

import android.net.Uri;

import java.util.Objects;

/**
 * This class is a helper class to access the configuration.
 * It contains key values to access the content provider.
 */
public class CPConfigItems extends ConfigItems {
    private final Uri mContentUri;

    public CPConfigItems(int dbType, Uri contentUri) {
        super(dbType);

        mContentUri = contentUri;
    }

    @Override
    public boolean equals(Object o) {
        if (!(o instanceof CPConfigItems)) {
            return false;
        }

        CPConfigItems ccis = (CPConfigItems)o;

        return (getDBType() == ccis.getDBType())
                && Objects.equals(mContentUri, ccis.mContentUri);
    }

    @Override
    public int hashCode() {
        int code = 17;

        code = 31 * code + Objects.hashCode(mContentUri);
        code = 31 * code + getDBType();

        return code;
    }

    /** For ContentProvider access */
    @Override
    public Uri getContentUri() {
        return mContentUri;
    }

    /** For DB access */
    @Override
    public String getDBName() {
        return null;
    }

    @Override
    public String getDBTable() {
        return null;
    }
}
