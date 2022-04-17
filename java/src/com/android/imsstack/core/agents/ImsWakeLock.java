
package com.android.imsstack.core.agents;

import android.os.PowerManager;

import com.android.imsstack.util.ImsLog;

import java.util.HashSet;

public class ImsWakeLock {
    private String mTag = ImsWakeLock.class.getSimpleName();
    private PowerManager mPM;
    private PowerManager.WakeLock mWakeLock;
    private HashSet<Object> mHolders = new HashSet<Object>();

    public ImsWakeLock(PowerManager pm) {
        mPM = pm;
    }

    public ImsWakeLock(PowerManager pm, String tag) {
        mPM = pm;
        mTag = tag;
    }

    public synchronized void clear() {
        ImsLog.i(mTag + " - clear :: refCount=" + mHolders.size());

        mHolders.clear();
        release(null);
    }

    public synchronized void acquire(Object holder) {
        mHolders.add(holder);

        if (mWakeLock == null) {
            mWakeLock = mPM.newWakeLock(
                    PowerManager.PARTIAL_WAKE_LOCK,
                    mTag);
        }

        if (!mWakeLock.isHeld()) {
            mWakeLock.acquire();
        }

        if (ImsLog.isDebuggable()) {
            ImsLog.d(mTag + " - acquire :: refCount=" + mHolders.size());
        }
    }

    public synchronized void release(Object holder) {
        mHolders.remove(holder);

        if ((mWakeLock != null) && mHolders.isEmpty() && mWakeLock.isHeld()) {
            mWakeLock.release();
        }

        if (ImsLog.isDebuggable()) {
            ImsLog.d(mTag + " - release :: refCount=" + mHolders.size());
        }
    }
}
