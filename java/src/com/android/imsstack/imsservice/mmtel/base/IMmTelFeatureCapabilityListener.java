package com.android.imsstack.imsservice.mmtel.base;

import android.telephony.ims.feature.MmTelFeature;

/**
 * Listener for MMTel feature capability changed events.
 */
public interface IMmTelFeatureCapabilityListener {
    /**
     * This is invoked when incoming call is received.
     */
    public void onFeatureCapabilityChanged(MmTelFeature.MmTelCapabilities capabilities);
}
