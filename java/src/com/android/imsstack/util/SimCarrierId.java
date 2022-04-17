package com.android.imsstack.util;

import android.telephony.TelephonyManager;

import java.util.Objects;

/**
 * This class provides the carrier information from SIM card.
 */
public final class SimCarrierId {
    public static final int UNKNOWN_ID = TelephonyManager.UNKNOWN_CARRIER_ID;
    public static final int GOOGLE_FI = 1989;

    private int mCarrierId;
    private int mSpecificCarrierId;
    private String mMcc;
    private String mMnc;
    private String mImsi;
    private String mGid1;
    private String mSpn;
    private String mIccId;
    private boolean mSimLocked = false;

    public SimCarrierId(int carrierId, int specificCarrierId) {
        this(carrierId, specificCarrierId, null, null, null, null, null, null, false);
    }
    public SimCarrierId(String iccId) {
        this(null, null, null, null, null, iccId);
    }

    public SimCarrierId(String mcc, String mnc, String imsi,
            String gid1, String spn, String iccId) {
        this(mcc, mnc, imsi, gid1, spn, iccId, false);
    }

    public SimCarrierId(String mcc, String mnc, String imsi,
            String gid1, String spn, String iccId, boolean simLocked) {
        this(UNKNOWN_ID, UNKNOWN_ID, mcc, mnc, imsi, gid1, spn, iccId, simLocked);
    }

    public SimCarrierId(int carrierId, int specificCarrierId,
            String mcc, String mnc, String imsi,
            String gid1, String spn, String iccId, boolean simLocked) {
        mCarrierId = carrierId;
        mSpecificCarrierId = specificCarrierId;
        mMcc = mcc;
        mMnc = mnc;
        mImsi = imsi;
        mGid1 = gid1;
        mSpn = spn;
        mIccId = iccId;
        mSimLocked = simLocked;
    }

    public int getCarrierId() {
        return mCarrierId;
    }

    public int getSpecificCarrierId() {
        return mSpecificCarrierId;
    }

    public String getIccId() {
        return mIccId;
    }

    public String getMcc() {
        return mMcc;
    }

    public String getMnc() {
        return mMnc;
    }

    public String getImsi() {
        return mImsi;
    }

    public String getGid1() {
        return mGid1;
    }

    public String getSpn() {
        return mSpn;
    }

    public boolean isSimLocked() {
        return mSimLocked;
    }

    @Override
    public int hashCode() {
        return Objects.hash(mCarrierId, mSpecificCarrierId,
                mMcc, mMnc, mImsi, mGid1, mSpn, mIccId);
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }

        if (obj == null || getClass() != obj.getClass()) {
            return false;
        }

        SimCarrierId scid = (SimCarrierId)obj;

        if (mCarrierId == UNKNOWN_ID && scid.mCarrierId == UNKNOWN_ID
                && mSpecificCarrierId == UNKNOWN_ID && scid.mSpecificCarrierId == UNKNOWN_ID) {
            return Objects.equals(mMcc, scid.mMcc)
                    && Objects.equals(mMnc, scid.mMnc)
                    && Objects.equals(mImsi, scid.mImsi)
                    && Objects.equals(mGid1, scid.mGid1)
                    && Objects.equals(mSpn, scid.mSpn)
                    && Objects.equals(mIccId, scid.mIccId);
        }

        return (mCarrierId == scid.mCarrierId)
                && (mSpecificCarrierId == scid.mSpecificCarrierId);
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();

        sb.append("[ SimCarrierId :: carrierId=");
        sb.append(mCarrierId);
        sb.append(", specificCarrierId=");
        sb.append(mSpecificCarrierId);
        sb.append(", mcc=");
        sb.append(mMcc);
        sb.append(", mnc=");
        sb.append(mMnc);
        sb.append(", imsi=");
        sb.append(Log.pii(mImsi));
        sb.append(", gid1=");
        sb.append(mGid1);
        sb.append(", spn=");
        sb.append(mSpn);
        sb.append(", iccId=");
        sb.append(Log.pii(mIccId));
        sb.append(", locked=");
        sb.append(mSimLocked);
        sb.append(" ]");

        return sb.toString();
    }
}
