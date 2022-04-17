package com.android.imsstack.system;

public interface ISystemAPIPreference {

    String getPreference4Sys(String fileName, String key, int valueType, int slotId);
    int setPreference4Sys(String fileName, String key, int valueType, String value, int slotId);

}
