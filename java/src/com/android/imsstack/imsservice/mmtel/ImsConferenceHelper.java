/*
    Author
    <table>
    date        author                  description
    --------    --------------          ----------
    20150403    hwangoo.park@           Created
    </table>

    Description
*/

package com.android.imsstack.imsservice.mmtel;

import android.telephony.ims.stub.ImsCallSessionImplBase;

import com.android.imsstack.enabler.mtc.MtcCall;
import com.android.imsstack.imsservice.mmtel.base.ICallContext;
import com.android.imsstack.imsservice.mmtel.internal.ConferenceProxy;
import com.android.imsstack.imsservice.mmtel.internal.ConferenceExtensionProxy;
import com.android.imsstack.imsservice.mmtel.internal.MergeProxy;
import com.android.imsstack.util.ImsLog;

import java.util.ArrayList;

public final class ImsConferenceHelper implements ConferenceProxy.DisposalCallback {
    private static ImsConferenceHelper sConferenceHelper = new ImsConferenceHelper();

    private ArrayList<ConferenceProxy> mConferenceProxys
            = new ArrayList<ConferenceProxy>();
    private ImsCallSessionImplBase mTransientConferenceSession = null;
    // To recover background session when foreground session is terminated
    // before call merge is completed.
    private ImsCallSessionImplBase mBackgroundSession = null;

    private ImsConferenceHelper() {
    }

    public static ImsConferenceHelper getInstance() {
        return sConferenceHelper;
    }

    @Override
    public void onConferenceProxyDisposed(ConferenceProxy confProxy) {
        synchronized(mConferenceProxys) {
            int size = mConferenceProxys.size();
            mConferenceProxys.remove(confProxy);
            log("onConferenceProxyDisposed :: size - "
                    + size + " >> " + mConferenceProxys.size());

            if (mConferenceProxys.isEmpty()) {
                setBackgroundSession(null);
            }
        }

        confProxy.dispose();
        confProxy = null;
    }

    public ImsCallSessionImplBase getBackgroundSession() {
        return mBackgroundSession;
    }

    public ImsCallSessionImplBase getTransientConferenceSession() {
        return mTransientConferenceSession;
    }

    public void setBackgroundSession(ImsCallSessionImplBase session) {
        mBackgroundSession = session;
    }

    public void setTransientConferenceSession(ImsCallSessionImplBase session) {
        mTransientConferenceSession = session;
    }

    public boolean extendToConference(ICallContext callContext, String[] participants) {
        ImsCallApp callApp = (ImsCallApp)callContext.getApp();
        ImsCallManager cm = callApp.getCallManager();
        ImsCallSessionImpl fgCallSession = cm.getActiveSession();

        if (fgCallSession == null) {
            loge("ImsConferenceHelper :: fgCall=" + fgCallSession);
            return false;
        }

        MtcCall fgCall = fgCallSession.getMtcCall();

        if (fgCall == null) {
            loge("ImsConferenceHelper :: fgCall=" + fgCall);
            return false;
        }

        ConferenceProxy confProxy = new ConferenceExtensionProxy(
                callContext, fgCall, participants);

        confProxy.setDisposalCallback(this);

        fgCallSession.setConferenceProxy(confProxy);

        if (!confProxy.start(cm.getMtcApp(), false)) {
            fgCallSession.setConferenceProxy(null);
            loge("ImsConferenceHelper :: extendToConference failed");
            return false;
        }

        synchronized(mConferenceProxys) {
            mConferenceProxys.add(confProxy);
        }

        return true;
    }

    public boolean merge(ICallContext callContext) {
        ImsCallApp callApp = (ImsCallApp)callContext.getApp();
        ImsCallManager cm = callApp.getCallManager();
        ImsCallSessionImpl fgCallSession = cm.getActiveSession();
        ImsCallSessionImpl bgCallSession = cm.getHoldSession();

        if (fgCallSession == null || bgCallSession == null) {
            loge("ImsConferenceHelper :: fgCall="
                    + fgCallSession + ", bgCall=" + bgCallSession);
            return false;
        }

        MtcCall fgCall = fgCallSession.getMtcCall();
        MtcCall bgCall = bgCallSession.getMtcCall();

        if (fgCall == null || bgCall == null) {
            loge("ImsConferenceHelper :: fgCall="
                    + fgCall + ", bgCall=" + bgCall);
            return false;
        }

        ConferenceProxy confProxy = new MergeProxy(callContext, fgCall, bgCall);

        confProxy.setDisposalCallback(this);

        fgCallSession.setConferenceProxy(confProxy);
        bgCallSession.setConferenceProxy(confProxy);

        if (!confProxy.start(cm.getMtcApp(), true)) {
            fgCallSession.setConferenceProxy(null);
            bgCallSession.setConferenceProxy(null);
            loge("ImsConferenceHelper :: merge failed");
            return false;
        }

        setBackgroundSession(bgCallSession);

        synchronized(mConferenceProxys) {
            mConferenceProxys.add(confProxy);
        }

        return true;
    }

    private static void log(String s) {
        ImsLog.d("[GII-IMPL] " + s);
    }

    private static void loge(String s) {
        ImsLog.e("[GII-IMPL] " + s);
    }
}
