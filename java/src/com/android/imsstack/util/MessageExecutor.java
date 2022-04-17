
package com.android.imsstack.util;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;

import java.util.concurrent.Executor;

/**
 * Executes the tasks in the other thread rather than the calling thread.
 */
public class MessageExecutor extends Handler implements Executor {
    public MessageExecutor() {
        super();
    }

    public MessageExecutor(String name) {
        super(createLooper(name));
    }

    public MessageExecutor(Looper looper) {
        super(looper);
    }

    @Override
    public void execute(Runnable r) {
        Message m = Message.obtain(this, 0 /* don't care */, r);
        m.sendToTarget();
    }

    @Override
    public void handleMessage(Message msg) {
        if (msg.obj instanceof Runnable) {
            executeInternal((Runnable)msg.obj);
        } else {
            log("handleMessage :: Not runnable object; ignore the msg=" + msg);
        }
    }

    private void executeInternal(Runnable r) {
        try {
            r.run();
        } catch (Throwable t) {
            loge("executeInternal :: run task=" + r);
            t.printStackTrace();
        } finally {
        }
    }

    private static void log(String s) {
        Log.d(Log.TAG, "[MessageExecutor] " + s);
    }

    private static void loge(String s) {
        Log.e(Log.TAG, "[MessageExecutor] " + s);
    }

    private static Looper createLooper(String name) {
        HandlerThread thread = new HandlerThread(name);
        thread.start();

        Looper looper = thread.getLooper();

        if (looper == null) {
            return Looper.getMainLooper();
        }

        return looper;
    }
}
