package com.android.imsstack.core.agents.agentif;

import android.location.Location;

import com.android.imsstack.system.ISystemAPILocation;

public interface ILocationAgent extends ISystemAPILocation {
    /** Cached locations */
    public static final int MAX_LOCATIONS = 3;
    public static final int CACHE_I_GPS = 0;
    public static final int CACHE_I_NLP = 1;
    public static final int CACHE_I_FLP = 2;

    // Location type
    public static final int LOCATION_ALL = 0;
    public static final int LOCATION_POSITION_N_COUNTRY = 1;
    public static final int LOCATION_POSITION = 2;

    /**
     * Initializes last known location information.
     */
    public void initLastKnownLocation();

    /**
     * Returns the location policy for this location agent.
     */
    public LocationPolicy getLocationPolicy();

    /**
     * Sets the location policy for this location agent.
     */
    public void setLocationPolicy(LocationPolicy lp);

    /**
     * Returns the cached location information.
     */
    public Location[] getCachedLocations();

    /**
     * Returns last known country code.
     */
    public String getLastKnownCountryCode();

    /**
     * Returns the best location information from the last known location.
     */
    public String[] getLastKnownLocation(int type);
}
