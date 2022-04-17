package com.android.imsstack.core.agents.agentif;

import android.os.Handler;

public interface ICallSetting {

    /**
     * Return VoLTE setting is enabled or not in call setting menu
     */
    boolean isVoLTEUsedForHDVoice();

    /**
     * Return VoWifi setting is enabled or not in call setting menu
     */
    boolean isWfcEnabled();

    /**
    * return video call setting is enabled or not in call setting menu
    */
    boolean isVideoCallEnabled();

    /**
    * return RTT setting value in call setting menu
    * IMS_RTT_MODE_NONE = 0;
    * IMS_RTT_VISIBLE_DURING_CALLS = 1;
    * IMS_RTT_ALWAYS_VISIBLE = 2;
    * IMS_RTT_CAPABLE_OFF = 3;
    */
    int getRTTMode();

    /**
    * notify the setting values to native layer if the setting values are needed from native layer
    */
    void notifySystemEvents();

    /**
    * notify the re-setting values to native layer if the setting values are needed from native layer
    */
    void updateForSetting();

    /**
    * notify the re-setting values to native layer if the roaming setting values
    * are needed from native layer
    */
    void updateForRoamingSetting(boolean bRoaming);

    /**
     * Register listener to receive events.
     */
    void registerForMobileDataSettingChanged(Handler h,int what,Object obj);

    void registerForVideoCallSetChanged(Handler h, int what, Object obj);

    void registerForVideoCallRoamingSetChanged(Handler h, int what, Object obj);

    void registerForVoLTESetChanged(Handler h, int what, Object obj);

    void registerForVoLTERoamingSetChanged(Handler h, int what, Object obj);

    void registerForVoWIFISetChanged(Handler h, int what, Object obj);

    void registerForVoWIFIPreferenceChanged(Handler h, int what, Object obj);

    void registerForVoWIFIRoamingSetChanged(Handler h, int what,Object obj);

    void registerForNetworkModeSettingChanged(Handler h, int what, Object obj);

    void registerForDataRoamingSettingChanged(Handler h, int what, Object obj);

    void registerForRttModeSettingChanged(Handler h, int what, Object obj);

    void registerForVoWIFINetworkPreferenceChanged(Handler h, int what, Object obj);

    void unregisterForMobileDataSettingChanged(Handler h);

    void unregisterForVideoCallSetChanged(Handler h);

    void unregisterForVideoCallRoamingSetChanged(Handler h);

    void unregisterForVoLTESetChanged(Handler h);

    void unregisterForVoLTERoamingSetChanged(Handler h);

    void unregisterForVoWIFISetChanged(Handler h);

    void unregisterForVoWIFIPreferenceChanged(Handler h);

    void unregisterForVoWIFIRoamingSetChanged(Handler h);

    void unregisterForNetworkModeSettingChanged(Handler h);

    void unregisterForDataRoamingSettingChanged(Handler h);

    void unregisterForRttModeSettingChanged(Handler h);

    void unregisterForVoWIFINetworkPreferenceChanged(Handler h);

    // REMOVE ME
    boolean isNetworkMode3GOnly();
}
