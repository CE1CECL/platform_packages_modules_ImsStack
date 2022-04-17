/*
    Author
    <table>
    date        author                  description
    --------    --------------          ----------
    20150706    hwangoo.park@    Created
    </table>

    Description
*/

package com.android.imsstack.core.agents.agentif;

import android.telephony.TelephonyManager;

/**
 * This class provides the call state of Telephony Framework.
 */
public final class PhoneCallState {
    private int mState = TelephonyManager.CALL_STATE_IDLE;
    private String mIncomingNumber = "";

    public PhoneCallState() {
    }

    public PhoneCallState(int state, String incomingNumber) {
        mState = state;
        mIncomingNumber = incomingNumber;
    }

    public String getIncomingNumber() {
        return mIncomingNumber;
    }

    /**
     * @return the call state
     *      TelephonyManager#CALL_STATE_IDLE (0)
     *      TelephonyManager#CALL_STATE_RINGING (1)
     *      TelephonyManager#CALL_STATE_OFFHOOK (2)
     */
    public int getState() {
        return mState;
    }

    /**
     * @param the call state
     *      TelephonyManager#CALL_STATE_IDLE (0)
     *      TelephonyManager#CALL_STATE_RINGING (1)
     *      TelephonyManager#CALL_STATE_OFFHOOK (2)
     */
    public void setState(int state) {
        mState = state;
    }

    public void setIncomingNumber(String number) {
        mIncomingNumber = number;
    }
}
