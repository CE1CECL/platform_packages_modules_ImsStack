package com.android.imsstack.core.agents.agentif;

import android.os.Handler;

public interface ITRM extends IAgent {
    /*
      Check if service is available or not
    */
    boolean isServiceAvailable(int slotId, int type);

    /*
      check if slot is available or not
    */
    boolean isSlotAvailable(int slotId);

    /*
      Check if TRM is supported or not
    */
    boolean isTRMSupported();

    /*
      Set the service start or end state
    */
    void setService(int slotId, int type, int mode);

    /*
        register handler to get the change of the service priority
    */
    void registerForServicePriorityChanged(Handler h, int what, Object obj);

    /*
        unregister handler to get the change of the service priority
    */
    void unregisterForServicePriorityChanged(Handler h);
}
