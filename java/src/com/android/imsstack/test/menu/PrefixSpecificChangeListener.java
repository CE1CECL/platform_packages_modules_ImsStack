/**
 * EnableSpecificChangeListener
 * Role
 *         Set value from setting menu to DB.
 *         This listener used for DB columns which have same column name.
 *         Column should have unique key value, so it has table related prefix string.
 *         During setting process, the prefix will be removed to set new value to Database table.
 */
package com.android.imsstack.test.menu;

import android.content.ContentValues;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteException;
import android.preference.Preference;

import com.android.imsstack.core.config.ImsDbController;
import com.android.imsstack.util.DBUtils;
import com.android.imsstack.util.ImsLog;

public class PrefixSpecificChangeListener implements Preference.OnPreferenceChangeListener {

    // Constants--------------------------------------------------
    // Variables--------------------------------------------------
    protected String mTableName;
    protected SQLiteDatabase mImsDB;
    protected String mKeyPrefix = null;
    protected String mLogTag = null;
    protected int mSlotId = 0;

    // Static loading materials ----------------------------------
    // Public methods --------------------------------------------
    public PrefixSpecificChangeListener(SQLiteDatabase imsDB, String keyPrefix, String tableName, String logTag_, int slotId) {
        this.mTableName = tableName;
        this.mImsDB = imsDB;
        this.mKeyPrefix = keyPrefix;
        this.mLogTag = logTag_;
        this.mSlotId = slotId;
    }

    public String getPrefix() {
        return mKeyPrefix;
    }

    public String getTableName() {
        return mTableName;
    }

    public boolean onPreferenceChange(Preference preference, Object newValue) {
        String key = preference.getKey();

        ImsLog.d(mLogTag + " :: key: " + key + " -> value:" + newValue.toString());

        if (mImsDB == null) {
            ImsLog.w("DB table (" + mTableName + ") is null");
            return false;
        }

        int affectedRows = 0;
        ContentValues values = new ContentValues();

        // Trim the key prefix, "vt_" / "voip_" / "sms_"
        values.put(key.substring(mKeyPrefix.length()), newValue.toString());

        mImsDB.beginTransaction();

        try
        {
            affectedRows = mImsDB.update(mTableName, values
                                        , ImsDbController.selectForSlot(mSlotId), null);
            mImsDB.setTransactionSuccessful();
        }
        catch (SQLiteException e) {
            e.printStackTrace();
        }
        finally
        {
            DBUtils.DB.endTransaction(mImsDB);
        }

        ImsLog.w(" :: updated row count: " + affectedRows);

        if (affectedRows != 1) {
            ImsLog.e("Update fails");
            return false;
        }

        //summary change
        SettingUtil.setSummary(preference, newValue);

        return true;
    }
    // Private/Protected methods ---------------------------------
}
