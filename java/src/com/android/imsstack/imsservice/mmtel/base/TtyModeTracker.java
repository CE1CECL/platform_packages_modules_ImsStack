/*
    Author
    <table>
    date        author                  description
    --------    --------------          ----------
    20150614    hwangoo.park@           Created
    </table>

    Description
*/

package com.android.imsstack.imsservice.mmtel.base;

import android.telecom.TelecomManager;

import com.android.imsstack.core.ImsGlobal;
import com.android.imsstack.util.ImsLog;

import java.util.Set;
import java.util.concurrent.CopyOnWriteArraySet;

public final class TtyModeTracker {
    /**
     * Listener to monitor the TTY mode changes
     */
    public static interface Listener {
        public void onTtyModeChanged(int newTtyMode);
    }

    private final Set<Listener> mListeners = new CopyOnWriteArraySet<Listener>();
    private int mTtyMode = TelecomManager.TTY_MODE_OFF;

    public TtyModeTracker() {
    }

    public TtyModeTracker(int ttyMode) {
        mTtyMode = ttyMode;
    }

    public void addListener(TtyModeTracker.Listener listener) {
        mListeners.add(listener);
    }

    public void removeListener(TtyModeTracker.Listener listener) {
        mListeners.remove(listener);
    }

    public int getTtyMode() {
        return mTtyMode;
    }

    public void setTtyMode(int ttyMode) {
        if (mTtyMode != ttyMode) {
            logi("setTtyMode :: " + mTtyMode + " >> " + ttyMode);
            mTtyMode = ttyMode;

            ImsGlobal.postAndRunTask(new Runnable() {
                @Override
                public void run() {
                    onTtyModeChanged();
                }
            });
        }
    }

    public static int getTtyModeOff() {
        return TelecomManager.TTY_MODE_OFF;
    }

    public static boolean isTtyModeOff(int ttyMode) {
        return (ttyMode == TelecomManager.TTY_MODE_OFF);
    }

    private void onTtyModeChanged() {
        for (Listener l : mListeners) {
            l.onTtyModeChanged(mTtyMode);
        }
    }

    private static void logi(String s) {
        ImsLog.i("[GII-IMPL] " + s);
    }
}
