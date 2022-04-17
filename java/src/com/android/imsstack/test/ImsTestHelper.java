package com.android.imsstack.test;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.telephony.TelephonyManager;

import com.android.imsstack.system.ISystem;
import com.android.imsstack.system.SystemInterface;
import com.android.imsstack.util.AppContext;
import com.android.imsstack.util.ImsLog;
import com.android.imsstack.util.MSimUtils;

/**
 * IMS Test Helper
 */
public final class ImsTestHelper {
    private static ImsTestHelper sImsTestHelper = null;

    // VoNR TEST INTENT
    // extra parameter : VoNR response
    // event 1/2/3 = call ready / handoff / uac response
    // type 1/2/4/8 = voice / video / sms / emergency
    // mode 1/2 = LTE / 5GNR
    // result 1 / 2 = SUCCESS / ACCESS_BARRED
    // ex) adb shell am broadcast -a "com.android.imsstack.action.IMS_TEST_VONR"
    //     --ei "event" 1 --ei "type" 1 --ei "mode" 1 --ei "result" 1 -- ei "time" 0
    // notifyUacResponse(EVENT_UAC_RESPONSE, callType, sysMode, result, barringTime);
    private static final String INTENT_VONR = "com.android.imsstack.action.IMS_TEST_VONR";

    private Context mContext;
    private ImsTestHelperReceiver mReceiver;

    public ImsTestHelper () {
        ImsLog.d("ImsTestHelper is loaded");

        mContext = AppContext.get();
        if (mContext == null) {
            return;
        }
        mReceiver = new ImsTestHelperReceiver();
        mContext.registerReceiver(mReceiver, mReceiver.getFilter(), Context.RECEIVER_EXPORTED);
    }

    public static ImsTestHelper getInstance() {

        if ( sImsTestHelper == null ) {
            sImsTestHelper = new ImsTestHelper();
        }
        return sImsTestHelper;
    }

    public void cleanup() {
        ImsLog.d("cleanup()");
    }

    private class ImsTestHelperReceiver extends BroadcastReceiver {
        IntentFilter mIntentFilter = new IntentFilter();

        public ImsTestHelperReceiver() {
            mIntentFilter.addAction(INTENT_VONR);
        }

        public IntentFilter getFilter() {
            return mIntentFilter;
        }

        @Override
        public synchronized void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            ImsLog.d(ImsLog.lastSubString(action, "."));

            if (action.equals(INTENT_VONR)) {
                sendVoNREvent(intent.getIntExtra("event", -1), intent.getIntExtra("type", -1),
                        intent.getIntExtra("mode", -1), intent.getIntExtra("result", -1),
                        intent.getIntExtra("time", -1), intent.getIntExtra("slot", 0));
            }
        }

        //VoNR
        private void sendVoNREvent(int event, int type, int mode, int result, int time,
                int slotId) {

            int nModeLte = 1; // 5GNR = 2

            ISystem iSystem = SystemInterface.getInstance().getSystem(slotId);
            if (iSystem == null) {
                return;
            }

            ImsLog.d("sendVoNREvent - event[" + event + "] type[" + type + "] mode[" + mode
                    + "] result[" + result + "] time[" + time + "]");
            switch (event) {
                case 1: // EVENT_CALL_READY
                    int nSysModeLte = 0x8;
                    int nSysModeNr = 0x0C;
                    iSystem.notifyCallReady(event, mode == nModeLte ? nSysModeLte : nSysModeNr);
                    break;
                case 2: // EVENT_HANDOFF_INFORMATION
                    int nHoRatLte = 0x03;
                    int nHoRatNr = 0x06;
                    int sRat = nHoRatLte;
                    int tRat = nHoRatNr;

                    if (mode == nModeLte) {
                        sRat = nHoRatNr;
                        tRat = nHoRatLte;
                    }
                    iSystem.notifyHandoffInformation(event, 1, sRat, tRat, 0, 0);
                    break;
                case 3: //EVENT_UAC_RESPONSE

                    int nNasRadioIfLte = 0x8;
                    int nNasRadioIfNr5g = 0x0C;
                    int sysMode = mode == nModeLte ? nNasRadioIfLte : nNasRadioIfNr5g;
                    iSystem.notifyUacResponse(event, type, sysMode, result, time);
                    break;
                default:
                    break;
            }
        }
    }
}
