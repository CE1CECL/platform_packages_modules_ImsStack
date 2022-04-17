package com.android.imsstack.core.config;

public interface IECallConfig {
    // capability for emergency call
    public static final int ECALL_RAT_NONE = 0x00000000;
    public static final int ECALL_RAT_LTE = 0x00000001;
    public static final int ECALL_RAT_WIFI = 0x00000002;

    int getEmergencyCallCapability();
    boolean isImsCallEndRequiredForImsEmergencyCall();
    boolean isImsEmergencyCallControlledByVoLteReg();
    boolean isImsEmergencyCallControlledByVoLteSetting();
    void setBlockSupportOverLTE(boolean bBlockSupportOverLTE);
}
