package com.android.imsstack.util;

import android.content.Context;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.telephony.TelephonyManager;

import java.util.concurrent.Executor;

/**
 * This class holds the application context.
 */
public final class AppContext {
    private static Context sContext;
    private static Handler sMainHandler;
    private static HandlerThread sMainHandlerThread =
            new HandlerThread(AppContext.class.getSimpleName());
    private static MessageExecutor sMainExecutor;

    public static Context get() {
        return sContext;
    }

    public static Executor getMainExecutor() {
        return sMainExecutor;
    }

    public static Handler getMainHandler() {
        return sMainHandler;
    }

    public static Looper getMainLooper() {
        return sMainHandlerThread.getLooper();
    }

    /**
     * This method is invoked when IMS application is created.
     */
    public static void init(Context context) {
        sContext = context;

        sMainHandlerThread.start();
        sMainHandler = new Handler(sMainHandlerThread.getLooper());
        sMainExecutor = new MessageExecutor(sMainHandlerThread.getLooper());
    }

    public static String getPackageName() {
        Context c = get();
        return (c != null) ? c.getPackageName() : ImsConstants.PACKAGE_NAME;
    }

    public static Object getSystemService(String name) {
        Context c = get();
        return (c != null) ? c.getSystemService(name) : null;
    }

    public static <T> T getSystemService(Class<T> serviceClass) {
        Context c = get();
        return (c != null) ? c.getSystemService(serviceClass) : null;
    }

    public static TelephonyManager getTelephonyManager() {
        return getSystemService(TelephonyManager.class);
    }

    public static TelephonyManager getTelephonyManager(int subId) {
        TelephonyManager tm = getTelephonyManager();
        return (tm != null) ? tm.createForSubscriptionId(subId) : null;
    }

    public static void runTask(Runnable task, long millis) {
        if (task == null) {
            return;
        }

        Handler h = getMainHandler();

        if (h != null) {
            if (millis > 0) {
                h.postDelayed(task, millis);
            } else {
                h.post(task);
            }
        } else {
            task.run();
        }
    }
}
