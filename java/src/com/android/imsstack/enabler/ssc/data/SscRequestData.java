package com.android.imsstack.enabler.ssc.data;

import java.util.ArrayDeque;

public class SscRequestData {
    private int mTransactionId = 0;
    private int mRetryCount = 0;
    private int mPreconditionFailedCount = 0;
    private ArrayDeque<SscData> mSscDatas = new ArrayDeque<>();

    public SscRequestData(int transactionId) {
        this.mTransactionId = transactionId;
    }

    public int getTransactionId() {
        return mTransactionId;
    }

    public void increaseRetryCount() {
        mRetryCount++;
    }

    public int getRetryCount() {
        return mRetryCount;
    }

    public void increasePreconditionFailedCount() {
        mPreconditionFailedCount++;
    }

    public int getPreconditionFailedCount() {
        return mPreconditionFailedCount;
    }

    public void offerSscDataFirst(SscData SscData) {
        mSscDatas.offerFirst(SscData);
    }

    public void offerSscData(SscData SscData) {
        mSscDatas.offerLast(SscData);
    }

    public SscData peakSscData() {
        return mSscDatas.peekFirst();
    }

    public SscData pollSscData() {
        return mSscDatas.pollFirst();
    }
}
