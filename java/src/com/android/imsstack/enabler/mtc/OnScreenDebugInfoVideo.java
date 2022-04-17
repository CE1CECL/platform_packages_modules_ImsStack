package com.android.imsstack.enabler.mtc;

import android.os.Parcel;
import android.os.Parcelable;

import com.android.imsstack.util.ImsLog;

public class OnScreenDebugInfoVideo implements Parcelable {

    public int sendBitRate;
    public int recvBitRate;
    public int sendFrameRate;
    public int recvFrameRate;

    public int videoQualityIndicator;

    //------------------------------------------------------------------------------------------//

    public OnScreenDebugInfoVideo(Parcel source) {
        ImsLog.d("");
        readFromParcel(source);
    }

    public OnScreenDebugInfoVideo(int nVQI) {
        ImsLog.d("");
        readFromData(nVQI);
    }

    public OnScreenDebugInfoVideo(int _sendBitRate, int _recvBitRate, int _sendFrameRate, int _recvFrameRate) {
        ImsLog.d("");

        sendBitRate = _sendBitRate;
        recvBitRate = _recvBitRate;
        sendFrameRate = _sendFrameRate;
        recvFrameRate = _recvFrameRate;

        ImsLog.d("sendBitRate : " + sendBitRate
                + " recvBitRate : " + recvBitRate
                + " sendFrameRate : " + sendFrameRate
                + " recvFrameRate : " + recvFrameRate
                 );
    }

    public void logIn() {
        ImsLog.d("sendBitRate : [" + sendBitRate
                + "], recvBitRate : [" + recvBitRate
                + "], sendFrameRate : [" + sendFrameRate
                + "], recvFrameRate : [" + recvFrameRate
                + "], videoQualityIndicator : [" + videoQualityIndicator
                + "]"
                 );
    }

    public void readFromParcel(Parcel source) {
        sendBitRate     = source.readInt();
        recvBitRate     = source.readInt();
        sendFrameRate   = source.readInt();
        recvFrameRate   = source.readInt();

        videoQualityIndicator   = source.readInt();
    }

    public void readFromData(int nVQI) {
        sendBitRate     = 0;
        recvBitRate     = 0;
        sendFrameRate   = 0;
        recvFrameRate   = 0;

        videoQualityIndicator   = nVQI;
    }

    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(sendBitRate);
        dest.writeInt(recvBitRate);
        dest.writeInt(sendFrameRate);
        dest.writeInt(recvFrameRate);
        dest.writeInt(videoQualityIndicator);
    }

    public int describeContents() {
        return 0;
    }

    public static final Parcelable.Creator<OnScreenDebugInfoVideo> CREATOR
            = new Parcelable.Creator<OnScreenDebugInfoVideo>() {
        public OnScreenDebugInfoVideo createFromParcel(Parcel source) {
            try {
                return new OnScreenDebugInfoVideo(source);
            } catch (Exception e) {
                ImsLog.e("Exception occurred when creating OnScreenDebugInfoVideo from parcel");
            }
            return null;
        }

        public OnScreenDebugInfoVideo[] newArray(int size) {
            return new OnScreenDebugInfoVideo[size];
        }
    };
};
