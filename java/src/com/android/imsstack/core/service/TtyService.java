package com.android.imsstack.core.service;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.Settings;
import android.telecom.TelecomManager;

import com.android.imsstack.core.service.serviceif.IService;
import com.android.imsstack.core.service.serviceif.IVoLteService;
import com.android.imsstack.enabler.aos.AosFactory;
import com.android.imsstack.enabler.aos.IAosInfo;
import com.android.imsstack.system.ImsEventDef;
import com.android.imsstack.system.ISystem;
import com.android.imsstack.system.SystemInterface;
import com.android.imsstack.util.ImsLog;

public class TtyService implements IService {
    /** Internal events */
    private static final int EVENT_TTY_PREFERRED_MODE_CHANGED = 1001;
    private static final int EVENT_HEADSET_PLUG_STATE_CHANGED = 1002;

    private IVoLteService mVoLteService = null;
    private TtyReceiver mReceiver = null;
    private TtyHandler mHandler = null;
    private boolean mHeadsetPlugged = false;

    public TtyService() {
    }

    @Override
    public boolean start(IVoLteService voLteService) {
        mVoLteService = voLteService;

        ImsLog.i(getSlotId(), "");

        mReceiver = new TtyReceiver();

        Context c = getContext();

        if (c != null) {
            c.registerReceiver(mReceiver, mReceiver.getFilter(), Context.RECEIVER_EXPORTED);
        }

        mHandler = new TtyHandler();

        return true;
    }

    @Override
    public void cleanup(Context context) {
        ImsLog.i(getSlotId(), "");

        Context c = getContext();

        if (mReceiver != null && c != null) {
            c.unregisterReceiver(mReceiver);
            mReceiver = null;
        }

        if (mHandler != null) {
            mHandler.removeCallbacksAndMessages(null);
            mHandler = null;
        }
    }

    @Override
    public void update(Context context) {
    }

    private Context getContext() {
        return (mVoLteService != null) ? mVoLteService.getContext() : null;
    }

    private int getSlotId() {
        return (mVoLteService != null) ? mVoLteService.getSlotID() : 0;
    }

    private class TtyReceiver extends BroadcastReceiver {
        IntentFilter mIntentFilter = new IntentFilter();

        public TtyReceiver() {
            mIntentFilter.addAction(TelecomManager.ACTION_TTY_PREFERRED_MODE_CHANGED);
            mIntentFilter.addAction(Intent.ACTION_HEADSET_PLUG);
        }

        public IntentFilter getFilter() {
            return mIntentFilter;
        }

        @Override
        public synchronized void onReceive(Context context, Intent intent) {
            if (intent == null) {
                return;
            }

            if (mHandler == null) {
                return;
            }

            ImsLog.d(getSlotId(), toString(intent));

            String action = intent.getAction();

            if (TelecomManager.ACTION_TTY_PREFERRED_MODE_CHANGED.equals(action)) {
                Message.obtain(mHandler,
                        EVENT_TTY_PREFERRED_MODE_CHANGED, intent).sendToTarget();
            } else if (Intent.ACTION_HEADSET_PLUG.equals(action)) {
                Message.obtain(mHandler,
                        EVENT_HEADSET_PLUG_STATE_CHANGED, intent).sendToTarget();
            }
        }

        private String toString(Intent intent) {
            if (intent == null) {
                return "null";
            }

            StringBuilder sb = new StringBuilder();

            sb.append("intent=" + intent.getAction());
            sb.append(", extra=[");

            Bundle extras = intent.getExtras();

            if (extras != null) {
                for (String key : extras.keySet()) {
                    sb.append(key + "=" + extras.get(key) + " ");
                }
            }

            sb.append("]");

            return sb.toString();
        }
    }

    private class TtyHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            if (msg == null) {
                return;
            }

            ImsLog.i(getSlotId(), "TtyHandler :: msg=" + msg.what);

            Intent intent = (Intent)msg.obj;

            if (intent == null) {
                return;
            }

            switch (msg.what) {
                case EVENT_TTY_PREFERRED_MODE_CHANGED:
                    handleTtyPreferredModeChanged(intent);
                    break;

                case EVENT_HEADSET_PLUG_STATE_CHANGED:
                    handleHeadsetPlugStateChanged(intent);
                    break;

                default:
                    break;
            }
        }

        private void handleTtyPreferredModeChanged(Intent intent) {
            int ttyMode = intent.getIntExtra(TelecomManager.EXTRA_TTY_PREFERRED_MODE,
                    TelecomManager.TTY_MODE_OFF);

            ImsLog.w(getSlotId(), "mode=" + ttyMode + ", headset-plugged=" + mHeadsetPlugged);

            //Check Headset Plugged
            if (mHeadsetPlugged) {
                IAosInfo aosInfo = AosFactory.getInstance().getAosInfo(getSlotId());
                if (aosInfo != null) {
                    boolean isOn = (ttyMode == TelecomManager.TTY_MODE_OFF) ? false : true;
                    aosInfo.notifyTtySetting(isOn);
                }
            }
        }

        private void handleHeadsetPlugStateChanged(Intent intent) {
            mHeadsetPlugged = (intent.getIntExtra("state", 0) == 1);

            Context c = getContext();

            if (c == null) {
                return;
            }

            int ttyMode = Settings.Secure.getInt(c.getContentResolver(),
                    Settings.Secure.PREFERRED_TTY_MODE, TelecomManager.TTY_MODE_OFF);

            ImsLog.w(getSlotId(), "mode=" + ttyMode + ", headset-plugged=" + mHeadsetPlugged);

            // Check TTY mode (0:OFF, else:ON)
            if (ttyMode != TelecomManager.TTY_MODE_OFF) {
                boolean isOn = false;
                if (mHeadsetPlugged) {
                    isOn = true;
                }

                IAosInfo aosInfo = AosFactory.getInstance().getAosInfo(getSlotId());
                if (aosInfo != null) {
                    aosInfo.notifyTtySetting(isOn);
                }
            }
        }
    }
}
