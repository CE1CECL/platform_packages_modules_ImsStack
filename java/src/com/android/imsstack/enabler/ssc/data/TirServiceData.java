package com.android.imsstack.enabler.ssc.data;

import com.android.imsstack.enabler.ssc.ESsType;

public class TirServiceData extends SscServiceData {
    private int mProvisionStatus = -1;

    public TirServiceData(int slotId, ESsType ssType, int eventNum, int transactionId, int state,
            int provisionStatus) {
        super(slotId, ssType, eventNum, transactionId, state);
        mProvisionStatus = provisionStatus;
    }

    public int getProvisionStatus() {
        return mProvisionStatus;
    }

    public String toString() {
        return super.toString() + ", Provision State : " + mProvisionStatus;
    }
}
