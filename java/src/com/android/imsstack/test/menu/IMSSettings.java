/**
 * IMSSettings
 * Role
 *         Setting menu factory which is requested by operator.
 *         Send intent to target setting menu based on Operator & Country Information in java layer.
 */

package com.android.imsstack.test.menu;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

import com.android.imsstack.core.ImsGlobal;
import com.android.imsstack.util.ImsLog;
import com.android.imsstack.util.MSimUtils;

public class IMSSettings extends Activity {

    private SettingSlot mSettingSlot = null;
    private IMSSettingsSlotListener mIMSSettingsSlotListener = new IMSSettingsSlotListener();

    @Override
    public void onCreate(Bundle savedInstanceState) {
        ImsLog.d("");
        super.onCreate(savedInstanceState);

        mSettingSlot = new SettingSlot(this);
        mSettingSlot.setSlotList(mIMSSettingsSlotListener);
    }

    @Override
    public void onStop() {
        ImsLog.d("");
        super.onStop();
        this.finish();
    }

    private void start(int slotID) {

        SettingUtil.init(getApplicationContext());

        String operator = ImsGlobal.getOperator(slotID);

        if ("TMO".equals(operator) && ImsGlobal.isCountry(slotID, "US")) {
            operator = "TMUS";
        }

        ImsLog.d("slotID[" + slotID + "]" + " operator[" + operator + "]");

        Intent intent = new Intent(this, IMSProvisioning.class);
        intent.putExtra(MSimUtils.EXTRA_KEY_SLOT_ID, slotID);
        startActivity(intent);

    }

    /* ---------------------------------------------------------------------------------------------
        subclass - IMSProvisionSlotListener
    --------------------------------------------------------------------------------------------- */
    private class IMSSettingsSlotListener extends SettingSlot.SlotListener {
        @Override
        public void onSlot(int slotID) {
            start(slotID);
        }
    }

}
