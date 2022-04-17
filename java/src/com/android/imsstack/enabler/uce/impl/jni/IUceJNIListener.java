package com.android.imsstack.enabler.uce.impl.jni;

import android.os.Parcel;

public interface IUceJNIListener {
    default void onPublishResponseMessage(Parcel parcel) {}
    default void onSubscribeResponseMessage(Parcel parcel) {}
    default void onOptionsResponseMessage(Parcel parcel) {}
    default void onServiceStatusMessage(Parcel parcel) {}
    default void onNetworkStatusMessage(Parcel parcel) {}
    default void onReceivedRemoteOptionsMessage(Parcel parcel) {}
    default void onPublishStatusMessage(Parcel parcel) {}
}
