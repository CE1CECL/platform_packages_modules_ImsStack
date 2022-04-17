package com.android.imsstack.test;

public interface IImsTestMode {
    public int getExtraTestmask();

    public boolean isCallOverWifiEnabled();

    public boolean isDebugEnabled();

    public boolean isDebuggable();

    public boolean isImsOff();

    public boolean isLocalHoldToneEnabled();

    public boolean isGenericTestMode();
}
