/*
    Author
    <table>
    date        author                  description
    --------    --------------          ----------
    20151217    raghavendra.niranjan@           Created
    </table>

    Description
*/
package com.android.imsstack.imsservice.mmtel;

import android.content.Context;
import android.database.ContentObserver;
import android.net.Uri;

import com.android.imsstack.core.agents.AgentFactory;
import com.android.imsstack.core.agents.agentif.ISubscription;
import com.android.imsstack.core.agents.agentif.SubscriptionListener;
import com.android.imsstack.enabler.IBaseContext;
import com.android.imsstack.imsservice.mmtel.base.IFDNTracker;
import com.android.imsstack.util.ImsLog;
import com.android.imsstack.util.MSimUtils;

import java.util.List;
import java.util.Locale;

public class ImsFDNTracker implements IFDNTracker {
    private static final String FDN_CONTENT_URI = "content://icc/fdn";
    private static final String FDN_CONTENT_PATH_WITH_SUB_ID = "content://icc/fdn/subId/";

    private final IBaseContext mContext;
    private SubscriptionListenerProxy mSubscriptionListener;
    private FDNTracker mFDNTracker;
    private boolean mIsFDNAvailable = false;

    public ImsFDNTracker(IBaseContext context) {
        mContext = context;

        mFDNTracker = new FDNTracker();
        mSubscriptionListener = new SubscriptionListenerProxy();

        init();
    }

    public void dispose() {
        log("dispose");

        clear();
    }

    public void init() {
        log("init");

        ISubscription isub = mContext.getSubscription();

        if (isub != null) {
            isub.addListener(mSubscriptionListener);
        }

        if (isSimLoaded(mContext.getSlotId())) {
            initToFetchList();
        }
    }

    public void clear() {
        log("clear");

        mIsFDNAvailable = false;

        if (mFDNTracker != null) {
            mFDNTracker.clearObserver();
        }

        if (mSubscriptionListener != null) {
            ISubscription isub = mContext.getSubscription();

            if (isub != null) {
                isub.removeListener(mSubscriptionListener);
            }
        }
    }

    public boolean isFDNAvailable() {
        return mIsFDNAvailable;
    }

    public boolean getFDNAvailability() {
        mIsFDNAvailable = false;

        log("FdnAvailability=" + mIsFDNAvailable);

        return mIsFDNAvailable;
    }

    public boolean isCallBlockedByFDN(String number) {
        if (!isFDNAvailable()) {
            return false;
        }

        if (mFDNTracker.isFDNEnabled() == false) {
            //FDN is disabled, allow the call
            return false;
        }

        List<String> fdnList = mFDNTracker.getFDNList();

        if (fdnList == null) {
            return true;
        }

        if (isContains(fdnList , number) == false) {
            //FDN is enabled and dialed number is not found in FDN list.
            //Block the call.
            return true;
        }
        //FDN is enabled and dialed number is found in FDN list.
        //allow the call.
        return false;
    }

    public boolean isCallBlockedByFDN(String[] numbers) {
        boolean isCallBlocked = false;

        if (!isFDNAvailable()) {
            return false;
        }

        if (mFDNTracker.isFDNEnabled() == false) {
            //FDN is disabled, allow the call
            return false;
        }

        List<String> fdnList = mFDNTracker.getFDNList();

        if (fdnList == null) {
            return true;
        }

        for (String number : numbers) {
            if (isContains(fdnList, number) == false) {
                //FDN is enabled and dialed number is not found in FDN list.
                //Block the call.
                isCallBlocked = true;
                break;
            }
            //FDN is enabled and all dialed numbers are found in FDN list.
            //allow the call.
            isCallBlocked = false;
        }
        return isCallBlocked;
    }

    private static boolean isContains(List<String> fdnList, String number) {
        if (number != null) {
            for (int i = 0; i < fdnList.size(); i++)
            {
                if (number.indexOf(fdnList.get(i)) == 0) {
                    return true;
                }
            }
        }
        return false;
    }

    private Uri getContentUri() {
        return Uri.parse(getContentUriString());
    }

    private String getContentUriString() {
        if (!MSimUtils.isMultiSimEnabled() || mContext.getSubId() == MSimUtils.INVALID_SUB_ID) {
            return FDN_CONTENT_URI;
        } else {
            return FDN_CONTENT_PATH_WITH_SUB_ID + mContext.getSubId();
        }
    }

    private void initToFetchList() {
        //initiallize FDN Tracker only if FDN functionality is available.
        if (getFDNAvailability()) {
            mFDNTracker.init();
            mFDNTracker.clearObserver();
            mFDNTracker.registerObserver();
        } else {
            mFDNTracker.clearObserver();
        }
    }

    private boolean isSimLoaded(int slotId) {
        ISubscription isub = (ISubscription)AgentFactory.getAgent(AgentFactory.SUBSCRIPTION);
        return (isub != null) ? isub.isSimLoaded(slotId) : false;
    }

    private static void log(String s) {
        ImsLog.d("[GII-IMPL] " + s);
    }

    private class FDNTracker extends ContentObserver {
        private List<String> mFDNList;

        public FDNTracker() {
            super(mContext.getDefaultHandler());
        }

        public void init() {
            runInThread(new Runnable() {
                @Override
                public void run() {
                    fetchFDNList();
                }
            });
        }

        public void clear() {
            mFDNList = null;
        }

        private void runInThread(Runnable r) {
            String name = String.format(Locale.US, "ImsFDNTracker%02d", mContext.getSlotId());
            new Thread(r, name).start();
        }

        @Override
        public void onChange(boolean selfChange) {
            super.onChange(selfChange);
            runInThread(new Runnable() {
                @Override
                public void run() {
                    fetchFDNList();
                }
            });
        }

        public void clearObserver() {
            try {
                mContext.getContext().getContentResolver().unregisterContentObserver(this);
            } catch (Throwable t) {
                t.printStackTrace();
            }
        }

        public void registerObserver() {
            mContext.getContext().getContentResolver().registerContentObserver(
                    getContentUri(), true, this);
        }

        public List<String> getFDNList() {
            return mFDNList;
        }

        public boolean isFDNEnabled() {
            return false;
        }

        private void fetchFDNList() {
            mFDNList = null;
        }
    }

    private final class SubscriptionListenerProxy extends SubscriptionListener {
        @Override
        public void onDefaultDataSubscriptionChanged(int subId) {
            log("onDefaultDataSubscriptionChanged :: subId=" + subId);

            initToFetchList();
        }

        @Override
        public void onSimLoadCompleted(int slotId) {
            log("onSimLoadCompleted :: slotId=" + slotId);

            if (mContext.getSlotId() != slotId) {
                // If Ims process is restarted in multi-SIM device,
                // one SIM only notifies SIM_LOADED state.
                if (isSimLoaded(mContext.getSlotId())) {
                    initToFetchList();
                }
                return;
            }

            if (isSimLoaded(slotId)) {
                initToFetchList();
            } else {
                getFDNAvailability();
                mFDNTracker.clear();
            }
        }
    }
}
