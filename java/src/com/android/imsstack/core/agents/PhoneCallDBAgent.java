/**
 * PhoneCallDBHelper
 *
 *    Role
 *     PhoneCallDBHelper
 *
 */

package com.android.imsstack.core.agents;

import android.content.ContentResolver;
import android.content.Context;

import com.android.imsstack.core.agents.agentif.IAgent;
import com.android.imsstack.core.agents.agentif.IIMSPhoneAgent;
import com.android.imsstack.system.ISystem;
import com.android.imsstack.system.ISystemAPICallInfo;
import com.android.imsstack.system.SystemInterface;
import com.android.imsstack.util.AppContext;
import com.android.imsstack.util.ImsLog;
import com.android.imsstack.util.MSimUtils;

public class PhoneCallDBAgent implements IAgent, ISystemAPICallInfo {
    // Constants--------------------------------------------------

    // Variables--------------------------------------------------
    private Context mContext;
    private final int mSlotId;

    // Public methods --------------------------------------------
    public PhoneCallDBAgent(int slotId) {
        mSlotId = slotId;
    }

    // Interface implementation methods --------------------------
    @Override
    public void init(Context context) {
        mContext = context;

        ISystem system = SystemInterface.getInstance().getSystem(mSlotId);
        if (system != null) {
            system.setISystemAPICallInfo(this);
        }
    }

    @Override
    public void cleanup() {
        ISystem system = SystemInterface.getInstance().getSystem(mSlotId);
        if (system != null) {
            system.setISystemAPICallInfo(null);
        }
    }

    @Override
    public int isEmergencyNumber(String number) {
        String formatted = android.telephony.PhoneNumberUtils.stripSeparators(number);

        // normal number is false
        boolean bEmergencyNumber =
                AppContext.getTelephonyManager(MSimUtils.getSubId(mSlotId)).
                        isEmergencyNumber(formatted);

        ImsLog.d(mSlotId, "number = " + number + " , formatted string: format[" + formatted + "]"
            + "Is[" + bEmergencyNumber + "]");

        if (bEmergencyNumber) {
            return 1;
        } else {
            return 0;
        }
    }

    @Override
    public int getTTYMode() {
        int settingsTtyMode = android.provider.Settings.Secure.getInt(mContext.getContentResolver(),
            android.provider.Settings.Secure.PREFERRED_TTY_MODE, 0);

        ImsLog.i(mSlotId, "tty mode setting = " + settingsTtyMode);
        return settingsTtyMode;
    }

    @Override
    public int getCallStateInOtherSlot() {
        int state = 0;

        IIMSPhoneAgent ipa = (IIMSPhoneAgent)AgentFactory.getAgent(
                AgentFactory.IMS_PHONE, mSlotId);
        if (ipa != null) {
            state = ipa.getCSCallStateInOtherSlot();
        }

        ImsLog.i(mSlotId, "call state in other slot = " + state);

        return state;
    }
}
