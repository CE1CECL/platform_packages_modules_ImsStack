package com.android.imsstack.enabler.ssc.data;

import com.android.imsstack.enabler.ssc.ESsType;

public class SscServiceData extends SscData {
    protected int mServiceState  = -1;
    protected int mCondition = -1;

    public SscServiceData(int slotId, ESsType ssType, int eventNum, int transactionId, int state) {
        super(slotId, ssType, eventNum, transactionId, 0);
        mServiceState = state;
    }

    public SscServiceData(int slotId, ESsType ssType, int eventNum, int transactionId, int state,
            int serviceClass) {
        super(slotId, ssType, eventNum, transactionId, serviceClass);
        mServiceState = state;
    }

    public int getState() {
        return mServiceState;
    }

    public int getCondition() {
        return mCondition;
    }

    public String toString() {
        return "SlotId : " + mSlotId + ", SStype : " + mSsType
                + ", EventNum : " + mEventNum + ", Transaction Id : " + mTransactionId
                + ", Service State : " + mServiceState;
    }
}
