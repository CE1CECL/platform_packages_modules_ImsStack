package com.android.imsstack.enabler.aos;

public interface IAosInfoListener {

    /**
     * Notify the application that AoS ISIM state.
     *
     * @param  {@code state} is type of {@code IsimState}
     * @see {@link IAosInfoListener.IsimState}
     */
    public void notifyAosIsimStateChanged(int state);

    /**
     * ISIM State
     */
    class IsimState {

        public static final int INVALID = 0;
        public static final int VALID = 1;
        public static final int REFRESH_STARTED = 2;
        public static final int REFRESH_COMPLETE = 3;
    }
}