package com.android.imsstack.core.agents.agentif;

import android.os.Handler;

public interface IWifiState extends IAgent {
    /**
     * Register listener to receive event for Wifi state change.
     */
    void registerForWifiStateChanged(Handler h, int what, Object obj);

    /**
     * Un-register listener to receive event for Wifi state change.
     */
    void unregisterForWifiStateChanged(Handler h);

    /**
     * Returns if wifi is connected or not.
     */
    boolean isWifiConnected();

    /**
     * Returns local address of device via WifiManager.
     */
    String getLocalAddress();

    /**
     * Returns detail status of current wifi connection.
     */
    int getWifiDetailedStatus();

    /**
     * Returns Wifi SSID value.
     */
    String getWifiSSID();

    /**
     * Set 'wifi supported' flag value. This change effect to behavior of this object
     */
    void setWifiSupported(boolean input);

    // Methods belows are used like private method, so remove them from this interface class.
    //int getWifiState();
    //String getWifiBSSID();

    // There is no place to use this method, so made it comment.
    // Please remove this code if this method will not be used.
    //int convertNetworkInfoDetailedStateToInt(NetworkInfo.DetailedState state);

}
