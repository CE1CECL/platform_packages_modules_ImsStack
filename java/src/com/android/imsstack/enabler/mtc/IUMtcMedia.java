package com.android.imsstack.enabler.mtc;

public class IUMtcMedia {

    public static final int IMS_MEDIA_MSG_REASON = 300;
    public static final int IMS_MEDIA_MSG_NOTIFY = 400;
    public static final int IMS_MSG_BASE_MEDIA = 1400;

    public static final int SETSURFACE_CMD                  = (IMS_MSG_BASE_MEDIA + 1);
    public static final int FARFRAME_IND                    = (IMS_MSG_BASE_MEDIA + 2);

    // for multi-tasking
    public static final int START_BACKGROUND_CMD            = (IMS_MSG_BASE_MEDIA + 3);
    public static final int BACKGROUND_STARTED_IND          = (IMS_MSG_BASE_MEDIA + 4);

    public static final int STOP_BACKGROUND_CMD             = (IMS_MSG_BASE_MEDIA + 5);
    public static final int BACKGROUND_STOPPED_IND          = (IMS_MSG_BASE_MEDIA + 6);

    // add-ons
    public static final int SELECT_CAMERA_CMD               = (IMS_MSG_BASE_MEDIA + 7);
    public static final int CAMERA_SELECTED_IND             = (IMS_MSG_BASE_MEDIA + 8);

    public static final int CHANGE_CAMERA_ZOOM_CMD          = (IMS_MSG_BASE_MEDIA + 9);
    public static final int CAMERA_ZOOM_CHANGED_IND         = (IMS_MSG_BASE_MEDIA + 10);

    public static final int CHANGE_CAMERA_BRIGHTNESS_CMD    = (IMS_MSG_BASE_MEDIA + 11);
    public static final int CAMERA_BRIGHTNESS_CHANGED_IND   = (IMS_MSG_BASE_MEDIA + 12);

    public static final int CAPTURE_CMD                     = (IMS_MSG_BASE_MEDIA + 13);
    public static final int CAPTURED_IND                    = (IMS_MSG_BASE_MEDIA + 14);

    public static final int START_RECODING_CMD              = (IMS_MSG_BASE_MEDIA + 15);
    public static final int RECODING_STARTED_IND            = (IMS_MSG_BASE_MEDIA + 16);

    public static final int STOP_RECODING_CMD               = (IMS_MSG_BASE_MEDIA + 17);
    public static final int RECODING_STOPPED_IND            = (IMS_MSG_BASE_MEDIA + 18);

    public static final int START_ALTERNATE_IMAGE_CMD       = (IMS_MSG_BASE_MEDIA + 19);
    public static final int ALTERNATE_IMAGE_STARTED_IND     = (IMS_MSG_BASE_MEDIA + 20);

    public static final int STOP_ALTERNATE_IMAGE_CMD        = (IMS_MSG_BASE_MEDIA + 21);
    public static final int ALTERNATE_IMAGE_STOPPED_IND     = (IMS_MSG_BASE_MEDIA + 22);

    public static final int AUDIO_STOP_CMD                  = (IMS_MSG_BASE_MEDIA + 23);

    public static final int AUDIO_STARTED_IND               = (IMS_MSG_BASE_MEDIA + 24);
    public static final int MEDIA_STARTED_IND               = (IMS_MSG_BASE_MEDIA + 25);

    public static final int UPDATE_DISPLAY_CMD              = (IMS_MSG_BASE_MEDIA + 26);
    public static final int DISPLAY_UPDATED_IND             = (IMS_MSG_BASE_MEDIA + 27);

    public static final int CHANGE_VIEW_SIZE_CMD            = (IMS_MSG_BASE_MEDIA + 28);
    public static final int VIEW_SIZE_CHANGED_IND           = (IMS_MSG_BASE_MEDIA + 29);

    public static final int SWAP_DISPLAY_CMD                = (IMS_MSG_BASE_MEDIA + 30);
    public static final int DISPLAY_SWAPPED_IND             = (IMS_MSG_BASE_MEDIA + 31);

    public static final int AUDIO_START_CMD                 = (IMS_MSG_BASE_MEDIA + 32);
    public static final int AUDIO_STOPPED_IND               = (IMS_MSG_BASE_MEDIA + 33);

    public static final int CHANGE_ORIENTATION_CMD          = (IMS_MSG_BASE_MEDIA + 34);
    public static final int ORIENTATION_CHANGED_IND         = (IMS_MSG_BASE_MEDIA + 35);

    public static final int ONSCREEN_DEBUG_INFO_VIDEO       = (IMS_MSG_BASE_MEDIA + 36);

    public static final int FARFRAME_ORIENTATION_CHANGED    = (IMS_MSG_BASE_MEDIA + 37);

    public static final int UPDATE_VIDEO_SURFACE_IND        = (IMS_MSG_BASE_MEDIA + 38);

    public static final int AUDIO_PAUSED_IND                = (IMS_MSG_BASE_MEDIA + 39);    //nam.bodam -- 140821

    public static final int PEER_DIMENSION_CHANGED_IND      = (IMS_MSG_BASE_MEDIA + 40);    //ym.park -- Dynamic Resolution for legacy
    public static final int UPDATE_SOURCE_RESOLUTION_CMD    = (IMS_MSG_BASE_MEDIA + 41);

    public static final int START_PREVIEW_CAMERA_CMD        = (IMS_MSG_BASE_MEDIA + 42);    //jin0925.choi -- For DCM VT call UI Interface
    public static final int STOP_PREVIEW_CAMERA_CMD         = (IMS_MSG_BASE_MEDIA + 43);

    public static final int CAMERA_FRAME_IND                = (IMS_MSG_BASE_MEDIA + 44);    //nam.bodam -- 1st camera frame received

    public static final int RTT_TEXT_SEND_CMD               = (IMS_MSG_BASE_MEDIA + 45);
    public static final int RTT_TEXT_RECEIVED_IND           = (IMS_MSG_BASE_MEDIA + 46);

    public static final int RTT_AUDIO_INDICATION_IND        = (IMS_MSG_BASE_MEDIA + 47);

    public static final int VIDEO_DATA_USAGE_CMD            = (IMS_MSG_BASE_MEDIA + 48);
    public static final int VIDEO_DATA_USAGE_INFO_IND       = (IMS_MSG_BASE_MEDIA + 49);

    public static final int VIDEO_CALL_EFFECT_CMD           = (IMS_MSG_BASE_MEDIA + 50);

    public class Reason {

        public static final int REASON_NOERROR      = (IMS_MEDIA_MSG_REASON + 1);
        public static final int REASON_EVENT_FAIL   = (IMS_MEDIA_MSG_REASON + 2);

        public static final int UNDEFINED           = (IMS_MEDIA_MSG_REASON + 99);
    };

    public class Notify {

        public static final int NOTIFY_ORIENTATION_PORTRAIT     = (IMS_MEDIA_MSG_NOTIFY + 1);
        public static final int NOTIFY_ORIENTATION_LANDSCAPE    = (IMS_MEDIA_MSG_NOTIFY + 2);

        public static final int UNDEFINED                       = (IMS_MEDIA_MSG_NOTIFY + 99);
    };

    // Google_IMS_IF :: VIDEO_CALL_PROVIDER {
    public class ParamValue {
        // SETSURFACE_CMD
        public static final int SURFACE_FAR = 1;
        public static final int SURFACE_NEAR = 2;
    };
    // Google_IMS_IF :: VIDEO_CALL_PROVIDER }
};
