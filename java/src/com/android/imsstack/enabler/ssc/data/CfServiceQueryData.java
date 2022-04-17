package com.android.imsstack.enabler.ssc.data;

import com.android.imsstack.enabler.ssc.ESsType;

public class CfServiceQueryData extends SscServiceQueryData {
    private String mForwardToNumber = null;

    public CfServiceQueryData(int slotId, ESsType sstype, int eventNum, int transactionId,
            int condition, String number, int serviceClass) {
        super(slotId, sstype, eventNum, transactionId, serviceClass);
        mCondition = condition;
        mForwardToNumber = number;
    }

    public String toString() {
        return super.toString() + ", mCondition : " + mCondition + ", ForwardTo Number : "
                + mForwardToNumber;
    }
}
