package com.android.imsstack.enabler.uce.impl.subscribe;

public class UceResourceInfo {
    private String mId;
    private String mReason;

    public UceResourceInfo(String id, String reason) {
        mId = id;
        mReason = reason;
    }

    public String getId() {
        return mId;
    }

    public String getReason() {
        return mReason;
    }
}
