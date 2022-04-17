package com.android.imsstack.enabler;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;

import java.util.concurrent.Executor;

public interface IContext {
    /**
     * Utilities for this context.
     */
    public Context getContext();
    public Executor getExecutor();

    public Handler getDefaultHandler();
    public Looper getDefaultLooper();

    /**
     * Returns the phone id of this context.
     */
    public int getPhoneId();

    /**
     * Returns the slot-id of this context.
     */
    public int getSlotId();

    /**
     * Returns the subscription id of this context.
     */
    public int getSubId();
}
