package com.android.imsstack.enabler.mtc;

import android.os.Parcel;
import android.os.Parcelable;

import com.android.imsstack.util.ImsLog;

public class IncomingCallInfo implements Parcelable
{
    public int mServiceType;
    public int mCallType;
    public int mOIR;
    public int mCNAP;
    public String mOI;
    public String mCNA;
    public boolean mVideoCapable;

    /* OIP_Type */
    public static final int OIPTYPE_INVALID      = -1;
    public static final int OIPTYPE_NONE         = 0;
    public static final int OIPTYPE_IDENTITY     = 1;
    public static final int OIPTYPE_RESTRICTED    = 2;

    //------------------------------------------------------------------------------------------//

    public IncomingCallInfo(Parcel in) {
        readFromParcel(in);
    }

    public void logIn() {
        ImsLog.d("serviceType : " + mServiceType + " callType : " + mCallType + " OIR : "
                + mOIR + " CNAP : " + mCNAP + " Number : " + mOI + " Name : " + mCNA
                + " videoCapable : " + mVideoCapable);
    }

    public void readFromParcel(Parcel in) {
        mServiceType = in.readInt();
        mCallType = in.readInt();
        mOIR = in.readInt();
        mCNAP = in.readInt();
        mOI = in.readString();
        mCNA = in.readString();
        mVideoCapable = (in.readInt() == 1) ? true : false;

        logIn();
    }

    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(mServiceType);
        dest.writeInt(mCallType);
        dest.writeInt(mCNAP);
        dest.writeString(mOI);
        dest.writeString(mCNA);
        dest.writeInt(mVideoCapable ? 1 : 0);
    }

    public int describeContents() {
        return 0;
    }

    public static final Parcelable.Creator CREATOR = new Parcelable.Creator() {
        public IncomingCallInfo createFromParcel(Parcel in) {
            return new IncomingCallInfo(in);
        }

        public IncomingCallInfo[] newArray(int size) {
            return new IncomingCallInfo[size];
        }
    };
};
