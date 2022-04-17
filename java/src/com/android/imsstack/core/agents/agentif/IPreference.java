package com.android.imsstack.core.agents.agentif;

public interface IPreference extends IAgent {
    /**
     * Returns Boolean type value from common ims shared preference file
     */
    boolean getPreferenceBoolValue(String key);

    /**
     * Returns Boolean type value from common ims shared preference file
     */
    boolean getPreferenceBoolValue(String fileName, String key);

    /**
     * Returns Boolean type value from common ims shared preference file
     */
    boolean getPreferenceBoolValue(String key, int slotId);

    /**
     * Returns Boolean type value from common ims shared preference file
     */
    boolean getPreferenceBoolValue(String fileName, String key, int slotId);

    /**
     * Returns String type value from common ims shared preference file
     */
    String getPreferenceStrValue(String key);

    /**
     * Returns Boolean type value from target shared preference file
     */
    String getPreferenceStrValue(String fileName, String key);

    /**
     * Returns String type value from common ims shared preference file
     */
    String getPreferenceStrValue(String key, int slotId);

    /**
     * Returns Boolean type value from target shared preference file
     */
    String getPreferenceStrValue(String fileName, String key, int slotId);

    /**
     * Set String value to common ims shared preference file
     */
    void setPreferenceStrValue(String key, String value);

    /**
     * Set String value to target shared preference file
     */
    void setPreferenceStrValue(String fileName, String key, String value);

    /**
     * Set String value to common ims shared preference file
     */
    void setPreferenceStrValue(String key, String value, int slotId);

    /**
     * Set String value to target shared preference file
     */
    void setPreferenceStrValue(String fileName, String key, String value, int slotId);

    /**
     * Remove String value from common ims shared preference file
     */
    //2015-11-12 , joengtae.kim@ , add method for remove preference data
    void removePreferenceValue(String key);

    /**
     * Remove String value from target shared preference file
     */
    void removePreferenceValue(String fileName, String key);

    /**
     * Remove String value from common ims shared preference file
     */
    //2015-11-12 , joengtae.kim@ , add method for remove preference data
    void removePreferenceValue(String key, int slotId);

    /**
     * Remove String value from target shared preference file
     */
    void removePreferenceValue(String fileName, String key, int slotId);
}
