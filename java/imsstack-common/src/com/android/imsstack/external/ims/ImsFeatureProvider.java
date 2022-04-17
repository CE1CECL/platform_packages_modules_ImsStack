package com.android.imsstack.external.ims;

import java.io.File;

import android.content.ContentResolver;
import android.content.Context;
import android.content.pm.PackageManager;
import android.database.ContentObserver;
import android.net.Uri;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;

/**
 * This class provides the utility methods to get the Ims features.
 * Ims features may be followings:
 *   - com.android.imsstack.volte / supportvolte
 *   - com.android.imsstack.vt / supportvt
 *   - com.android.imsstack.vowifi / supportvowifi
 *   - com.android.imsstack.service.eab
 *
 *   - com.android.imsstack.server.sms
 *
 *   - com.android.imsstack.media.evs
 *   - com.android.imsstack.media.evs.wb
 *   - com.android.imsstack.media.camera
 *
 *   - com.android.imsstack.rtt
 *   - com.android.imsstack.callcomposer
 *
 * @hide
 */
public final class ImsFeatureProvider {
    /** Indicates that FEATURE_IMS is true or not. */
    public static boolean hasFeatureIms() {
        return true;
    }

    /** Indicates that IMS call over 5G SA(VoNR / EPS-FB) is supported or not. */
    public static boolean hasVoNr(Context c) {
        return ImsExternalFeature.FEATURE_VONR;
    }

    /** Indicates that VoLTE feature is allowed or not. */
    public static boolean hasVoLte(Context c) {
        return hasFeature(c, ImsExternalFeature.FEATURE_VOLTE);
    }

    /** Indicates that VT feature is allowed or not. */
    public static boolean hasVt(Context c) {
        return hasFeature(c, ImsExternalFeature.FEATURE_VT);
    }

    /** Indicates that VoWiFi (WFC) feature is allowed or not. */
    public static boolean hasVoWiFi(Context c) {
        return hasFeature(c, ImsExternalFeature.FEATURE_VOWIFI);
    }

    /** Indicates that EAB(Enhanced Address Book) feature is allowed or not. */
    public static boolean hasUce(Context c) {
        return hasFeature(c, ImsExternalFeature.FEATURE_UCE);
    }

    /** Indicates that RTT feature is allowed or not. */
    public static boolean hasRtt(Context c) {
        return hasFeature(c, ImsExternalFeature.FEATURE_RTT);
    }

    /** Indicates that Call Composer feature is allowed or not. */
    public static boolean hasCallComposer(Context c) {
        return hasFeature(c, ImsExternalFeature.FEATURE_CALLCOMPOSER);
    }

    /** Indicates that EVS feature is allowed or not. */
    public static boolean hasMediaEvs(Context c) {
        return hasFeature(c, ImsExternalFeature.FEATURE_MEDIA_EVS);
    }

    /** Indicates that the device supports EVS, but it doesn't support SWB/FB mode. */
    public static boolean hasMediaEvsWb(Context c) {
        return hasFeature(c, ImsExternalFeature.FEATURE_MEDIA_EVS_WB);
    }

    /** Indicates that IMS camera feature(including version) is allowed or not. */
    public static boolean hasMediaCamera(Context c, int version) {
        return hasFeature(c, ImsExternalFeature.FEATURE_MEDIA_CAMERA, version);
    }

    /** Indicates that hVoLTE feature is allowed or not. */
    public static boolean hasHVoLte(Context c) {
        return hasFeature(c, ImsExternalFeature.FEATURE_HVOLTE);
    }

    public static boolean hasHVoLteForPhone(Context c) {
        File file = ImsExternalFeature.getFile(ImsExternalFeature.FILE_XML_HVOLTE);
        return file != null;
    }

    /** Indicates that CdmaLess feature is allowed or not. */
    public static boolean hasCdmaLess(Context c) {
        return hasFeature(c, ImsExternalFeature.FEATURE_CDMALESS);
    }

    public static boolean hasCdmaLessForPhone(Context c) {
        File file = ImsExternalFeature.getFile(ImsExternalFeature.FILE_XML_CDMALESS);
        return file != null;
    }

    /** Indicates that IMS service is enabled or not. */
    public static boolean hasServerIms(Context c) {
        return hasFeatureIms();
    }

    /** Indicates that SMS service is provided or not. (SMS over IMS on AP) */
    public static boolean hasServerSms(Context c) {
        return hasFeature(c, ImsExternalFeature.FEATURE_SERVER_SMS);
    }

    // CHANGE_S [SMS_Patch_0621][VZW], MSG_SMS_SMS_TO_911 [
    public static boolean hasServerSmsSCBM(Context c) {
        return hasFeature(c, ImsExternalFeature.FEATURE_SERVER_SMS_SCBM);
    }

    public static boolean hasServerSmsSCBMForPhone(Context c) {
        File file = ImsExternalFeature.getFile(ImsExternalFeature.FILE_XML_SMS_SCBM);
        return file != null;
    }
    // CHANGE_E [SMS_Patch_0621][VZW], MSG_SMS_SMS_TO_911 ]

    public static boolean hasServerSmsForPhone(Context c) {
        File file = ImsExternalFeature.getFile(ImsExternalFeature.FILE_XML_SMS);
        return file != null;
    }

    public static boolean hasFeature(Context c, String name) {
        if (c == null || TextUtils.isEmpty(name)) {
            return false;
        }

        return hasFeatureInternal(c, name, null);
    }

    public static boolean hasFeature(Context c, String name, int version) {
        if (version <= 0) {
            return hasFeature(c, name);
        }

        if (c == null || TextUtils.isEmpty(name)) {
            return false;
        }

        Bundle args = new Bundle();
        args.putInt(ImsExternalFeature.KEY_ARG_VERSION, version);

        return hasFeatureInternal(c, name, args);
    }

    public static boolean registerObserver(Context c, ContentObserver observer) {
        if (c == null) {
            return false;
        }

        try {
            c.getContentResolver().registerContentObserver(
                    ImsExternalFeature.CONTENT_URI, true, observer);
        } catch (Throwable t) {
            return false;
        }

        return true;
    }

    public static void unregisterObserver(Context c, ContentObserver observer) {
        if (c == null) {
            return;
        }

        try {
            c.getContentResolver().unregisterContentObserver(observer);
        } catch (Throwable t) {
            // Ignore exception
            return;
        }
    }

    private static boolean hasFeatureInternal(Context c, String name, Bundle args) {
        ContentResolver cr = c.getContentResolver();
        Bundle result = null;

        try {
            result = cr.call(ImsExternalFeature.CONTENT_URI,
                    ImsExternalFeature.METHOD_HAS_FEATURE, name, args);
        } catch (NullPointerException e) {
            log(e.toString());
            result = null;
        } catch (IllegalArgumentException e) {
            log(e.toString());
            result = null;
        }

        return (result != null) ? result.getBoolean(ImsExternalFeature.KEY_RESULT, false) : false;
    }

    private static void log(String s) {
        Log.d("IMS", "[ImsFeatureProvider] " + s);
    }

    // Indicates that dual VoLte is enabled or not.
    // Only for SYSTEMUI
    // Do not use this method in other modules.
    // If required, contact IMS team.
    public static boolean hasDualVoLte(Context c) {
        String value = getProperty(c, "persist.vendor.xxx.ims.dualvolte");
        return "1".equals(value);
    }

    private static String getProperty(Context c, String name) {
        ContentResolver cr = c.getContentResolver();
        Bundle result = null;

        try {
            result = cr.call(ImsExternalFeature.CONTENT_URI, "getProperty", name, null);
        } catch (NullPointerException e) {
            log(e.toString());
            result = null;
        } catch (IllegalArgumentException e) {
            log(e.toString());
            result = null;
        }

        return (result != null) ? result.getString(ImsExternalFeature.KEY_RESULT, "") : "";
    }
}
