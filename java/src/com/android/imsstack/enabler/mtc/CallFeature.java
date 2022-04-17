/*
    Author
    <table>
    date        author                  description
    --------    --------------          ----------
    20150428    hwangoo.park@           Created
    </table>

    Description
*/

package com.android.imsstack.enabler.mtc;

import com.android.imsstack.core.ImsGlobal;
import com.android.imsstack.util.FeatureUtils;
import com.android.imsstack.util.MSimUtils;

public final class CallFeature {
    /**
     * Features for calls
     */
    private static final int FEATURE_NONE = 0;
    private static final int FEATURE_CALL_HOLD_USING_INACTIVE = 0x00000001;
    private static final int FEATURE_SRVCC_SUPPORTED = 0x00000002;
    private static final int FEATURE_TTY_SUPPORTED = 0x00000004;
    private static final int FEATURE_VIDEO_DIRECTION_INACTIVE_ON_VIDEO_CALL_HOLD = 0x00000008;
    private static final int FEATURE_AUDIO_EVS_SUPPORTED = 0x00000010;
    private static final int FEATURE_CONNECTED_IDENTITY_PRESENTATION_SUPPORTED = 0x00000020;
    private static final int FEATURE_VIDEO_RESOLUTION_QCIF_SUPPORTED = 0x00000040;
    // Indicates that the rejection of call type change should be processed by accepting
    // with the current call type.
    private static final int FEATURE_REQUIRE_ACCEPT_ON_REJECTING_CALL_TYPE_CHANGE = 0x00000080;
    // Indicates that the requested media profile is reflected as the local one
    // when new call request or call update is received from the remote end.
    // (Native enabler reflects the local media profile)
    private static final int FEATURE_LOCAL_MEDIA_PROFILE_REQUIRED_ON_REMOTE_REQUEST = 0x00000100;
    // Indicates that INCOMING_RESUME event is supported for the resume of voice/video call.
    private static final int FEATURE_INCOMING_RESUME_EVENT_SUPPORTED = 0x00000200;
    // Indicates that HD Voice is determined by the audio quality of MediaInfo
    private static final int FEATURE_HD_VOICE_DETERMINED_BY_AUDIO_QUALITY = 0x00000400;
    // Indicates that foreground call's HOLD requires on ECT(Consultative Call Transfer).
    private static final int FEATURE_HOLD_REQUIRED_ON_ECT = 0x00000800;
    // Indicates that callee's number should be displayed by the changed number
    // after USAT interworking.
    private static final int FEATURE_USAT_CHANGED_CALLEE_NUMBER_DISPLAYED = 0x00001000;
    // Indictates that DTMF tone should be played if the device receives DTMF tones.
    private static final int FEATURE_RECEIVED_DTMF_TONE_PLAY_REQUIRED = 0x00002000;
    // i.e. SKT group video calling
    private static final int FEATURE_CONFERENCE_BY_PARTIAL_MERGE = 0x00010000;
    // Indicates that the participant can be removed from the conference-info or not
    // when call merged and dropped.
    private static final int FEATURE_PARTICIPANT_REMOVABLE_FROM_CONFERENCE = 0x00020000;
    // Indicates that the participant can be removed from the conference-info or not
    // when conference extended or invited and dropped.
    private static final int FEATURE_PARTICIPANT_REMOVABLE_FROM_ADHOC_GROUP = 0x00040000;
    // Indicates that the participant can be removed from the conference-info or not
    // when invitation is failed.
    private static final int FEATURE_PARTICIPANT_REMOVABLE_ON_INVITATION_FAILED = 0x00080000;
    // Indicates that the 1-to-1 call (A side) can be terminated by the application or not
    // after conference merged or extended.
    private static final int FEATURE_CALL_TERMINATABLE_ON_CONFERENCE_HOST = 0x00100000;
    // Indicates that the 1-to-1 call (B side) can be terminated by the application or not
    // after conference extended.
    private static final int FEATURE_CALL_TERMINATABLE_ON_CONFERENCE_PEER = 0x00200000;
    // Indicates that the call end notification is required when call merge is completed.
    private static final int FEATURE_CALL_END_NOTICE_ENABLED_ON_MERGE_COMPLETED = 0x00400000;
    // Indicates that the call merge will be done without unhold of conference call
    // if the conference call already exists and it is a background call.
    private static final int FEATURE_CALL_MERGEABLE_ON_CONFERENCE_ON_HOLD = 0x00800000;
    // Indicates that it supports the supplementary service notification to notify
    // that the call is extended to the multiparty call by the network.
    private static final int FEATURE_SS_NOTICE_ENABLED_FOR_CONFERENCE_ON_MT = 0x01000000;
    // TEMPORARY: Indicates that endpoint will be applied as UUID in the conference state.
    private static final int FEATURE_UUID_USED_FOR_ENDPOINT_IN_CONFERENCE_STATE = 0x02000000;
    // Indicates that the call is supporting RTT (Real-time Text)
    private static final int FEATURE_RTT_SUPPORTED = 0x04000000;
    // Indicates that the text direction shall be inactive when the rtt call is on hold
    private static final int FEATURE_TEXT_DIRECTION_INACTIVE_ON_RTT_CALL_HOLD = 0x08000000;

    private static boolean sInitialized = false;
    private static int[] sFeatures;

    private CallFeature() {
        // no-op
    }

    public static synchronized void init(int slotId) {
        if (!sInitialized) {
            sFeatures = new int[MSimUtils.getMaxSimSlot()];

            for (int i = 0; i < MSimUtils.getMaxSimSlot(); i++) {
                sFeatures[i] = FEATURE_NONE;
            }

            sInitialized = true;
        }

        sFeatures[slotId] = FEATURE_NONE;

        // Common
        enableFeature(slotId, FEATURE_SRVCC_SUPPORTED);
        enableFeature(slotId, FEATURE_LOCAL_MEDIA_PROFILE_REQUIRED_ON_REMOTE_REQUEST);
        enableFeature(slotId, FEATURE_HD_VOICE_DETERMINED_BY_AUDIO_QUALITY);
        enableFeature(slotId, FEATURE_PARTICIPANT_REMOVABLE_FROM_CONFERENCE);
        enableFeature(slotId, FEATURE_PARTICIPANT_REMOVABLE_FROM_ADHOC_GROUP);
        enableFeature(slotId, FEATURE_PARTICIPANT_REMOVABLE_ON_INVITATION_FAILED);
        enableFeature(slotId, FEATURE_SS_NOTICE_ENABLED_FOR_CONFERENCE_ON_MT);
        enableFeature(slotId, FEATURE_UUID_USED_FOR_ENDPOINT_IN_CONFERENCE_STATE);
        enableFeature(slotId, FEATURE_HOLD_REQUIRED_ON_ECT);
        enableFeature(slotId, FEATURE_USAT_CHANGED_CALLEE_NUMBER_DISPLAYED);

        if (FeatureUtils.isMediaEvsSupported(ImsGlobal.getInstance())) {
            enableFeature(slotId, FEATURE_AUDIO_EVS_SUPPORTED);
        }

        if (FeatureUtils.isRttSupported(ImsGlobal.getInstance())) {
            enableFeature(slotId, FEATURE_RTT_SUPPORTED);
        }

        setOperatorSpecificFeatures(slotId);
    }

    public static synchronized int getFeatures(int slotId) {
        if (!sInitialized) {
            return FEATURE_NONE;
        }

        return sFeatures[slotId];
    }

    public static boolean canConferenceByPartialMerge(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_CONFERENCE_BY_PARTIAL_MERGE);
    }

    public static boolean isAcceptRequiredOnRejectingCallTypeChange(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_REQUIRE_ACCEPT_ON_REJECTING_CALL_TYPE_CHANGE);
    }

    public static boolean isAudioEvsSupported(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_AUDIO_EVS_SUPPORTED);
    }

    public static boolean isCallEndNoticeEnabledOnMergeCompleted(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_CALL_END_NOTICE_ENABLED_ON_MERGE_COMPLETED);
    }

    public static boolean isCallHoldUsingInactive(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_CALL_HOLD_USING_INACTIVE);
    }

    public static boolean isCallMergeableOnConferenceOnHold(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_CALL_MERGEABLE_ON_CONFERENCE_ON_HOLD);
    }

    public static boolean isCallTerminatableOnConferenceHost(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_CALL_TERMINATABLE_ON_CONFERENCE_HOST);
    }

    public static boolean isCallTerminatableOnConferencePeer(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_CALL_TERMINATABLE_ON_CONFERENCE_PEER);
    }

    public static boolean isConnectedIdentityPresentationSupported(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_CONNECTED_IDENTITY_PRESENTATION_SUPPORTED);
    }

    public static boolean isHDVoiceDeterminedByAudioQuality(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_HD_VOICE_DETERMINED_BY_AUDIO_QUALITY);
    }

    public static boolean isIncomingResumeEventSupported(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_INCOMING_RESUME_EVENT_SUPPORTED);
    }

    public static boolean isLocalMediaProfileRequiredOnRemoteRequest(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_LOCAL_MEDIA_PROFILE_REQUIRED_ON_REMOTE_REQUEST);
    }

    public static boolean isHoldRequiredOnEct(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_HOLD_REQUIRED_ON_ECT);
    }

    public static boolean isParticipantRemovableFromConference(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_PARTICIPANT_REMOVABLE_FROM_CONFERENCE);
    }

    public static boolean isParticipantRemovableFromAdhocGroup(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_PARTICIPANT_REMOVABLE_FROM_ADHOC_GROUP);
    }

    public static boolean isParticipantRemovableOnInvitationFailed(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_PARTICIPANT_REMOVABLE_ON_INVITATION_FAILED);
    }

    public static boolean isSrvccSupported(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_SRVCC_SUPPORTED);
    }

    public static boolean isSSNoticeEnabledForConferenceOnMT(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_SS_NOTICE_ENABLED_FOR_CONFERENCE_ON_MT);
    }

    public static boolean isTtySupported(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_TTY_SUPPORTED);
    }

    public static boolean isRttSupported(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_RTT_SUPPORTED);
    }

    public static boolean isUsatChangedCalleeNumberDisplayed(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_USAT_CHANGED_CALLEE_NUMBER_DISPLAYED);
    }

    public static boolean isUuidUsedForEndpointInConferenceState(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_UUID_USED_FOR_ENDPOINT_IN_CONFERENCE_STATE);
    }

    public static boolean isVideoDirectionInactiveOnVideoCallHold(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_VIDEO_DIRECTION_INACTIVE_ON_VIDEO_CALL_HOLD);
    }

    public static boolean isVideoResolutionQcifSupported(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_VIDEO_RESOLUTION_QCIF_SUPPORTED);
    }

    public static boolean isTextDirectionInactiveOnRttCallHold(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_TEXT_DIRECTION_INACTIVE_ON_RTT_CALL_HOLD);
    }

    public static boolean isReceivedDtmfTonePlayRequired(int slotId) {
        return isFeatureEnabled(slotId, FEATURE_RECEIVED_DTMF_TONE_PLAY_REQUIRED);
    }

    private static void disableFeature(int slotId, int feature) {
        if (slotId < 0 || slotId >= MSimUtils.getMaxSimSlot()) {
            return;
        }

        sFeatures[slotId] &= (~feature);
    }

    private static void enableFeature(int slotId, int feature) {
        if (slotId < 0 || slotId >= MSimUtils.getMaxSimSlot()) {
            return;
        }

        sFeatures[slotId] |= feature;
    }

    private static synchronized boolean isFeatureEnabled(int slotId, int feature) {
        if (slotId < 0 || slotId >= MSimUtils.getMaxSimSlot()) {
            return false;
        }

        return (sFeatures[slotId] & feature) == feature;
    }

    private static void setOperatorSpecificFeatures(int slotId) {
        String op = ImsGlobal.getOperator(slotId);
        String co = ImsGlobal.getCountry(slotId);

        if (ImsGlobal.equalsCountry(co, "KR")) {
            setOperatorSpecificFeaturesForKR(slotId, op);
        } else if (ImsGlobal.equalsCountry(co, "US")) {
            setOperatorSpecificFeaturesForUS(slotId, op);
        } else if (ImsGlobal.equalsCountry(co, "CA")) {
            enableFeature(slotId, FEATURE_RECEIVED_DTMF_TONE_PLAY_REQUIRED);
            enableFeature(slotId, FEATURE_REQUIRE_ACCEPT_ON_REJECTING_CALL_TYPE_CHANGE);
            if (ImsGlobal.equalsOperator(op, "TLS") || ImsGlobal.equalsOperator(op, "VTR")) {
                disableFeature(slotId, FEATURE_USAT_CHANGED_CALLEE_NUMBER_DISPLAYED);
            }
        } else if (ImsGlobal.equalsOperator(op, "KDDI")) {
            disableFeature(slotId, FEATURE_SRVCC_SUPPORTED);
            //disableFeature(FEATURE_PARTICIPANT_REMOVABLE_FROM_CONFERENCE);
            //disableFeature(FEATURE_PARTICIPANT_REMOVABLE_ON_INVITATION_FAILED);
            //disableFeature(FEATURE_UUID_USED_FOR_ENDPOINT_IN_CONFERENCE_STATE);
        } else if (ImsGlobal.equalsOperator(op, "RJIL")) {
            disableFeature(slotId, FEATURE_SRVCC_SUPPORTED);
        }

        setOperatorSpecificFeaturesForGlobal(slotId, op, co);
    }

    private static void setOperatorSpecificFeaturesForGlobal(int slotId,
            String operator, String country) {
        if (ImsGlobal.equalsOperatorCountry(operator, country, "VDF", "CZ")
                || ImsGlobal.equalsOperatorCountry(operator, country, "H3G", "SE")
                || ImsGlobal.equalsOperatorCountry(operator, country, "O2", "ES")) {
            disableFeature(slotId, FEATURE_HOLD_REQUIRED_ON_ECT);
        }

        if (ImsGlobal.equalsOperatorCountry(operator, country, "MTN", "ZA")) {
            enableFeature(slotId, FEATURE_RECEIVED_DTMF_TONE_PLAY_REQUIRED);
        }
    }

    private static void setOperatorSpecificFeaturesForKR(int slotId, String operator) {
        if (ImsGlobal.equalsOperator(operator, "SKT")) {
            enableFeature(slotId, FEATURE_VIDEO_RESOLUTION_QCIF_SUPPORTED);
            enableFeature(slotId, FEATURE_CONFERENCE_BY_PARTIAL_MERGE);
            enableFeature(slotId, FEATURE_CALL_TERMINATABLE_ON_CONFERENCE_HOST);
            disableFeature(slotId, FEATURE_CALL_TERMINATABLE_ON_CONFERENCE_PEER);
            disableFeature(slotId, FEATURE_SS_NOTICE_ENABLED_FOR_CONFERENCE_ON_MT);
            disableFeature(slotId, FEATURE_UUID_USED_FOR_ENDPOINT_IN_CONFERENCE_STATE);
        } else if (ImsGlobal.equalsOperator(operator, "LGU")) {
            disableFeature(slotId, FEATURE_SRVCC_SUPPORTED);
            enableFeature(slotId, FEATURE_CALL_HOLD_USING_INACTIVE);
            enableFeature(slotId, FEATURE_VIDEO_RESOLUTION_QCIF_SUPPORTED);
            disableFeature(slotId, FEATURE_PARTICIPANT_REMOVABLE_ON_INVITATION_FAILED);
            disableFeature(slotId, FEATURE_SS_NOTICE_ENABLED_FOR_CONFERENCE_ON_MT);
            disableFeature(slotId, FEATURE_UUID_USED_FOR_ENDPOINT_IN_CONFERENCE_STATE);
        } else if (ImsGlobal.equalsOperator(operator, "KT")) {
            disableFeature(slotId, FEATURE_SRVCC_SUPPORTED);
            enableFeature(slotId, FEATURE_VIDEO_RESOLUTION_QCIF_SUPPORTED);
            disableFeature(slotId, FEATURE_SS_NOTICE_ENABLED_FOR_CONFERENCE_ON_MT);
            disableFeature(slotId, FEATURE_UUID_USED_FOR_ENDPOINT_IN_CONFERENCE_STATE);
        }
    }

    private static void setOperatorSpecificFeaturesForUS(int slotId, String operator) {
        if (ImsGlobal.equalsOperator(operator, "VZW")) {
            enableFeature(slotId, FEATURE_VIDEO_DIRECTION_INACTIVE_ON_VIDEO_CALL_HOLD);
            enableFeature(slotId, FEATURE_RECEIVED_DTMF_TONE_PLAY_REQUIRED);
            disableFeature(slotId, FEATURE_SRVCC_SUPPORTED);
            disableFeature(slotId, FEATURE_HD_VOICE_DETERMINED_BY_AUDIO_QUALITY);

            if (!isRttSupported(slotId)) {
                enableFeature(slotId, FEATURE_TTY_SUPPORTED);
            }

            enableFeature(slotId, FEATURE_INCOMING_RESUME_EVENT_SUPPORTED);
            enableFeature(slotId, FEATURE_CALL_END_NOTICE_ENABLED_ON_MERGE_COMPLETED);
            enableFeature(slotId, FEATURE_CALL_MERGEABLE_ON_CONFERENCE_ON_HOLD);
            disableFeature(slotId, FEATURE_UUID_USED_FOR_ENDPOINT_IN_CONFERENCE_STATE);
            enableFeature(slotId, FEATURE_TEXT_DIRECTION_INACTIVE_ON_RTT_CALL_HOLD);
        } else if (ImsGlobal.equalsOperator(operator, "ATT")) {
            enableFeature(slotId, FEATURE_REQUIRE_ACCEPT_ON_REJECTING_CALL_TYPE_CHANGE);
            enableFeature(slotId, FEATURE_RECEIVED_DTMF_TONE_PLAY_REQUIRED);
        } else if (ImsGlobal.equalsOperator(operator, "TMO")
                || ImsGlobal.equalsOperator(operator, "MPCS")) {
            enableFeature(slotId, FEATURE_REQUIRE_ACCEPT_ON_REJECTING_CALL_TYPE_CHANGE);
        } else if (ImsGlobal.equalsOperator(operator, "SPR")) {
            enableFeature(slotId, FEATURE_RECEIVED_DTMF_TONE_PLAY_REQUIRED);
        }
    }
}
