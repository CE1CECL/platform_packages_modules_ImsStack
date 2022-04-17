package com.android.imsstack.enabler.ssc;

import android.content.Context;
import android.os.Handler;

import com.android.imsstack.core.agents.dcmif.EApnType;

import org.w3c.dom.Document;

public interface ISscHttpConnectionGov {
    public void open(int slotId, Context context, EApnType apntype);
    public ISscHttpConnection get(int slotId);

    public void close(int slotId);
    public int  sendRequest(int slotId, int requestType, String requestURI, String body);
    public void setCredentialOnChallenge(int slotId, String body);
    public void setTransactionHandler(int slotId, Handler handler);
    public void setXuiValue(int slotId, String xui);
    public Document getInputStream(int slotId);
}
