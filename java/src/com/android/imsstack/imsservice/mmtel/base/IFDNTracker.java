package com.android.imsstack.imsservice.mmtel.base;

public interface IFDNTracker {
    /**
     * Checks if the specified number is blocked by FDN.
     */
    public boolean isCallBlockedByFDN(String number);

    /**
     * Checks if the specified numbers are blocked by FDN.
     */
    public boolean isCallBlockedByFDN(String[] numbers);
}
