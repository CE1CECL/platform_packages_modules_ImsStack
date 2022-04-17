/*
    Author
    <table>
    date        author                  description
    --------    --------------          ----------
    20151115    hwangoo.park@           Created
    </table>

    Description
*/

package com.android.imsstack.internal.imsservice;

import com.android.imsstack.util.AppContext;
import com.android.imsstack.util.MessageExecutor;

/**
 * Google(Android) IMS Interface Registry.
 */
public final class GImsInterfaceRegistry {
    private static GImsInterfaceRegistry sGIIRegistry;

    private final GImsInterfaceServiceRegistry mServiceRegistry;

    private GImsInterfaceRegistry() {
        mServiceRegistry = new GImsInterfaceServiceRegistry(
                new MessageExecutor(AppContext.getMainLooper()));
    }

    public static GImsInterfaceRegistry getInstance() {
        if (sGIIRegistry == null) {
            sGIIRegistry = new GImsInterfaceRegistry();
        }

        return sGIIRegistry;
    }

    /** Google IMS service registry - starts */
    public GImsServiceRegistry getServiceRegistry() {
        return mServiceRegistry;
    }

    public boolean isImsOn() {
        return mServiceRegistry.isImsOn();
    }

    public boolean isImsOn(int phoneId) {
        return mServiceRegistry.isImsOn(phoneId);
    }

    public void addServiceStateListener(GImsServiceStateListener listener) {
        mServiceRegistry.addListener(listener);
    }

    public void removeServiceStateListener(GImsServiceStateListener listener) {
        mServiceRegistry.removeListener(listener);
    }
    /** Google IMS service registry - ends */
}
