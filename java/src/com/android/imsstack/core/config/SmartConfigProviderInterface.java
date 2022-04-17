/**
 * SmartConfigProviderInterface
 * Role
 *
 */

package com.android.imsstack.core.config;

import android.net.Uri;

import com.android.imsstack.util.ImsConstants;

public class SmartConfigProviderInterface
{

    // Authority for ConfigurationProvider
    // Constants--------------------------------------------------
    public static final String AUTHORITY = "com.android.imsstack.provider.smart_configuration";
    public static final String DB_NAME = "smart_configuration.db";
    public static final String DB_PATH = ImsConstants.IMS_STORAGE_ROOT_DIR + "/databases/";
    public static final String DB_FULL_PATH = DB_PATH + DB_NAME;

    // Variables--------------------------------------------------
    // Static loading materials ----------------------------------
    // Public methods --------------------------------------------
    public static final class SmartModifiedTime
    {
        public static final String TABLE_NAME = "smart_modified_time";

        public static final Uri CONTENT_URI = Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final int ATTR_LENGTH = 6;
        public static final String ID = "id";
        public static final String PROPERTY = "property";
        public static final String XML_FILE = "xml_file";
        public static final String XML_MODIFIED_TIME = "xml_modified_time";
        public static final String SW_VERSION = "software_version";
        public static final String VERSION = "version";
    }

    public static final class OperatorList
    {
        public static final String TABLE_NAME = "operator_list";

        public static final Uri CONTENT_URI = Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String OPERATOR = "operator";
        public static final String COUNTRY = "country";
        public static final String REGION = "region";
        public static final String GROUP_ID = "group_id";
        public static final String CATEGORY = "category";
        public static final String BRAND = "brand";
        public static final String OPERATOR_BASED_ON = "operator_based_on";
        public static final String ENABLER_TYPE = "enabler_type";
        public static final String SUPPORT_SIM_MOVED = "support_sim_moved";
        public static final String CONFIG_PER_MODEL = "config_per_model";
    }

    public static final class MccMncList
    {
        public static final String TABLE_NAME = "mccmnc_list";

        public static final Uri CONTENT_URI = Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String MCCMNC   = "mccmnc";
        public static final String OPERATOR = "operator";
        public static final String COUNTRY  = "country";
        public static final String GID      = "gid";
        public static final String SPN      = "spn";
        public static final String IMSI     = "imsi";
        public static final String ENABLED  = "enabled";
    }

    public static final class NtCodeList
    {
        public static final String TABLE_NAME = "ntcode_list";

        public static final Uri CONTENT_URI = Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String NTCODE = "ntcode";
        public static final String OPERATOR = "operator";
        public static final String COUNTRY = "country";
        public static final String REGION = "region";
        public static final String CATEGORY = "category";
        public static final String RESTRICTION = "restriction";
    }

    public static final class ServiceAcceptanceList
    {
        public static final String TABLE_NAME = "service_acceptance_list";

        public static final Uri CONTENT_URI = Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String OPERATOR = "operator";
        public static final String COUNTRY = "country";
        public static final String MCC = "mcc";
        public static final String MCCMNC = "mccmnc";
    }

    public static final class NaoCarrierList
    {
        public static final String TABLE_NAME = "nao_carrier_list";

        public static final Uri CONTENT_URI = Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String CARRIER = "carrier";
        public static final String OPERATOR = "operator";
        public static final String COUNTRY = "country";
    }

    public static final class ServiceEnableListByToTc
    {
        public static final String TABLE_NAME = "service_enable_list_by_totc";

        public static final Uri CONTENT_URI = Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String TOTC = "totc"; // Key

        // Note : Parameters are equal to ServiceEnableListByNtCode's parameters.
    }

    public static final class ServiceEnableListByNtCode
    {
        public static final String TABLE_NAME = "service_enable_list_by_ntcode";

        public static final Uri CONTENT_URI = Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String NTCODE = "ntcode"; // Key

        public static final String OPERATOR = "operator";
        public static final String COUNTRY = "country";
        public static final String MCCMNC = "mccmnc";
        public static final String MCC = "mcc";
        public static final String REGION = "region";
        public static final String GROUP_ID = "group_id";
        public static final String OPERATOR_EXEMPT = "operator_exempt";
        public static final String MCCMNC_EXEMPT = "mccmnc_exempt";
        public static final String INBOUNDING_ROAMING = "inbounding_roaming";
        public static final String DESC = "desc";
    }

    // Interface implementation methods --------------------------
    // Private/Protected methods ---------------------------------

}
