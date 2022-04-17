package com.android.imsstack.enabler.ssc.data;

import com.android.imsstack.enabler.ssc.ESsType;

public class CwServiceData extends SscServiceData {
    public CwServiceData(int slotId, ESsType ssType, int eventNum, int transactionId, int state) {
        super(slotId, ssType, eventNum, transactionId, state);
    }

    public String toString() {
        return super.toString();
    }
}
