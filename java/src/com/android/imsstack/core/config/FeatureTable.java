package com.android.imsstack.core.config;

import com.android.imsstack.system.SystemConfig;

import com.android.imsstack.core.config.FeatureConfig;

import java.util.ArrayList;
import java.util.List;

/**
 * This class provides the feature information to convert
 * between IMS configuration XML and system configuration.
 */
public final class FeatureTable {
    public static class Feature {
        /** Feature string value for IMS configuration (gims_feature) */
        private String mFeature;
        /** Feature mask value for SystemConfig */
        private int mFeatureMask;

        /* package */ Feature(String feature, int featureMask) {
            mFeature = feature;
            mFeatureMask = featureMask;
        }

        public String getFeature() {
            return mFeature;
        }

        public int getFeatureMask() {
            return mFeatureMask;
        }
    }

    private static List<Feature> sFeatureList;
    private static List<Feature> sServiceFeatureList;

    public static List<Feature> getFeatures() {
        return sFeatureList;
    }

    public static List<Feature> getServiceFeatures() {
        return sServiceFeatureList;
    }

    static {
        /** Functional feature list */
        sFeatureList = new ArrayList<Feature>();

        sFeatureList.add(new Feature(FeatureConfig.IPSEC,
                SystemConfig.FEATURE_IPSEC));
        sFeatureList.add(new Feature(FeatureConfig.TLS,
                SystemConfig.FEATURE_TLS));
        sFeatureList.add(new Feature(FeatureConfig.AUTH_SIP_DIGEST,
                SystemConfig.FEATURE_AUTH_SIP_DIGEST));
        sFeatureList.add(new Feature(FeatureConfig.SDP_PRECONDITION,
                SystemConfig.FEATURE_SDP_PRECONDITION));
        sFeatureList.add(new Feature(FeatureConfig.GRUU, SystemConfig.FEATURE_GRUU));
        sFeatureList.add(new Feature(FeatureConfig.MULTIPLE_REGISTRATION,
                SystemConfig.FEATURE_MULTIPLE_REGISTRATION));
        sFeatureList.add(new Feature(FeatureConfig.REQUEST_URI_VALIDATION_IN_MID_DIALOG,
                SystemConfig.FEATURE_REQUEST_URI_VALIDATION_IN_MID_DIALOG));
        sFeatureList.add(new Feature(FeatureConfig.NO_SESSION_REFRESH_BY_REINVITE,
                SystemConfig.FEATURE_NO_SESSION_REFRESH_BY_REINVITE));
        sFeatureList.add(new Feature(FeatureConfig.INVITE_TXN_HANDLING_CORRECTION,
                SystemConfig.FEATURE_INVITE_TXN_HANDLING_CORRECTION));
        sFeatureList.add(new Feature(FeatureConfig.GEOLOCATION,
                SystemConfig.FEATURE_GEOLOCATION));

        /** Roaming **/
        sFeatureList.add(new Feature(
                FeatureConfig.VOLTE_IN_ROAMING, SystemConfig.FEATURE_VOLTE_IN_ROAMING));
        sFeatureList.add(new Feature(
                FeatureConfig.VT_IN_ROAMING, SystemConfig.FEATURE_VT_IN_ROAMING));

        /** Service feature list */
        sServiceFeatureList = new ArrayList<Feature>();

        sServiceFeatureList.add(new Feature(
                FeatureConfig.VOLTE, SystemConfig.FEATURE_S_VOLTE));
        sServiceFeatureList.add(new Feature(
                FeatureConfig.VOWIFI, SystemConfig.FEATURE_S_VOWIFI));
        sServiceFeatureList.add(new Feature(
                FeatureConfig.VT, SystemConfig.FEATURE_S_VT));
        sServiceFeatureList.add(new Feature(
                FeatureConfig.SMS, SystemConfig.FEATURE_S_SMS));
        sServiceFeatureList.add(new Feature(
                FeatureConfig.UCE, SystemConfig.FEATURE_S_UCE));
    }
}
