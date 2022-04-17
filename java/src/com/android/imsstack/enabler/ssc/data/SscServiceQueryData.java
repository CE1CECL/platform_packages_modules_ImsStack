package com.android.imsstack.enabler.ssc.data;

import com.android.imsstack.enabler.ssc.ESsType;

public class SscServiceQueryData extends SscData {
    protected int mCondition = -1;

    public SscServiceQueryData(int slotId, ESsType ssType, int eventNum, int transactionId,
            int serviceClass) {
        super(slotId, ssType, eventNum, transactionId, serviceClass);
    }

    public int getCondition() {
        return mCondition;
    }

    public void setCondition(int condition) {
        mCondition = condition;
    }

    public String toString() {
        return ("SlotId : " + mSlotId + ", SStype : " + mSsType
                + ", EventNum : " + mEventNum + ", TransactionId : " + mTransactionId);
    }
}
