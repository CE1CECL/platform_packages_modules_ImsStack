package com.android.imsstack.external.ims;

/**
 * Parcelable object to handle IMS call profile.
 * It is created from GSMA IR.92/IR.94, 3GPP TS 24.229/TS 26.114/TS26.111.
 * It provides the service and call type, the additional information related to the call.
 * @hide
 */
public class ImsCallProfileEx {
    /**
     * Type of call transfer.
     */
    public static final int CT_TYPE_DEFAULT = 0;
    public static final int CT_TYPE_ECT = 1;
    public static final int CT_TYPE_CALL_PUSH = 2;

    /**
     * Type of calling number (originating identity) verification status.
     */
    // No number validation was performed
    public static final int OI_VER_STATUS_NO_TN_VALIDATION = 0;
    // The number passed the validation
    public static final int OI_VER_STATUS_TN_VALIDATION_PASSED = 1;
    // The number failed the validation
    public static final int OI_VER_STATUS_TN_VALIDATION_FAILED = 2;

    /**
     * Extra properties for IMS call.
     */
    /**
     * Boolean extra properties - "true" / "false"
     *  conference_event : Indicates if conference event package is used or not.
     *  gtt : Indicates if GTT is available or not.
     *  hd_voice : Indicates if the call supports HD Voice or not.
     *           It depends on the operator's requirement.
     *  uhd_voice : Indicates if the call supports UHD Voice or not.
     *           It depends on the operator's requirement and
     *           this indicates that the call supports EVS at the moment.
     *  wifi_call : Indicates that this call should be made using WFC.
     *          In the moment, this is used to identify emergency call via WFC.
     *  mo_alerting : Indicates that MO session receives 180 Ringing response
     *             for any early dialogs. It's used to identify pre-alerting (bSRVCC)
     *             or alerting (aSRVCC) phase of SRVCC.
     *  rat_changed : Indicates that second Emergency RAT selection result is PS
     *              (case of emergency VoLTE retry after CS emergency failure)
     *  rtt : Indicates if the session is rtt call or not
     *  rtt_avail : Indicates if the session is able to upgrade rtt during voice call
     *  vrbt : Indicates that this call supports video ringback tone.
     */
    public static final String EXTRA_CONFERENCE_EVENT = "conference_event";
    public static final String EXTRA_GTT = "gtt";
    public static final String EXTRA_HD_VOICE = "hd_voice";
    public static final String EXTRA_UHD_VOICE = "uhd_voice";
    public static final String EXTRA_WIFI_CALL = "wifi_call";
    public static final String EXTRA_MO_ALERTING = "mo_alerting";
    public static final String EXTRA_RAT_CHANGED = "rat_changed";
    public static final String EXTRA_RTT_AVAIL = "rtt_avail";
    public static final String EXTRA_VRBT = "vrbt";

    /**
     * Integer extra properties
     *  cdiv_cause : Indicates the cause value of CDIV (call diversion).
     *  oi_ver_status : Indicates the status of calling number verification. MT only.
     *               The values are ImsCallProfileEx#OI_VER_STAT_XXX.
     */
    public static final String EXTRA_CDIV_CAUSE = "cdiv_cause";
    public static final String EXTRA_OI_VER_STATUS = "oi_ver_status";

    /**
     * String extra properties
     *  cna_ext : First 16 bytes of CNAP field value (U+'s call message or MCID).
     *  mcid : Indicates if the call contains the multimedia contents or not.
     *      For U+'s photo-ring,
     *          This field will be set to "30;CNAP2" (instant type) or "CNAP2".
     *          In MT side, the UE can receive "19:CNAP" (pre-set type).
     *      For MCID,
     *          This field includes the URL information obtained from the Content Server.
     *  cdiv_history : Contains the history info of call diversion.
     *      In the moment, it contains the last forwarded number.
     *  dialog_id : Indicates that if call pull service via "dialog" event package is provided,
     *      this will be used to indicate which call is pulled by the user.
     *      @see ImsDialog#mId
     *  subaddress : Indicates the sub-address when dialing the original number with *XXX.
     *      09012341234*123*456
     *      original-dialed-number: 09012341234
     *      subaddress: 123*456
     */
    public static final String EXTRA_CNA_EXT = "cna_ext";
    public static final String EXTRA_MCID = "mcid";
    public static final String EXTRA_CDIV_HISTORY = "cdiv_history";
    public static final String EXTRA_DIALOG_ID = "dialog_id";
    public static final String EXTRA_SUBADDRESS = "subaddress";

    // CALL_COMPOSER {
    /**
     * Integer extra properties
     *  call_composer_importance : Indicates importance information for call composer service.
     *      The candidate value is 0 or 1.
     *      If this extra doesn't exist, there is no importance information in this call.
     *
     * String extra properties
     *  call_composer_subject : Identifies the subject for call composer service.
     *  call_composer_location : Identifies the location information for call composer service.
     *      The format: latitude,longitude (comma-separated string)
     *  call_composer_picture : Identifies the picture URL for call composer service.
     */
    public static final String EXTRA_CALL_COMPOSER_IMPORTANCE = "call_composer_importance";
    public static final String EXTRA_CALL_COMPOSER_SUBJECT = "call_composer_subject";
    public static final String EXTRA_CALL_COMPOSER_LOCATION = "call_composer_location";
    public static final String EXTRA_CALL_COMPOSER_PICTURE = "call_composer_picture";
    // }
}
