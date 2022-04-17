package com.android.imsstack.core.agents.dcmif;

public class ApnStateListener {
    /**
     * Notifies the application that IPCAN(IP Connectivity Access Network) category is changed.
     *
     * @param apnType APN type
     *          {@link EApnType#mType}
     * @param ipcanCategory IPCAN category
     *          {@link IApn#IPCAN_CATEGORY_MOBILE}
     *          {@link IApn#IPCAN_CATEGORY_WLAN}
     */
    public void onIpcanCategoryChanged(int apnType, int ipcanCategory) {
        // no-op
    }

    /**
     * Notifies the application that status of handover between WWAN and WLAN is changed.
     *
     * @param handoverState status of handover progress
     *          {@link IApn#HANDOVER_UNKNOWN}
     *          {@link IApn#HANDOVER_STARTED}
     *          {@link IApn#HANDOVER_SUCCESS}
     *          {@link IApn#HANDOVER_FAILURE}
     * @param networkType network type that have connected.
     * In case of handover from LTE to IWLAN, networkType in HANDOVER_STARTED is LTE.
     * If handover fails, networkType in HANDOVER_FAILURE is LTE.
     * If handover is successful, networkType in HANDOVER_SUCCESS is IWLAN.
     *          {@link TelephonyManager#NETWORK_TYPE_LTE}
     *          {@link TelephonyManager#NETWORK_TYPE_IWLAN}
     *          {@link TelephonyManager#NETWORK_TYPE_NR}
     * @param failCause cause code when the handover fails
     *          {@link DataFailCause}
     */
    public void onHandoverInfoChanged(int handoverState, int networkType, int failCause) {
        // no-op
    }
}
