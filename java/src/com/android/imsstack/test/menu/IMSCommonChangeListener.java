/**
 * IMSCommonChangeListener
 * Role
 *         Set value from setting menu to DB.
 *         This listener is basic form of OnPreferenceChangeListener in setting menu.
 *         This class will be extended in setting menu to provide additional process.
 */
package com.android.imsstack.test.menu;

import android.content.ContentValues;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteException;
import android.preference.Preference;

import com.android.imsstack.core.config.ImsDbController;
import com.android.imsstack.util.DBUtils;
import com.android.imsstack.util.ImsLog;
import com.android.imsstack.util.MSimUtils;

public class IMSCommonChangeListener implements Preference.OnPreferenceChangeListener {

    // Constants--------------------------------------------------
    // Variables--------------------------------------------------
    protected String mTableName;
    protected SQLiteDatabase mImsDB;
    protected int mSlotID;

    // Static loading materials ----------------------------------
    // Public methods --------------------------------------------
    public String getTableName() {
        return this.mTableName;
    }

    // Interface implementation methods --------------------------
    public IMSCommonChangeListener(SQLiteDatabase imsDB, String tableName, int slotID) {
        this.mTableName = tableName;
        this.mImsDB = imsDB;
        this.mSlotID = slotID;
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {

        String key = preference.getKey();

        if (checkUpdateInfo(newValue, key) == false) {
            return false;
        }

        int adminFeature = SettingUtil.getBitmaskForAdminFeatures(key);

        if (adminFeature > 0) {
            int enabledFeatures = 0;
            int disabledFeatures = 0;
            boolean newBoolValue = "true".equalsIgnoreCase(newValue.toString());

            if (newBoolValue) {
                enabledFeatures |= adminFeature;
            } else {
                disabledFeatures |= adminFeature;
            }

            if (!ImsDbController.Subscriber.setAdminFeatures(mSlotID, enabledFeatures, disabledFeatures)) {
                ImsLog.e("Hidden :: update failed - " + key);
                return false;
            }

            if (MSimUtils.isMultiSimEnabled()) {
                if ("admin_debug".equals(key)) {
                    int otherSlotId = (mSlotID == 0) ? 1 : 0;
                    if (!ImsDbController.Subscriber.setAdminFeatures(otherSlotId,
                                enabledFeatures, disabledFeatures)) {
                        ImsLog.e("Hidden :: update failed - " + key);
                        return false;
                    }
                }
            }

            return true;
        }

        ContentValues values = new ContentValues();
        values.put(key, newValue.toString());

        if (updateDatabase(preference, newValue, values) == false) {
            return false;
        }

        return true;
    }

    protected boolean updateDatabase(Preference preference, Object newValue,
    ContentValues values) {
        int affectedRows = 0;

        mImsDB.beginTransaction();

        try
        {
            if (values.size() > 0) {
                affectedRows = mImsDB.update(mTableName, values
                                            , ImsDbController.selectForSlot(mSlotID), null);
                mImsDB.setTransactionSuccessful();
            }
            else {
                ImsLog.w("ContentValue is empty");
            }
        }
        catch (SQLiteException e) {
            e.printStackTrace();
        }
        finally
        {
            DBUtils.DB.endTransaction(mImsDB);
        }

        ImsLog.w(mTableName + " :: updated row count: " + affectedRows);

        if (values.size() > 0 && affectedRows != 1) {
            ImsLog.e("Update fails");
            return false;
        }

        SettingUtil.setSummary(preference, newValue);
        return true;
    }

    protected boolean checkUpdateInfo(Object newValue, String key) {
        ImsLog.d(mTableName + " :: key: " + key + " -> value:" + newValue.toString());

        if (mImsDB == null) {
            ImsLog.w("DB table (" + mTableName + ") is null");
            return false;
        }
        return true;
    }
}
