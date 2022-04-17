
package com.android.imsstack.jni;

import android.os.Parcel;

import com.android.imsstack.util.ImsLog;

import java.io.FileDescriptor;
import java.util.Hashtable;

public class JNIIms {

    private static Hashtable<Long, JNIImsListener> sListeners
            = new Hashtable<Long, JNIImsListener>();
    private static Hashtable<Long, JNIImsListenerEx> sListenerExs
            = new Hashtable<Long, JNIImsListenerEx>();

    // =========================================================================
    // from JAVA to Native
    // =========================================================================
    public static native void construct();

    public static native void destruct();

    public static native long getInterface(int interfaceType, int slotId);

    public static native int releaseInterface(long nativeObj);

    public static native int sendData(long nativeObj, byte[] baData);

    public static native byte[] sendDataEx(long nativeObj, byte[] baData);

    public static native int setConfiguration(int event, byte[] baData);

    // =========================================================================
    // For UI Interface Object
    // =========================================================================
    public static int setListener(long nativeObj, JNIImsListener listener) {
        if (nativeObj == 0) {
            return 0;
        }

        if (listener == null) {
            return 0;
        }

        Long key = Long.valueOf(nativeObj);

        synchronized (sListeners) {
            sListeners.put(key, listener);
        }

        return 1;
    }

    public static JNIImsListener getListener(long nativeObj) {
        if (nativeObj == 0) {
            return null;
        }

        Long key = Long.valueOf(nativeObj);

        JNIImsListener listener = null;

        synchronized (sListeners) {
            listener = sListeners.get(key);
        }

        return listener;
    }

    public static int removeListener(long nativeObj, JNIImsListener listener) {
        if (nativeObj == 0) {
            return 0;
        }

        if (listener == null) {
            return 0;
        }

        Long key = Long.valueOf(nativeObj);

        synchronized (sListeners) {
            sListeners.remove(key);
        }

        return 1;
    }

    // =========================================================================
    // For System Interface object
    // =========================================================================
    public static int setListenerEx(long nativeObj, JNIImsListenerEx listenerEx) {
        if (nativeObj == 0) {
            return 0;
        }

        if (listenerEx == null) {
            return 0;
        }

        Long key = Long.valueOf(nativeObj);

        synchronized (sListenerExs) {
            sListenerExs.put(key, listenerEx);
        }

        return 1;
    }

    public static JNIImsListenerEx getListenerEx(long nativeObj) {
        if (nativeObj == 0) {
            return null;
        }

        Long key = Long.valueOf(nativeObj);

        JNIImsListenerEx listener = null;

        synchronized (sListenerExs) {
            listener = sListenerExs.get(key);
        }

        return listener;
    }

    public static int removeListenerEx(long nativeObj, JNIImsListenerEx listenerEx) {
        if (nativeObj == 0) {
            return 0;
        }

        Long key = Long.valueOf(nativeObj);

        synchronized (sListenerExs) {
            sListenerExs.remove(key);
        }

        return 1;
    }

    // =========================================================================
    // from Native to JAVA for UI Interface object
    // =========================================================================
    public static int sendData2Java(long nativeObj, byte[] baData) {

        JNIImsListener listener = getListener(nativeObj);

        if (listener == null) {
            ImsLog.d("No listener :: nativeObject=" + nativeObj);
            return -1;
        }

        // retrieve parcel object from pool
        Parcel parcel = Parcel.obtain();
        parcel.unmarshall(baData, 0, baData.length);
        parcel.setDataPosition(0);

        listener.onMessage(parcel);

        // put parcel object back into the pool
        parcel.recycle();

        return 1;
    }

    // =========================================================================
    // from Native to JAVA for System Interface object
    // =========================================================================
    public static byte[] sendData2JavaEx(long nativeObj, byte[] baData, FileDescriptor fd) {

        JNIImsListenerEx listener = getListenerEx(nativeObj);

        if (listener == null) {
            ImsLog.d("No listener :: nativeObject=" + nativeObj);
            return new byte[] {(byte)0};
        }

        // retrieve parcel object from pool
        Parcel parcel = Parcel.obtain();
        parcel.unmarshall(baData, 0, baData.length);
        parcel.setDataPosition(0);

        byte result[] = listener.onMessage(parcel, fd);

        // put parcel object back into the pool
        parcel.recycle();

        return result;
    }

    static {
        try {
            ImsLog.i("Loading library... start");
            System.loadLibrary("imsstack");
            ImsLog.i("Loading library... end");
        } catch (UnsatisfiedLinkError e) {
            ImsLog.e("Loading library failed: libimsstack");
            e.printStackTrace();
        }
    }
}
