package com.android.imsstack.util;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public final class ImsFeatures {
    /**
     * Feture Define
     */
    public static final int FEATURE_NONE = 0;

    private static ConcurrentHashMap<Integer, IImsFeature> sFeatures =
            new ConcurrentHashMap<Integer, IImsFeature>();

    public static IImsFeature getInstance(int slotID) {

        if(!sFeatures.containsKey(slotID)) {
            sFeatures.put(slotID, new ImsRuntimeFeature(slotID));
        }

        return sFeatures.get(slotID);
    }

}

class ImsRuntimeFeature implements IImsFeature {
    private static int sFeatures = ImsFeatures.FEATURE_NONE;
    private int mSlotID = -1;

    public ImsRuntimeFeature(int slotID) {
        mSlotID = slotID;
    }

    // Interface implementation methods --------------------------
    @Override
    public void disableFeature(int feature) {
        sFeatures &= (~feature);
    }

    @Override
    public void enableFeature(int feature) {
        sFeatures |= feature;
    }

    @Override
    public boolean isFeatureEnabled(int feature) {
        return (sFeatures & feature) > 0;
    }
}
