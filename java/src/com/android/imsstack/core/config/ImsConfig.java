package com.android.imsstack.core.config;

public final class ImsConfig {

    // Flags to identify the configuration repository
    // It will be used in lParam or LOWORD(wParam) of IMS_EVENT_CONFIG_UPDATE
    public static final int FLAG_IMS_NONE = 0x0000;
    public static final int FLAG_IMS_SUBSCRIBER = 0x0001;
    public static final int FLAG_IMS_ENGINE = 0x0002;
    public static final int FLAG_IMS_SIP = 0x0004;
    public static final int FLAG_IMS_AOS_CONNECTION = 0x0010;
    public static final int FLAG_IMS_AOS_REG = 0x0020;
    public static final int FLAG_IMS_COM_SIP = 0x0100;
    public static final int FLAG_IMS_COM_MEDIA = 0x0200;

    public static final int FLAG_IMS_COM_SERVICE_SMS = 0x1000;

    public static final int FLAG_IMS_ALL = 0xFFFF;

    public final class VZW {
        // It will be used in LOWORD(wParam) of IMS_EVENT_CONFIG_UPDATE
        // SDM
        // SMS
        public static final int SDM_I_HOME_DOMAIN_NAME = 1;
        public static final int SDM_I_TV_T1 = 2;
        public static final int SDM_I_TV_T2 = 3;
        public static final int SDM_I_TV_TF = 4;
        public static final int SDM_I_SMS_FORMAT = 5;
        public static final int SDM_I_SMS_OVER_IP_NETWORK_INDICATION = 6;
        // VoLTE
        public static final int SDM_I_AMR_MODE_SET = 11;
        public static final int SDM_I_AMR_WB_MODE_SET = 12;
        public static final int SDM_I_PUBLISH_TIMER = 13;
        public static final int SDM_I_PUBLISH_TIMER_EXTENDED = 14;
        public static final int SDM_I_CAPABILITIES_CACHE_EXPIRATION = 15;
        public static final int SDM_I_AVAILABILITY_CACHE_EXPIRATION = 16;
        public static final int SDM_I_CAPABILITIES_POLL_INTERVAL = 17;
        public static final int SDM_I_SOURCE_THROTTLE_PUBLISH = 18;
        public static final int SDM_I_MAX_NUMBER_OF_ENTRIES_IN_REQUEST_CONTAINED_LIST = 19;
        public static final int SDM_I_CAPABILITIES_POLL_LIST_SUBSCRIPTION_EXPIRATION = 20;
        public static final int SDM_I_GZIP_FLAG = 21;
        public static final int SDM_I_SIP_SESSION_TIMER = 22;
        public static final int SDM_I_MIN_SE = 23;
        public static final int SDM_I_TIMER_VZW = 24;
        public static final int SDM_I_TDELAY = 25;
        public static final int SDM_I_SILENT_REDIAL_ENABLE = 26;
        public static final int SDM_I_TLTE_911FAIL = 27;
        public static final int SDM_I_VLT = 28;
        public static final int SDM_I_EAB = 29;
        public static final int SDM_I_LVC = 30;
        public static final int SDM_I_MULTIDEVICE = 31;
        public static final int SDM_I_VOWIFI = 32;

        // PST
        // SMS
        public static final int PST_I_PCSCF_ADDRESS = 1;
        public static final int PST_I_PCSCF_PORT = 2;
        public static final int PST_I_TV_T1 = 3;
        public static final int PST_I_TV_T2 = 4;
        public static final int PST_I_TV_TF = 5;
        public static final int PST_I_SMS_FORMAT = 6;
        public static final int PST_I_SMS_OVER_IP_NETWORK_INDICATION = 7;

        // VOLTE
        public static final int PST_I_URI_MEDIA_RESOURCE_SERVER = 11;
        public static final int PST_I_SESSION_TIMER = 12;
        public static final int PST_I_MIN_SE = 13;
        public static final int PST_I_AMR_WB = 14;
        public static final int PST_I_SCR_AMR = 15;
        public static final int PST_I_SCR_AMR_WB = 16;
        public static final int PST_I_AMR_MODE_SET = 17;
        public static final int PST_I_AMR_WB_MODE_SET = 18;
        public static final int PST_I_CAPABILITY_DISCOVERY_VOLTE = 19;
        public static final int PST_I_RINGING_TIMER = 20;
        public static final int PST_I_RINGBACK_TIMER = 21;
        public static final int PST_I_RTP_RTCP_INACTIVITY_TIMER = 22;
        public static final int PST_I_UDP_KEEP_ALIVE = 23;
        public static final int PST_I_MULTIDEVICE = 24;
        public static final int PST_I_VOWIFI = 25;
        public static final int PST_I_TIMER_VZW = 26;
        public static final int PST_I_TDELAY = 27;
        public static final int PST_I_SILENT_REDIAL_ENABLE = 28;
        public static final int PST_I_TLTE_911FAIL = 29;

        //RCS
        public static final int PST_I_EAB = 31;
        public static final int PST_I_PUBLISH_TIMER = 32;
        public static final int PST_I_PUBLISH_TIMER_EXTENDED = 33;
        public static final int PST_I_CAPABILITIES_CACHE_EXPIRATION = 34;
        public static final int PST_I_AVAILABILITY_CACHE_EXPIRATION = 35;
        public static final int PST_I_SOURCE_THROTTLE_PUBLISH = 36;
        public static final int PST_I_CAPABILITY_POLL_INTERVAL = 37;
        public static final int PST_I_MAX_NUMBER_OF_ENTRIES_IN_REQUEST_CONTAINED_LIST = 38;
        public static final int PST_I_CAPABILITY_POLL_LIST_SUBSCRIPTION_EXPIRATION = 39;
        public static final int PST_I_CAPABILITY_DISCOVERY = 40;
        public static final int PST_I_GZIP_ENABLE = 41;
        public static final int PST_I_PUBLISH_ERROR_RETRY_TIMER = 42;

        //Subscriber
        public static final int PST_I_HOME_DOMAIN_NAME = 51;
        public static final int PST_I_IMPU = 52;
        public static final int PST_I_IMPI = 53;
        public static final int PST_I_PHONE_CONTEXT = 54;
        public static final int PST_I_PASSWORD = 55;
        public static final int PST_I_DOMAIN_FOR_IMPU = 56;
    }

    public final class SPR {
        public static final int SDM_I_SMS_OVER_IP_NETWORK_INDICATION = 6;

        public static final int SDM_I_SUBSCRIBER = 40;
        public static final int SDM_I_PCSCF = 41;
        public static final int SDM_I_IMS_APN_NAME = 42;
        public static final int SDM_I_ACTIVE_CALL_HO = 43;
    }
}
