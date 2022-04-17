package com.android.imsstack.enabler.uce.impl.define;

public class UceResponseData {
    private int mResponseCode;
    private String mReason;
    private int mReasonHeaderCause;
    private String mReasonHeaderText;
    private String mEtag;
    private long mCapability;
    private int mNeedToRetry;

    public UceResponseData(int responseCode, String reason) {
        mResponseCode = responseCode;
        mReason = reason;
        mReasonHeaderCause = 0;
        mReasonHeaderText = "";
        mEtag = "";
        mCapability = 0;
        mNeedToRetry = 0;
    }

    public void setReasonHeaderCause(int reasonHeaderCause) {
        mReasonHeaderCause = reasonHeaderCause;
    }

    public void setReasonHeaderText(String reasonHeaderText) {
        mReasonHeaderText = reasonHeaderText;
    }

    public void setEtag(String etag) {
        mEtag = etag;
    }

    public void setCapability(long capability) {
        mCapability = capability;
    }

    public void setNeedToRetry(int needToRetry) {
        mNeedToRetry = needToRetry;
    }

    public int getResponseCode() {
        return mResponseCode;
    }

    public String getReason() {
        return mReason;
    }

    public int getReasonHeaderCause() {
        return mReasonHeaderCause;
    }

    public String getReasonHeaderText() {
        return mReasonHeaderText;
    }

    public String getEtag() {
        return mEtag;
    }

    public long getCapability() {
        return mCapability;
    }

    public int getNeedToRetry() {
        return mNeedToRetry;
    }
}