package com.android.imsstack.util;

import android.text.TextUtils;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

/**
 * This class provides Reflect wrapper APIs.
 */
public final class ReflectApi {
    public static Object newInstance(Class<?> cls, Class<?>[] clsArgs, Object[] args) {
        if (cls == null) {
            ImsLog.d("Class is null");
            return null;
        }

        try {
            Constructor constructor = cls.getConstructor(clsArgs);

            if (args == null) {
                return constructor.newInstance();
            } else {
                return constructor.newInstance(args);
            }
        } catch (NoSuchMethodException e) {
            ImsLog.d("ReflectApi :: " + e.toString());
            e.printStackTrace();
        } catch (InstantiationException e) {
            ImsLog.d("ReflectApi :: " + e.toString());
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            ImsLog.d("ReflectApi :: " + e.toString());
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            ImsLog.d("ReflectApi :: " + e.toString());
            e.printStackTrace();
        }

        return null;
    }

    public static Class getClass(String name) {
        try {
            return Class.forName(name);
        } catch (ClassNotFoundException e) {
            ImsLog.d("ReflectApi :: " + e.toString());
            e.printStackTrace();
        }

        return null;
    }

    public static Method getMethod(Class<?> cls, String name, Class... parameterTypes) {
        if (cls == null) {
            ImsLog.d("Class is null");
            return null;
        }

        if (TextUtils.isEmpty(name)) {
            ImsLog.d("Method name is empty");
            return null;
        }

        try {
            return cls.getMethod(name, parameterTypes);
        } catch (NoSuchMethodException e) {
            ImsLog.d("ReflectApi :: " + e.toString());
            e.printStackTrace();
        }

        return null;
    }

    public static Object invoke(Method method, Object receiver, Object... args) {
        if (method == null) {
            ImsLog.d("Method is null");
            return null;
        }

        if (receiver == null) {
            ImsLog.d("Receiver is null");
            return null;
        }

        try {
            return method.invoke(receiver, args);
        } catch (IllegalAccessException e) {
            ImsLog.d("ReflectApi :: " + e.toString());
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            ImsLog.d("ReflectApi :: " + e.toString());
            e.printStackTrace();
        }

        return null;
    }
}
