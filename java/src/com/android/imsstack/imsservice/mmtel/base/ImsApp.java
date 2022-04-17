/*
    Author
    <table>
    date        author                  description
    --------    --------------          ----------
    20131015    hwangoo.park@           Created
    </table>

    Description
*/

package com.android.imsstack.imsservice.mmtel.base;

import com.android.imsstack.util.ImsLog;

public abstract class ImsApp {
    private final int mPhoneId;

    public ImsApp(int phoneId) {
        mPhoneId = phoneId;
    }

    public abstract void close();

    public final int getPhoneId() {
        return mPhoneId;
    }

    /**
     * Check if the specified service and call type is connected (registered).
     *
     * @param serviceType ImsCallProfile#SERVICE_TYPE_XXX
     * @param serviceType ImsCallProfile#CALL_TYPE_XXX
     */
    public boolean isConnected(int serviceType, int callType) {
        return false;
    }

    public void onBinderDied() {
        // no-op
    }

    protected static void log(String s) {
        ImsLog.d("[GII-IMPL] " + s);
    }

    protected static void logi(String s) {
        ImsLog.i("[GII-IMPL] " + s);
    }
}
