/*
    Author
    <table>
    date        author                  description
    --------    --------------          ----------
    20151115    hwangoo.park@           Created
    </table>

    Description
*/

package com.android.imsstack.internal.imsservice;

import android.os.Handler;

import com.android.imsstack.enabler.mtc.Call;
import com.android.imsstack.enabler.mtc.CallStateListener;
import com.android.imsstack.enabler.mtc.ICallStateTracker;
import com.android.imsstack.enabler.mtc.IServiceStateTracker;
import com.android.imsstack.util.AppContext;
import com.android.imsstack.util.ImsLog;
import com.android.imsstack.util.MSimUtils;
import com.android.imsstack.util.SettingsUtils;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.CopyOnWriteArraySet;
import java.util.concurrent.Executor;

/**
 * Implementation for Google(Android) IMS service registry.
 */
public class GImsInterfaceServiceRegistry implements GImsServiceRegistry {
    private static final int MAX_PHONE = MSimUtils.getPhoneCount();

    private final Object mLock = new Object();
    private final Set<GImsServiceStateListener> mListeners
                = new CopyOnWriteArraySet<GImsServiceStateListener>();
    private final Executor mExecutor;
    private final ServiceRegistry[] mServiceRegistry = new ServiceRegistry[MAX_PHONE];
    private final CallRegistry[] mCallRegistry = new CallRegistry[MAX_PHONE];

    public GImsInterfaceServiceRegistry(Executor executor) {
        mExecutor = executor;

        for (int i = 0; i < MAX_PHONE; ++i) {
            mServiceRegistry[i] = new ServiceRegistry(i);
        }

        for (int i = 0; i < MAX_PHONE; ++i) {
            mCallRegistry[i] = new CallRegistry();
        }
    }

    public void addListener(GImsServiceStateListener listener) {
        if (mListeners.add(listener)) {
            for (int i = 0; i < MAX_PHONE; ++i) {
                if (isImsOn(i)) {
                    notifyServiceOn(listener, i);
                } else {
                    notifyServiceOff(listener, i);
                }
            }
        }
    }

    public void removeListener(GImsServiceStateListener listener) {
        mListeners.remove(listener);
    }

    public boolean isImsOn() {
        return isImsOn(MSimUtils.DEFAULT_PHONE_ID);
    }

    public void setCallStateTracker(int phoneId, ICallStateTracker csTracker) {
        CallRegistry cr = getCallRegistry(phoneId);

        if (cr != null) {
            cr.setCallStateTracker(csTracker);
        }
    }

    public void setServiceStateTracker(int phoneId, IServiceStateTracker ssTracker) {
        ServiceRegistry sr = getServiceRegistry(phoneId);

        if (sr != null) {
            sr.setServiceStateTracker(ssTracker);
        }
    }

    /** Method Overrides */
    @Override
    public boolean isImsOn(int phoneId) {
        ServiceRegistry sr = getServiceRegistry(phoneId);
        return (sr != null) ? sr.isImsOn() : false;
    }

    @Override
    public void turnOnIms(int phoneId) {
        ServiceRegistry sr = getServiceRegistry(phoneId);

        if (sr != null) {
            if (sr.isImsOn()) {
                // no-op
                return;
            }

            sr.setImsOn(true);
        }

        notifyServiceOn(phoneId);
    }

    @Override
    public void turnOffIms(int phoneId) {
        ServiceRegistry sr = getServiceRegistry(phoneId);

        if (sr != null) {
            if (!sr.isImsOn()) {
                // no-op
                return;
            }

            sr.setImsOn(false);
        }

        notifyServiceOff(phoneId);
    }

    @Override
    public ICallStateTracker getCallStateTracker(int phoneId) {
        CallRegistry cr = getCallRegistry(phoneId);
        return (cr != null) ? cr.getCallStateTracker() : null;
    }

    @Override
    public IServiceStateTracker getServiceStateTracker(int phoneId) {
        ServiceRegistry sr = getServiceRegistry(phoneId);
        return (sr != null) ? sr.getServiceStateTracker() : null;
    }

    @Override
    public void addCallStateListener(int phoneId, CallStateListener listener) {
        CallRegistry cr = getCallRegistry(phoneId);

        if (cr != null) {
            cr.addListener(listener);
        }
    }

    @Override
    public void removeCallStateListener(int phoneId, CallStateListener listener) {
        CallRegistry cr = getCallRegistry(phoneId);

        if (cr != null) {
            cr.removeListener(listener);
        }
    }

    // The Message object contains AsyncResult object in "obj" field.
    // The AsyncResult object contains ImsServiceState object in "result" field.
    @Override
    public void registerForEmergencyServiceStateChanged(
            int phoneId, Handler h, int what, Object obj) {
        ServiceRegistry sr = getServiceRegistry(phoneId);

        if (sr != null) {
            sr.registerForEmergencyServiceStateChanged(h, what, obj);
        }
    }

    @Override
    public void unregisterForEmergencyServiceStateChanged(int phoneId, Handler h) {
        ServiceRegistry sr = getServiceRegistry(phoneId);

        if (sr != null) {
            sr.unregisterForEmergencyServiceStateChanged(h);
        }
    }

    // The Message object contains AsyncResult object in "obj" field.
    // The AsyncResult object contains ImsServiceState object in "result" field.
    @Override
    public void registerForServiceStateChanged(int phoneId, Handler h, int what, Object obj) {
        ServiceRegistry sr = getServiceRegistry(phoneId);

        if (sr != null) {
            sr.registerForServiceStateChanged(h, what, obj);
        }
    }

    @Override
    public void unregisterForServiceStateChanged(int phoneId, Handler h) {
        ServiceRegistry sr = getServiceRegistry(phoneId);

        if (sr != null) {
            sr.unregisterForServiceStateChanged(h);
        }
    }

    private CallRegistry getCallRegistry(int phoneId) {
        if ((phoneId < 0) || (phoneId >= MAX_PHONE))  {
            return null;
        }

        return mCallRegistry[phoneId];
    }

    private ServiceRegistry getServiceRegistry(int phoneId) {
        if ((phoneId < 0) || (phoneId >= MAX_PHONE))  {
            return null;
        }

        return mServiceRegistry[phoneId];
    }

    private void postAndRunTask(Runnable task) {
        if (mExecutor == null) {
            task.run();
        } else {
            mExecutor.execute(task);
        }
    }

    private void notifyServiceOn(final int phoneId) {
        if (mListeners.isEmpty()) {
            return;
        }

        postAndRunTask(new Runnable() {
            @Override
            public void run() {
                for (GImsServiceStateListener l : mListeners) {
                    l.onImsServiceOn(phoneId);
                }
            }
        });
    }

    private void notifyServiceOn(final GImsServiceStateListener listener, final int phoneId) {
        postAndRunTask(new Runnable() {
            @Override
            public void run() {
                listener.onImsServiceOn(phoneId);
            }
        });
    }

    private void notifyServiceOff(final int phoneId) {
        if (mListeners.isEmpty()) {
            return;
        }

        postAndRunTask(new Runnable() {
            @Override
            public void run() {
                for (GImsServiceStateListener l : mListeners) {
                    l.onImsServiceOff(phoneId);
                }
            }
        });
    }

    private void notifyServiceOff(final GImsServiceStateListener listener, final int phoneId) {
        postAndRunTask(new Runnable() {
            @Override
            public void run() {
                listener.onImsServiceOff(phoneId);
            }
        });
    }

    private static void logi(String s) {
        ImsLog.i("[GII-Registry] " + s);
    }

    /**
     * Registrant for event handling with Handler.
     */
    private class Registrant {
        WeakReference<Handler> mRefH;
        int mWhat;
        Object mUserObj;

        public Registrant(Handler h, int what, Object obj) {
            mRefH = new WeakReference<>(h);
            mWhat = what;
            mUserObj = obj;
        }

        public void clear() {
            mRefH = null;
            mUserObj = null;
        }

        public Handler getHandler() {
            if (mRefH == null) {
                return null;
            }

            return (Handler)mRefH.get();
        }
    }

    /**
     * Registry to provide service related information.
     */
    private class ServiceRegistry {
        /** Registrants for emergency service state */
        private final List<Registrant> mRegistrantsForESS = new ArrayList<Registrant>();
        /** Registrants for service state */
        private final List<Registrant> mRegistrantsForSS = new ArrayList<Registrant>();
        private boolean mImsOn;
        private IServiceStateTracker mServiceStateTracker;

        public ServiceRegistry(int phoneId) {
            mImsOn = SettingsUtils.isEnhanced4GModeEnabled(
                    AppContext.get(), phoneId);
        }

        public boolean isImsOn() {
            return mImsOn;
        }

        public void setImsOn(boolean imsOn) {
            if (mImsOn != imsOn) {
                logi("setImsOn :: " + mImsOn + " >> " + imsOn);
                mImsOn = imsOn;
            }
        }

        public IServiceStateTracker getServiceStateTracker() {
            return mServiceStateTracker;
        }

        public void setServiceStateTracker(IServiceStateTracker ssTracker) {
            synchronized (mLock) {
                logi("setServiceStateTracker :: ss=" + mRegistrantsForSS.size()
                        + ", ess=" + mRegistrantsForESS.size());

                IServiceStateTracker oldSst = mServiceStateTracker;

                mServiceStateTracker = ssTracker;

                if (oldSst != null) {
                    unregisterRegistrants(oldSst,
                            mRegistrantsForSS, mRegistrantsForESS);
                }

                if (mServiceStateTracker != null) {
                    registerRegistrants(mServiceStateTracker,
                            mRegistrantsForSS, mRegistrantsForESS);
                }
            }
        }

        // The Message object contains AsyncResult object in "obj" field.
        // The AsyncResult object contains ImsServiceState object in "result" field.
        public void registerForEmergencyServiceStateChanged(Handler h, int what, Object obj) {
            synchronized (mLock) {
                addRegistrant(mRegistrantsForESS, h, what, obj);

                if (mServiceStateTracker != null) {
                    mServiceStateTracker.registerForEmergencyServiceStateChanged(h, what, obj);
                }
            }
        }

        public void unregisterForEmergencyServiceStateChanged(Handler h) {
            synchronized (mLock) {
                removeRegistrant(mRegistrantsForESS, h);

                if (mServiceStateTracker != null) {
                    mServiceStateTracker.unregisterForEmergencyServiceStateChanged(h);
                }
            }
        }

        // The Message object contains AsyncResult object in "obj" field.
        // The AsyncResult object contains ImsServiceState object in "result" field.
        public void registerForServiceStateChanged(Handler h, int what, Object obj) {
            synchronized (mLock) {
                addRegistrant(mRegistrantsForSS, h, what, obj);

                if (mServiceStateTracker != null) {
                    mServiceStateTracker.registerForServiceStateChanged(h, what, obj);
                }
            }
        }

        public void unregisterForServiceStateChanged(Handler h) {
            synchronized (mLock) {
                removeRegistrant(mRegistrantsForSS, h);

                if (mServiceStateTracker != null) {
                    mServiceStateTracker.unregisterForServiceStateChanged(h);
                }
            }
        }

        private void addRegistrant(List<Registrant> registrants, Handler h, int what, Object obj) {
            removeCleared(registrants);
            registrants.add(new Registrant(h, what, obj));
        }

        private void removeRegistrant(List<Registrant> registrants, Handler h) {
            int size = registrants.size();

            for (int i = 0; i < size; i++) {
                Registrant r = registrants.get(i);
                Handler rH = r.getHandler();

                if (rH == null || rH == h) {
                    r.clear();
                }
            }

            removeCleared(registrants);
        }

        private void removeCleared(List<Registrant> registrants) {
            for (int i = registrants.size() - 1; i >= 0; i--) {
                Registrant r = registrants.get(i);

                if (r.mRefH == null) {
                    registrants.remove(i);
                }
            }
        }

        private void registerRegistrants(IServiceStateTracker sst,
                List<Registrant> registrantsForSS, List<Registrant> registrantsForESS) {
            for (Registrant r : registrantsForSS) {
                Handler h = r.getHandler();

                if (h != null) {
                    sst.registerForServiceStateChanged(h, r.mWhat, r.mUserObj);
                }
            }

            for (Registrant r : registrantsForESS) {
                Handler h = r.getHandler();

                if (h != null) {
                    sst.registerForEmergencyServiceStateChanged(h, r.mWhat, r.mUserObj);
                }
            }
        }

        private void unregisterRegistrants(IServiceStateTracker sst,
                List<Registrant> registrantsForSS, List<Registrant> registrantsForESS) {
            for (Registrant r : registrantsForSS) {
                Handler h = r.getHandler();

                if (h != null) {
                    sst.unregisterForServiceStateChanged(h);
                }
            }

            for (Registrant r : registrantsForESS) {
                Handler h = r.getHandler();

                if (h != null) {
                    sst.unregisterForEmergencyServiceStateChanged(h);
                }
            }
        }
    }

    /**
     * Registry to provide call related information.
     */
    private class CallRegistry extends CallStateListener {
        private final Set<CallStateListener> mCallStateListeners
                = new HashSet<CallStateListener>();
        private ICallStateTracker mCallStateTracker;

        public CallRegistry() {
        }

        public ICallStateTracker getCallStateTracker() {
            return mCallStateTracker;
        }

        public void setCallStateTracker(ICallStateTracker csTracker) {
            synchronized (mCallStateListeners) {
                logi("setCallStateTracker :: listeners=" + mCallStateListeners.size());
            }

            ICallStateTracker oldCallStateTracker = mCallStateTracker;

            mCallStateTracker = csTracker;

            if (oldCallStateTracker != null) {
                oldCallStateTracker.removeListener(this);
            }

            if (mCallStateTracker != null) {
                mCallStateTracker.addListener(this);
            }
        }

        public void addListener(CallStateListener listener) {
            synchronized (mCallStateListeners) {
                mCallStateListeners.add(listener);
            }
        }

        public void removeListener(CallStateListener listener) {
            synchronized (mCallStateListeners) {
                mCallStateListeners.remove(listener);
            }
        }

        @Override
        public void onCallCreated(Call call) {
            synchronized (mCallStateListeners) {
                mCallStateListeners.forEach(l -> l.onCallCreated(call));
            }
        }

        @Override
        public void onCallDestroyed(Call call) {
            synchronized (mCallStateListeners) {
                mCallStateListeners.forEach(l -> l.onCallDestroyed(call));
            }
        }

        @Override
        public void onCallEstablishing(Call call) {
            synchronized (mCallStateListeners) {
                mCallStateListeners.forEach(l -> l.onCallEstablishing(call));
            }
        }

        @Override
        public void onCallRinging(Call call) {
            synchronized (mCallStateListeners) {
                mCallStateListeners.forEach(l -> l.onCallRinging(call));
            }
        }

        @Override
        public void onCallAccepted(Call call) {
            synchronized (mCallStateListeners) {
                mCallStateListeners.forEach(l -> l.onCallAccepted(call));
            }
        }

        @Override
        public void onCallEstablished(Call call) {
            synchronized (mCallStateListeners) {
                mCallStateListeners.forEach(l -> l.onCallEstablished(call));
            }
        }

        @Override
        public void onCallUpdated(Call call) {
            synchronized (mCallStateListeners) {
                mCallStateListeners.forEach(l -> l.onCallUpdated(call));
            }
        }

        @Override
        public void onCallTerminating(Call call) {
            synchronized (mCallStateListeners) {
                mCallStateListeners.forEach(l -> l.onCallTerminating(call));
            }
        }

        @Override
        public void onCallTerminated(Call call) {
            synchronized (mCallStateListeners) {
                mCallStateListeners.forEach(l -> l.onCallTerminated(call));
            }
        }
    }
}
