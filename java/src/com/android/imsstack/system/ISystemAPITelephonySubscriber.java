package com.android.imsstack.system;

public interface ISystemAPITelephonySubscriber {

    String getDeviceId4Sys();
    String getDeviceSoftwareVersion4Sys();
    String getSubscriberId4Sys();
    String getMnc4Sys(boolean fromUICC);
    String getMcc4Sys(boolean fromUICC);
    String getPhoneNUmberExcludingNationalPrefix4Sys();
    String getOperator4Sys();
    String getCountry4Sys();
    String getNetworkCountry4Sys();
    String getEmergencyNumberListFromSIM4Sys();
    int getEmergencyPriorityFromModem4Sys();
    boolean isUiccGbaSupported4Sys();
}
