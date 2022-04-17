package com.android.imsstack.core.agents.dcmif;

public enum EIpVersion {

    // Enumerations ----------------------------------------------
    IPV4 (4),
    IPV6 (6),
    IPV4V6 (46),
    IPV6V4 (64);

    // Variables--------------------------------------------------
    private final int mVersion;

    // Private/Protected methods ---------------------------------
    private EIpVersion(int nVersion) {
        this.mVersion    = nVersion;
    }

    // Public methods --------------------------------------------
    public int getInt() {
        return this.mVersion;
    }
}
