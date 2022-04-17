package com.android.imsstack.provider;

import android.content.ContentProvider;
import android.content.ContentValues;
import android.content.Context;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.ArrayMap;
import android.util.Xml;

import com.android.imsstack.external.ims.ImsExternalFeature;
import com.android.imsstack.util.ImsFeature;
import com.android.imsstack.util.ImsProperties;
import com.android.imsstack.util.Log;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

/*
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
 *   - com.android.imsstack.media.hevc
 *   - com.android.imsstack.media.camera
 *
 *   - com.android.imsstack.rtt
 *   - com.android.imsstack.callcomposer
 *   - com.android.imsstack.vrbt
 */
public class ImsFeatureProvider extends ContentProvider {
    private static final boolean DBG = !android.os.Build.TYPE.equals("user");
    private static final String[] FEATURES =
        {
            ImsExternalFeature.FEATURE_VOLTE,
            ImsExternalFeature.FEATURE_VT,
            ImsExternalFeature.FEATURE_VOWIFI,
            ImsExternalFeature.FEATURE_UCE,
            ImsExternalFeature.FEATURE_HVOLTE,
            ImsExternalFeature.FEATURE_RTT,
            ImsExternalFeature.FEATURE_CALLCOMPOSER,
            ImsExternalFeature.FEATURE_SERVER_SMS,
            ImsExternalFeature.FEATURE_MEDIA_EVS,
            ImsExternalFeature.FEATURE_MEDIA_EVS_WB,
            // IMS_CHANGE [SMS_Patch_0621][VZW], MSG_SMS_SMS_TO_911
            ImsExternalFeature.FEATURE_SERVER_SMS_SCBM,
            ImsExternalFeature.FEATURE_CDMALESS,
            ImsFeature.FEATURE_DISABLE_IMS,
            ImsFeature.FEATURE_DSDV_SV,
            ImsFeature.FEATURE_MEDIA_HEVC,
            ImsFeature.FEATURE_VRBT,
            ImsFeature.FEATURE_SMSONLY,
            ImsFeature.FEATURE_EPSONLY,
            ImsFeature.FEATURE_VOLTEONLY,
            ImsFeature.FEATURE_LTEONLY,
            ImsFeature.FEATURE_ROAMING,
            ImsFeature.FEATURE_VERIZON_IMS,
            ImsFeature.FEATURE_STIR_SHAKEN
        };

    private static final FeatureInfo[] FEATURE_INFO_LIST =
        {
            new FeatureInfo(ImsExternalFeature.FEATURE_MEDIA_CAMERA, 2)
        };

    private static final String METHOD_GET_PROPERTY = "getProperty";

    private static final ArrayMap<String, String> sMVNOperatorList;
    private final ArrayMap<String, FeatureInfo> mAvailableFeatures = new ArrayMap<>();
    private String mOperator = "";
    private String mCountry = "";
    private boolean mHasOperatorFeatures = false;
    private boolean mHasRttFeature = false;
    private String mDualVoLte = "0";

    private static class FeatureInfo {
        public String mName;
        public int mVersion;

        public FeatureInfo() {
        }

        public FeatureInfo(String name, int version) {
            mName = name;
            mVersion = version;
        }

        @Override
        public String toString() {
            return "[FeatureInfo=" + mName + ", " + mVersion + "]";
        }
    }

    public ImsFeatureProvider() {
    }

    @Override
    public boolean onCreate() {
        logw("ImsFeatureProvider is created");

        setOperatorCountry();

        try {
            readPermissions();
            readSystemPermissions();
        } catch (Exception e) {
            e.printStackTrace();
            logw(e.toString());
        }

        mHasRttFeature = hasFeature(ImsExternalFeature.FEATURE_RTT);

        addOrRemoveFeaturesByRuntimeCondition();

        if (DBG) {
            logd(mAvailableFeatures.toString());
        }

        mDualVoLte = "0";

        return true;
    }

    @Override
    public Bundle call(String method, String arg, Bundle extras) {
        Bundle result = null;

        switch (method) {
            case ImsExternalFeature.METHOD_HAS_FEATURE: {
                int version = (extras != null) ?
                        extras.getInt(ImsExternalFeature.KEY_ARG_VERSION, -1) : -1;

                result = new Bundle();

                if (version >= 0) {
                    result.putBoolean(ImsExternalFeature.KEY_RESULT, hasFeature(arg, version));
                } else {
                    result.putBoolean(ImsExternalFeature.KEY_RESULT, hasFeature(arg));
                }
                break;
            }
            case ImsExternalFeature.METHOD_UPDATE_FEATURE: {
                String prefOperator = (extras != null) ?
                        extras.getString(ImsFeature.KEY_ARG_OPERATOR, null) : null;
                String prefCountry = (extras != null) ?
                        extras.getString(ImsFeature.KEY_ARG_COUNTRY, null) : null;

                result = new Bundle();
                result.putBoolean(ImsExternalFeature.KEY_RESULT,
                        updateFeature(prefOperator, prefCountry));
                break;
            }
            case METHOD_GET_PROPERTY: {
                result = new Bundle();
                result.putString(ImsExternalFeature.KEY_RESULT, getProperty(arg));
                break;
            }
            default: {
                break;
            }
        }

        return result;
    }

    @Override
    public int delete(Uri uri, String where, String[] whereArgs) {
        return 0;
    }

    @Override
    public String getType(Uri uri) {
        return null;
    }

    @Override
    public Uri insert(Uri uri, ContentValues initialValues) {
        return null;
    }

    @Override
    public Cursor query(Uri uri, String[] projection, String selection, String[] selectionArgs,
            String sortOrder) {
        return null;
    }

    @Override
    public int update(Uri uri, ContentValues values, String where, String[] whereArgs) {
        return 0;
    }

    private void readPermissions() {
        if (DBG) {
            logd("readPermissions");
        }

        File xmlFile = ImsExternalFeature.getFile(ImsExternalFeature.FILE_XML);

        if (xmlFile == null) {
            logw("Cupss :: no " + ImsExternalFeature.FILE_XML);
            return;
        }

        FileReader reader = null;

        try {
            reader = new FileReader(xmlFile);

            XmlPullParser parser = Xml.newPullParser();

            parser.setInput(reader);

            beginDocument(parser, "permissions");

            boolean operatorFeature = false;

            while (true) {
                nextElement(parser);

                if (parser.getEventType() == XmlPullParser.END_DOCUMENT) {
                    break;
                }

                String name = parser.getName();

                if ("feature".equals(name)) {
                    String fname = parser.getAttributeValue(null, "name");
                    int fversion = readIntAttribute(parser, "version", 0);

                    addFeature(fname, fversion);

                    skipCurrentTag(parser);
                    continue;
                } else if ("operator-feature".equals(name)) {
                    if (!mHasOperatorFeatures) {
                        mHasOperatorFeatures = true;
                    }

                    if (parser.getEventType() == XmlPullParser.START_TAG) {
                        String operator = parser.getAttributeValue(null, "operator");
                        String country = parser.getAttributeValue(null, "country");

                        if (mOperator.equals(operator) && mCountry.equals(country)) {
                            logw("operator-feature :: " + operator + "/" + country);
                            continue;
                        } else if (DBG) {
                            logd("operator-feature(not-match) :: " + operator + "/" + country);
                        }
                    }

                    skipCurrentTag(parser);
                    continue;
                }
            }
        } catch (XmlPullParserException e) {
            e.printStackTrace();
            logw("Got exception parsing permissions :: " + e.toString());
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            logw("Got exception parsing permissions :: " + e.toString());
        } catch (IOException e) {
            e.printStackTrace();
            logw("Got exception parsing permissions :: " + e.toString());
        } finally {
            closeQuietly(reader);
        }
    }

    private void readSystemPermissions() {
        if (!hasDeviceFeatures()) {
            logw("LAOP :: No system features");
            return;
        } else {
            logw("LAOP :: System features exist");
        }

        if (DBG) {
            logd("readSystemPermissions");
        }

        PackageManager pm = getContext().getPackageManager();

        if (pm != null) {
            for (String name : FEATURES) {
                addSystemFeature(pm, name);
            }

            for (FeatureInfo fi : FEATURE_INFO_LIST) {
                addSystemFeature(pm, fi.mName, fi.mVersion);
            }
        }
    }

    private void addFeature(String name, int version) {
        FeatureInfo fi = mAvailableFeatures.get(name);

        if (fi == null) {
            fi = new FeatureInfo();
            fi.mName = name;
            fi.mVersion = version;
            mAvailableFeatures.put(name, fi);
        } else {
            fi.mVersion = Math.max(fi.mVersion, version);
        }
    }

    private void addSystemFeature(PackageManager pm, String name) {
        if (pm.hasSystemFeature(name)) {
            addFeature(name, 0);
        }
    }

    private void addSystemFeature(PackageManager pm, String name, int version) {
        if (pm.hasSystemFeature(name, version)) {
            addFeature(name, version);
        }
    }

    private void removeFeature(String name) {
        if (mAvailableFeatures.remove(name) != null) {
            logw("Removed unavailable feature :: " + name);
        }
    }

    private void removeAllFeatures() {
        mAvailableFeatures.clear();
    }

    private boolean addOrRemoveFeaturesByRuntimeCondition() {
        if (mHasRttFeature) {
            // Check if Google Fi SIM is inserted
            int googleFi = 0; // SimCarrierId.GOOGLE_FI;
            String opSub = getSysSimOperatorSub();
            boolean rttDisabled = (googleFi == 1);

            if ("US".equals(mCountry) && "TMO".equals(mOperator)
                    && "RPW".equals(opSub)) {
                rttDisabled = true;
            }

            if ("US".equals(ImsProperties.TARGET_COUNTRY)
                    && "NAO".equals(ImsProperties.TARGET_OPERATOR)
                    && "TRF_ATT".equals(getSysSimOperator())) {
                rttDisabled = true;
            }

            logd("RTT :: rttDisabled=" + rttDisabled + ", Google-Fi=" + googleFi);

            if (rttDisabled) {
                // Disable RTT
                if (hasFeature(ImsExternalFeature.FEATURE_RTT)) {
                    removeFeature(ImsExternalFeature.FEATURE_RTT);
                    return true;
                }
            } else {
                // Enable RTT
                if (!hasFeature(ImsExternalFeature.FEATURE_RTT)) {
                    addFeature(ImsExternalFeature.FEATURE_RTT, 0);
                    return true;
                }
            }
        }

        // Add Camera API version 2 : from Q-OS
        if (!hasFeature(ImsExternalFeature.FEATURE_MEDIA_CAMERA)) {
            addFeature(ImsExternalFeature.FEATURE_MEDIA_CAMERA, 2);
        }

        // AOSP-IMS: always enable VoLte/Vt feature {
        if (!hasFeature(ImsExternalFeature.FEATURE_VOLTE)) {
            addFeature(ImsExternalFeature.FEATURE_VOLTE, 0);
        }

        if (!hasFeature(ImsExternalFeature.FEATURE_VT)) {
            addFeature(ImsExternalFeature.FEATURE_VT, 0);
        }
        // }

        return false;
    }

    private boolean hasFeature(String name) {
        FeatureInfo fi = mAvailableFeatures.get(name);
        return fi != null;
    }

    private boolean hasFeature(String name, int version) {
        FeatureInfo fi = mAvailableFeatures.get(name);
        return (fi != null) && (fi.mVersion >= version);
    }

    private boolean updateFeature(String prefOperator, String prefCountry) {
        logw("updateFeature - hasOperatorFeatures=" + mHasOperatorFeatures);

        if (!mHasOperatorFeatures) {
            return false;
        }

        String oldOperator = mOperator;
        String oldCountry = mCountry;

        setOperatorCountry();

        if (!TextUtils.isEmpty(prefOperator) && !TextUtils.isEmpty(prefCountry)) {
            logw("Preferred operator/country is overwritten :: "
                    + prefOperator + "/" + prefCountry);

            mOperator = prefOperator;
            mCountry = prefCountry;
        }

        if (mOperator.equals(oldOperator) && mCountry.equals(oldCountry)) {
            logd("No changes :: same operator & country");

            if (addOrRemoveFeaturesByRuntimeCondition()) {
                return true;
            }

            return false;
        }

        try {
            mHasOperatorFeatures = false;
            removeAllFeatures();

            readPermissions();
            readSystemPermissions();

            getContext().getContentResolver().notifyChange(ImsExternalFeature.CONTENT_URI, null);
        } catch (Exception e) {
            e.printStackTrace();
            logw(e.toString());
            return false;
        }

        mHasRttFeature = hasFeature(ImsExternalFeature.FEATURE_RTT);

        addOrRemoveFeaturesByRuntimeCondition();

        if (DBG) {
            logd(mAvailableFeatures.toString());
        }

        return true;
    }

    private String getProperty(String key) {
        return "";
    }

    private void setOperatorCountry() {
        mOperator = ImsProperties.TARGET_OPERATOR;
        mCountry = ImsProperties.TARGET_COUNTRY;

        if (isVoLteOpen()) {
            // Single SIM only case
            String operator = getSysSimOperator();
            String country = getSysSimCountry();
            String targetOperator = getTargetOperator(operator);

            if (targetOperator != null) {
                // If MVNO is defined, then it needs to be changed to IMS-known operator.
                mOperator = targetOperator;

                if (!TextUtils.isEmpty(country)) {
                    mCountry = country;
                }
            } else if (!TextUtils.isEmpty(operator) && !TextUtils.isEmpty(country)) {
                mOperator = operator;
                mCountry = country;
            }
        }

        logw(mOperator + "/" + mCountry);
    }

    private boolean isVoLteOpen() {
        if ("US".equals(mCountry)
                && ("TRF".equals(mOperator) || "NAO".equals(mOperator))) {
            return true;
        }

        return ImsExternalFeature.FEATURE_VOLTE_OPEN;
    }

    private String getSysSimCountry() {
        return ImsProperties.getSysSimCountry(getContext(), 0);
    }

    private String getSysSimOperator() {
        return ImsProperties.getSysSimOperator(getContext(), 0);
    }

    private String getSysSimOperatorSub() {
        return ImsProperties.getSysSimOperatorSub(getContext(), 0);
    }

    private static boolean hasDeviceFeatures() {
        try {
            File file = new File("/system/etc/permissions", ImsExternalFeature.FILE_XML);
            return file.exists();
        } catch (Exception e) {
            e.printStackTrace();
        }

        return false;
    }

    private static String getTargetOperator(String simOperator) {
        return sMVNOperatorList.get(simOperator);
    }

    private static void closeQuietly(AutoCloseable closeable) {
        if (closeable != null) {
            try {
                closeable.close();
            } catch (RuntimeException rethrown) {
               throw rethrown;
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    private static void beginDocument(XmlPullParser parser, String firstElementName)
            throws XmlPullParserException, IOException {
        int type;

        while ((type = parser.next()) != XmlPullParser.START_TAG
                && type != XmlPullParser.END_DOCUMENT) {
            ;
        }

        if (type != parser.START_TAG) {
            throw new XmlPullParserException("No start tag found");
        }

        if (!parser.getName().equals(firstElementName)) {
            throw new XmlPullParserException("Unexpected start tag: found "
                    + parser.getName() + ", expected " + firstElementName);
        }
    }

    private static void nextElement(XmlPullParser parser)
            throws XmlPullParserException, IOException {
        int type;

        while ((type = parser.next()) != XmlPullParser.START_TAG
                && type != XmlPullParser.END_DOCUMENT) {
            ;
        }
    }

    private static void skipCurrentTag(XmlPullParser parser)
            throws XmlPullParserException, IOException {
        int outerDepth = parser.getDepth();
        int type;

        while ((type = parser.next()) != XmlPullParser.END_DOCUMENT
                && (type != XmlPullParser.END_TAG
                    || parser.getDepth() > outerDepth)) {
        }
    }

    private static int readIntAttribute(XmlPullParser parser, String name, int defaultValue) {
        final String value = parser.getAttributeValue(null, name);

        if (value == null) {
            return defaultValue;
        }

        try {
            return Integer.parseInt(value);
        } catch (NumberFormatException e) {
            return defaultValue;
        }
    }

    private static void logd(String s) {
        Log.d("IMS", "[ImsFeatureProvider] " + s);
    }

    private static void logw(String s) {
        Log.w("IMS", "[ImsFeatureProvider] " + s);
    }

    static {
        sMVNOperatorList = new ArrayMap<>();

        sMVNOperatorList.put("TRF_ATT", "ATT");
        sMVNOperatorList.put("CRK", "ATT");

        sMVNOperatorList.put("TRF_TMO", "TMO");
        sMVNOperatorList.put("TRF_SM", "TMO");
        sMVNOperatorList.put("TRF_WFM", "TMO");

        sMVNOperatorList.put("LRA", "LRA");
        sMVNOperatorList.put("TRF_VZW", "VZW");
    }
}
