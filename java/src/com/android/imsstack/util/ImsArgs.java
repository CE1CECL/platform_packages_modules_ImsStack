package com.android.imsstack.util;

public final class ImsArgs {
    public Object mArg1;
    public Object mArg2;
    public Object mArg3;
    public int mIntArg1;
    public int mIntArg2;
    public int mIntArg3;
    public long mLongArg;

    private static final int MAX_POOL_SIZE = 5;
    private static ImsArgs sPool;
    private static int sPoolSize;
    private static Object sPoolLock = new Object();

    private ImsArgs mNext;
    private boolean mInPool;

    private ImsArgs() {
    }

    public static ImsArgs obtain() {
        synchronized (sPoolLock) {
            if (sPoolSize > 0) {
                ImsArgs args = sPool;
                sPool = sPool.mNext;
                args.mNext = null;
                args.mInPool = false;
                sPoolSize--;
                return args;
            } else {
                return new ImsArgs();
            }
        }
    }

    public static ImsArgs obtain(int arg1, int arg2, int arg3) {
        ImsArgs args = obtain();

        args.mIntArg1 = arg1;
        args.mIntArg2 = arg2;
        args.mIntArg3 = arg3;

        return args;
    }

    public static ImsArgs obtain(Object arg1, Object arg2, Object arg3) {
        ImsArgs args = obtain();

        args.mArg1 = arg1;
        args.mArg2 = arg2;
        args.mArg3 = arg3;

        return args;
    }

    public static ImsArgs obtain(Object arg1, Object arg2, Object arg3,
            int intArg1, int intArg2, int intArg3) {
        ImsArgs args = obtain();

        args.mArg1 = arg1;
        args.mArg2 = arg2;
        args.mArg3 = arg3;

        args.mIntArg1 = intArg1;
        args.mIntArg2 = intArg2;
        args.mIntArg3 = intArg3;

        return args;
    }

    public void recycle() {
        if (mInPool) {
            return;
        }

        synchronized (sPoolLock) {
            clear();

            if (sPoolSize < MAX_POOL_SIZE) {
                mNext = sPool;
                mInPool = true;
                sPool = this;
                sPoolSize++;
            }
        }
    }

    public void clear() {
        mArg1 = null;
        mArg2 = null;
        mArg3 = null;
        mIntArg1 = 0;
        mIntArg2 = 0;
        mIntArg3 = 0;
        mLongArg = 0;
    }
}
