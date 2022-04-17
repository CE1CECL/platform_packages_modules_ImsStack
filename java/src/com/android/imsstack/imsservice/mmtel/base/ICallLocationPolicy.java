package com.android.imsstack.imsservice.mmtel.base;

import android.telephony.ims.ImsCallProfile;

/**
 * A class that provides any policy to determine whether geolocation is required for the call.
 */
public interface ICallLocationPolicy {
    /**
     * Checks if location information is required for the specified callee and call profile.
     */
    public boolean isLocationRequired(String callee, ImsCallProfile profile);

    /**
     * Checks if GPS location is required.
     */
    public boolean isPositionInfoRequired();

    /**
     * Returns the validity period of the last known location information.
     * Milli-seconds
     */
    public long getValidityPeriod();

    /**
     * Returns the waiting time after location information is requested.
     * Milli-seconds
     */
    public long getWaitingTimeForLocationFix();
}
