package com.android.imsstack.system;

public interface ISystemAPICallInfo {
    int isEmergencyNumber(String number);
    int getTTYMode();
    int getCallStateInOtherSlot();
}
