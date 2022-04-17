
package com.android.imsstack.enabler.mtc;

import java.util.Hashtable;

/**
 * This class provides the utility methods to control items of SuppInfo.
 */
public final class SuppInfoUtils {
    public static final int TYPE_BOOLEAN = 1;
    public static final int TYPE_INT = 2;
    public static final int TYPE_STRING = 3;

    private static final Hashtable<Integer, String> sTypeToKey
            = new Hashtable<Integer, String>();
    private static final Hashtable<Integer, Integer> sTypeToValueType
            = new Hashtable<Integer, Integer>();

    public static void addKey(int type, String key) {
        sTypeToKey.put(type, key);
    }

    public static void removeKey(int type) {
        sTypeToKey.remove(type);
    }

    public static void addValueType(int type, int valueType) {
        sTypeToValueType.put(type, valueType);
    }

    public static void removeValueType(int type) {
        sTypeToValueType.remove(type);
    }

    public static String getKey(int type) {
        return sTypeToKey.get(type);
    }

    public static boolean isValueBoolean(int type) {
        Integer valueType = sTypeToValueType.get(type);
        return (valueType != null) ? (valueType.intValue() == TYPE_BOOLEAN) : false;
    }

    public static boolean isValueInt(int type) {
        Integer valueType = sTypeToValueType.get(type);
        return (valueType != null) ? (valueType.intValue() == TYPE_INT) : false;
    }

    public static boolean isValueString(int type) {
        Integer valueType = sTypeToValueType.get(type);
        return (valueType != null) ? (valueType.intValue() == TYPE_STRING) : false;
    }
}
