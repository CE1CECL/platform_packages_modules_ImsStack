package com.android.imsstack.enabler.uce.impl.define;

import java.util.List;

public class UceOptionsReceivedData {
    private String mRemoteUri;
    private long mRemoteCaps;

    public UceOptionsReceivedData(String remoteUri, long remoteCaps) {
        mRemoteUri = remoteUri;
        mRemoteCaps = remoteCaps;
    }

    public String getRemoteUri() {
        return mRemoteUri;
    }

    public long getRemoteCaps() {
        return mRemoteCaps;
    }
}