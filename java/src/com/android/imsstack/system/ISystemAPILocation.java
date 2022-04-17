package com.android.imsstack.system;

public interface ISystemAPILocation {

    int startLocationInfo4Sys(int nUpdateIntervalInSec);
    void stopLocationInfo4Sys();
    int makeInstantLocationUpdates4Sys();
    String[] getLocationInformation4Sys(int nType);

}
