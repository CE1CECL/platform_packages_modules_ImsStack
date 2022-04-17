package com.android.imsstack.external.ims;

import java.io.File;

import android.net.Uri;

/**
 * Ims features may be followings:
 *   - com.android.imsstack.volte / supportvolte
 *   - com.android.imsstack.vt / supportvt
 *   - com.android.imsstack.vowifi / supportvowifi
 *   - com.android.imsstack.eab
 *
 *   - com.android.imsstack.server.sms
 *
 *   - com.android.imsstack.media.evs
 *   - com.android.imsstack.media.evs.wb
 *   - com.android.imsstack.media.camera
 *
 *   - com.android.imsstack.rtt
 *   - com.android.imsstack.callcomposer
 */
public final class ImsExternalFeature {
    public static final String FEATURE_VOLTE = "com.android.imsstack.volte";
    public static final String FEATURE_VT = "com.android.imsstack.vt";
    public static final String FEATURE_VOWIFI = "com.android.imsstack.vowifi";
    public static final String FEATURE_UCE = "com.android.imsstack.uce";

    public static final String FEATURE_HVOLTE = "com.android.imsstack.hvolte";
    public static final String FEATURE_CDMALESS = "com.android.imsstack.cdmaless";
    public static final String FEATURE_RTT = "com.android.imsstack.rtt";
    public static final String FEATURE_CALLCOMPOSER = "com.android.imsstack.callcomposer";

    public static final String FEATURE_SERVER_SMS = "com.android.imsstack.server.sms";
    // CHANGE [SMS_Patch_0621][VZW], MSG_SMS_SMS_TO_911
    public static final String FEATURE_SERVER_SMS_SCBM = "com.android.imsstack.server.sms.scbm";

    public static final String FEATURE_MEDIA_EVS = "com.android.imsstack.media.evs";
    // The device supports EVS, but it doesn't support SWB/FB modes.
    public static final String FEATURE_MEDIA_EVS_WB = "com.android.imsstack.media.evs.wb";
    public static final String FEATURE_MEDIA_CAMERA = "com.android.imsstack.media.camera";

    public static final boolean FEATURE_VOLTE_OPEN = false;
    public static final boolean FEATURE_VONR = false;

    /** Internal usages */
    public static final String AUTHORITY = "com.android.imsstack.provider.ims_feature";
    public static final Uri CONTENT_URI = Uri.parse("content://" + AUTHORITY);

    public static final String METHOD_HAS_FEATURE = "hasFeature";
    public static final String METHOD_UPDATE_FEATURE = "updateFeature";
    public static final String KEY_ARG_VERSION = "version";
    public static final String KEY_RESULT = "result";

    public static final String PATH_CUPSS_DEFAULT = getDefaultCupssPath(null);
    public static final String PATH_CUPSS_ROOTDIR = PATH_CUPSS_DEFAULT;
    public static final String PATH_CONFIG_LEGACY = PATH_CUPSS_ROOTDIR + "/config";
    public static final String PATH_CONFIG_ETC = PATH_CUPSS_ROOTDIR + "/etc";

    public static final String FILE_XML = "com.android.imsstack.xml";
    public static final String FILE_XML_HVOLTE = "com.android.imsstack.hvolte.xml";
    public static final String FILE_XML_CDMALESS = "com.android.imsstack.cdmaless.xml";
    public static final String FILE_XML_SMS = "com.android.imsstack.server.sms.xml";
    // CHANGE [SMS_Patch_0621][VZW], MSG_SMS_SMS_TO_911
    public static final String FILE_XML_SMS_SCBM = "com.android.imsstack.server.sms.scbm.xml";

    public static File getFile(String name) {
        File file = null;

        try {
            file = new File(getCupssConfigDir(), name);

            if (!file.exists()) {
                return null;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        return file;
    }

    public static String getDefaultCupssPath(String defValue) {
        final boolean overlayFs = true;
        final String defaultRootPath = overlayFs ? "/product" : "/product/OP";
        final String cupssOP = (defValue != null) ? defValue : defaultRootPath;
        return (cupssOP != null) ? cupssOP : ((defValue != null) ? defValue : defaultRootPath);
    }

    public static String getCupssConfigDir() {
        final boolean overlayFs = true;
        return overlayFs ? PATH_CONFIG_ETC : PATH_CONFIG_LEGACY;
    }
}
