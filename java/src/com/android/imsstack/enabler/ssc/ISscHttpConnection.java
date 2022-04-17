package com.android.imsstack.enabler.ssc;

import android.os.Handler;

import org.w3c.dom.Document;

public interface ISscHttpConnection {
    public void close();
    public int  sendRequest(int requestType, String requestURI, String body);
    public void setCredentialOnChallenge(String body);
    public void setTransactionHandler(Handler handler);
    public void setXuiValue(String xui);
    public Document getInputStream();
}
