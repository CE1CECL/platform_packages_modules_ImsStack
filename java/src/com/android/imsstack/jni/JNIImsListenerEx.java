package com.android.imsstack.jni;

import android.os.Parcel;

import java.io.FileDescriptor;

public interface JNIImsListenerEx
{
    byte[] onMessage(Parcel parcelIn, FileDescriptor fd);
}
