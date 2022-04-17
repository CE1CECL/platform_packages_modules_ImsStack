package com.android.imsstack.core.agents.dcmif;

import android.net.ConnectivityManager;
import android.net.NetworkCapabilities;
import android.telephony.data.ApnSetting;

import com.android.imsstack.util.ImsLog;
import com.android.internal.telephony.PhoneConstants;

public enum EApnType {
    /**
     * WIFI type is only for IMS internal usage. (16/05/09)
     * It's defined to synchronize the constant value in Native Layer.
     */
    IMS (DCConstants.TYPE_IMS,              "mobile_ims",        NetworkCapabilities.NET_CAPABILITY_IMS),
    INTERNET (DCConstants.TYPE_INTERNET,    "mobile_internet",   NetworkCapabilities.NET_CAPABILITY_INTERNET),
    XCAP (DCConstants.TYPE_XCAP,            "mobile_xcap",       NetworkCapabilities.NET_CAPABILITY_XCAP),
    EMERGENCY (DCConstants.TYPE_EMERGENCY,  "mobile_emergency",  NetworkCapabilities.NET_CAPABILITY_EIMS),
    WIFI (DCConstants.TYPE_WIFI,            "wifi",              NetworkCapabilities.NET_CAPABILITY_INTERNET);

    private final int mType;
    private final String mStrType;
    private final int mNetCapability;

    private EApnType(int type, String strType, int netCapability) {
        mType = type;
        mStrType = strType;
        mNetCapability = netCapability;
    }

    public static int getTypeFromApnSettingType(String type) {
        if (ApnSetting.TYPE_IMS_STRING.equals(type)) {
            return IMS.getType();
        } else if (ApnSetting.TYPE_DEFAULT_STRING.equals(type)) {
            return INTERNET.getType();
        } else if (ApnSetting.TYPE_EMERGENCY_STRING.equals(type)) {
            return EMERGENCY.getType();
        } else if (ApnSetting.TYPE_XCAP_STRING.equals(type)) {
            return XCAP.getType();
        }

        ImsLog.w("Not-Exist-ApnType=" + type);

        return DCConstants.TYPE_NONE;
    }

    public static String getApnSettingTypeFromType(int type) {
        if (IMS.getType() == type) {
            return ApnSetting.TYPE_IMS_STRING;
        } else if (INTERNET.getType() == type) {
            return ApnSetting.TYPE_DEFAULT_STRING;
        } else if (EMERGENCY.getType() == type) {
            return ApnSetting.TYPE_EMERGENCY_STRING;
        } else if (XCAP.getType() == type) {
            return ApnSetting.TYPE_XCAP_STRING;
        }

        ImsLog.w("Not-Exist-Type=" + type);

        return null;
    }

    public static int getNetCapabilityFromType(int type) {
        if (IMS.getType() == type) {
            return EApnType.IMS.getNetCapability();
        } else if (INTERNET.getType() == type) {
            return EApnType.INTERNET.getNetCapability();
        } else if (EMERGENCY.getType() == type) {
            return EApnType.EMERGENCY.getNetCapability();
        } else if (XCAP.getType() == type) {
            return EApnType.XCAP.getNetCapability();
        } else if (WIFI.getType() == type) { // DNS :: SimpleResolver
            return EApnType.WIFI.getNetCapability();
        }

        ImsLog.w("Not-Exist-Type=" + type);

        return DCConstants.CAPABILITY_NONE;
    }

    public int getType() {
        return mType;
    }

    public String getString() {
        return mStrType;
    }

    public int getNetCapability() {
        return mNetCapability;
    }
}
