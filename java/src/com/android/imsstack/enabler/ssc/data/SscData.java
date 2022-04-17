package com.android.imsstack.enabler.ssc.data;

import com.android.imsstack.enabler.ssc.ESsType;

public abstract class SscData {
    protected int mSlotId        = -1;
    protected ESsType mSsType    = ESsType.NONE;
    protected int mEventNum      = -1;
    protected int mTransactionId = -1;
    protected int mServiceClass  = -1;
    protected int mResponseCode  = -1;

    public SscData(int slotId, ESsType ssType, int eventNum, int transactionId, int serviceClass) {
        mSlotId = slotId;
        mSsType = ssType;
        mEventNum = eventNum;
        mTransactionId = transactionId;
        mServiceClass = serviceClass;
    }

    public int getSlotId() {
        return this.mSlotId;
    }

    public ESsType getSsType() {
        return this.mSsType;
    }

    public int getEventNumber() {
        return mEventNum;
    }

    public int getTransactionId() {
        return mTransactionId;
    }

    public int getServiceClass() {
        return mServiceClass;
    }

    public void setServiceClass(int serviceClass) {
        mServiceClass = serviceClass;
    }

    public int getResponseCode() {
        return mResponseCode;
    }

    public void setResponseCode(int responseCode) {
        this.mResponseCode = responseCode;
    }
}
