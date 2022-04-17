package com.android.imsstack.core.service;

import android.content.Context;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;

import com.android.imsstack.core.ImsGlobal;
import com.android.imsstack.core.config.ECallConfigUtil;
import com.android.imsstack.core.config.IECallConfig;
import com.android.imsstack.core.service.serviceif.IService;
import com.android.imsstack.core.service.serviceif.IVoLteService;
import com.android.imsstack.enabler.mtc.Call;
import com.android.imsstack.enabler.mtc.CallStateListener;
import com.android.imsstack.enabler.mtc.IUMtcService;
import com.android.imsstack.enabler.mtc.reg.ImsServiceState;
import com.android.imsstack.internal.imsservice.CallUtils;
import com.android.imsstack.util.ImsLog;

public class ECallStateService implements IService {
    /** Call state */
    private static final int CALLSTATE_CONNECTING = 0;
    private static final int CALLSTATE_CONNECTED = 1;
    private static final int CALLSTATE_DISCONNECTING = 2;
    /** Bearer type */
    private static final int BEARER_TYPE_NONE = 0;
    private static final int BEARER_TYPE_LTE = 1;
    private static final int BEARER_TYPE_WIFI = 2;
    /** Final state (sync with modem) */
    private static final int LTE_CONNECTING = 101;
    private static final int LTE_CONNECTED = 102;
    private static final int LTE_DISCONNECTING = 103;
    private static final int WIFI_CONNECTING = 201;
    private static final int WIFI_CONNECTED = 202;
    private static final int WIFI_DISCONNECTING = 203;
    /** Internal events */
    private static final int EVENT_EMERGENCY_SERVICE_STATE_CHANGED = 1001;

    private final ECallStateHandler mHandler
            = new ECallStateHandler(ImsGlobal.getInstance().getMainLooper());

    private IVoLteService mVoLteService = null;
    private ECallStateAgentCallStateListener mCallListener = null;
    private int mLastCreatedBearer = BEARER_TYPE_NONE;
    private int mOldState = IUMtcService.ES_IDLE;
    private int mState = IUMtcService.ES_IDLE;
    private int mCallState = (-1);

    public ECallStateService() {
    }

    /* ---------------------------------------------------------------------------------------------
        implements IService
    --------------------------------------------------------------------------------------------- */
    @Override
    public boolean start(IVoLteService voLteService) {
        mVoLteService = voLteService;

        ImsLog.i(getSlotId(), "");

        if (!isECallStateServiceRequired()) {
            ImsLog.w(getSlotId(), "ECallStateService is not required");
            return false;
        }

        mCallListener = new ECallStateAgentCallStateListener();
        CallUtils.addCallStateListener(getSlotId(), mCallListener);

        CallUtils.addEmergencyServiceStateListener(getSlotId(),
                mHandler, EVENT_EMERGENCY_SERVICE_STATE_CHANGED, null);

        return true;
    }

    @Override
    public void cleanup(Context context) {
        ImsLog.i(getSlotId(), "");

        CallUtils.removeEmergencyServiceStateListener(getSlotId(), mHandler);

        CallUtils.removeCallStateListener(getSlotId(), mCallListener);
    }

    @Override
    public void update(Context context) {
    }

    public int getState() {
        return mState;
    }

    public int getOldState() {
        return mOldState;
    }

    public void setState(int state) {
        mOldState = mState;
        mState = state;
    }

    private boolean isECallStateServiceRequired() {
        IECallConfig ecca = ECallConfigUtil.getInstance(getSlotId());

        if (ecca.getEmergencyCallCapability() != IECallConfig.ECALL_RAT_NONE) {
            return true;
        }

        return false;
    }

    private void onEmergencyServiceStateChanged(int state, int reason) {
        setState(state);
        int oldState = getOldState();

        // Sends emergency status to modem (only for failure before send INVITE)
        if (((oldState == IUMtcService.ES_IDLE)
                || (oldState == IUMtcService.ES_OPENING)
                || (oldState == IUMtcService.ES_UNAVAILABLE))
                        && (state == IUMtcService.ES_UNAVAILABLE)) {
            updateECallStateToModem(CALLSTATE_DISCONNECTING, mLastCreatedBearer);
            mCallState = CALLSTATE_DISCONNECTING;
        }
    }

    private void processCallStateChanged(Call call, int nCallState) {
        if (!call.isEmergencyCall()) {
            return;
        }

        if (mCallState != nCallState) {
            if (isWiFiCall(call)) {
                updateECallStateToModem(nCallState, BEARER_TYPE_WIFI);
            } else {
                updateECallStateToModem(nCallState, BEARER_TYPE_LTE);
            }

            mCallState = nCallState;
        }
    }

    private void updateECallStateToModem(int nCallState, int nBearerType) {
        int convertedState = covertToCallStateForModem(nCallState, nBearerType);
        //setEmergencyCallState(convertedState);
    }

    private int covertToCallStateForModem(int nCallState, int nBearerType) {
        int nConvertedState = 0;

        if (nBearerType == BEARER_TYPE_LTE) {
            switch (nCallState) {
                case CALLSTATE_CONNECTING:
                    mLastCreatedBearer = BEARER_TYPE_LTE;
                    nConvertedState = LTE_CONNECTING;
                    break;

                case CALLSTATE_CONNECTED:
                    nConvertedState = LTE_CONNECTED;
                    break;

                case CALLSTATE_DISCONNECTING:
                    mLastCreatedBearer = BEARER_TYPE_NONE;
                    nConvertedState = LTE_DISCONNECTING;
                    break;

                default:
                    break;
            }
        } else if (nBearerType == BEARER_TYPE_WIFI) {
            switch (nCallState) {
                case CALLSTATE_CONNECTING:
                    mLastCreatedBearer = BEARER_TYPE_WIFI;
                    nConvertedState = WIFI_CONNECTING;
                    break;

                case CALLSTATE_CONNECTED:
                    nConvertedState = WIFI_CONNECTED;
                    break;

                case CALLSTATE_DISCONNECTING:
                    mLastCreatedBearer = BEARER_TYPE_NONE;
                    nConvertedState = WIFI_DISCONNECTING;
                    break;

                default:
                    break;
            }
        }

        displayConvertedCallState(nConvertedState);

        return nConvertedState;
    }

    private void displayConvertedCallState(int nConvertedState) {
        String state= "";

        switch (nConvertedState) {
            case LTE_CONNECTING:
                state = "LTE_CONNECTING";
                break;

            case LTE_CONNECTED:
                state = "LTE_CONNECTED";
                break;

            case LTE_DISCONNECTING:
                state = "LTE_DISCONNECTING";
                break;

            case WIFI_CONNECTING:
                state = "WIFI_CONNECTING";
                break;

            case WIFI_CONNECTED:
                state = "WIFI_CONNECTED";
                break;

            case WIFI_DISCONNECTING:
                state = "WIFI_DISCONNECTING";
                break;

            default:
                break;
        }

        ImsLog.i(getSlotId(), "ConvertedCallState=" + state);
    }

    private boolean isWiFiCall(Call call) {
        return call.getCallExtraBoolean(Call.EXTRA_WIFI_E_CALL, false);
    }

    private int getSlotId() {
        return (mVoLteService != null) ? mVoLteService.getSlotID() : 0;
    }

    private final class ECallStateHandler extends Handler {
        public ECallStateHandler(Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(Message msg) {
            if (msg == null) {
                return;
            }

            ImsLog.d(getSlotId(), "ECallStateHandler :: what=" + msg.what);

            switch (msg.what) {
                case EVENT_EMERGENCY_SERVICE_STATE_CHANGED: {
                    AsyncResult ar = (AsyncResult)msg.obj;
                    ImsServiceState ss = (ar != null) ? (ImsServiceState)ar.result : null;

                    if (ss != null) {
                        onEmergencyServiceStateChanged(ss.mExtraState, ss.mReason);
                    }
                    break;
                }

                default:
                    break;
            }
        }
    }

    private class ECallStateAgentCallStateListener extends CallStateListener {
        public ECallStateAgentCallStateListener() {
        }

        @Override
        public void onCallCreated(Call call) {
            processCallStateChanged(call, CALLSTATE_CONNECTING);
        }

        @Override
        public void onCallDestroyed(Call call) {
            processCallStateChanged(call, CALLSTATE_DISCONNECTING);
        }

        @Override
        public void onCallEstablishing(Call call) {
            // no-op
        }

        @Override
        public void onCallRinging(Call call) {
            // no-op
        }

        @Override
        public void onCallAccepted(Call call) {
            // no-op
        }

        @Override
        public void onCallEstablished(Call call) {
            processCallStateChanged(call, CALLSTATE_CONNECTED);
        }

        @Override
        public void onCallUpdated(Call call) {
            // no-op
        }

        @Override
        public void onCallTerminating(Call call) {
            // no-op
        }

        @Override
        public void onCallTerminated(Call call) {
            processCallStateChanged(call, CALLSTATE_DISCONNECTING);
        }
    }

}
