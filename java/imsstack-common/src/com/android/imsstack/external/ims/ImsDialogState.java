package com.android.imsstack.external.ims;

import java.util.ArrayList;
import java.util.List;

import android.os.Parcel;
import android.os.Parcelable;

/**
 * Provides the dialog information (defined in RFC 4235) for INVITE-initiated dialog.
 * @hide
 */
public class ImsDialogState implements Parcelable {
    private final List<ImsDialog> mDialogs;

    public ImsDialogState(List<ImsDialog> dialogs) {
        mDialogs = dialogs;
    }

    public ImsDialogState(Parcel source) {
        mDialogs = new ArrayList<ImsDialog>();

        ClassLoader classLoader = ImsDialogState.class.getClassLoader();
        int size = source.readInt();

        for (int i = 0; i < size; ++i) {
            mDialogs.add((ImsDialog)source.readParcelable(classLoader));
        }
    }

    public List<ImsDialog> getDialogs() {
        return mDialogs;
    }

    @Override
    public String toString() {
        StringBuffer sb = new StringBuffer();

        sb.append("[ ImsDialogState: ");
        sb.append(mDialogs.size());

        if (mDialogs.size() > 0) {
            sb.append(", [ ");

            ImsDialog dialog = mDialogs.get(0);

            sb.append(dialog.toString());

            for (int i = 1; i < mDialogs.size(); ++i) {
                dialog = mDialogs.get(i);
                sb.append(", ");
                sb.append(dialog.toString());

            }

            sb.append(" ]");
        }

        sb.append(" ]");

        return sb.toString();
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel out, int flags) {
        out.writeInt(mDialogs.size());

        if (!mDialogs.isEmpty()) {
            for (int i = 0; i < mDialogs.size(); ++i) {
                out.writeParcelable(mDialogs.get(i), 0);
            }
        }
    }

    public static final Creator<ImsDialogState> CREATOR =
            new Creator<ImsDialogState>() {
        @Override
        public ImsDialogState createFromParcel(Parcel source) {
            return new ImsDialogState(source);
        }

        @Override
        public ImsDialogState[] newArray(int size) {
            return new ImsDialogState[size];
        }
    };
}
