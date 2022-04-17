package com.android.imsstack.enabler.mtc;

import android.os.Parcel;
import android.os.Parcelable;

import com.android.imsstack.util.ImsLog;

public class FailInfo implements Parcelable
{
    public int          Reason;
    public int          Code;
    public String       Phrase;

    //------------------------------------------------------------------------------------------//

    public FailInfo() {
        ImsLog.i("");

        Reason = -1;
        Code = -1;
        Phrase = "";

        logIn();
    }

    public FailInfo(FailInfo failInfo) {
        ImsLog.i("");

        Reason = failInfo.Reason;
        Code = failInfo.Code;
        Phrase = failInfo.Phrase;

        logIn();
    }

    public FailInfo(Parcel source) {
        ImsLog.i("");
        readFromParcel(source);
    }

    public FailInfo(int _Reason, int _Code, String _Phrase) {
        ImsLog.i("");

        Reason = _Reason;
        Code = _Code;
        Phrase = _Phrase;

        logIn();
    }
    public void logIn() {
        ImsLog.i("Reason : " + Reason
                + " Code : " + Code
                + " Phrase : " + Phrase
                 );
    }

    public void readFromParcel(Parcel source) {
        ImsLog.i("");

        Reason = source.readInt();
        Code = source.readInt();
        Phrase = source.readString();

        logIn();
    }

    public void writeToParcel(Parcel dest, int flags) {
        ImsLog.i("");

        dest.writeInt(Reason);
        dest.writeInt(Code);
        dest.writeString(Phrase);
    }

    public int describeContents() {
        return 0;
    }

    public static final Parcelable.Creator<FailInfo> CREATOR
            = new Parcelable.Creator<FailInfo>() {
        public FailInfo createFromParcel(Parcel source) {
            try {
                return new FailInfo(source);
            } catch (Exception e) {
                ImsLog.e("Exception occurred when creating FailInfo from parcel", e);
            }
            return null;
        }

        public FailInfo[] newArray(int size) {
            return new FailInfo[size];
        }
    };

};
