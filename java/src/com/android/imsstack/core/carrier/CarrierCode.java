package com.android.imsstack.core.carrier;

import java.util.Objects;

import com.android.imsstack.util.Log;

/**
 * This class provides the carrier information from "config/carrier_code.xml".
 */
public final class CarrierCode {
    public static final String TAG_PROFILE = "profile";
    public static final String KEY_CARRIER_CODE_FAST = "carrier_code_fast";
    public static final String KEY_CARRIER_CODE_LATE = "carrier_code_late";
    public static final String KEY_CARRIER_CODE_SUB = "carrier_code_sub";
    public static final String KEY_COUNTRY = "country";
    public static final String KEY_ICCID = "iccid";
    public static final String KEY_MCC = "mcc";
    public static final String KEY_MNC = "mnc";
    public static final String KEY_MVNO_TYPE = "mvno_type";
    public static final String KEY_MVNO_MATCH_DATA = "mvno_match_data";

    public static final String KEY_MVNO_TYPE_GID = "gid";
    public static final String KEY_MVNO_TYPE_IMSI = "imsi";
    public static final String KEY_MVNO_TYPE_SPN = "spn";

    public static final int MVNO_TYPE_NONE = 0;
    public static final int MVNO_TYPE_GID = 1;
    public static final int MVNO_TYPE_IMSI = 2;
    public static final int MVNO_TYPE_SPN = 3;

    public static final int SCORE_DEFAULT = -1;

    private String mSimIccId;
    private String mOperatorFast;
    private String mOperatorLate;
    private String mOperatorSub;
    private String mCountry;
    private String mIccId;
    private String mMcc;
    private String mMnc;
    private String mMvnoType;
    private String mMvnoMatchData;
    private int mScore;

    public CarrierCode(String operatorFast, String operatorLate, String operatorSub,
            String country, String iccId, String mcc, String mnc,
            String mvnoType, String mvnoMatchData) {
        mSimIccId = "";
        mOperatorFast = operatorFast;
        mOperatorLate = operatorLate;
        mOperatorSub = operatorSub;
        mCountry = country;
        mIccId = iccId;
        mMcc = mcc;
        mMnc = mnc;
        mMvnoType = mvnoType;
        mMvnoMatchData = mvnoMatchData;
        mScore = SCORE_DEFAULT;
    }

    public CarrierCode(String operatorFast, String operatorLate, String operatorSub,
            String country, String iccId, String mcc, String mnc,
            String mvnoType, String mvnoMatchData, int score) {
        mSimIccId = "";
        mOperatorFast = operatorFast;
        mOperatorLate = operatorLate;
        mOperatorSub = operatorSub;
        mCountry = country;
        mIccId = iccId;
        mMcc = mcc;
        mMnc = mnc;
        mMvnoType = mvnoType;
        mMvnoMatchData = mvnoMatchData;
        mScore = score;
    }

    public String getOperatorFast() {
        return mOperatorFast;
    }

    public String getOperatorLate() {
        return mOperatorLate;
    }

    public String getOperatorSub() {
        return mOperatorSub;
    }

    public String getCountry() {
        return mCountry;
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

    public String getMvnoType() {
        return mMvnoType;
    }

    public String getMvnoMatchData() {
        return mMvnoMatchData;
    }

    public int getScore() {
        return mScore;
    }

    public String getSimIccId() {
        return mSimIccId;
    }

    public void setSimIccId(String iccId) {
        mSimIccId = iccId;
    }

    @Override
    public int hashCode() {
        return Objects.hash(mOperatorFast, mOperatorLate, mOperatorSub, mCountry,
                mIccId, mMcc, mMnc, mMvnoType, mMvnoMatchData);
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }

        if (obj == null || getClass() != obj.getClass()) {
            return false;
        }

        CarrierCode cc = (CarrierCode)obj;

        return Objects.equals(mOperatorFast, cc.mOperatorFast)
                && Objects.equals(mOperatorLate, cc.mOperatorLate)
                && Objects.equals(mOperatorSub, cc.mOperatorSub)
                && Objects.equals(mCountry, cc.mCountry)
                && Objects.equals(mIccId, cc.mIccId)
                && Objects.equals(mMcc, cc.mMcc)
                && Objects.equals(mMnc, cc.mMnc)
                && Objects.equals(mMvnoType, cc.mMvnoType)
                && Objects.equals(mMvnoMatchData, cc.mMvnoMatchData);
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();

        sb.append("[ CarrierCode :: opFast=");
        sb.append(mOperatorFast);
        sb.append(", opLate=");
        sb.append(mOperatorLate);
        sb.append(", opSub=");
        sb.append(mOperatorSub);
        sb.append(", co=");
        sb.append(mCountry);
        sb.append(", iccId=");
        sb.append(mIccId);
        sb.append(", mcc=");
        sb.append(mMcc);
        sb.append(", mnc=");
        sb.append(mMnc);
        sb.append(", mvnoType=");
        sb.append(mMvnoType);
        sb.append(", mvnoMatchData=");
        sb.append(mMvnoMatchData);
        sb.append(", simIccId=");
        sb.append(Log.pii(mSimIccId));
        sb.append(" ]");

        return sb.toString();
    }
}
