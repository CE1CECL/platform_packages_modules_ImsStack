package com.android.imsstack.util;

import android.net.Uri;
import android.text.TextUtils;

import java.util.ArrayList;
import java.util.List;

/**
 * This class provides the carrier identifiers for IMS.
 */
public final class ImsCarrierId {
    // Constants values to identify the specific carrier from DB {
    public static final Uri MCCMNC_LIST = Uri.parse(
            "content://com.android.imsstack.provider.smart_configuration/mccmnc_list");
    public static final String OPERATOR = "operator";
    public static final String COUNTRY = "country";
    public static final String MCCMNC = "mccmnc";
    public static final String IMSI = "imsi";
    public static final String GID = "gid";
    public static final String SPN = "spn";
    // }

    private String mOperator = null;
    private String mCountry = null;
    // From SIM or DB {
    private String mMccMnc = null;
    private String mImsi = null;
    private String mGid = null;
    private String mSpn = null;
    private List<String> mMccMncList = new ArrayList<String>();
    // From SIM or DB }

    public ImsCarrierId(String op, String co, String mccMnc, String imsi, String gid, String spn){
        mOperator = op;
        mCountry = co;
        mMccMnc = mccMnc;
        mImsi = imsi;
        mGid = gid;
        mSpn = spn;
    }

    public ImsCarrierId(String mccMnc, String imsi, String gid, String spn){
        mMccMnc = mccMnc;
        mImsi = imsi;
        mGid = gid;
        mSpn = spn;
    }

    public String getOperator() {
        return mOperator;
    }

    public String getCountry() {
        return mCountry;
    }

    public String getMccMnc() {
        return mMccMnc;
    }

    public String getImsi() {
        return mImsi;
    }

    public String getGid() {
        return mGid;
    }

    public String getSpn() {
        return mSpn;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();

        sb.append("[ ImsCarrierId :: op=");
        sb.append(toStringOrNull(getOperator()));
        sb.append(", co=");
        sb.append(toStringOrNull(getCountry()));
        sb.append(", mccmnc=");
        sb.append(toStringOrNull(getMccMnc()));
        sb.append(", imsi=");
        sb.append(toStringOrNull(getImsi()));
        sb.append(", gid=");
        sb.append(toStringOrNull(getGid()));
        sb.append(", spn=");
        sb.append(toStringOrNull(getSpn()));
        sb.append(" ]");

        return sb.toString();
    }

    public static boolean equals(String a, String b, boolean ignoreCase) {
        boolean isEmptyA = TextUtils.isEmpty(a);
        boolean isEmptyB = TextUtils.isEmpty(b);

        if (isEmptyA && isEmptyB) {
            return true;
        }

        if (!isEmptyA && !isEmptyB) {
            return ignoreCase ? a.equalsIgnoreCase(b) : a.equals(b);
        }

        return false;
    }

    public static boolean matchImsi(String imsi, String prefixPattern) {
        if (TextUtils.isEmpty(prefixPattern)) {
            return true;
        }

        if (TextUtils.isEmpty(imsi)) {
            return false;
        }

        if (prefixPattern.length() > imsi.length()) {
            // Wrong IMSI value
            return false;
        }

        for (int i = 0; i < prefixPattern.length(); i++) {
            char ch = prefixPattern.charAt(i);

            if ((ch != 'x') && (ch != 'X') && (ch != imsi.charAt(i))) {
                return false;
            }
        }

        return true;
    }

    private static String toStringOrNull(String s) {
        return (s != null) ? s : "(null)";
    }
}
