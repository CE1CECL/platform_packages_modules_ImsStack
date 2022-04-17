package com.android.imsstack.enabler.uce.impl.subscribe;

import com.android.imsstack.util.ImsLog;
import com.android.imsstack.enabler.uce.impl.define.UceMessage;
import com.android.imsstack.enabler.uce.impl.jni.UceJNI;
import com.android.imsstack.enabler.uce.interf.UceApiConstant;
import com.android.imsstack.enabler.uce.interf.SubscribeResponse;

import android.os.Parcel;
import android.text.TextUtils;
import android.net.Uri;
import android.util.Pair;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.TimeUnit;

public class UceSubscribeRequest {
    private int mSlotId;
    private SubscribeResponse callback;
    private int mKey;

    public UceSubscribeRequest(SubscribeResponse cb, int slotId, int key) {
        mKey = key;
        callback = cb;
        mSlotId = slotId;
    }

    public boolean sendRequest(ArrayList<String> remoteUris) {
        ImsLog.i("");
        if (remoteUris == null || remoteUris.size() == 0) {
            ImsLog.e("remoteUris is empty");
            informCommandError(UceApiConstant.COMMAND_CODE_INVALID_PARAM);
            return false;
        }
        Parcel parcel = Parcel.obtain();

        int size = remoteUris.size();
        if (size == 1) {
            parcel.writeInt(UceMessage.UCE_SEND_SINGLE_SUBSCRIBE_CMD);
        } else {
            parcel.writeInt(UceMessage.UCE_SEND_LIST_SUBSCRIBE_CMD);
        }
        parcel.writeInt(mKey);
        parcel.writeInt(size);
        for (int i = 0; i < size; i++) {
            parcel.writeString(remoteUris.get(i));
        }
        UceJNI.getInstance().sendMessage(mSlotId, parcel);
        return true;
    }

    public void informNetworkResponse(int responseCode, String reason,
        int reasonHdrCause, String reasonHdrText) {
        ImsLog.d("informNetworkResponse:responseCode=" + responseCode +
            ", reasonHdrCause=" + reasonHdrCause);
        try {
            if (reasonHdrCause == 0) {
                callback.onNetworkResponse(responseCode, reason);
            } else {
                callback.onNetworkResponse(responseCode, reason, reasonHdrCause, reasonHdrText);
            }
        } catch (Exception e) {
            ImsLog.e("Exception:" + e.toString());
        }
    }

    public void informCommandError(int code) {
        ImsLog.d("informCommandError:code=" + code);
        try {
            callback.onCommandError(code);
        } catch (Exception e) {
            ImsLog.e("Exception:" + e.toString());
        }
    }

    public void informCapabilitiesUpdate(List<String> pidfXmls) {
        try {
            callback.onNotifyCapabilitiesUpdate(pidfXmls);
        } catch (Exception e) {
            ImsLog.e("Exception:" + e.toString());
        }
    }

    public void informTerminate(String reason, int retryAfterSecond) {
        try {
            callback.onTerminated(reason, TimeUnit.SECONDS.toMillis(retryAfterSecond));
        } catch (Exception e) {
            ImsLog.e("Exception:" + e.toString());
        }
    }

    public void informResourceTerminate(ArrayList<UceResourceInfo> resourceInfoList) {
        List<Pair<Uri, String>> uriTerminatedReason = new ArrayList<>();
        for (UceResourceInfo info : resourceInfoList) {
            uriTerminatedReason.add(new Pair<>(Uri.parse(info.getId()), info.getReason()));
        }
        try {
            callback.onResourceTerminated(uriTerminatedReason);
        } catch (Exception e) {
            ImsLog.e("Exception:" + e.toString());
        }
    }
}
