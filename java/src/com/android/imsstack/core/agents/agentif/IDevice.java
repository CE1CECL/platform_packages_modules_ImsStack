package com.android.imsstack.core.agents.agentif;

public interface IDevice extends IAgent {
    /**
     * Returns the application directory path if present.
     */
    String getApplicationPath(String packageName);

    /**
     * Returns device name from 'xxx_device_name' setting database.
     */
    String getDeviceName();

    /**
     * Returns the external storage path if it's already mounted.
     */
    String getExternalStoragePath();
}
