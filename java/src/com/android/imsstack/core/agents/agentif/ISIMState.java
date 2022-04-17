package com.android.imsstack.core.agents.agentif;

import android.os.Handler;

public interface ISIMState extends IAgent {
    void requestPSIInfo();
    void setPSIValueToDB(String psi);
    String getIccState();
    String getIsimState();
    String getSMSPAddress();
    boolean isIccLoaded();
    boolean isIsimLoaded();
    void registerForSimStateChanged(Handler h, int what, Object obj);
    void unregisterForSimStateChanged(Handler h);
    void registerForIsimStateChanged(Handler h, int what, Object obj);
    void unregisterForIsimStateChanged(Handler h);
}
