package com.android.imsstack.enabler.ssc.data;

import com.android.imsstack.enabler.ssc.ESsType;

public class CfServiceUpdateData extends SscServiceData {
    private String mForwardToNumber = null;
    private int mNoReplyTimer = -1;

    public CfServiceUpdateData(int slotId, ESsType ssType, int eventNum, int transactionId,
            int state, int conidition, String forwardToNumber, int noReplyTimer, int serviceClass) {
        super(slotId, ssType, eventNum, transactionId, state, serviceClass);
        mCondition = conidition;
        mForwardToNumber = forwardToNumber;
        mNoReplyTimer = noReplyTimer;
    }

    public String getForwardToNumber() {
        return mForwardToNumber;
    }

    public int getReplyTimer() {
        return mNoReplyTimer;
    }

    public String toString() {
        return super.toString() + "\nCondition :" + mCondition + "\n" + "ForwardTo :"
                + mForwardToNumber + "\n" + "NoReplyTimer :" + mNoReplyTimer;
    }
}
