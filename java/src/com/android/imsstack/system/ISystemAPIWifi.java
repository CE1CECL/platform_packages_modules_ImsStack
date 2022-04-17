package com.android.imsstack.system;

public interface ISystemAPIWifi {

    int getWifiState4Sys();
    int getWifiDetailedState4Sys();
    String getWifiBssId4Sys();
    String getWifiSsId4Sys();
    int getWifiIfaceId4Sys();
    int getWifiMtu4Sys();
    void turnOnOff(boolean on);
}
