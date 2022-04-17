package com.android.imsstack.enabler.ssc;

import android.content.Context;
import android.os.Handler;

import com.android.imsstack.core.agents.dcmif.EApnType;

import java.io.InputStream;

public interface ISscNetConnection {
    public void init(Context context, EApnType apnType);
    public void cleanup(Context recentCnx);
    public boolean isConnected();
    public boolean connect();
    public boolean isPDNAvailable();
    public void disconnect();
    public void setTransactionHandler(Handler handler);
    public void refreshConnectionTimer();
}
