package com.android.imsstack.util;

import android.content.ContentResolver;
import android.content.Context;
import android.os.Bundle;
import android.text.TextUtils;

import com.android.imsstack.external.ims.ImsExternalFeature;

/*
 * Ims features may be followings:
 *   - com.android.imsstack.disable_ims
 *   - com.android.imsstack.volte / supportvolte
 *   - com.android.imsstack.vt / supportvt
 *   - com.android.imsstack.vowifi / supportvowifi
 *   - com.android.imsstack.service.eab
 *
 *   - com.android.imsstack.imsphone
 *   - com.android.imsstack.server.ims
 *   - com.android.imsstack.server.sms
 *
 *   - com.android.imsstack.media.evs
 *   - com.android.imsstack.media.hevc
 *
 *   - com.android.imsstack.vrbt
 */
public final class ImsFeature {
    /**
     * Argument keys to update IMS features.
     */
    public static final String KEY_ARG_OPERATOR = "operator";
    public static final String KEY_ARG_COUNTRY = "country";

    /**
     * IMS disable feature: it's a runtime feature to block IMS functionalities.
     * It can be used to disable IMS without the removal of ported IMS modules.
     */
    public static final String FEATURE_DISABLE_IMS = "com.android.imsstack.disable_ims";

    // DSDV-SV
    public static final String FEATURE_DSDV_SV = "com.android.imsstack.dsdv_sv";
    public static final String FEATURE_MEDIA_HEVC = "com.android.imsstack.media.hevc";
    public static final String FEATURE_VRBT = "com.android.imsstack.vrbt";

    public static final String FEATURE_SMSONLY = "com.android.imsstack.smsonly";
    public static final String FEATURE_EPSONLY = "com.android.imsstack.epsonly";
    public static final String FEATURE_VOLTEONLY = "com.android.imsstack.volteonly";
    public static final String FEATURE_LTEONLY = "com.android.imsstack.lteonly";

    /** VZW */
    public static final String FEATURE_ROAMING = "com.android.imsstack.roaming";
    public static final String FEATURE_VERIZON_IMS = "com.verizon.ims";

    /** ATT */
    public static final String FEATURE_STIR_SHAKEN = "com.android.imsstack.stir.shaken";

    public static boolean reloadFeatures(Context c) {
        return reloadFeatures(c, null, null);
    }

    public static boolean reloadFeatures(Context c, String operator, String country) {
        ContentResolver cr = c.getContentResolver();
        Bundle result = null;
        Bundle args = null;

        if (!TextUtils.isEmpty(operator) && !TextUtils.isEmpty(country)) {
            args = new Bundle();
            args.putString(KEY_ARG_OPERATOR, operator);
            args.putString(KEY_ARG_COUNTRY, country);
        }

        try {
            result = cr.call(ImsExternalFeature.CONTENT_URI,
                    ImsExternalFeature.METHOD_UPDATE_FEATURE, null, args);
        } catch (NullPointerException e) {
            e.printStackTrace();
            result = null;
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
            result = null;
        }

        return (result != null) ? result.getBoolean(ImsExternalFeature.KEY_RESULT, false) : false;
    }
}
