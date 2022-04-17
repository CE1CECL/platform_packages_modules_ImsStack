package com.android.imsstack.external.ims;

import java.util.Locale;

import android.net.Uri;
import android.os.Bundle;
import android.os.Parcel;
import android.os.Parcelable;
import android.telephony.ims.ImsStreamMediaProfile;

/**
 * Provides the dialog information (defined in RFC 4235) for INVITE-initiated dialog.
 * @hide
 */
public class ImsDialog implements Parcelable {
    public static final int DIRECTION_INITIATOR = 1;
    public static final int DIRECTION_RECIPIENT = 2;

    /**
     * "state" element
     */
    public static class State {
        public static final int STATE_IDLE = 0;
        public static final int STATE_TRYING = 1;
        public static final int STATE_PROCEEDING = 2;
        public static final int STATE_EARLY = 3;
        public static final int STATE_CONFIRMED = 4;
        public static final int STATE_TERMINATED = 5;
        // Operator-specific definition
        public static final int STATE_ON_HOLD = 6;

        public static final int EVENT_NONE = 0;
        public static final int EVENT_CANCELLED = 1;
        public static final int EVENT_REJECTED = 2;
        public static final int EVENT_REPLACED = 3;
        public static final int EVENT_LOCAL_BYE = 4;
        public static final int EVENT_REMOTE_BYE = 5;
        public static final int EVENT_ERROR = 6;
        public static final int EVENT_TIMEOUT = 7;

        /**
         * Indicates the state of the dialog.
         */
        private final int mState;
        /**
         * Indicates the event that caused any transition into the terminated state.
         */
        private final int mEvent;
        /**
         * Indicates the response code associated with any transaction
         * caused by a response to the original INVITE.
         */
        private final int mCode;

        public State(int state, int event, int code) {
            mState = state;
            mEvent = event;
            mCode = code;
        }

        public State(Parcel in) {
            mState = in.readInt();
            mEvent = in.readInt();
            mCode = in.readInt();
        }

        public int getState() {
            return mState;
        }

        public int getEvent() {
            return mEvent;
        }

        public int getCode() {
            return mCode;
        }

        @Override
        public String toString() {
            return String.format(Locale.US, "[State: state=%d, event=%d, code=%d]",
                    getState(), getEvent(), getCode());
        }

        public void writeToParcel(Parcel out, int flags) {
            out.writeInt(getState());
            out.writeInt(getEvent());
            out.writeInt(getCode());
        }

        public static State createState(Parcel in) {
            int state = in.readInt();
            int event = in.readInt();
            int code = in.readInt();

            return new State(state, event, code);
        }
    }

    /**
     * "local" or "remote" element
     * "target" -> "uri" attribute is not defined.(managed by IMS stack)
     * "sessd" (session description) is not defined in the moment.
     */
    public static class Participant {
        /**
         * "identity" -> "display" attribute
         */
        private final String mDisplayName;
        /**
         * "identity" element
         */
        private final Uri mUri;
        /**
         * "target" -> "param" -> "isfocus" pname
         */
        private final boolean mConference;
        /**
         * "target" -> "param" elements
         */
        private final Bundle mExtraParams;

        public Participant(String displayName, Uri uri, boolean conference, Bundle extraParams) {
            mDisplayName = displayName;
            mUri = uri;
            mConference = conference;
            mExtraParams = extraParams;
        }

        public String getDisplayName() {
            return mDisplayName;
        }

        public Uri getUri() {
            return mUri;
        }

        public boolean isConference() {
            return mConference;
        }

        public Bundle getExtraParams() {
            return mExtraParams;
        }

        @Override
        public String toString() {
            StringBuffer sb = new StringBuffer();

            sb.append("[Participant: display=");
            sb.append(getDisplayName());
            sb.append(", uri=");
            sb.append(getUri());
            sb.append(", conference=");
            sb.append(isConference() ? "Y" : "N");
            sb.append("]");

            return sb.toString();
        }

        public void writeToParcel(Parcel out, int flags) {
            out.writeString(getDisplayName());
            out.writeParcelable(getUri(), 0);
            out.writeInt(isConference() ? 1 : 0);
            out.writeParcelable(getExtraParams(), 0);
        }

        public static Participant createParticipant(Parcel in) {
            ClassLoader classLoader = ImsDialog.class.getClassLoader();

            String displayName = in.readString();
            Uri uri = in.readParcelable(classLoader);
            boolean conference = (in.readInt() == 1) ? true : false;
            Bundle extraParams = in.readParcelable(classLoader);

            return new Participant(displayName, uri, conference, extraParams);
        }
    }

    /**
     * "dialog" element
     *    id: Unique identifier to identify all current dialogs.
     *    direction: Indicates whether the observed user was the initiator of
     *            the dialog, or the recipient of the INVITE that created it.
     *    call-id, local-tag, remote-tag : It's managed by the IMS stack.
     */
    /**
     * "dialog" -> "id" attribute
     */
    private final String mId;
    /**
     * "dialog" -> "direction" attribute
     */
    private final int mDirection;
    /**
     * "state" element
     */
    private final State mState;
    /**
     * "local" element
     */
    private final Participant mLocal;
    /**
     * "remote" element
     */
    private final Participant mRemote;

    // Operator-specific
    private final boolean mPullEnabled;
    private final ImsStreamMediaProfile mMediaProfile;

    public ImsDialog(String id, int direction, State state, Participant local, Participant remote,
            boolean pullEnabled, ImsStreamMediaProfile mediaProfile) {
        mId = id;
        mDirection = direction;
        mState = state;
        mLocal = local;
        mRemote = remote;

        mPullEnabled = pullEnabled;
        mMediaProfile = mediaProfile;
    }

    public String getId() {
        return mId;
    }

    public int getDirection() {
        return mDirection;
    }

    public State getState() {
        return mState;
    }

    public Participant getLocal() {
        return mLocal;
    }

    public Participant getRemote() {
        return mRemote;
    }

    public ImsStreamMediaProfile getMediaProfile() {
        return mMediaProfile;
    }

    public boolean isConference() {
        return (mLocal != null && mLocal.isConference())
                || (mRemote != null && mRemote.isConference());
    }

    public boolean isPullEnabled() {
        return mPullEnabled;
    }

    @Override
    public String toString() {
        StringBuffer sb = new StringBuffer();

        sb.append("{ Dialog: id=");
        sb.append(getId());
        sb.append(", direction=");
        sb.append(getDirection());
        sb.append(", ");
        sb.append(getState());
        sb.append(", Local ");
        sb.append(getLocal());
        sb.append(", Remote ");
        sb.append(getRemote());
        sb.append(", pullEnabled=");
        sb.append(isPullEnabled() ? "Y" : "N");
        sb.append(", mediaProfile=");
        sb.append(getMediaProfile());
        sb.append(" }");

        return sb.toString();
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel out, int flags) {
        out.writeString(mId);
        out.writeInt(mDirection);
        mState.writeToParcel(out, flags);
        mLocal.writeToParcel(out, flags);
        mRemote.writeToParcel(out, flags);

        // Operator-specific
        out.writeInt(isPullEnabled() ? 1 : 0);
        out.writeParcelable(mMediaProfile, 0);
    }

    public static final Creator<ImsDialog> CREATOR = new Creator<ImsDialog>() {
        @Override
        public ImsDialog createFromParcel(Parcel source) {
            String id = source.readString();
            int direction = source.readInt();
            ImsDialog.State state = ImsDialog.State.createState(source);
            ImsDialog.Participant local = ImsDialog.Participant.createParticipant(source);
            ImsDialog.Participant remote = ImsDialog.Participant.createParticipant(source);

            boolean pullEnabled = (source.readInt() == 1) ? true : false;
            ImsStreamMediaProfile mediaProfile = source.readParcelable(null);

            return new ImsDialog(id, direction,
                    state, local, remote, pullEnabled, mediaProfile);
        }

        @Override
        public ImsDialog[] newArray(int size) {
            return new ImsDialog[size];
        }
    };
}
