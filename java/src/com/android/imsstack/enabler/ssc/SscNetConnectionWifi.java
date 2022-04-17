package com.android.imsstack.enabler.ssc;

import android.content.Context;

import com.android.imsstack.core.agents.dcmif.EApnType;
import com.android.imsstack.util.ImsLog;

public class SscNetConnectionWifi extends SscNetConnection {
    public SscNetConnectionWifi(int slotId) {
        super(slotId);
        ImsLog.w("");
    }

    @Override
    public void init(Context context, EApnType apnType) {
        super.init(context, apnType);
    }

    @Override
    public boolean isConnected() {
        ImsLog.d("");
        return true;
    }

    @Override
    public boolean connect() {
        ImsLog.d("");
        return true;
    }

    @Override
    public void disconnect() {
        ImsLog.d("");
        return;
    }
}
