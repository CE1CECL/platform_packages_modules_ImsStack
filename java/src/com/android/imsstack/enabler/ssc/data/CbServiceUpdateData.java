package com.android.imsstack.enabler.ssc.data;

import com.android.imsstack.enabler.ssc.ESsType;

public class CbServiceUpdateData extends SscServiceData  {
    private String[] mBarrList = null;
    private String mPassword = null;

    public CbServiceUpdateData(int slotId, ESsType ssType, int evnetNum, int transactionId,
            int state, int condition, String[] barrList, int serviceClass, String password) {
        super(slotId, ssType, evnetNum, transactionId, state, serviceClass);
        mCondition = condition;
        mBarrList = barrList;
        mPassword = password;
    }

    public String[] getBarrList() {
        return mBarrList;
    }

    public String getPassword() {
        return mPassword;
    }

    public String toString() {
        return super.toString() + ", mCondition : " + mCondition + ", Barr List : " + mBarrList;
    }
}
