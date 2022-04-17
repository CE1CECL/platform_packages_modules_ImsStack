/*
    Author
    <table>
    date        author                  description
    --------    --------------          ----------
    20150410    hwangoo.park@           Created
    </table>

    Description
*/

package com.android.imsstack.enabler.mtc;

import android.text.TextUtils;

import com.android.imsstack.enabler.mtc.ConferenceInfo.User;
import com.android.imsstack.util.ImsLog;

import java.util.ArrayList;
import java.util.List;

public final class ConferenceInfoManager {
    private static ConferenceInfoManager sConferenceInfoManager = new ConferenceInfoManager();

    // List of conference-info
    private List<ConferenceInfo> mConferenceInfos = new ArrayList<ConferenceInfo>();

    private ConferenceInfoManager() {
    }

    public static ConferenceInfoManager getInstance() {
        return sConferenceInfoManager;
    }

    public ConferenceInfo createConferenceInfo(String ccid, int slotId) {
        if (TextUtils.isEmpty(ccid)) {
            return null;
        }

        ConferenceInfo ci = getConferenceInfo(ccid);

        if (ci != null) {
            return ci;
        }

        ci = new ConferenceInfo(ccid, slotId);

        synchronized(mConferenceInfos) {
            mConferenceInfos.add(ci);
        }

        log("ConferenceInfo(add): ccid=" + ccid + ", size=" + mConferenceInfos.size());

        return ci;
    }

    public void destroyConferenceInfo(String ccid) {
        if (TextUtils.isEmpty(ccid)) {
            return;
        }

        synchronized(mConferenceInfos) {
            for (int i = 0; i < mConferenceInfos.size(); ++i) {
                ConferenceInfo ci = mConferenceInfos.get(i);

                if ((ci != null) && ccid.equalsIgnoreCase(ci.getCcid())) {
                    mConferenceInfos.remove(i);

                    log("ConferenceInfo(remove): ccid="
                            + ccid + ", size=" + mConferenceInfos.size());
                    break;
                }
            }
        }
    }

    public void destroyAllConferenceInfos() {
         synchronized(mConferenceInfos) {
            if (mConferenceInfos.isEmpty()) {
                return;
            }

            log("destroyAllConferenceInfos :: size=" + mConferenceInfos.size());

            for (int i = 0; i < mConferenceInfos.size(); ++i) {
                ConferenceInfo ci = mConferenceInfos.get(i);
            }

            mConferenceInfos.clear();
        }
    }

    public ConferenceInfo getConferenceInfo(String ccid) {
        if (TextUtils.isEmpty(ccid)) {
            return null;
        }

        synchronized(mConferenceInfos) {
            for (ConferenceInfo ci : mConferenceInfos) {
                if (ccid.equalsIgnoreCase(ci.getCcid())) {
                    return ci;
                }
            }
        }

        return null;
    }

    public ConferenceInfo getConferenceInfoByUser(String callId, String uid) {
        if (TextUtils.isEmpty(callId) && TextUtils.isEmpty(uid)) {
            return null;
        }

        synchronized(mConferenceInfos) {
            for (ConferenceInfo ci : mConferenceInfos) {
                User user = ci.getUser(callId, uid);

                if (user != null) {
                    return ci;
                }
            }
        }

        return null;
    }

    public boolean hasConferenceInfo() {
        synchronized (mConferenceInfos) {
            return !mConferenceInfos.isEmpty();
        }
    }

    private static void log(String s) {
        ImsLog.d("[GII-CALL] " + s);
    }
}
