/*
    Author
    <table>
    date        author                  description
    --------    --------------          ----------
    20141026    hwangoo.park@           Created
    </table>

    Description
*/

package com.android.imsstack.imsservice.mmtel;

import android.telephony.ims.stub.ImsEcbmImplBase;

import com.android.imsstack.enabler.mtc.EcbmListener;
import com.android.imsstack.enabler.mtc.IECallStateTracker;
import com.android.imsstack.imsservice.mmtel.base.ICallContext;
import com.android.imsstack.util.ImsLog;

public final class ImsEcbmImpl extends ImsEcbmImplBase {
    private final ICallContext mCallContext;
    private final EcbmListenerProxy mListenerProxy = new EcbmListenerProxy();

    public ImsEcbmImpl(ICallContext callContext) {
        mCallContext = callContext;

        init();
    }

    public void dispose() {
        logi("dispose");

        clear();
    }

    public void init() {
        IECallStateTracker ecst = mCallContext.getECallStateTracker();

        if (ecst != null) {
            ecst.addEcbmListener(mListenerProxy);
        }
    }

    public void clear() {
        IECallStateTracker ecst = mCallContext.getECallStateTracker();

        if (ecst != null) {
            ecst.removeEcbmListener(mListenerProxy);
        }
    }

    @Override
    public void exitEmergencyCallbackMode() {
        logi("exitEmergencyCallbackMode");

        IECallStateTracker ecst = mCallContext.getECallStateTracker();

        if (ecst != null) {
            ecst.exitEmergencyCallbackMode(false);
        }
    }

    private void loge(Throwable t, String message) {
        ImsLog.e("[GII-IMPL] " + message + t, t);
    }

    private static void logi(String s) {
        ImsLog.i("[GII-IMPL] " + s);
    }

    private class EcbmListenerProxy implements EcbmListener {

        public EcbmListenerProxy() {
        }

        /**
         * This is invoked if emergency callback mode is entered.
         */
        @Override
        public void onEcbmEntered() {
            logi("onEcbmEntered");

            try {
                enteredEcbm();
            } catch (Throwable t) {
                loge(t, "enteredEcbm");
            }
        }

        /**
         * This is invoked if emergency callback mode is exited.
         */
        @Override
        public void onEcbmExited() {
            logi("onEcbmExited");

            try {
                exitedEcbm();
            } catch (Throwable t) {
                loge(t, "exitedEcbm");
            }
        }
    }

    // @Override
    // @PRIVATE_IMS_API
    public void exitEmergencyCallbackModeByESMS(boolean bExitedByESMS) {
        logi("exitEmergencyCallbackMode by ESMS, bExitedByESMS=" + bExitedByESMS);

        IECallStateTracker ecst = mCallContext.getECallStateTracker();

        if (ecst != null) {
            ecst.exitEmergencyCallbackMode(bExitedByESMS);
        }
    }
}
