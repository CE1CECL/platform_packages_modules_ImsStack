package com.android.imsstack.core.agents.dcmif;

public interface IDCUtil extends IDC {
    public static class AccessNetworkInfo {
        public int mNetworkType;
        public String[] mANI;

        public AccessNetworkInfo(int networkType, String[] ani) {
            mNetworkType = networkType;
            mANI = ani;
        }
    }

    /**
     * Return PANI header information string.
     */
    AccessNetworkInfo getAccessNetworkInfo(int defaultNetworkType);

    /**
     * Return mobile data is enabled or not in setting menu.
     */
    public boolean isMobileDataEnabled();

    /**
     * Return signal strength value via IMSPhone.
     * If current network type is not LTE, it returns 0;
     */
    public int getLteRsrpStrength();

    /**
     *
     */
    public boolean sendPingToHostAddress(int apnType, String hostAddress);

    /**
     * Update all cell-info forcingly if the device doesn't have SIM card
     * or is in limited service state (emergency only).
     */
    public void updateAllCellInfoForcinglyOnLimitedServiceState();
}
