package com.android.imsstack.internal.imsservice;

import android.os.Handler;

import com.android.imsstack.enabler.mtc.CallStateListener;
import com.android.imsstack.enabler.mtc.ICallStateTracker;
import com.android.imsstack.enabler.mtc.IServiceStateTracker;

public class CallUtils {
    public static ICallStateTracker getCallStateTracker(int slotId) {
        GImsServiceRegistry gisr = GImsInterfaceRegistry.getInstance().getServiceRegistry();
        return gisr.getCallStateTracker(slotId);
    }

    public static IServiceStateTracker getServiceStateTracker(int slotId) {
        GImsServiceRegistry gisr = GImsInterfaceRegistry.getInstance().getServiceRegistry();
        return gisr.getServiceStateTracker(slotId);
    }

    public static void addCallStateListener(int slotId, CallStateListener listener) {
        GImsServiceRegistry gisr = GImsInterfaceRegistry.getInstance().getServiceRegistry();
        gisr.addCallStateListener(slotId, listener);
    }

    public static void removeCallStateListener(int slotId, CallStateListener listener) {
        GImsServiceRegistry gisr = GImsInterfaceRegistry.getInstance().getServiceRegistry();
        gisr.removeCallStateListener(slotId, listener);
    }

    public static void addEmergencyServiceStateListener(int slotId,
            Handler h, int what, Object obj) {
        GImsServiceRegistry gisr = GImsInterfaceRegistry.getInstance().getServiceRegistry();
        gisr.registerForEmergencyServiceStateChanged(slotId, h, what, obj);
    }

    public static void removeEmergencyServiceStateListener(int slotId, Handler h) {
        GImsServiceRegistry gisr = GImsInterfaceRegistry.getInstance().getServiceRegistry();
        gisr.unregisterForEmergencyServiceStateChanged(slotId, h);
    }

    public static void addServiceStateListener(int slotId,
            Handler h, int what, Object obj) {
        GImsServiceRegistry gisr = GImsInterfaceRegistry.getInstance().getServiceRegistry();
        gisr.registerForServiceStateChanged(slotId, h, what, obj);
    }

    public static void removeServiceStateListener(int slotId, Handler h) {
        GImsServiceRegistry gisr = GImsInterfaceRegistry.getInstance().getServiceRegistry();
        gisr.unregisterForServiceStateChanged(slotId, h);
    }
}
