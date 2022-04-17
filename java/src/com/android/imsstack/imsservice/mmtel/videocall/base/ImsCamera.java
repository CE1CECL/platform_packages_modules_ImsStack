
package com.android.imsstack.imsservice.mmtel.videocall.base;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.util.Size;

import com.android.imsstack.core.ImsGlobal;
import com.android.imsstack.util.ImsLog;

/**
 * This class provides an information of Camera for video call.
 */
public class ImsCamera {
    /*
     * Camera type for video call
     * It will be used for setCamera(...) as string format of int value.
     */
    public final static int CAMERA_REAR = 0;
    public final static int CAMERA_FRONT = 1;
    // For special case, other front camera is used for video call
    public final static int CAMERA_FRONT2 = 2;

    public static final float ZOOM_RATIO_UNZOOMED = 1.0f;
    public static final Size DEFAULT_PREVIEW_SIZE = new Size(1920, 1080);

    private String mCameraId = String.valueOf(CAMERA_FRONT);
    private CameraCharacteristics mCharacteristics = null;

    public ImsCamera() {
    }

    public ImsCamera(String cameraId) {
        log("ImsCamera :: cameraId=" + cameraId);
        mCameraId = cameraId;
    }

    public Size getPreviewSize() {
        if (mCharacteristics == null) {
            mCharacteristics = getCameraCharacteristics(mCameraId);

            if (mCharacteristics == null) {
                return DEFAULT_PREVIEW_SIZE;
            }
        }

        StreamConfigurationMap configs = null;

        try {
            configs = mCharacteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
        } catch (Exception e) {
            loge("Unable to obtain preview size");
        }

        if (configs == null) {
            return DEFAULT_PREVIEW_SIZE;
        }

        Size[] previewSizes = configs.getOutputSizes(SurfaceTexture.class);

        if (previewSizes == null) {
            return DEFAULT_PREVIEW_SIZE;
        }

        if (previewSizes.length == 0) {
            return DEFAULT_PREVIEW_SIZE;
        }

        return previewSizes[0];
    }

    public float getMaxZoom() {
        if (mCharacteristics == null) {
            mCharacteristics = getCameraCharacteristics(mCameraId);

            if (mCharacteristics == null) {
                return ZOOM_RATIO_UNZOOMED;
            }
        }

        try {
            return mCharacteristics.get(CameraCharacteristics.SCALER_AVAILABLE_MAX_DIGITAL_ZOOM);
        } catch (Exception e) {
            loge("Unable to obtain max digital zoom");
        }

        return ZOOM_RATIO_UNZOOMED;
    }

    public boolean isZoomSupported(float zoomRatio) {
        return zoomRatio > ZOOM_RATIO_UNZOOMED;
    }

    private CameraCharacteristics getCameraCharacteristics(String cameraId) {
        CameraManager cm = getCameraManager();

        if (cm == null) {
            return null;
        }

        try {
            return cm.getCameraCharacteristics(cameraId);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }

        return null;
    }

    private CameraManager getCameraManager() {
        return ImsGlobal.getInstance().getSystemService(CameraManager.class);
    }

    private static void log(String s) {
        ImsLog.d("[GII-IMPL] " + s);
    }

    private static void loge(String s) {
        ImsLog.e("[GII-IMPL] " + s);
    }
}
