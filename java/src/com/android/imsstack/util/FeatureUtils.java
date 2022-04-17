package com.android.imsstack.util;

import android.content.Context;

import com.android.imsstack.external.ims.ImsExternalFeature;
import com.android.imsstack.external.ims.ImsFeatureProvider;
import com.android.imsstack.util.ImsFeature;
import com.android.imsstack.util.ImsLog;

/**
 * This class provides the APIs to check if a feature is supported or not.
 */
public final class FeatureUtils {
    /** common */
    private static final int FLAG_SYSTEM_SERVER_IMS_PHONE = 0x00000001;
    private static final int FLAG_SYSTEM_SERVER_IMS = 0x00000002;
    private static final int FLAG_SYSTEM_SERVER_SMS = 0x00000004;
    /** DSDV-SV (Dual SIM Dual VoLTE - Single VoLTE) */
    private static final int FLAG_DSDV_SV = 0x00000008;
    /** DSSV-DV (Dual SIM Single VoLTE - Dual VoLTE for emergency) */
    private static final int FLAG_DSSV_DV = 0x00000020;
    private static final int FLAG_RTT = 0x00000040;
    /** VoNR */
    private static final int FLAG_VONR = 0x00000080;
    // IMS_CHANGE [SMS_Patch_0621][VZW], MSG_SMS_SMS_TO_911
    private static final int FLAG_SYSTEM_SERVER_SMS_SCBM = 0x00000100;
    /** Call Composer */
    private static final int FLAG_CALL_COMPOSER = 0x00000200;
    /** VRBT */
    private static final int FLAG_VRBT = 0x00000400;

    /** media */
    private static final int FLAG_MEDIA_EVS = 0x00010000;
    private static final int FLAG_MEDIA_HEVC = 0x00040000;
     private static final int FLAG_MEDIA_CAMERA_V2 = 0x00080000;
    /** operator specific services */
    private static final int FLAG_STIR_SHAKEN = 0x00100000;
    private static final int FLAG_ROAMING = 0x00200000;
    private static final int FLAG_CDMALESS = 0x00400000;
    private static final int FLAG_LTEONLY = 0x00800000;
    private static final int FLAG_HVOLTE = 0x01000000;
    private static final int FLAG_VOWIFI = 0x04000000;
    private static final int FLAG_SMSONLY = 0x08000000;
    private static final int FLAG_EPSONLY = 0x20000000;
    private static final int FLAG_VOLTEONLY = 0x40000000;
    private static final int FLAG_UCE = 0x80000000;

    private static boolean sInitialized = false;
    private static int sFeatures = 0;

    public static synchronized void init(Context c) {
        if (c == null) {
            return;
        }

        if (sInitialized) {
            return;
        }

        setCommonFeatures(c);
        setMediaFeatures(c);
        setOperatorSpecificFeatures(c);

        sInitialized = true;
    }

    public static synchronized void updateAll(Context c) {
        sInitialized = false;

        sFeatures = 0;

        init(c);
    }

    public static synchronized int getFeatures() {
        return sFeatures;
    }

    public static void addFeature(String feature) {
        if (feature == null) {
            return;
        }

        setFeature(getFeatureFlag(feature));
    }

    public static boolean isImsPhoneSystemServerSupported(Context c) {
        return true;
    }

    public static boolean isImsSystemServerSupported(Context c) {
        return true;
    }

    public static boolean isSmsSystemServerSupported(Context c) {
        if (isInitialized()) {
            return hasFeature(FLAG_SYSTEM_SERVER_SMS);
        } else {
            return ImsFeatureProvider.hasServerSms(c);
        }
    }

    public static boolean isMediaEvsSupported(Context c) {
        if (isInitialized()) {
            return hasFeature(FLAG_MEDIA_EVS);
        } else {
            return ImsFeatureProvider.hasMediaEvs(c);
        }
    }

    public static boolean isMediaHevcSupported(Context c) {
        if (isInitialized()) {
            return hasFeature(FLAG_MEDIA_HEVC);
        } else {
            return ImsFeatureProvider.hasFeature(c, ImsFeature.FEATURE_MEDIA_HEVC);
        }
    }

    public static boolean isHVolteSupported(Context c) {
        if (isInitialized()) {
            return hasFeature(FLAG_HVOLTE);
        } else {
            return ImsFeatureProvider.hasHVoLte(c);
        }
    }

    public static boolean isVoWiFiSupported(Context c) {
        if (isInitialized()) {
            return hasFeature(FLAG_VOWIFI);
        } else {
            return ImsFeatureProvider.hasVoWiFi(c);
        }
    }

    public static boolean isSMSOnlySupported(Context c) {
        if (isInitialized()) {
            return hasFeature(FLAG_SMSONLY);
        } else {
            return ImsFeatureProvider.hasFeature(c, ImsFeature.FEATURE_SMSONLY);
        }
    }

    public static boolean isEPSOnlySupported(Context c) {
        if (isInitialized()) {
            return hasFeature(FLAG_EPSONLY);
        } else {
            return ImsFeatureProvider.hasFeature(c, ImsFeature.FEATURE_EPSONLY);
        }
    }

    public static boolean isVoLTEOnlySupported(Context c) {
        if (isInitialized()) {
            return hasFeature(FLAG_VOLTEONLY);
        } else {
            return ImsFeatureProvider.hasFeature(c, ImsFeature.FEATURE_VOLTEONLY);
        }
    }

    public static boolean isRoamingSupported(Context c) {
        if (isInitialized()) {
            return hasFeature(FLAG_ROAMING);
        } else {
            return ImsFeatureProvider.hasFeature(c, ImsFeature.FEATURE_ROAMING);
        }
    }

    public static boolean isCdmaLessSupported(Context c) {
        if (isInitialized()) {
            return hasFeature(FLAG_CDMALESS);
        } else {
            return ImsFeatureProvider.hasFeature(c, ImsExternalFeature.FEATURE_CDMALESS);
        }
    }

    public static boolean isLTEOnlySupported(Context c) {
        if (isInitialized()) {
            return hasFeature(FLAG_LTEONLY);
        } else {
            return ImsFeatureProvider.hasFeature(c, ImsFeature.FEATURE_LTEONLY);
        }
    }

    public static boolean isRttSupported(Context c) {
        if (isInitialized()) {
            return hasFeature(FLAG_RTT);
        } else {
            return ImsFeatureProvider.hasRtt(c);
        }
    }

    public static boolean isCallComposerSupported(Context c) {
        if (isInitialized()) {
            return hasFeature(FLAG_CALL_COMPOSER);
        } else {
            return ImsFeatureProvider.hasCallComposer(c);
        }
    }

    public static boolean isVrbtSupported(Context c) {
        if (isInitialized()) {
            return hasFeature(FLAG_VRBT);
        } else {
            return ImsFeatureProvider.hasFeature(c, ImsFeature.FEATURE_VRBT);
        }
    }

    public static boolean isUceSupported(Context c) {
        if (isInitialized()) {
            return hasFeature(FLAG_UCE);
        } else {
            return false;
        }
    }

    public static boolean isVoNRSupported() {
        if (isInitialized()) {
            return hasFeature(FLAG_VONR);
        } else {
            return ImsProperties.isVoNrEnabled();
        }
    }

    private static int getFeatureFlag(String feature) {
        if (feature.equals(ImsExternalFeature.FEATURE_MEDIA_EVS)) {
            return FLAG_MEDIA_EVS;
        }

        if (feature.equals(ImsFeature.FEATURE_MEDIA_HEVC)) {
            return FLAG_MEDIA_HEVC;
        }

        if (feature.equals(ImsExternalFeature.FEATURE_HVOLTE)) {
            return FLAG_HVOLTE;
        }

        if (feature.equals(ImsExternalFeature.FEATURE_VOWIFI)) {
            return FLAG_VOWIFI;
        }

        if (feature.equals(ImsFeature.FEATURE_SMSONLY)) {
            return FLAG_SMSONLY;
        }

        if (feature.equals(ImsFeature.FEATURE_EPSONLY)) {
            return FLAG_EPSONLY;
        }

        if (feature.equals(ImsExternalFeature.FEATURE_RTT)) {
            return FLAG_RTT;
        }

        if (feature.equals(ImsFeature.FEATURE_VOLTEONLY)) {
            return FLAG_VOLTEONLY;
        }

        if (feature.equals(ImsFeature.FEATURE_ROAMING)) {
            return FLAG_ROAMING;
        }

        if (feature.equals(ImsExternalFeature.FEATURE_CDMALESS)) {
            return FLAG_CDMALESS;
        }

        if (feature.equals(ImsFeature.FEATURE_LTEONLY)) {
            return FLAG_LTEONLY;
        }

        if (feature.equals(ImsFeature.FEATURE_STIR_SHAKEN)) {
            return FLAG_STIR_SHAKEN;
        }

        if (feature.equals(ImsFeature.FEATURE_VRBT)) {
            return FLAG_VRBT;
        }

        return 0;
    }

    private static synchronized boolean hasFeature(int feature) {
        return (sFeatures & feature) != 0;
    }

    private static synchronized boolean isInitialized() {
        return sInitialized;
    }

    private static void setFeature(int feature) {
        sFeatures |= feature;
    }

    private static void setCommonFeatures(Context c) {
        setFeature(FLAG_SYSTEM_SERVER_IMS_PHONE);
        setFeature(FLAG_SYSTEM_SERVER_IMS);

        if (MSimUtils.isSingleImsEnabledOnDsdv()) {
            setFeature(FLAG_DSDV_SV);
        }

        if (MSimUtils.isMultiImsEnabledOnDssv()) {
            setFeature(FLAG_DSSV_DV);
        }

        if (ImsFeatureProvider.hasServerSms(c)) {
            setFeature(FLAG_SYSTEM_SERVER_SMS);
        }

        if (ImsFeatureProvider.hasServerSmsSCBM(c)) {
            setFeature(FLAG_SYSTEM_SERVER_SMS_SCBM);
        }

        if (ImsFeatureProvider.hasRtt(c)) {
            setFeature(FLAG_RTT);
        }

        if (ImsFeatureProvider.hasCallComposer(c)) {
            setFeature(FLAG_CALL_COMPOSER);
        }

        if (ImsFeatureProvider.hasFeature(c, ImsFeature.FEATURE_VRBT)) {
            setFeature(FLAG_VRBT);
        }

        if (ImsProperties.isVoNrEnabled()) {
            setFeature(FLAG_VONR);
        }
    }

    private static void setMediaFeatures(Context c) {
        if (ImsFeatureProvider.hasMediaEvs(c)) {
            setFeature(FLAG_MEDIA_EVS);
        }

        if (ImsFeatureProvider.hasFeature(c, ImsFeature.FEATURE_MEDIA_HEVC)) {
            setFeature(FLAG_MEDIA_HEVC);
        }

        if (ImsFeatureProvider.hasMediaCamera(c, 2)) {
            setFeature(FLAG_MEDIA_CAMERA_V2);
        }
    }

    private static void setOperatorSpecificFeatures(Context c) {
        if (ImsFeatureProvider.hasHVoLte(c)) {
            setFeature(FLAG_HVOLTE);
        }

        if (ImsFeatureProvider.hasVoWiFi(c)) {
            setFeature(FLAG_VOWIFI);
        }

        if (ImsFeatureProvider.hasFeature(c, ImsFeature.FEATURE_SMSONLY)) {
            setFeature(FLAG_SMSONLY);
        }

        if (ImsFeatureProvider.hasFeature(c, ImsFeature.FEATURE_EPSONLY)) {
            setFeature(FLAG_EPSONLY);
        }

        if (ImsFeatureProvider.hasFeature(c, ImsFeature.FEATURE_VOLTEONLY)) {
            setFeature(FLAG_VOLTEONLY);
        }

        if (ImsFeatureProvider.hasFeature(c, ImsFeature.FEATURE_ROAMING)) {
            setFeature(FLAG_ROAMING);
        }

        if (ImsFeatureProvider.hasFeature(c, ImsExternalFeature.FEATURE_CDMALESS)) {
            setFeature(FLAG_CDMALESS);
        }

        if (ImsFeatureProvider.hasFeature(c, ImsFeature.FEATURE_LTEONLY)) {
            setFeature(FLAG_LTEONLY);
        }

        if (ImsFeatureProvider.hasUce(c)) {
            setFeature(FLAG_UCE);
        }

        if (ImsFeatureProvider.hasFeature(c, ImsFeature.FEATURE_STIR_SHAKEN)) {
            setFeature(FLAG_STIR_SHAKEN);
        }
    }
}
