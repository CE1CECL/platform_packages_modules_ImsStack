package com.android.imsstack.core.config;

public final class ConfigurationFactory {

    public static IConfigDBLoader newConfigDBLoader(int slotId) {
        return new ConfigDBLoaderImpl(slotId);
    }
}
