/*
    Author
    <table>
    date        author                  description
    --------    --------------          ----------
    20150531    hwangoo.park@           Created
    </table>

    Description
*/

package com.android.imsstack.enabler.mtc.telephony;

import android.telephony.TelephonyManager;

public final class TelephonyCallState {
    private int mState = TelephonyManager.CALL_STATE_IDLE;
    private String mIncomingNumber = "";

    public TelephonyCallState() {
    }

    public TelephonyCallState(int state, String incomingNumber) {
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
