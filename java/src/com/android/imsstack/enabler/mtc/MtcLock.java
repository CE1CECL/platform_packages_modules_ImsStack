
package com.android.imsstack.enabler.mtc;

import com.android.imsstack.util.ImsLog;

public class MtcLock {
    private final Object mLock = new Object();
    /** milli-seconds */
    private long mWaitTime = 1000L;
    private boolean mLocked = false;

    public MtcLock() {
    }

    public MtcLock(long waitTime) {
        mWaitTime = waitTime;
    }

    public boolean isLocked() {
        synchronized (mLock) {
            return mLocked;
        }
    }

    public void lock() {
        synchronized (mLock) {
            mLocked = true;
        }
    }

    public void unlock() {
        synchronized (mLock) {
            if (!mLocked) {
                return;
            }

            mLocked = false;
            mLock.notifyAll();
        }
    }

    public void waitForReady() {
        synchronized (mLock) {
            if (!mLocked) {
                return;
            }

            try {
                mLock.wait(mWaitTime);
            } catch (InterruptedException e) {
                ImsLog.d("[GII-MTC] MtcLock - Interrupted");
            }
        }
    }
}
