package com.android.imsstack.core.config;

import android.net.Uri;

import com.android.imsstack.util.ImsConstants;

public class ProviderInterface {
    // Constants--------------------------------------------------
    public static final String AUTHORITY = "com.android.imsstack.provider.gims";
    public static final String DB = "gims.db";
    public static final String DB_DIR = ImsConstants.IMS_STORAGE_ROOT_DIR + "/databases";
    public static final String DBFP = DB_DIR + "/" + DB;

    public static final String ID = "id";
    public static final String PROPERTY = "property";

    // Defines the constant values for call(...) method of ConfigurationProvider
    public static final String METHOD_GET_TABLE_IN_CONFIG_XML
            = "getTableInConfigXml";
    public static final String METHOD_GET_COLUMN_COUNT_IN_CONFIG_XML
            = "getColumnCountInConfigXml";
    public static final String KEY_RESULT = "result";

    // Variables--------------------------------------------------
    // Static loading materials ----------------------------------
    // Public methods --------------------------------------------
    public static final class Subscriber {
        // Table name
        public static final String TABLE_NAME = "gims_subscriber";
        public static final String TABLE_NAME_FAKE = "gims_subscriber_fake";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        // admin
        public static final String FEATURES = "admin_features";
        public static final String PCSCF = "admin_pcscf";
        public static final String SERVICES = "admin_services";

        public static final class AdminFeatures {
            public static final int IMS = 0x00000001;
            public static final int ISIM = 0x00000002;
            public static final int USIM = 0x00000004;

            public static final int DM = 0x00000100;

            public static final int TESTMODE_GCF = 0x10000000;
            public static final int TESTMODE = 0x20000000;
            public static final int DEBUG = 0x40000000;
        };

        public static final class AdminServices {
            public static final int VOLTE = 0x00000001;
            public static final int VILTE = 0x00000002;
            public static final int VOWIFI = 0x00000004;
            public static final int VIWIFI = 0x00000008;
        };

        // pcscf
        public static final String PCSCF_ADDRESS_0 = "server_pcscf_0_address";
        public static final String PCSCF_PORT_0 = "server_pcscf_0_port";
        public static final String PCSCF_ADDRESS_1 = "server_pcscf_1_address";
        public static final String PCSCF_PORT_1 = "server_pcscf_1_port";
        public static final String PCSCF_ADDRESS_2 = "server_pcscf_2_address";
        public static final String PCSCF_PORT_2 = "server_pcscf_2_port";
        public static final String PCSCF_ADDRESS_3 = "server_pcscf_3_address";
        public static final String PCSCF_PORT_3 = "server_pcscf_3_port";
        public static final String PCSCF_ADDRESS_4 = "server_pcscf_4_address";
        public static final String PCSCF_PORT_4 = "server_pcscf_4_port";
        public static final String PCSCF_ADDRESS_5 = "server_pcscf_5_address";
        public static final String PCSCF_PORT_5 = "server_pcscf_5_port";
        public static final String PCSCF_ADDRESS_6 = "server_pcscf_6_address";
        public static final String PCSCF_PORT_6 = "server_pcscf_6_port";
        public static final String PCSCF_ADDRESS_7 = "server_pcscf_7_address";
        public static final String PCSCF_PORT_7 = "server_pcscf_7_port";
        public static final String PCSCF_ADDRESS_8 = "server_pcscf_8_address";
        public static final String PCSCF_PORT_8 = "server_pcscf_8_port";
        public static final String PCSCF_ADDRESS_9 = "server_pcscf_9_address";
        public static final String PCSCF_PORT_9 = "server_pcscf_9_port";

        // subscriber info.
        public static final String HOME_DOMAIN_NAME = "subscriber_0_home_domain_name";
        public static final String IMPI = "subscriber_0_impi";
                                                  //gsma
        public static final String IMPU_VALIDATION = "subscriber_0_impu_validation";
        public static final String IMPU_COUNT = "subscriber_0_impu_count";
        public static final String IMPU_0 = "subscriber_0_impu_0";
        public static final String IMPU_1 = "subscriber_0_impu_1";
        public static final String IMPU_2 = "subscriber_0_impu_2";
        public static final String PHONE_CONTEXT = "subscriber_0_phone_context";
        public static final String AUTH_USERNAME = "subscriber_0_auth_username";
        public static final String AUTH_PASSWORD = "subscriber_0_auth_password";
        public static final String AUTH_REALM = "subscriber_0_auth_realm";
        public static final String AUTH_ALGORITHM = "subscriber_0_auth_algorithm";
        public static final String SCSCF_ADDRESS = "subscriber_0_server_scscf";
    }

    public static final class Engine
    {

        // Table name
        public static final String TABLE_NAME = "gims_engine";
        public static final String DEBUG_TABLE_NAME = "gims_engine_debug";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        // trace
        public static final String TRACE_OPTION = "trace_option";
        public static final String TRACE_MODULE = "trace_module";
    }

    public static final class SIP
    {

        // Table name
        public static final String TABLE_NAME = "gims_sip";
        public static final String TABLE_NAME_COM = "gims_com_sip";
        public static final String TABLE_NAME_COM_VT = "gims_com_sip_vt";
        public static final String TABLE_NAME_COM_SMS = "gims_com_sip_sms";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);
        public static final Uri CONTENT_URI_COM =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME_COM);

        // timer
        public static final String TV_T1 = "timer_tv_t1";
        public static final String TV_T2 = "timer_tv_t2";
        public static final String TV_T4 = "timer_tv_t4";
        public static final String TV_TA = "timer_tv_ta";
        public static final String TV_TB = "timer_tv_tb";
        public static final String TV_TC = "timer_tv_tc";
        public static final String TV_TD = "timer_tv_td";
        public static final String TV_TE = "timer_tv_te";
        public static final String TV_TF = "timer_tv_tf";
        public static final String TV_TG = "timer_tv_tg";
        public static final String TV_TH = "timer_tv_th";
        public static final String TV_TI = "timer_tv_ti";
        public static final String TV_TJ = "timer_tv_tj";
        public static final String TV_TK = "timer_tv_tk";

        // registration
        public static final String REG_EXPIRATION = "reg_expiration";
        public static final String REG_SUBSCRIPTION = "reg_subscription";
        public static final String REG_SUB_EXPIRATION = "reg_sub_expiration";

        // service
        public static final String SERVICE_SHARED = "service_shared";

        // header info.
        public static final String TARGET_SCHEME = "header_info_target_scheme";
        public static final String PREFERRED_ID = "header_info_preferred_id";
        public static final String SERVICE_VERSION = "header_info_service_version";
        public static final String USER_AGENT_TEMPLATE = "header_info_useragent_fmt";
        public static final String FEATURE_TAGS = "header_info_feature_tags";

        // session
        public static final String ST_REFRESHER = "session_st_refresher";
        public static final String ST_METHOD = "session_st_method";
        public static final String ST_MINSE = "session_st_minse";
        public static final String ST_SESSION_EXPIRES = "session_st_session_expires";

        // common
        public static final String COMMON_DEVICE_ID = "common_device_id";
        public static final String COMMON_TCP_CRITERION_LEN = "common_tcp_criterion_len";
        public static final String COMMON_SIP_FEATURES = "common_sip_features";

    }

    public static final class AoSApplication
    {
        // Table name
        public static final String TABLE_NAME = "gims_aosapplication";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String UNIQUENESS = "Uniqueness_aos_application";

        public static final String ID_0 = "aos_application_0_id";
        public static final String ID_1 = "aos_application_1_id";
        public static final String ID_2 = "aos_application_2_id";
        public static final String ID_3 = "aos_application_3_id";
        public static final String ID_4 = "aos_application_4_id";
        public static final String ID_5 = "aos_application_5_id";

        public static final String FEATURES_0 = "aos_application_0_features";
        public static final String FEATURES_1 = "aos_application_1_features";
        public static final String FEATURES_2 = "aos_application_2_features";
        public static final String FEATURES_3 = "aos_application_3_features";
        public static final String FEATURES_4 = "aos_application_4_features";
        public static final String FEATURES_5 = "aos_application_5_features";

        public static final String STATE_START_INTERVAL_0 =
                "aos_application_0_state_start_interval";
        public static final String STATE_START_INTERVAL_1 =
                "aos_application_1_state_start_interval";
        public static final String STATE_START_INTERVAL_2 =
                "aos_application_2_state_start_interval";
        public static final String STATE_START_INTERVAL_3 =
                "aos_application_3_state_start_interval";
        public static final String STATE_START_INTERVAL_4 =
                "aos_application_4_state_start_interval";
        public static final String STATE_START_INTERVAL_5 =
                "aos_application_5_state_start_interval";

        public static final String RECONFIG_GUARD_INTERVAL_0 =
                "aos_application_0_reconfig_guard_interval";
        public static final String RECONFIG_GUARD_INTERVAL_1 =
                "aos_application_1_reconfig_guard_interval";
        public static final String RECONFIG_GUARD_INTERVAL_2 =
                "aos_application_2_reconfig_guard_interval";
        public static final String RECONFIG_GUARD_INTERVAL_3 =
                "aos_application_3_reconfig_guard_interval";
        public static final String RECONFIG_GUARD_INTERVAL_4 =
                "aos_application_4_reconfig_guard_interval";
        public static final String RECONFIG_GUARD_INTERVAL_5 =
                "aos_application_5_reconfig_guard_interval";

        public static final String REG_STOP_WAITING_INTERVAL_0 =
                "aos_application_0_reg_stop_waiting_interval";
        public static final String REG_STOP_WAITING_INTERVAL_1 =
                "aos_application_1_reg_stop_waiting_interval";
        public static final String REG_STOP_WAITING_INTERVAL_2 =
                "aos_application_2_reg_stop_waiting_interval";
        public static final String REG_STOP_WAITING_INTERVAL_3 =
                "aos_application_3_reg_stop_waiting_interval";
        public static final String REG_STOP_WAITING_INTERVAL_4 =
                "aos_application_4_reg_stop_waiting_interval";
        public static final String REG_STOP_WAITING_INTERVAL_5 =
                "aos_application_5_reg_stop_waiting_interval";

        public static final String REG_TERMINATED_TRYING_INTERVAL_0 =
                "aos_application_0_reg_terminated_trying_interval";
        public static final String REG_TERMINATED_TRYING_INTERVAL_1 =
                "aos_application_1_reg_terminated_trying_interval";
        public static final String REG_TERMINATED_TRYING_INTERVAL_2 =
                "aos_application_2_reg_terminated_trying_interval";
        public static final String REG_TERMINATED_TRYING_INTERVAL_3 =
                "aos_application_3_reg_terminated_trying_interval";
        public static final String REG_TERMINATED_TRYING_INTERVAL_4 =
                "aos_application_4_reg_terminated_trying_interval";
        public static final String REG_TERMINATED_TRYING_INTERVAL_5 =
                "aos_application_5_reg_terminated_trying_interval";

        public static final String UNEXPECTED_ERROR_REG_TRYING_MAX_COUNT_0 =
                "aos_application_0_unexpected_error_reg_trying_max_count";
        public static final String UNEXPECTED_ERROR_REG_TRYING_MAX_COUNT_1 =
                "aos_application_1_unexpected_error_reg_trying_max_count";
        public static final String UNEXPECTED_ERROR_REG_TRYING_MAX_COUNT_2 =
                "aos_application_2_unexpected_error_reg_trying_max_count";
        public static final String UNEXPECTED_ERROR_REG_TRYING_MAX_COUNT_3 =
                "aos_application_3_unexpected_error_reg_trying_max_count";
        public static final String UNEXPECTED_ERROR_REG_TRYING_MAX_COUNT_4 =
                "aos_application_4_unexpected_error_reg_trying_max_count";
        public static final String UNEXPECTED_ERROR_REG_TRYING_MAX_COUNT_5 =
                "aos_application_5_unexpected_error_reg_trying_max_count";

        public static final String UNEXPECTED_ERROR_REG_TRYING_MAX_INTERVAL_0 =
                "aos_application_0_unexpected_error_trying_max_interval";
        public static final String UNEXPECTED_ERROR_REG_TRYING_MAX_INTERVAL_1 =
                "aos_application_1_unexpected_error_trying_max_interval";
        public static final String UNEXPECTED_ERROR_REG_TRYING_MAX_INTERVAL_2 =
                "aos_application_2_unexpected_error_trying_max_interval";
        public static final String UNEXPECTED_ERROR_REG_TRYING_MAX_INTERVAL_3 =
                "aos_application_3_unexpected_error_trying_max_interval";
        public static final String UNEXPECTED_ERROR_REG_TRYING_MAX_INTERVAL_4 =
                "aos_application_4_unexpected_error_trying_max_interval";
        public static final String UNEXPECTED_ERROR_REG_TRYING_MAX_INTERVAL_5 =
                "aos_application_5_unexpected_error_trying_max_interval";

        public static final String PDN_RELEASE_DELAY_ON_TERMINATION_0 =
                "aos_application_0_pdn_release_delay_on_termination";
        public static final String PDN_RELEASE_DELAY_ON_TERMINATION_1 =
                "aos_application_1_pdn_release_delay_on_termination";
        public static final String PDN_RELEASE_DELAY_ON_TERMINATION_2 =
                "aos_application_2_pdn_release_delay_on_termination";
        public static final String PDN_RELEASE_DELAY_ON_TERMINATION_3 =
                "aos_application_3_pdn_release_delay_on_termination";
        public static final String PDN_RELEASE_DELAY_ON_TERMINATION_4 =
                "aos_application_4_pdn_release_delay_on_termination";
        public static final String PDN_RELEASE_DELAY_ON_TERMINATION_5 =
                "aos_application_5_pdn_release_delay_on_termination";
    }

    public static final class AoSCondition
    {
        // Table name
        public static final String TABLE_NAME = "gims_aoscondition";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final class ConditionFeatures {
          //  public static final int UPDATE_DB  = 0x00000001;
          //  public static final int ISIM_TO_USIM_FALLBACK  = 0x00000002;
          //  public static final int ISIM_TO_IMSI_BASED_ISIM_FALLBACK = 0x00000004;
          //  public static final int USIM_REFRESH = 0x00000008;
          //  public static final int PCSCF_PORT_FROM_CONFIG = 0x00000010;
          //  public static final int SET_EPDG_PREFERENCE = 0x00000020;
            public static final int CELL_SIGNAL_STRENGTH_CHECK = 0x00000040;
            public static final int ALLOW_WIFI_IN_NON_LTE_NETWORK = 0x00000800;
        };

        public static final String UNIQUENESS = "Uniqueness_aos_condition";

        public static final String ID_0 = "aos_condition_0_id";
        public static final String ID_1 = "aos_condition_1_id";
        public static final String ID_2 = "aos_condition_2_id";
        public static final String ID_3 = "aos_condition_3_id";
        public static final String ID_4 = "aos_condition_4_id";
        public static final String ID_5 = "aos_condition_5_id";

        public static final String FEATURES_0 = "aos_condition_0_features";
        public static final String FEATURES_1 = "aos_condition_1_features";
        public static final String FEATURES_2 = "aos_condition_2_features";
        public static final String FEATURES_3 = "aos_condition_3_features";
        public static final String FEATURES_4 = "aos_condition_4_features";
        public static final String FEATURES_5 = "aos_condition_5_features";

        public static final String ISIM_INDEX_FOR_IMPU_0 =
                "aos_condition_0_isim_index_for_impu";
        public static final String ISIM_INDEX_FOR_IMPU_1 =
                "aos_condition_1_isim_index_for_impu";
        public static final String ISIM_INDEX_FOR_IMPU_2 =
                "aos_condition_2_isim_index_for_impu";
        public static final String ISIM_INDEX_FOR_IMPU_3 =
                "aos_condition_3_isim_index_for_impu";
        public static final String ISIM_INDEX_FOR_IMPU_4 =
                "aos_condition_4_isim_index_for_impu";
        public static final String ISIM_INDEX_FOR_IMPU_5 =
                "aos_condition_5_isim_index_for_impu";

        public static final String ISIM_INDEX_FOR_PCSCF_0 =
                "aos_condition_0_isim_index_for_pcscf";
        public static final String ISIM_INDEX_FOR_PCSCF_1 =
                "aos_condition_1_isim_index_for_pcscf";
        public static final String ISIM_INDEX_FOR_PCSCF_2 =
                "aos_condition_2_isim_index_for_pcscf";
        public static final String ISIM_INDEX_FOR_PCSCF_3 =
                "aos_condition_3_isim_index_for_pcscf";
        public static final String ISIM_INDEX_FOR_PCSCF_4 =
                "aos_condition_4_isim_index_for_pcscf";
        public static final String ISIM_INDEX_FOR_PCSCF_5 =
                "aos_condition_5_isim_index_for_pcscf";

        public static final String PCSCF_PORT_0 = "aos_condition_0_pcscf_port";
        public static final String PCSCF_PORT_1 = "aos_condition_1_pcscf_port";
        public static final String PCSCF_PORT_2 = "aos_condition_2_pcscf_port";
        public static final String PCSCF_PORT_3 = "aos_condition_3_pcscf_port";
        public static final String PCSCF_PORT_4 = "aos_condition_4_pcscf_port";
        public static final String PCSCF_PORT_5 = "aos_condition_5_pcscf_port";

        public static final String MULTIPLE_DISCOVERY_SCHEME_0 =
                "aos_condition_0_multiple_discovery_scheme";
        public static final String MULTIPLE_DISCOVERY_SCHEME_1 =
                "aos_condition_1_multiple_discovery_scheme";
        public static final String MULTIPLE_DISCOVERY_SCHEME_2 =
                "aos_condition_2_multiple_discovery_scheme";
        public static final String MULTIPLE_DISCOVERY_SCHEME_3 =
                "aos_condition_3_multiple_discovery_scheme";
        public static final String MULTIPLE_DISCOVERY_SCHEME_4 =
                "aos_condition_4_multiple_discovery_scheme";
        public static final String MULTIPLE_DISCOVERY_SCHEME_5 =
                "aos_condition_5_multiple_discovery_scheme";

        public static final String PCSCF_CHANGE_CONTROL_0 =
                "aos_condition_0_pcscf_changed_control";
        public static final String PCSCF_CHANGE_CONTROL_1 =
                "aos_condition_1_pcscf_changed_control";
        public static final String PCSCF_CHANGE_CONTROL_2 =
                "aos_condition_2_pcscf_changed_control";
        public static final String PCSCF_CHANGE_CONTROL_3 =
                "aos_condition_3_pcscf_changed_control";
        public static final String PCSCF_CHANGE_CONTROL_4 =
                "aos_condition_4_pcscf_changed_control";
        public static final String PCSCF_CHANGE_CONTROL_5 =
                "aos_condition_5_pcscf_changed_control";

        public static final String NETWORK_SUSPEND_CONDITION_0 =
                "aos_condition_0_network_suspend_condition";
        public static final String NETWORK_SUSPEND_CONDITION_1 =
                "aos_condition_1_network_suspend_condition";
        public static final String NETWORK_SUSPEND_CONDITION_2 =
                "aos_condition_2_network_suspend_condition";
        public static final String NETWORK_SUSPEND_CONDITION_3 =
                "aos_condition_3_network_suspend_condition";
        public static final String NETWORK_SUSPEND_CONDITION_4 =
                "aos_condition_4_network_suspend_condition";
        public static final String NETWORK_SUSPEND_CONDITION_5 =
                "aos_condition_5_network_suspend_condition";

        public static final String SERVICE_COUNCIL_BLOCK_REASONS_0 =
                "aos_condition_0_service_council_block_reasons";
        public static final String SERVICE_COUNCIL_BLOCK_REASONS_1 =
                "aos_condition_1_service_council_block_reasons";
        public static final String SERVICE_COUNCIL_BLOCK_REASONS_2 =
                "aos_condition_2_service_council_block_reasons";
        public static final String SERVICE_COUNCIL_BLOCK_REASONS_3 =
                "aos_condition_3_service_council_block_reasons";
        public static final String SERVICE_COUNCIL_BLOCK_REASONS_4 =
                "aos_condition_4_service_council_block_reasons";
        public static final String SERVICE_COUNCIL_BLOCK_REASONS_5 =
                "aos_condition_5_service_council_block_reasons";

        public static final String LTE_ROVEIN_THRESHOLD_0 =
                "aos_condition_0_lte_rovein_threshold";
        public static final String LTE_ROVEIN_THRESHOLD_1 =
                "aos_condition_1_lte_rovein_threshold";
        public static final String LTE_ROVEIN_THRESHOLD_2 =
                "aos_condition_2_lte_rovein_threshold";
        public static final String LTE_ROVEIN_THRESHOLD_3 =
                "aos_condition_3_lte_rovein_threshold";
        public static final String LTE_ROVEIN_THRESHOLD_4 =
                "aos_condition_4_lte_rovein_threshold";
        public static final String LTE_ROVEIN_THRESHOLD_5 =
                "aos_condition_5_lte_rovein_threshold";

        public static final String LTE_ROVEOUT_THRESHOLD_0 =
                "aos_condition_0_lte_roveout_threshold";
        public static final String LTE_ROVEOUT_THRESHOLD_1 =
                "aos_condition_1_lte_roveout_threshold";
        public static final String LTE_ROVEOUT_THRESHOLD_2 =
                "aos_condition_2_lte_roveout_threshold";
        public static final String LTE_ROVEOUT_THRESHOLD_3 =
                "aos_condition_3_lte_roveout_threshold";
        public static final String LTE_ROVEOUT_THRESHOLD_4 =
                "aos_condition_4_lte_roveout_threshold";
        public static final String LTE_ROVEOUT_THRESHOLD_5 =
                "aos_condition_5_lte_roveout_threshold";

        public static final String THREE_G_ROVEIN_THRESHOLD_0 =
                "aos_condition_0_3g_rovein_threshold";
        public static final String THREE_G_ROVEIN_THRESHOLD_1 =
                "aos_condition_1_3g_rovein_threshold";
        public static final String THREE_G_ROVEIN_THRESHOLD_2 =
                "aos_condition_2_3g_rovein_threshold";
        public static final String THREE_G_ROVEIN_THRESHOLD_3 =
                "aos_condition_3_3g_rovein_threshold";
        public static final String THREE_G_ROVEIN_THRESHOLD_4 =
                "aos_condition_4_3g_rovein_threshold";
        public static final String THREE_G_ROVEIN_THRESHOLD_5 =
                "aos_condition_5_3g_rovein_threshold";

        public static final String THREE_G_ROVEOUT_THRESHOLD_0 =
                "aos_condition_0_3g_roveout_threshold";
        public static final String THREE_G_ROVEOUT_THRESHOLD_1 =
                "aos_condition_1_3g_roveout_threshold";
        public static final String THREE_G_ROVEOUT_THRESHOLD_2 =
                "aos_condition_2_3g_roveout_threshold";
        public static final String THREE_G_ROVEOUT_THRESHOLD_3 =
                "aos_condition_3_3g_roveout_threshold";
        public static final String THREE_G_ROVEOUT_THRESHOLD_4 =
                "aos_condition_4_3g_roveout_threshold";
        public static final String THREE_G_ROVEOUT_THRESHOLD_5 =
                "aos_condition_5_3g_roveout_threshold";

        public static final String TWO_G_ROVEIN_THRESHOLD_0 =
                "aos_condition_0_2g_rovein_threshold";
        public static final String TWO_G_ROVEIN_THRESHOLD_1 =
                "aos_condition_1_2g_rovein_threshold";
        public static final String TWO_G_ROVEIN_THRESHOLD_2 =
                "aos_condition_2_2g_rovein_threshold";
        public static final String TWO_G_ROVEIN_THRESHOLD_3 =
                "aos_condition_3_2g_rovein_threshold";
        public static final String TWO_G_ROVEIN_THRESHOLD_4 =
                "aos_condition_4_2g_rovein_threshold";
        public static final String TWO_G_ROVEIN_THRESHOLD_5 =
                "aos_condition_5_2g_rovein_threshold";

        public static final String TWO_G_ROVEOUT_THRESHOLD_0 =
                "aos_condition_0_2g_roveout_threshold";
        public static final String TWO_G_ROVEOUT_THRESHOLD_1 =
                "aos_condition_1_2g_roveout_threshold";
        public static final String TWO_G_ROVEOUT_THRESHOLD_2 =
                "aos_condition_2_2g_roveout_threshold";
        public static final String TWO_G_ROVEOUT_THRESHOLD_3 =
                "aos_condition_3_2g_roveout_threshold";
        public static final String TWO_G_ROVEOUT_THRESHOLD_4 =
                "aos_condition_4_2g_roveout_threshold";
        public static final String TWO_G_ROVEOUT_THRESHOLD_5 =
                "aos_condition_5_2g_roveout_threshold";

        public static final String WIFI_ROVEIN_THRESHOLD_0 =
                "aos_condition_0_wifi_rovein_threshold";
        public static final String WIFI_ROVEIN_THRESHOLD_1 =
                "aos_condition_1_wifi_rovein_threshold";
        public static final String WIFI_ROVEIN_THRESHOLD_2 =
                "aos_condition_2_wifi_rovein_threshold";
        public static final String WIFI_ROVEIN_THRESHOLD_3 =
                "aos_condition_3_wifi_rovein_threshold";
        public static final String WIFI_ROVEIN_THRESHOLD_4 =
                "aos_condition_4_wifi_rovein_threshold";
        public static final String WIFI_ROVEIN_THRESHOLD_5 =
                "aos_condition_5_wifi_rovein_threshold";

        public static final String WIFI_ROVEOUT_THRESHOLD_0 =
                "aos_condition_0_wifi_roveout_threshold";
        public static final String WIFI_ROVEOUT_THRESHOLD_1 =
                "aos_condition_1_wifi_roveout_threshold";
        public static final String WIFI_ROVEOUT_THRESHOLD_2 =
                "aos_condition_2_wifi_roveout_threshold";
        public static final String WIFI_ROVEOUT_THRESHOLD_3 =
                "aos_condition_3_wifi_roveout_threshold";
        public static final String WIFI_ROVEOUT_THRESHOLD_4 =
                "aos_condition_4_wifi_roveout_threshold";
        public static final String WIFI_ROVEOUT_THRESHOLD_5 =
                "aos_condition_5_wifi_roveout_threshold";

    }

    public static final class AoSConnection
    {
        // Table name
        public static final String TABLE_NAME = "gims_aosconnection";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        // Profile name
        public static final String PROFILE_NAME_0 = "aos_connection_0_profile_name";
        public static final String PROFILE_NAME_1 = "aos_connection_1_profile_name";
        public static final String PROFILE_NAME_2 = "aos_connection_2_profile_name";
        public static final String PROFILE_NAME_3 = "aos_connection_3_profile_name";
        public static final String PROFILE_NAME_4 = "aos_connection_4_profile_name";
        public static final String PROFILE_NAME_5 = "aos_connection_5_profile_name";
        // Access policy
        public static final String ACCESS_POLICY_0 = "aos_connection_0_access_policy";
        public static final String ACCESS_POLICY_1 = "aos_connection_1_access_policy";
        public static final String ACCESS_POLICY_2 = "aos_connection_2_access_policy";
        public static final String ACCESS_POLICY_3 = "aos_connection_3_access_policy";
        public static final String ACCESS_POLICY_4 = "aos_connection_4_access_policy";
        public static final String ACCESS_POLICY_5 = "aos_connection_5_access_policy";
        // Access policy Roaming
        public static final String ACCESS_POLICY_ROAMING_0 =
                "aos_connection_0_access_policy_roaming";
        public static final String ACCESS_POLICY_ROAMING_1 =
                "aos_connection_1_access_policy_roaming";
        public static final String ACCESS_POLICY_ROAMING_2 =
                "aos_connection_2_access_policy_roaming";
        public static final String ACCESS_POLICY_ROAMING_3 =
                "aos_connection_3_access_policy_roaming";
        public static final String ACCESS_POLICY_ROAMING_4 =
                "aos_connection_4_access_policy_roaming";
        public static final String ACCESS_POLICY_ROAMING_5 =
                "aos_connection_5_access_policy_roaming";
        // IP version
        public static final String IP_VERSION_0 = "aos_connection_0_ip_version";
        public static final String IP_VERSION_1 = "aos_connection_1_ip_version";
        public static final String IP_VERSION_2 = "aos_connection_2_ip_version";
        public static final String IP_VERSION_3 = "aos_connection_3_ip_version";
        public static final String IP_VERSION_4 = "aos_connection_4_ip_version";
        public static final String IP_VERSION_5 = "aos_connection_5_ip_version";
        // Connection Feature
        public static final String FEATURES_0 = "aos_connection_0_features";
        public static final String FEATURES_1 = "aos_connection_1_features";
        public static final String FEATURES_2 = "aos_connection_2_features";
        public static final String FEATURES_3 = "aos_connection_3_features";
        public static final String FEATURES_4 = "aos_connection_4_features";
        public static final String FEATURES_5 = "aos_connection_5_features";

        public static final String IPV6_DELAY_INTERVAL_0 =
                "aos_connection_0_ipv6_delay_interval";
        public static final String IPV6_DELAY_INTERVAL_1 =
                "aos_connection_1_ipv6_delay_interval";
        public static final String IPV6_DELAY_INTERVAL_2 =
                "aos_connection_2_ipv6_delay_interval";
        public static final String IPV6_DELAY_INTERVAL_3 =
                "aos_connection_3_ipv6_delay_interval";
        public static final String IPV6_DELAY_INTERVAL_4 =
                "aos_connection_4_ipv6_delay_interval";
        public static final String IPV6_DELAY_INTERVAL_5 =
                "aos_connection_5_ipv6_delay_interval";

        public static final String RAT_GUARD_INTERVAL_0 = "aos_connection_0_rat_guard_interval";
        public static final String RAT_GUARD_INTERVAL_1 = "aos_connection_1_rat_guard_interval";
        public static final String RAT_GUARD_INTERVAL_2 = "aos_connection_2_rat_guard_interval";
        public static final String RAT_GUARD_INTERVAL_3 = "aos_connection_3_rat_guard_interval";
        public static final String RAT_GUARD_INTERVAL_4 = "aos_connection_4_rat_guard_interval";
        public static final String RAT_GUARD_INTERVAL_5 = "aos_connection_5_rat_guard_interval";

        public static final String EPDG_SCHEME_0 = "aos_connection_0_epdg_scheme";
        public static final String EPDG_SCHEME_1 = "aos_connection_1_epdg_scheme";
        public static final String EPDG_SCHEME_2 = "aos_connection_2_epdg_scheme";
        public static final String EPDG_SCHEME_3 = "aos_connection_3_epdg_scheme";
        public static final String EPDG_SCHEME_4 = "aos_connection_4_epdg_scheme";
        public static final String EPDG_SCHEME_5 = "aos_connection_5_epdg_scheme";

        // Support MPDN for GSMA
        public static final String MPDN = "common_mpdn";

        // Check IMS voice over PS Session Supported or not
        public static final String PS_SUPPORTED = "common_ps_supported";

        // DCN Guard Time Interval
        public static final String DCN_SERVICE_IN_TIME = "dcn_service_in_time";
    }

    public static final class AoSHandle
    {
        public static final String TABLE_NAME = "gims_aoshandle";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String UNIQUENESS = "Uniqueness_aos_handle";

        public static final String ID_0 = "aos_handle_0_id";
        public static final String ID_1 = "aos_handle_1_id";
        public static final String ID_2 = "aos_handle_2_id";
        public static final String ID_3 = "aos_handle_3_id";
        public static final String ID_4 = "aos_handle_4_id";
        public static final String ID_5 = "aos_handle_5_id";

        public static final String FEATURES_0 = "aos_handle_0_features";
        public static final String FEATURES_1 = "aos_handle_1_features";
        public static final String FEATURES_2 = "aos_handle_2_features";
        public static final String FEATURES_3 = "aos_handle_3_features";
        public static final String FEATURES_4 = "aos_handle_4_features";
        public static final String FEATURES_5 = "aos_handle_5_features";

    }

    public static final class AoSProvider
    {
        // Table name
        public static final String TABLE_NAME = "gims_aosprovider";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String COMMON_FEATURES = "aos_provider_common_features";
    }

    public static final class AoSReg
    {
        // Table name
        public static final String TABLE_NAME = "gims_aosreg";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final class RegFeatures {
            public static final int STANDARD_SPECIFICATION_SUPPORTED = 0x00100000;
        };

        // Retry interval
        public static final String RETRY_INTERVAL_0 = "aos_reg_0_retry_interval";
        public static final String RETRY_INTERVAL_1 = "aos_reg_1_retry_interval";
        public static final String RETRY_INTERVAL_2 = "aos_reg_2_retry_interval";
        public static final String RETRY_INTERVAL_3 = "aos_reg_3_retry_interval";
        // Retry repeat interval
        public static final String RETRY_REPEAT_INTERVAL_0 = "aos_reg_0_retry_repeat_interval";
        public static final String RETRY_REPEAT_INTERVAL_1 = "aos_reg_1_retry_repeat_interval";
        public static final String RETRY_REPEAT_INTERVAL_2 = "aos_reg_2_retry_repeat_interval";
        public static final String RETRY_REPEAT_INTERVAL_3 = "aos_reg_3_retry_repeat_interval";
        // IPSec
        public static final String IPSEC_0 = "aos_reg_0_ipsec";
        public static final String IPSEC_1 = "aos_reg_1_ipsec";
        public static final String IPSEC_2 = "aos_reg_2_ipsec";
        public static final String IPSEC_3 = "aos_reg_3_ipsec";

        //for GSMA
        public static final String IPSEC_SPI_3GPP_0 = "aos_reg_0_ipsec_spi_3gpp";
        public static final String IPSEC_SPI_3GPP_1 = "aos_reg_1_ipsec_spi_3gpp";
        public static final String IPSEC_SPI_3GPP_2 = "aos_reg_2_ipsec_spi_3gpp";
        public static final String IPSEC_SPI_3GPP_3 = "aos_reg_3_ipsec_spi_3gpp";

        // IPSec Algorithm
        public static final String IPSEC_0_ALGS = "aos_reg_0_ipsec_algs";
        public static final String IPSEC_1_ALGS = "aos_reg_1_ipsec_algs";
        public static final String IPSEC_2_ALGS = "aos_reg_2_ipsec_algs";
        public static final String IPSEC_3_ALGS = "aos_reg_3_ipsec_algs";

        public static final String RETRY_BASE_TIME_0 = "aos_reg_0_retry_base_time";
        public static final String RETRY_BASE_TIME_1 = "aos_reg_1_retry_base_time";
        public static final String RETRY_BASE_TIME_2 = "aos_reg_2_retry_base_time";
        public static final String RETRY_BASE_TIME_3 = "aos_reg_3_retry_base_time";

        public static final String RETRY_MAX_TIME_0 = "aos_reg_0_retry_max_time";
        public static final String RETRY_MAX_TIME_1 = "aos_reg_1_retry_max_time";
        public static final String RETRY_MAX_TIME_2 = "aos_reg_2_retry_max_time";
        public static final String RETRY_MAX_TIME_3 = "aos_reg_3_retry_max_time";

        public static final String FEATURES_0 = "aos_reg_0_features";
        public static final String FEATURES_1 = "aos_reg_1_features";
        public static final String FEATURES_2 = "aos_reg_2_features";
        public static final String FEATURES_3 = "aos_reg_3_features";

        public static final String NORMAL_REG_FLOW_ID_0 = "aos_reg_0_normal_reg_flow_id";
        public static final String NORMAL_REG_FLOW_ID_1 = "aos_reg_1_normal_reg_flow_id";
        public static final String NORMAL_REG_FLOW_ID_2 = "aos_reg_2_normal_reg_flow_id";
        public static final String NORMAL_REG_FLOW_ID_3 = "aos_reg_3_normal_reg_flow_id";

        public static final String AUTHENTICATION_MAX_COUNT_0 =
                "aos_reg_0_authentication_max_count";
        public static final String AUTHENTICATION_MAX_COUNT_1 =
                "aos_reg_1_authentication_max_count";
        public static final String AUTHENTICATION_MAX_COUNT_2 =
                "aos_reg_2_authentication_max_count";
        public static final String AUTHENTICATION_MAX_COUNT_3 =
                "aos_reg_3_authentication_max_count";

        public static final String REG_DEFAULT_RETRY_AFTER_INTERVAL_0 =
                "aos_reg_0_reg_default_retry_after_interval";
        public static final String REG_DEFAULT_RETRY_AFTER_INTERVAL_1 =
                "aos_reg_1_reg_default_retry_after_interval";
        public static final String REG_DEFAULT_RETRY_AFTER_INTERVAL_2 =
                "aos_reg_2_reg_default_retry_after_interval";
        public static final String REG_DEFAULT_RETRY_AFTER_INTERVAL_3 =
                "aos_reg_3_reg_default_retry_after_interval";

        public static final String SCHEME_0 = "aos_reg_0_scheme";
        public static final String SCHEME_1 = "aos_reg_1_scheme";
        public static final String SCHEME_2 = "aos_reg_2_scheme";
        public static final String SCHEME_3 = "aos_reg_3_scheme";

        public static final String SUB_DEFAULT_RETRY_AFTER_INTERVAL_0 =
                "aos_reg_0_sub_default_retry_after_interval";
        public static final String SUB_DEFAULT_RETRY_AFTER_INTERVAL_1 =
                "aos_reg_1_sub_default_retry_after_interval";
        public static final String SUB_DEFAULT_RETRY_AFTER_INTERVAL_2 =
                "aos_reg_2_sub_default_retry_after_interval";
        public static final String SUB_DEFAULT_RETRY_AFTER_INTERVAL_3 =
                "aos_reg_3_sub_default_retry_after_interval";

        public static final String IPSEC_GUARD_LIFE_TIME_0 = "aos_reg_0_ipsec_guard_life_time";
        public static final String IPSEC_GUARD_LIFE_TIME_1 = "aos_reg_1_ipsec_guard_life_time";
        public static final String IPSEC_GUARD_LIFE_TIME_2 = "aos_reg_2_ipsec_guard_life_time";
        public static final String IPSEC_GUARD_LIFE_TIME_3 = "aos_reg_3_ipsec_guard_life_time";

        public static final String IPSEC_PORT_INTERVAL_0 = "aos_reg_0_ipsec_port_interval";
        public static final String IPSEC_PORT_INTERVAL_1 = "aos_reg_1_ipsec_port_interval";
        public static final String IPSEC_PORT_INTERVAL_2 = "aos_reg_2_ipsec_port_interval";
        public static final String IPSEC_PORT_INTERVAL_3 = "aos_reg_3_ipsec_port_interval";

        public static final String REGINFO_CONTACT_MATCH_RULE_0 =
                "aos_reg_0_reginfo_contact_match_rule";
        public static final String REGINFO_CONTACT_MATCH_RULE_1 =
                "aos_reg_1_reginfo_contact_match_rule";
        public static final String REGINFO_CONTACT_MATCH_RULE_2 =
                "aos_reg_2_reginfo_contact_match_rule";
        public static final String REGINFO_CONTACT_MATCH_RULE_3 =
                "aos_reg_3_reginfo_contact_match_rule";

        public static final String RETRY_PCSCF_COUNT_0 = "aos_reg_0_retry_pcscf_count";
        public static final String RETRY_PCSCF_COUNT_1 = "aos_reg_1_retry_pcscf_count";
        public static final String RETRY_PCSCF_COUNT_2 = "aos_reg_2_retry_pcscf_count";
        public static final String RETRY_PCSCF_COUNT_3 = "aos_reg_3_retry_pcscf_count";

        public static final String RETRY_RESPONSE_CODES_0 = "aos_reg_0_retry_response_codes";
        public static final String RETRY_RESPONSE_CODES_1 = "aos_reg_1_retry_response_codes";
        public static final String RETRY_RESPONSE_CODES_2 = "aos_reg_2_retry_response_codes";
        public static final String RETRY_RESPONSE_CODES_3 = "aos_reg_3_retry_response_codes";

        public static final String BLOCK_CONDITIONS_0 = "aos_reg_0_block_conditions";
        public static final String BLOCK_CONDITIONS_1 = "aos_reg_1_block_conditions";
        public static final String BLOCK_CONDITIONS_2 = "aos_reg_2_block_conditions";
        public static final String BLOCK_CONDITIONS_3 = "aos_reg_3_block_conditions";

        public static final String USERINFO_POLICY_0 = "aos_reg_0_userinfo_policy";
        public static final String USERINFO_POLICY_1 = "aos_reg_1_userinfo_policy";
        public static final String USERINFO_POLICY_2 = "aos_reg_2_userinfo_policy";
        public static final String USERINFO_POLICY_3 = "aos_reg_3_userinfo_policy";

        public static final String TCP_KEEPALIVE_OPTION_0 = "aos_reg_0_tcp_keepalive_option";
        public static final String TCP_KEEPALIVE_OPTION_1 = "aos_reg_1_tcp_keepalive_option";
        public static final String TCP_KEEPALIVE_OPTION_2 = "aos_reg_2_tcp_keepalive_option";
        public static final String TCP_KEEPALIVE_OPTION_3 = "aos_reg_3_tcp_keepalive_option";

        public static final String TCP_KEEPALIVE_VALUE_0 = "aos_reg_0_tcp_keepalive_value";
        public static final String TCP_KEEPALIVE_VALUE_1 = "aos_reg_1_tcp_keepalive_value";
        public static final String TCP_KEEPALIVE_VALUE_2 = "aos_reg_2_tcp_keepalive_value";
        public static final String TCP_KEEPALIVE_VALUE_3 = "aos_reg_3_tcp_keepalive_value";

        public static final String TCP_CRITERION_LENGTH_IPV4_0 =
                "aos_reg_0_tcp_criterion_length_ipv4";
        public static final String TCP_CRITERION_LENGTH_IPV4_1 =
                "aos_reg_1_tcp_criterion_length_ipv4";
        public static final String TCP_CRITERION_LENGTH_IPV4_2 =
                "aos_reg_2_tcp_criterion_length_ipv4";
        public static final String TCP_CRITERION_LENGTH_IPV4_3 =
                "aos_reg_3_tcp_criterion_length_ipv4";

        public static final String TCP_CRITERION_LENGTH_IPV6_0 =
                "aos_reg_0_tcp_criterion_length_ipv6";
        public static final String TCP_CRITERION_LENGTH_IPV6_1 =
                "aos_reg_1_tcp_criterion_length_ipv6";
        public static final String TCP_CRITERION_LENGTH_IPV6_2 =
                "aos_reg_2_tcp_criterion_length_ipv6";
        public static final String TCP_CRITERION_LENGTH_IPV6_3 =
                "aos_reg_3_tcp_criterion_length_ipv6";

        public static final String DSCP_VALUE_0 = "aos_reg_0_dscp_value";
        public static final String DSCP_VALUE_1 = "aos_reg_1_dscp_value";
        public static final String DSCP_VALUE_2 = "aos_reg_2_dscp_value";
        public static final String DSCP_VALUE_3 = "aos_reg_3_dscp_value";

        // Check GBA support or not
        public static final String GBA = "aos_reg_gba";

        public static final String IMS_INDICATOR_0 = "aos_reg_0_ims_indicator_onoff";
    }

    public static final class Session
    {
        // Session parameters
        public static final String MAX_COUNT = "session_max_count";
        public static final String TV_MO_NO_ANSWER = "session_tv_mo_no_answer";
        public static final String TV_MO_1XX_WAIT = "session_tv_mo_1xx_wait";
        public static final String TV_MT_ALERTING = "session_tv_mt_alerting";
        public static final String CONF_FACTORY_URI = "session_conf_factory_uri";
        public static final String RPR_SUPPORTED = "session_rpr_supported";
        public static final String PRECONDITION_SUPPORTED = "session_precondition_supported";
        public static final String PRECONDITION_TIMER_MODE = "session_precondition_timer_mode";
    }

    public static final class Media
    {
        // Table name
        public static final String TABLE_NAME = "gims_com_media";

        // Content URL for table
        public static final Uri CONTENT_URI
                            = Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        // Media parameters
        public static final String LOOPBACK = "media_loopback";
        public static final String TV_HEARTBEAT = "media_tv_heartbeat";
        public static final String SESSION_LEVEL_BW = "media_session_level_bw";
        public static final String AUDIO_COUNT = "media_audio_count";
        public static final String AUDIO_REF = "media_audio_ref";
        public static final String VIDEO_COUNT = "media_video_count";
        public static final String VIDEO_REF = "media_video_ref";
        public static final String RTP_ANALYZER = "media_rtp_analyzer";

        public static final String PORT_RTP = "port_rtp";
        public static final String PORT_RTCP = "port_rtcp";
        public static final String JITTER_BUFFER_SIZE = "jitter_buffer_size";
        public static final String CODECS = "codecs";
        public static final String CODEC_REF = "codec_ref";

        public static final class SubTable {
            // Audio
            public static final String AUDIO = "gims_com_media_audio";
            public static final String AUDIO_LVT = "gims_com_medialegacyvt_audio";

            public static final class MediaAudio {
                // TABLE : gims_com_media_audio
                public static final String AUDIO_SRTP_ENABLE = "audio_0_srtp_enable";
                public static final String AUDIO_SOCKET_TOS = "audio_0_socket_tos";
            }

            // Video
            public static final String VIDEO = "gims_com_media_video";
            //public static final String VIDEO_LVT = "gims_com_medialegacyvt_video";

            public static final class MediaVideo {
                // TABLE : gims_com_media_video
                public static final String VIDEO_SRTP_ENABLE = "video_0_srtp_enable";
                public static final String VIDEO_AVPF_ENABLE = "video_0_avpf_enable";
                public static final String VIDEO_SOCKET_TOS = "video_0_socket_tos";
            }

            // Text
            public static final String TEXT = "gims_com_media_text";

            // Codec
            public static final String CODEC_AUDIO_VOLTE = "gims_com_media_audio_codec_volte";
            public static final String CODEC_AUDIO_VT = "gims_com_media_audio_codec_vt";
            public static final String CODEC_VIDEO_VT = "gims_com_media_video_codec_vt";
            public static final String CODEC_TEXT_VOLTE = "gims_com_media_text_codec_volte";
            public static final String CODEC_TEXT_VT = "gims_com_media_text_codec_vt";
        }

        public static final class Audio
        {
            public static final String PTIME = "ptime";
            public static final String MAXPTIME = "maxptime";
        }

        public static final class Video
        {
            public static final String FRAMERATE = "framerate";
            public static final String FRAMESIZE = "framesize";
            public static final String BITRATE = "bitrate";
            public static final String BITRATE_CONTROL = "bitrate_control";
        }

        public static final class Text
        {
            // Nothing
        }

        public static final class Codec
        {
            public static final String PAYLOAD_TYPE = "payload_type";

            public static final class AMR
            {
                public static final String MODE_SET = "mode_set";
                public static final String OCTET_ALIGN = "octet_align";
                public static final String MODE_CHANGE_CAPABILITY = "mode_change_capability";
            }

            public static final class H263
            {
                public static final String PROFILE = "profile";
                public static final String LEVEL = "level";
            }

            public static final class H264
            {
                public static final String PACKETIZATION_MODE = "packetization_mode";
                public static final String PROFILE_LEVEL_ID = "profile_level_id";
            }

            public static final class RED
            {
                public static final String REDUNDANCY = "redundancy";
                public static final String RED_SUBTYPE = "red_subtype";
            }
        }
    }

    public static final class EAB
    {

        // Table name
        public static final String TABLE_NAME = "gims_com_service_eab";

        // Content URL for eab table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String EAB_SERVICE_FLAG = "presence_eab_flag";
        public static final String EAB_SERVICE_STATUS = "presence_eab_status";
        public static final String EAB_ALLOW_VT_REGRADLESS_OF_EAB =
                "presence_allow_vt_regardless_of_eab";
        public static final String EAB_VIDEO_CAPABILITY_SHARING = "eab_video_capability_sharing";
        //PUBLICATION
        public static final String EAB_EXPIRE_VALUE_IN_PUBLISH = "eab_expire_value_in_publish";
        public static final String EAB_EXTENDED_EXPIRE_VALUE_IN_PUBLISH =
                "eab_extended_expire_value_in_publish";
        public static final String EAB_THROTTLE_TIME_IN_PUBLISH = "eab_throttle_time_in_publish";
        public static final String EAB_PUBLISH_489_RETRY_TIMER = "eab_publish_489_retry_timer";
        public static final String EAB_PUBLISH_ERROR_RETRY_TIMER = "eab_publish_error_retry_timer";
        public static final String EAB_PUBLISH_REFRESH_RATIO = "eab_publish_refresh_ratio";
        //SINGLE SUBSCRIPTION
        public static final String EAB_SUBSCRIBE_PER_SECOND = "eab_subscribe_per_second";
        //LIST SUBSCRIPTION
        public static final String EAB_MAX_NUMBER_OFSUBSCRIPTIONS_INPRESENCE_LIST =
                "eab_max_number_ofsubscriptions_inpresence_list";
        public static final String EAB_EXPIRE_VALUE_IN_LIST_SUBSCRIBE =
                "eab_expire_value_in_list_subscribe";
        //SETTINGS & MISC
        public static final String EAB_SUPPORT_GZIP = "eab_support_gzip";
        public static final String EAB_POLLING_PERIOD = "eab_polling_period";
        public static final String EAB_POLLING_RATE = "eab_polling_rate";
        public static final String EAB_POLLING_RATE_PERIOD = "eab_polling_rate_period";
        public static final String EAB_CAPABILITY_INFO_EXPIRY = "eab_capability_info_expiry";
        public static final String EAB_NON_RCS_CAPABILITY_INFO_EXPIRY =
                "eab_non_rcs_capability_info_expiry";
        public static final String EAB_AVAILABILITY_INFO_EXPIRY = "eab_availability_info_expiry";
        public static final String EAB_POLLING_PERIOD_RANDOMIZATION =
                "eab_polling_period_randomization";
        public static final String EAB_POLLING_PERIOD_INITIALIZATION =
                "eab_polling_period_initialization";
        public static final String EAB_ALWAYS_AVAILABILITY_FETCH = "eab_always_availability_fetch";
        public static final String EAB_USE_EAB_VALUE_FROM_AC = "eab_use_eab_value_from_ac";
        public static final String EAB_RLS_URI = "eab_rls_uri";
        public static final String EAB_DISABLE_INITIAL_ADDRESS_BOOK_SCAN =
                "eab_disable_initial_address_book_scan";
        public static final String EAB_SUBSCIRBE_INDEPENDENT_OF_PUBLISH =
                "eab_subscribe_independent_of_publish";
        public static final String EAB_CHECK_VALID_NUMBER = "eab_check_valid_number";
        //SERVICE CAPABILITY & NETWORK
        public static final String EAB_CAPABILITY_DISCOVERY_ALLOWED_PREFIXES =
                "eab_capability_discovery_allowed_prefixes";
        public static final String EAB_SERVICE_CAPABILITY_DISCOVERY_VIA_PRESENCE_POLICY =
                "eab_service_capability_discovery_via_presence_policy";
        public static final String EAB_SERVICE_IP_CALL_POLICY = "eab_service_ip_call_policy";
        public static final String EAB_SERVICE_STANDALONE_MESSAGING_POLICY =
                "eab_service_standalone_messaging_policy";
        public static final String EAB_SERVICE_CHAT_POLICY = "eab_service_chat_policy";
        public static final String EAB_SERVICE_IM_POLICY = "eab_service_im_policy";
        public static final String EAB_SERVICE_FILE_TRANSFER_POLICY =
                "eab_service_file_transfer_policy";
        public static final String EAB_SERVICE_GROUP_CHAT_FULL_STAND_FWD_POLICY =
                "eab_service_group_chat_full_stand_fwd_policy";
        public static final String EAB_SERVICE_FILE_TRANSFER_THUMBNAIL_POLICY =
                "eab_service_file_transfer_thumbnail_policy";
        public static final String EAB_SERVICE_FILE_TRANSFER_STORE_FORWARD_POLICY =
                "eab_service_file_transfer_store_forward_policy";
        public static final String EAB_SERVICE_FILE_TRANSFER_HTTP_POLICY =
                "eab_service_file_transfer_http_policy";
        public static final String EAB_SERVICE_GEOLOCATION_PUSH_POLICY =
                "eab_service_geolocation_push_policy";
        public static final String EAB_SERVICE_SHARED_MAP_POLICY = "eab_service_shared_map_policy";
        public static final String EAB_SERVICE_SHARED_SKETCH_POLICY =
                "eab_service_shared_sketch_policy";
        public static final String EAB_SERVICE_CALL_COMPOSER_POLICY =
                "eab_service_call_composer_policy";
        public static final String EAB_SERVICE_POST_CALL_POLICY = "eab_service_post_call_policy";
        public static final String EAB_SERVICE_CHATBOT_POLICY = "eab_service_chatbot_policy";
        public static final String EAB_SERVICE_CHATBOT_STANDALONE_POLICY =
                "eab_service_chatbot_standalone_policy";
        public static final String EAB_SERVICE_CHATBOT_EXTEND_MSG_POLICY =
                "eab_service_chatbot_extend_msg_policy";
        public static final String EAB_SERVICE_GEOLOCATION_PUSH_VIA_SMS_POLICY =
                "eab_service_geolocation_push_via_sms_policy";
        public static final String EAB_SERVICE_FILE_TRANSFER_VIA_SMS_POLICY =
                "eab_service_file_transfer_via_sms_policy";
    }

    public static final class MMTel
    {
        // Table name
        public static final String TABLE_NAME = "gims_com_service_mmtel";

        // Content URL for mmtel table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String MMTEL_PDN = "mmtel_pdn";
        public static final String MMTEL_ADDRESS_TYPE = "mmtel_address_type";
        public static final String MMTEL_TLS = "mmtel_tls";
        public static final String MMTEL_SERVER_IP = "mmtel_server_ip";
        public static final String MMTEL_SERVER_PORT  = "mmtel_server_port";
        public static final String MMTEL_PROVISIONING = "mmtel_provisioning";
        public static final String MMTEL_SERVICE_VERSION = "mmtel_service_version";
        public static final String MMTEL_CONTROL_PREFERENCE = "mmtel_control_preference";
        public static final String MMTEL_CHECK_REG = "mmtel_check_reg";
        public static final String MMTEL_AUTH_TYPE = "mmtel_auth_type";
        public static final String MMTEL_AUTH_USERNAME = "mmtel_auth_username";
        public static final String MMTEL_AUTH_PASSWORD = "mmtel_auth_password";
        public static final String MMTEL_NAMESPACE = "mmtel_namespace";
        public static final String MMTEL_TARGET = "mmtel_target";
        public static final String MMTEL_SOFTINDIA_WIFI_TEST = "mmtel_softindia_wifi_test";
        public static final String MMTEL_XUI_PRIORITY = "mmtel_xui_priority";
        public static final String MMTEL_AUTO_CS_RETRY = "mmtel_auto_cs_retry";
        public static final String MMTEL_RULEID_FROM_RESPONSE = "mmtel_ruleID_from_response";
        public static final String MMTEL_SRV_RECORD = "mmtel_srv_record";
        public static final String MMTEL_USSD_OVER_IMS = "mmtel_ussd_over_ims";
        public static final String MMTEL_DOMAIN_SETTING = "mmtel_domain_setting";
        public static final String MMTEL_SUPPORT_ERASURE = "mmtel_support_erasure";
        public static final String MMTEL_ACCESS_POLICY = "mmtel_access_policy";
        public static final String MMTEL_SUPPORT_SERVICE = "mmtel_support_services";
        public static final String MMTEL_PROVISIONING_VOWIFI = "mmtel_provisioning_vowifi";
        public static final String MMTEL_URL_QUERYPREFIX = "mmtel_url_queryPrefix";
        public static final String MMTEL_QUERY_ON_BOOTUP = "mmtel_query_on_bootup";
        public static final String MMTEL_TRUSTALLHOSTS = "mmtel_trustAllHosts";
        public static final String MMTEL_URLADDR_INCLUDE_URLPORT = "mmtel_urlAddr_include_urlPort";

        public static final String MMTEL_CW_METHOD = "mmtel_cw_method";
        public static final String MMTEL_SETTING_UNDER_CS_ON_TB_CW =
                "mmtel_setting_under_cs_on_tb_cw";
        public static final String MMTEL_OIR_METHOD = "mmtel_oir_method";
        public static final String MMTEL_SETTING_UNDER_CS_ON_TB_OIR =
                "mmtel_setting_under_cs_on_tb_oir";

    }

    public static final class MMTel_CF
    {
        // Table name
        public static final String TABLE_NAME = "gims_com_service_mmtel_cf";

        // Content URL for mmtel_cf table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String MMTEL_SUPPORT_CF_RULES = "mmtel_support_cf_rules";
        public static final String MMTEL_CF_TARGET_NUMBER_PREFIX = "mmtel_cf_target_number_prefix";
        public static final String MMTEL_CF_GET_REQUEST_EACH_RULE =
                "mmtel_cf_get_request_each_rule";
        public static final String MMTEL_CF_RULE_ID_UNCONDITIONAL =
                "mmtel_cf_rule_id_unconditional";
        public static final String MMTEL_CF_RULE_ID_BUSY = "mmtel_cf_rule_id_busy";
        public static final String MMTEL_CF_RULE_ID_NO_REPLY = "mmtel_cf_rule_id_no_reply";
        public static final String MMTEL_CF_RULE_ID_NOT_REACHABLE =
                "mmtel_cf_rule_id_not_reachable";
        public static final String MMTEL_CF_RULE_ID_NOT_LOGGED_IN =
                "mmtel_cf_rule_id_not_logged_in";
        public static final String MMTEL_CF_RULE_ID_UNCONDITIONAL_VIDEO =
                "mmtel_cf_rule_id_unconditional_video";
        public static final String MMTEL_CF_RULE_ID_BUSY_VIDEO = "mmtel_cf_rule_id_busy_video";
        public static final String MMTEL_CF_RULE_ID_NO_REPLY_VIDEO =
                "mmtel_cf_rule_id_no_reply_video";
        public static final String MMTEL_CF_RULE_ID_NOT_REACHABLE_VIDEO =
                "mmtel_cf_rule_id_not_reachable_video";
        public static final String MMTEL_CF_RULE_ID_NOT_LOGGED_IN_VIDEO =
                "mmtel_cf_rule_id_not_logged_in_video";
        public static final String MMTEL_TIMER_IN_NO_ANSWER = "mmtel_timer_in_no_answer";
        public static final String MMTEL_CF_MEDIA_AUDIO = "mmtel_cf_media_audio";
        public static final String MMTEL_CF_MEDIA_VIDEO = "mmtel_cf_media_video";

    }

    public static final class MMTel_CB
    {
        // Table name
        public static final String TABLE_NAME = "gims_com_service_mmtel_cb";

        // Content URL for mmtel_cf table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String MMTEL_SUPPORT_CB_RULES = "mmtel_support_cb_rules";
        public static final String MMTEL_CB_MEDIA_AUDIO = "mmtel_cb_media_audio";
        public static final String MMTEL_CB_MEDIA_VIDEO = "mmtel_cb_media_video";
        public static final String MMTEL_CB_RULE_ID_ALL_INCOMING_CALL =
                "mmtel_cb_rule_id_all_incoming_call";
        public static final String MMTEL_CB_RULE_ID_ALL_OUTGOING_CALL =
                "mmtel_cb_rule_id_all_outgoing_call";
        public static final String MMTEL_CB_RULE_ID_OUTGOING_INTERNATIONAL_CALL =
                "mmtel_cb_rule_id_outgoing_international_call";
        public static final String MMTEL_CB_RULE_ID_OUTGOING_INTERNATIONAL_CALL_EX_HOME_COUNTRY =
                "mmtel_cb_rule_id_outgoing_international_call_ex_home_country";
        public static final String MMTEL_CB_RULE_ID_INCOMING_CALL_WHEN_ROAMING =
                "mmtel_cb_rule_id_incoming_call_when_roaming";
        public static final String MMTEL_CB_RULE_ID_ALL_INCOMING_CALL_VIDEO =
                "mmtel_cb_rule_id_all_incoming_call_video";
        public static final String MMTEL_CB_RULE_ID_ALL_OUTGOING_CALL_VIDEO =
                "mmtel_cb_rule_id_all_outgoing_call_video";
        public static final String MMTEL_CB_RULE_ID_OUTGOING_INTERNATIONAL_CALL_VIDEO =
                "mmtel_cb_rule_id_outgoing_international_call_video";
        public static final String
                MMTEL_CB_RULE_ID_OUTGOING_INTERNATIONAL_CALL_EX_HOME_COUNTRY_VIDEO =
                "mmtel_cb_rule_id_outgoing_international_call_ex_home_country_video";
        public static final String MMTEL_CB_RULE_ID_INCOMING_CALL_WHEN_ROAMING_VIDEO =
                "mmtel_cb_rule_id_incoming_call_when_roaming_video";

    }

    public static final class GBA
    {
        // Table name
        public static final String TABLE_NAME = "gims_com_service_gba";

        // Content URL for gba table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String GBA_ENABLED = "gba_enabled";
        public static final String GBA_TYPE = "gba_type";
        public static final String GBA_REQUEST_URI = "gba_request_uri";
        public static final String GBA_HOST = "gba_host";
        public static final String GBA_USERNAME = "gba_username";
        public static final String GBA_REALM = "gba_realm";
        public static final String GBA_BSF_DNS_NAME = "gba_bsf_dns_name";
        public static final String GBA_PDN = "gba_pdn";
        public static final String GBA_BSF_PORT = "gba_bsf_port";
        public static final String GBA_SCHEMA = "gba_schema";
        public static final String GBA_BSF_SELECTION = "gba_bsf_selection";

    }

    public static final class DBInfo
    {
        // Table name
        public static final String TABLE_NAME = "gims_db_info";

        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        // Fields
        public static final String CONFIG_XML_FILE = "config_xml_file";
        public static final String MEDIA_XML_FILE = "media_xml_file";
        public static final String XML_BASE_TAG = "xml_base_tag";
        public static final String CONFIG_XML_MODIFIED_TIME = "config_xml_modified_time";
        public static final String IMSI = "IMSI";
        public static final String MAKE_DB_ON_NEXT_TIME = "make_db_on_next_time";
        public static final String SW_VERSION = "software_version";
    }

    public static final class MediaDBInfo
    {
        // Table name
        public static final String TABLE_NAME = "gims_media_db_info";

        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        // Fields
        public static final String MEDIA_XML_MODIFIED_TIME = "media_xml_modified_time";
    }

    public static final class Statistics
    {
        // Table name
        public static final String TABLE_NAME = "gims_statistics";

        //ON/OFF
        public static final String STATISTICS_ONOFF = "statistics_onoff";
    }

    public static final class Resource
    {
        // Table name
        public static final String TABLE_NAME = "gims_resource";

        // Content URL for mmtel table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        // Check volte icon onoff
        public static final String VOLTE_ICON_INDICATOR_ONOFF = "volte_icon_indicator_onoff";
        public static final String VOLTE_ICON_INDICATOR_RESOURCE = "volte_icon_indicator_resource";
        // Check vowifi icon onoff
        public static final String VOWIFI_ICON_INDICATOR_ONOFF = "vowifi_icon_indicator_onoff";
        public static final String VOWIFI_ICON_INDICATOR_RESOURCE =
                "vowifi_icon_indicator_resource";
    }

    public static final class FEATURE
    {
        // Table name
        public static final String TABLE_NAME = "gims_feature";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI = Uri
                .parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        // trace
        public static final String FEATURE_IPSEC = "FEATURE_IPSEC";
        public static final String FEATURE_ISIM = "FEATURE_ISIM";
        public static final String FEATURE_SDP_PRECONDITION = "FEATURE_SDP_PRECONDITION";
        public static final String FEATURE_GEOLOCATION = "FEATURE_GEOLOCATION";
        public static final String FEATURE_VOLTE = "FEATURE_VOLTE";
        public static final String FEATURE_VOWIFI = "FEATURE_VOWIFI";
        public static final String FEATURE_VT = "FEATURE_VT";
        public static final String FEATURE_SMS = "FEATURE_SMS";
        public static final String FEATURE_UCE = "FEATURE_UCE";
        public static final String FEATURE_VOLTE_IN_ROAMING = "FEATURE_VOLTE_IN_ROAMING";
        public static final String FEATURE_VT_IN_ROAMING = "FEATURE_VT_IN_ROAMING";
    }

    public static final class AOS
    {
        // Table name
        public static final String TABLE_NAME = "gims_aos";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI = Uri
                .parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        // trace
        public static final String CONF = "conf";
    }

    public static final class SMS {
        // Table name
        public static final String TABLE_NAME =  "ims_service_mts";

        // Content URL for service table
        public static Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String SMS_FORMAT = "sdm_sms_format";
        public static final String SMS_OVER_IP_NETWORK = "sdm_sms_over_ip_network";

        public static final String PSI = "common_psi"; //gsma, kddi
    }

    public static final class Setting {
        // Table name
        public static final String TABLE_NAME = "gims_setting";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String VIDEO_SETTING = "setting_video";
        public static final String VOIP_SETTING = "setting_voip";
        public static final String MOBILE_DATA_SETTING = "setting_mobile_data";
        public static final String IRAT_TRANSITION_TIME = "setting_irat_transition_time";
        public static final String TEST_MASK = "setting_test_mask";
        public static final String DEVICE_MODE = "setting_device_mode";
        public static final String MULTIDEVICE_ENABLE = "setting_multidevice_enable";
        public static final String VOWIFI_ENABLE = "setting_vowifi_enable";
        public static final String CHECK_VOPS_FEATURE = "setting_vops_feature";
        public static final String CHECK_VOIP_CAPABILITY = "setting_voip_capability";
        public static final String IMS_APN_NAME = "setting_ims_apn_name";
        public static final String ACTIVE_CALL_HO = "setting_active_call_ho";
     }

    public static final class UC_APP {
        // Table name
        public static final String TABLE_NAME = "gims_uc_app";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String CNAP_TYPE = "nCNAPType";
        public static final String LOCALNUMBER_TYPE = "nLocalNumberType";
    }

    public static final class SessionCommon {
        // Table name
        public static final String TABLE_NAME = "gims_uc_session_common";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String TIMER_MO_NOANSWER = "nUETIMER_MO_NOANSWER";
        public static final String TIMER_MT_ALERTING = "nUETIMER_MT_ALERTING";
        public static final String SRVCC_SUPPORTED_TYPE = "nSRVCCSupportedType";

        public static final String CUSTOMIZED_SESSION_CONFIG = "tCustomizedSessionConfig";
        public static final String CW = "nCW";
    }

    public static final class SessionVoIP {
        // Table name
        public static final String TABLE_NAME = "gims_uc_session_voip";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String TIME_ACTIVE_MEDIA_TH = "nMediaThresholdATime";
        public static final String TIME_HOLD_MEDIA_TH = "nMediaThresholdHTime";

        public static final String CONF_URI = "tConfURI";
        public static final String LOCAL_FEATURE = "nLocalFeature";
    }

    public static final class SessionVt {
        // Table name
        public static final String TABLE_NAME = "gims_uc_session_vt";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String TIME_ACTIVE_MEDIA_TH = "nMediaThresholdATime";
        public static final String TIME_HOLD_MEDIA_TH = "nMediaThresholdHTime";

        public static final String CONF_URI = "tConfURI";
        public static final String LOCAL_FEATURE = "nLocalFeature";
    }

    public static final class UCEmergency {
        // Table name
        public static final String TABLE_NAME = "gims_uc_emergency";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String USINGEPDN_LTE = "bUsingEPDN_LTE";
        public static final String USINGEPDN_WIFI = "bUsingEPDN_WIFI";

        public static final String ECM = "bECM";
    }

    public static final class UCConference {
        // Table name
        public static final String TABLE_NAME = "gims_uc_conference";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String CONF_FEATURES = "features";
    }

    public static final class SessionVzw {
        // Table name
        public static final String TABLE_NAME = "gims_uc_session_vzw";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String SILENT_REDIAL = "bSilentRedial";
        public static final String TIMER_VZW = "nUETIMER_VZW";
        public static final String TIMER_LTE911_FAIL = "nUETIMER_LTE_911_FAIL";
        public static final String TIMER_WIFI911_FAIL = "nUETIMER_WIFI_911_FAIL";

        public static final String CONF_SUBS = "bConfSubs";
        public static final String CONF_RES_LIST = "bConfResList";
        public static final String VICE_SYNC = "bViceSync";
        public static final String VICE_PULL = "bVicePull";
        public static final String CANID = "bCaNID";
    }

    public static final class MediaAudio {
        // Table name
        public static final String TABLE_NAME = "gims_com_media_audio";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String RTP_INACTIVITY_0 = "audio_0_tv_rtp_inactivity";
        public static final String RTP_INACTIVITY_1 = "audio_1_tv_rtp_inactivity";
    }

    public static final class MediaAudioCodecVoLTE {
        // Table name
        public static final String TABLE_NAME = "gims_com_media_audio_codec_volte";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);


    }

    public static final class MediaAudioCodecVT {
        // Table name
        public static final String TABLE_NAME = "gims_com_media_audio_codec_vt";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);
    }

    public static final class COM_EMERGENCY {
        // Table name
        public static final String TABLE_NAME = "gims_com_emergency";

        // Content URL for table
        public static final Uri CONTENT_URI
                            = Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String SUPPORT_OVER_LTE = "support_over_lte";
        public static final String SUPPORT_OVER_WIFI = "support_over_wifi";
        public static final String CONTROL_BY_VOLTE_SETTING = "control_by_volte_setting";
        public static final String CONTROL_BY_IMS_REG = "control_by_ims_reg";
        public static final String REQUIRE_NORMAL_CALL_END = "require_normal_call_end";
        public static final String WIFI_ECALL_BLOCK_FEATURE = "wifi_ecall_block_features";
    }
    public static final class COM_APP_UC
    {
        // Table name
        public static final String TABLE_NAME = "ims_app_mtc";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI = Uri
                .parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        // trace
        public static final String CONF = "conf";
    }

    public static final class MEDIA
    {
        // Table name
        public static final String TABLE_NAME = "gims_media";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI = Uri
                .parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        // trace
        public static final String CONF = "conf";
    }

    public static final class COM_MEDIA_CAPABILITIES
    {
        // Table name
        public static final String TABLE_NAME = "ims_media_capabilities";

        // Content URL for subscriber table
        public static final Uri CONTENT_URI = Uri
                .parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        // trace
        public static final String CONF = "conf";
    }

    public static final class IMS_MO
    {
        // Table name
        public static final String TABLE_NAME = "gims_mo";

        // Content URL for gims_mo table
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        public static final String VOICE_DOMAIN_PREFERENCE_EUTRAN =
                "voice_domain_preference_eutran";

        public static final String[] PI_TTA2_MO_DEFAULT_KEY_FOR_SKT = {
                                                                "ipsec_enabled",
                                                                "lbo_pcscf_address_1",
                                                                "lbo_pcscf_address_2",
                                                                "lbo_pcscf_address_3",
                                                                "lbo_pcscf_address_4",
                                                                "lbo_pcscf_address_5",
                                                                "lbo_pcscf_address_6",
                                                                "lbo_pcscf_address_7",
                                                                "lbo_pcscf_address_8",
                                                                "lbo_pcscf_address_9",
                                                                "lbo_pcscf_address_10"
                                                              };

        public static final String[] PI_TTA2_MO_DEFAULT_VALUE_FOR_SKT = {
                                                               "false",
                                                               "172.28.102.150:5060",
                                                               "[fd00:e15:501:2::150]:5060",
                                                               "172.28.102.100:5060",
                                                               "[fd00:e15:301:2::100]:5060",
                                                               "220.103.220.50:5060",
                                                               "[2001:2d8:e0:220::50]:5060",
                                                               "",
                                                               "",
                                                               "",
                                                               "",
                                                              };

        public static final String SMS_OVER_IP_NETWORKS_INDICATION =
                "sms_over_ip_networks_indication";
        public static final String H265_720P = "h265_720p";
    }
}
