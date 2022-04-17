package com.android.imsstack.core;

import android.content.Context;

import com.android.imsstack.core.config.FeatureConfig;
import com.android.imsstack.enabler.uce.interf.UceManager;
import com.android.imsstack.util.FeatureUtils;
import com.android.imsstack.util.ImsLog;

import java.util.ArrayList;
import java.util.List;

public class CapaAgent {
    private static CapaAgent mCapaAgent = null;

    private Context mContext = null;
    private List<Integer> mInitSimList = new ArrayList<Integer>();
    private boolean bIsReadyForCreating = false;

    private CapaAgent() {
    }

    public static CapaAgent getInstance() {
        if (mCapaAgent == null) {
            mCapaAgent = new CapaAgent();
        }
        return mCapaAgent;
    }

    public void init(Context context, int nSimSlot) {
        ImsLog.i(" context : " + context + ", nSimSlot : " + nSimSlot +
                ", bIsReadyForCreating : " + bIsReadyForCreating);
        mContext = context;
        if (mInitSimList.contains(nSimSlot) == false) {
            mInitSimList.add(nSimSlot);
            ImsLog.d(" mInitSimList is added for nSimSlot : " + nSimSlot +
                    ", after adding, and then mInitSimList.size()" + mInitSimList.size());
        }
        if (bIsReadyForCreating == true) {
            start();
        }
    }

    public void deinit(int nSimSlot) {
        ImsLog.d(" ");
        UceManager.delete(nSimSlot);

        mInitSimList.remove(Integer.valueOf(nSimSlot));
        ImsLog.i("mInitSimList.size() : " + mInitSimList.size());
    }

    public void start() {
        ImsLog.d("CapaAgent is started.");
        bIsReadyForCreating = true;
        if (mContext == null) {
            ImsLog.i("mContext is null. mInitSimList.size() : " + mInitSimList.size());
            return;
        }
        for (int nSimSlot : mInitSimList) {
            ImsLog.d("nSimSlot : " + nSimSlot);
            if (isAvailableUce(nSimSlot) == true) {
                UceManager.create(mContext, nSimSlot);
            }
        }
    }

    //check to create UCE
    private boolean isAvailableUce(int nSimSlot) {
        boolean bUceAvail = false;
        boolean bFeatureUce = FeatureConfig.isEnabled(nSimSlot, FeatureConfig.UCE);
        boolean bSystemFeatureUce = FeatureUtils.isUceSupported(mContext);
        ImsLog.d("nSimSlot : " + nSimSlot + ", bFeatureEab : " + bFeatureUce +
                ", bSystemFeatureEab : " + bSystemFeatureUce);

        if (bFeatureUce == true && bSystemFeatureUce == true) {
            bUceAvail = true;
        }
        ImsLog.i("bUceAvail : " + bUceAvail);
        return bUceAvail;
    }
}
