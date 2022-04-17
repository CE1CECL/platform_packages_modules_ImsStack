package com.android.imsstack.enabler.ssc.data;

import com.android.imsstack.enabler.ssc.ESsType;

public class CbServiceQueryData extends SscServiceQueryData {
    public CbServiceQueryData(int slotId, ESsType sstype, int eventNum, int transactionId,
            int condition, int serviceClass) {
        super(slotId, sstype, eventNum, transactionId, serviceClass);
        mCondition = condition;
    }

    public String toString() {
        return super.toString() + ", mCondition : " + mCondition;
    }
}
