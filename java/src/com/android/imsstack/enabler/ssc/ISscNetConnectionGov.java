package com.android.imsstack.enabler.ssc;

import android.content.Context;
import android.os.Handler;

import com.android.imsstack.core.agents.dcmif.EApnType;

import java.io.InputStream;

public interface ISscNetConnectionGov {
    public void init(int slotId, Context context, EApnType apnType, boolean bWiFi);
    public ISscNetConnection get(int slotId);

    public void init(int slotId, Context context, EApnType apnType);
    public void cleanup(int slotId, Context recentCnx);
    public boolean isConnected(int slotId);
    public boolean connect(int slotId);
    public void disconnect(int slotId);
    public void setTransactionHandler(int slotId, Handler handler);
    public void refreshConnectionTimer(int slotId);
}
