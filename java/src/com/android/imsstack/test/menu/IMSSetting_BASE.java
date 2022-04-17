/**
 * IMSSetting_BASE
 * Role
 *         Setting Menu that contains features which are operator requested.
 *         Other IMSSettings & IMSProvisioning class extends this class to implement setting menu.
 *         Don't use this class without extension because select listener doesn't work without registration.
 *         If registration added in this class, duplicated listener registration will be occur.
 */

package com.android.imsstack.test.menu;

import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteException;
import android.os.Bundle;
import android.os.PowerManager;
import android.os.SystemClock;
import android.preference.CheckBoxPreference;
import android.view.Window;
import android.view.WindowManager.LayoutParams;

import com.android.imsstack.R;
import com.android.imsstack.core.config.ProviderInterface;
import com.android.imsstack.util.AppContext;
import com.android.imsstack.util.DBUtils;
import com.android.imsstack.util.ImsLog;
import com.android.imsstack.util.ImsPrivateProperties;
import com.android.imsstack.util.MSimUtils;

import java.util.ArrayList;

public class IMSSetting_BASE extends SettingMenu {
    protected IMSCommonChangeListener onSubscriberListener = null;

    protected SQLiteDatabase imsDB = null;

    protected boolean bChange = false;
    protected int mSlotID = -1;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Window wd = getWindow();

        if (wd != null) {
            LayoutParams layoutParams = wd.getAttributes();
            wd.setAttributes(layoutParams);
        }

        mSlotID = getSlotIdFromIntent();
        if (mSlotID < 0) {
            ImsLog.d("slotID[" + mSlotID + "] is invalid");
        }

        // Initialize IMS configuration database
        if (imsDB == null) {
            try {
                imsDB = SQLiteDatabase.openDatabase(ProviderInterface.DBFP,
                        null, SQLiteDatabase.OPEN_READWRITE);
            } catch (SQLiteException e) {
                e.printStackTrace();
                return;
            }
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();

        if (imsDB != null) {
            imsDB.close();
            imsDB = null;
        }
    }

    protected void registerSubscriberChangeListener(IMSCommonChangeListener listener) {
        ImsLog.d("");

        ArrayList<String> checkboxPreferenceList = new ArrayList<String>();
        ArrayList<String> listPreferenceList = new ArrayList<String>();
        ArrayList<String> editPreferenceList = new ArrayList<String>();
        ArrayList<String> neditPreferenceList = new ArrayList<String>();

        Cursor cursor = DBUtils.DB.getCursor(mSlotID, imsDB, listener.getTableName());
        if (cursor == null) {
            ImsLog.e("Cursor :: (" + listener.getTableName() + ") is null");
            return;
        }

        String columns[] = cursor.getColumnNames();

        SettingUtil.displayColumns(columns, getSlotID());

        // Move the cursor to the first row
        cursor.moveToNext();

        checkboxPreferenceList.add(getString(R.string.ims_key_admin_ims));
        checkboxPreferenceList.add(getString(R.string.ims_key_admin_debug));
        for (String preference : checkboxPreferenceList) {
            int adminFeature = SettingUtil.getBitmaskForAdminFeatures(preference);

            if (adminFeature == 0) {
                setCheckboxPreference(cursor, preference, columns, listener);
            } else {
                setCheckboxPreferenceForAdminFeatures(cursor,
                        preference, adminFeature, columns, listener);
            }
        }

        listPreferenceList.add(getString(R.string.ims_key_admin_pcscf));
        for (String preference : listPreferenceList) {
            setListPreference(cursor, preference, columns, listener);
        }

        editPreferenceList.add(getString(R.string.ims_key_pcscf_address_0));
        for (String preference : editPreferenceList) {
            setEditTextPreference(cursor, preference, columns, listener);
        }

        neditPreferenceList.add(getString(R.string.ims_key_pcscf_port_0));
        for (String preference : neditPreferenceList) {
            setNumberEditTextPreference(cursor, preference, columns, listener);
        }

        // Close the cursor; release all the resource related with the cursor
        cursor.close();
    }

    protected void setSlotID(int slotID) {
        ImsLog.d("SlotID[" + mSlotID + "]" + "->[" + slotID + "]");
        mSlotID = slotID;
    }

    protected int getSlotID() {
        return mSlotID;
    }

    protected class RebootThread extends Thread {
        public RebootThread() {
            super();
        }

        @Override
        public void run() {
            SystemClock.sleep(200);

            PowerManager pm = AppContext.getSystemService(PowerManager.class);

            try {
                if (pm != null) {
                    pm.reboot("IMS settings changed");
                }
            } catch (SecurityException e) {
                e.printStackTrace();
            }
        }
    }

    protected boolean setPersistentProperty(String key, String newValue) {
        String oldValue = ImsPrivateProperties.Persistent.get(key, "", getSlotID());

        if (!oldValue.equalsIgnoreCase(newValue)) {
            ImsPrivateProperties.Persistent.set(key, newValue, getSlotID());
            ImsLog.w("Persistent property changed : " + key + "="
                    + ImsPrivateProperties.Persistent.get(key, getSlotID()));
            return true;
        }

        return false;
    }

    protected int getSlotIdFromIntent() {
        return getIntent().getIntExtra(MSimUtils.EXTRA_KEY_SLOT_ID, MSimUtils.INVALID_PHONE_ID);
    }

    protected void setExtraForSlot(Intent intent) {
        intent.putExtra(MSimUtils.EXTRA_KEY_SLOT_ID, getSlotID());
    }

    protected boolean getUsePredefinedUserAgent() {
        return ImsPrivateProperties.Persistent.getBoolean(
                ImsPrivateProperties.Persistent.KEY_USE_PREDEFINED_USER_AGENT,
                false, getSlotID());
    }

    protected void setUsePredefinedUserAgent(boolean value) {
        ImsLog.d("usePredefinedUserAgent=" + value);
        ImsPrivateProperties.Persistent.setBoolean(
                ImsPrivateProperties.Persistent.KEY_USE_PREDEFINED_USER_AGENT,
                value, getSlotID());
    }
}
