package com.android.imsstack.util;

public interface IImsFeature {

    void disableFeature(int feature);
    void enableFeature(int feature);
    boolean isFeatureEnabled(int feature);
}
