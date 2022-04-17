
package com.android.imsstack.imsservice.mmtel.util;

/**
 * This class provides the width / height representation of video resolution.
 */
public final class VideoDimension {
    /** The width of a video resolution */
    private final int mWidth;
    /** The height of a video resolution */
    private final int mHeight;

    public VideoDimension(int width, int height) {
        mWidth = width;
        mHeight = height;
    }

    public int getWidth() {
        return mWidth;
    }

    public int getHeight() {
        return mHeight;
    }

    @Override
    public String toString() {
        return "[ VideoDimension: width=" + mWidth + ", height=" + mHeight + " ]";
    }
}
