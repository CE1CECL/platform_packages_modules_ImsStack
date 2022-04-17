package com.android.imsstack.core.agents.agentif;

public interface ISubscriberInfo extends IAgent {
    /**
     * These values are defined during process booting time.
     * So if you changed this values in ims hidden menu,
     * it need process restart to apply those change to return values below.
     */

    /**
     * Return value of 'admin_features' in gims_subscriber table.
     */
    boolean isImsOn();
    boolean isIsimOn();
    boolean isUsimOn();
    boolean isDebugOn();
    boolean isDMOn();
    boolean isTestMode();
    boolean isTestModeForGCF();

    /**
     * Return enable/disable of VoLTE service in 'admin_services' in 'gims_subscriber' table
     */
    boolean isVoLTEServiceOn();

    /**
     * Return enable/disable of ViLTE service in 'admin_services' in 'gims_subscriber' table
     */
    boolean isViLTEServiceOn();

    /**
     * Return enable/disable of VoWiFi service in 'admin_services' in 'gims_subscriber' table
     */
    boolean isVoWiFiServiceOn();

    /**
     * Return enable/disable of ViWiFi service in 'admin_services' in 'gims_subscriber' table
     */
    boolean isViWiFiServiceOn();
}
