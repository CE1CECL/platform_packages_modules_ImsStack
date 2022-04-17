package com.android.imsstack.imsservice.mmtel.config.base;

class ProvisioningItem
{
    String mName; // preference key
    Class<?> mType; // boolean.class / int.class / String.class
    String mCarrierConfigKey; // for default value, if null, do not any action.

    public ProvisioningItem(String name, Class<?> type, String carrierConfigKey) {
        mName = name;
        mType = type;
        mCarrierConfigKey = carrierConfigKey;
    }

    public String getName() {
        return mName;
    }
    public String getCarrierConfigKey() {
        return mCarrierConfigKey;
    }

    public boolean isBooleanFormat() {
        return (mType == Boolean.class);
    }

    public boolean isIntegerFormat() {
        return (mType == Integer.class);
    }

    public boolean isStringFormat() {
        return (mType == String.class);
    }
}
