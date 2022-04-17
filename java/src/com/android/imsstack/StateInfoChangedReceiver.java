package com.android.imsstack;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.telephony.CarrierConfigManager;
import android.telephony.TelephonyManager;

import com.android.imsstack.util.AppContext;
import com.android.imsstack.util.ImsConstants;
import com.android.imsstack.util.Log;
import com.android.imsstack.util.MSimUtils;
import com.android.internal.telephony.TelephonyIntents;

public class StateInfoChangedReceiver {
    private final IntentReceiver mIntentReceiver = new IntentReceiver();
    private IStateInfoChangedObserver mObserver;

    public StateInfoChangedReceiver() {
    }

    public void init(Context context, IStateInfoChangedObserver observer) {
        mObserver = observer;

        mIntentReceiver.register(context);
    }

    private final class IntentReceiver extends BroadcastReceiver {
        public void register(Context c) {
            IntentFilter filter = new IntentFilter();
            filter.addAction(TelephonyIntents.ACTION_SIM_STATE_CHANGED);

            if (MSimUtils.isMultiSimEnabled()) {
                filter.addAction(TelephonyManager.ACTION_DEFAULT_DATA_SUBSCRIPTION_CHANGED);
            }

            filter.addAction(CarrierConfigManager.ACTION_CARRIER_CONFIG_CHANGED);

            c.registerReceiver(this, filter, null,
                    AppContext.getMainHandler(), Context.RECEIVER_EXPORTED);
        }

        @Override
        public void onReceive(Context context, Intent intent) {
            Log.d(Log.TAG, "Received : " + intent);

            if (mObserver == null) {
                return;
            }

            String action = intent.getAction();

            if (TelephonyIntents.ACTION_SIM_STATE_CHANGED.equals(action)) {
                mObserver.notifyStateInfoChanged(intent);
            } else if (TelephonyManager.ACTION_DEFAULT_DATA_SUBSCRIPTION_CHANGED.equals(action)) {
                mObserver.notifyStateInfoChanged(intent);
            } else if (CarrierConfigManager.ACTION_CARRIER_CONFIG_CHANGED.equals(action)) {
                mObserver.notifyStateInfoChanged(intent);
            } else {
                Log.w(Log.TAG, "Unexpected Intent : " + intent);
            }
        }
    }
}
