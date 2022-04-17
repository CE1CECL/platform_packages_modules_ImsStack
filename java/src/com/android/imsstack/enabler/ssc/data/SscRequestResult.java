package com.android.imsstack.enabler.ssc.data;

// Class for deliver result to SscServiceImpl.
// this values will be send to UI part
public class SscRequestResult {
    private int mSlotId = -1;
    private int mTransactionId = -1;
    private int mResultState = -1;
    private int mCode = -1;
    private int mExtraCode = -1;

    private SscServiceData mSscServiceData = null;

    public SscRequestResult(int slotId, int transactionId, int resultState, int code,
            int extraCode) {
        mSlotId = slotId;
        mTransactionId = transactionId;
        mResultState = resultState;
        mCode = code;
        mExtraCode = extraCode;
    }

    public int getSlotId() {
        return mSlotId;
    }

    public int getTransactionId() {
        return mTransactionId;
    }

    public int getResultState() {
        return mResultState;
    }

    public int getCode() {
        return mCode;
    }

    public int getExtraCode() {
        return mExtraCode;
    }

    public void setSscServiceData(SscServiceData data) {
        mSscServiceData = data;
    }

    public SscServiceData getSscServiceData() {
        return mSscServiceData;
    }

    public String toString() {
        return "TransactionId : " + mTransactionId + "\n" + "ResultState : " + mResultState + "\n"
                + "Code : " + mCode + "\n" + "ExtraCode : " + mExtraCode + "\n"
                + (getSscServiceData() != null ?  "\n" + getSscServiceData().toString() : "\n");
    }
}
