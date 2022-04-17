package com.android.imsstack.external.ims;

/**
 * This class enables an application to get details on why a method call failed.
 * @hide
 */
public class ImsReasonInfoEx {
     /**
     * LOCAL
     */
    // IMS <-> Telephony
    // Retry emergency call; it depends on the device's local policy for emergency calling.
    // EXTRA_CODE_CALL_RETRY_SILENT_REDIAL can be accompanied by this code.
    // It's for IMS emergency call retry.
    public static final int CODE_LOCAL_ECALL_RETRY_REQUIRED = 171;

    // IMS -> Telephony
    // For glare condition between MO and MT call.
    public static final int CODE_LOCAL_CALL_BUSY_BY_MT_CALL = 172;

    // IMS <-> Telephony
    // IMS call is terminated by SRVCC
    public static final int CODE_LOCAL_CALL_TERMINATED_BY_SRVCC = 173;

    // IMS -> Telephony
    // IMS call is barred by the network (if SSAC or MO-Data barring is enabled)
    public static final int CODE_LOCAL_NETWORK_BARRED = 174;

    // IMS -> Telephony
    // Indicates that Ims normal call should be retried to CS emergency call.
    // When originating the call, the device recognizes the number as a normal call,
    // so, it makes a normal Ims call and network replies the response that this number
    // should be handled as a CS emergency call.
    // At the moment, the device should try to make a CS emergency call.
    public static final int CODE_LOCAL_IMS_CALL_TO_CS_ECALL_RETRY_REQUIRED = 175;

    // IMS -> Telephony
    // Indicates that Ims normal call should be retried to Ims emergency call.
    // When originating the call, the device recognizes the number as a normal call,
    // so, it makes a normal Ims call and network replies the response that this number
    // should be handled as an Ims emergency call.
    // At the moment, the device should try to make a Ims emergency call.
    public static final int CODE_LOCAL_IMS_CALL_TO_ECALL_RETRY_REQUIRED = 176;

    // IMS -> Telephony
    // Indicates that Ims call is terminated by joining the conference call.
    // It's only used when Ims call is immediately terminated as soon as call merge completes.
    // Especially, it's for CMCC, VZW.
    public static final int CODE_LOCAL_CALL_TERMINATED_BY_CONFERENCE_JOINED = 177;

    // IMS -> Telephony
    // Service unavailable; by no WIFI coverage
    public static final int CODE_LOCAL_NETWORK_NO_WIFI_COVERAGE = 178;

    // IMS -> Telephony
    // Indicates that Ims normal call should be retried to Ims/CS emergency call or CS call
    // by RAT selection result.
    // so, it makes a normal Ims call and network replies the response that this number
    // should be handled as Non UE detectable Emergency Session.
    // At the moment, the device should try to do domain selection
    public static final int CODE_LOCAL_IMS_CALL_RETRY_BY_RAT_SELECTION = 179;

    // IMS -> Telephony
    // Indicates that Ims call should be retried to CS call.
    // Call app will retry the CS call even though there is an Ims call.
    // In this case the existing Ims call will be disconnected by the Call app.
    // Especially, it's for ATT WPS call scenario.
    public static final int CODE_LOCAL_CALL_CS_RETRY_REQUIRED_FORCE = 180;

    // IMS -> Telephony
    // Indicates that Ims call is blocked for network error in roaming area
    // so, a notification to inform this to the user should be shown.
    public static final int CODE_LOCAL_IMS_CALL_NOT_ALLOWED_IN_ROAMING = 181;

    /**
     * Extra codes for an emergency call retry when normal call is redirected.
     * The following codes can use this codes.
     *      CODE_LOCAL_ECALL_RETRY_REQUIRED
     *      CODE_LOCAL_IMS_CALL_TO_CS_ECALL_RETRY_REQUIRED
     *      CODE_LOCAL_IMS_CALL_TO_ECALL_RETRY_REQUIRED
     *      CODE_LOCAL_IMS_CALL_RETRY_BY_RAT_SELECTION
     */
    // Generic [urn:service:sos]
    public static final int EXTRA_CODE_ECALL_RETRY_GENERIC = 0;
    // Police
    public static final int EXTRA_CODE_ECALL_RETRY_POLICE = 101;
    // Ambulance
    public static final int EXTRA_CODE_ECALL_RETRY_AMBULANCE = 102;
    // Fire Brigade
    public static final int EXTRA_CODE_ECALL_RETRY_FIRE_BRIGADE = 103;
    // Marine Guard
    public static final int EXTRA_CODE_ECALL_RETRY_MARINE_GUARD = 104;
    // Mountain Rescue
    public static final int EXTRA_CODE_ECALL_RETRY_MOUNTAIN_RESCUE = 105;
    // CS call if domain selection result is CS domain
    // (This code is only used for CODE_LOCAL_IMS_CALL_RETRY_BY_RAT_SELECTION)
    public static final int EXTRA_CODE_CS_CALL_RETRY = 201;

    /**
     * Reason codes for operator specific services
     * 701 ~
     */
    public static final int CODE_SERVICE_MULTI_DEVICE_ACCEPTED = 701;
    public static final int CODE_SERVICE_MULTI_DEVICE_REJECTED = 702;
    public static final int CODE_SERVICE_MULTI_DEVICE_LIMITED = 703;
    public static final int CODE_SERVICE_MULTI_DEVICE_PULLED = 704;
}
