package com.android.imsstack.enabler.ssc.data;

import com.android.imsstack.enabler.ssc.ESsType;

public class OirServiceData extends SscServiceData {
    private int mProvisionStatus = -1;
    private int mOutgoingState = -1;

    public OirServiceData(int slotId, ESsType ssType, int eventNum, int transactionId, int state,
            int provisionStatus, int outgoingState) {
        super(slotId, ssType, eventNum, transactionId, state);
        mProvisionStatus = provisionStatus;
        mOutgoingState = outgoingState;
    }

    public int getProvisionStatus() {
        return mProvisionStatus;
    }

    public int getOutgoingState() {
        return mOutgoingState;
    }

    public String toString() {
        return super.toString() + ", Provision State : " + mProvisionStatus;
    }
}
