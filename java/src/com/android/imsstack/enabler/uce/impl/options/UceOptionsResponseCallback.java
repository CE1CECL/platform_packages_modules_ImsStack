package com.android.imsstack.enabler.uce.impl.options;

import android.os.Parcel;

import com.android.imsstack.util.ImsLog;
import com.android.imsstack.enabler.uce.impl.define.UceMessage;
import com.android.imsstack.enabler.uce.impl.jni.UceJNI;
import com.android.imsstack.enabler.uce.impl.utils.UceUtils;
import com.android.imsstack.enabler.uce.interf.RemoteOptionsCallback;
import com.android.internal.annotations.VisibleForTesting;

import java.util.Set;

public class UceOptionsResponseCallback implements RemoteOptionsCallback{
    @VisibleForTesting
    public int mKey = 0;
    private int mSlotId = -1;

    public UceOptionsResponseCallback(int key, int slotId) {
        mKey = key;
        mSlotId = slotId;
        return;
    }

    @Override
    public void onRespondToCapabilityRequest(Set<String> ownCapabilities, boolean isBlocked) {
        Parcel parcel = Parcel.obtain();
        parcel.writeInt(UceMessage.UCE_SEND_OPTIONS_RESP_CMD);

        parcel.writeInt(mKey);
        parcel.writeInt(200); // response code
        parcel.writeString(""); // reason
        if (isBlocked) {
            parcel.writeLong(0); // my capabilities
        } else{
            if (ownCapabilities.isEmpty()) {
                parcel.writeLong(0); // my capabilities
            } else {
                parcel.writeLong(UceUtils.getCapabilities(ownCapabilities));
            }
        }
        UceJNI.getInstance().sendMessage(mSlotId, parcel);
    }

    @Override
    public void onRespondToCapabilityRequestWithError(int code, String reason) {
        Parcel parcel = Parcel.obtain();
        parcel.writeInt(UceMessage.UCE_SEND_OPTIONS_RESP_CMD);

        parcel.writeInt(mKey);
        parcel.writeInt(code); // response code
        parcel.writeString(reason); // reason
        parcel.writeLong(0); // my capabilities
        UceJNI.getInstance().sendMessage(mSlotId, parcel);
    }
}
