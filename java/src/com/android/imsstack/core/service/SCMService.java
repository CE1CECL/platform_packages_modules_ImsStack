package com.android.imsstack.core.service;

import android.content.Context;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;

import com.android.imsstack.core.ImsGlobal;
import com.android.imsstack.core.OperatorInfo;
import com.android.imsstack.core.agents.IIMSPhoneGov;
import com.android.imsstack.core.agents.agentif.IIMSPhoneAgent;
import com.android.imsstack.core.service.serviceif.IService;
import com.android.imsstack.core.service.serviceif.IVoLteService;
import com.android.imsstack.system.ImsEventDef;
import com.android.imsstack.system.IJNIUpCallEvt;
import com.android.imsstack.system.JNIUpCallEvtManager;
import com.android.imsstack.util.ImsLog;

public class SCMService implements IService {
    /** Call type */
    private static final int TYPE_NONE = 0;
    private static final int TYPE_VOICE = 1;
    private static final int TYPE_VIDEO = 2;
    private static final int TYPE_SMS = 3;
    private static final int TYPE_REGISTRATION = 4;
    /** Call state */
    private static final int MODE_END = 0;
    private static final int MODE_START = 1;
    /** Emergency */
    private static final int EMERGENCY_NORMAL = 0;
    /** Service type */
    private static final int SERVICE_NONE = 0x0000;
    private static final int SERVICE_REG = 0x0001;
    private static final int SERVICE_REG_EVENT = 0x0002;
    /** Internal events */
    private static final int EVENT_DELAY_FOR_SERVICE_END = 1001;

    private IVoLteService mVoLteService = null;
    private Handler mSCMHandler = null;
    private int mOngoingService = SERVICE_NONE;
    private boolean mServiceStarted = false;
    private boolean mIsEndingService = false;

    public SCMService() {
    }

    @Override
    public boolean start(IVoLteService voLteService) {
        mVoLteService = voLteService;

        ImsLog.i(getSlotId(), "");

        if (!isSCMServiceRequired()) {
            ImsLog.w(getSlotId(), "SCMService is not required");
            return false;
        }

        mSCMHandler = new SCMHandler();

        IJNIUpCallEvt jniEvt = JNIUpCallEvtManager.getInstance().getJNIUpCallEvt(getSlotId());

        if (jniEvt != null) {
            jniEvt.registerForSendSCMToModem(mSCMHandler,
                    ImsEventDef.IMS_EVENT_SEND_SCM_TO_MODEM, null);
        }

        return true;
    }

    @Override
    public void cleanup(Context context) {
        ImsLog.i(getSlotId(), "");

        if (mSCMHandler != null) {
            IJNIUpCallEvt jniEvt = JNIUpCallEvtManager.getInstance().getJNIUpCallEvt(getSlotId());

            if (jniEvt != null) {
                jniEvt.unregisterForSendSCMToModem(mSCMHandler);
            }

            mSCMHandler.removeCallbacksAndMessages(null);
            mSCMHandler = null;
        }
    }

    @Override
    public void update(Context context) {
    }

    private boolean isSCMServiceRequired() {
        if (ImsGlobal.isOperatorCountry(getSlotId(), "TMO", "US")
                || ImsGlobal.isOperator(getSlotId(), "MPCS")
                || ImsGlobal.isOperator(getSlotId(), "TRF")
                || ImsGlobal.isOperator(getSlotId(), "VZW")
                || ImsGlobal.isOperator(getSlotId(), "ATT")
                || ImsGlobal.isCountry(getSlotId(), "KR")
                || ImsGlobal.isCountry(getSlotId(), "JP")
                || OperatorInfo.isEnablerTypeGlobal(getSlotId())) {
            return true;
        }

        return false;
    }

    private void setSysInfoForSCM(int scmMode, int scmType) {
        ImsLog.i(getSlotId(), "mode=" + scmMode + ", type=" + scmType);

        IIMSPhoneAgent ip = IIMSPhoneGov.getInstance(getSlotId());

        if (ip != null) {
            ip.setScmMode(scmType, scmMode, EMERGENCY_NORMAL);
        }
    }

    private void handleRegSignalForSCM(int mode, int type) {
        ImsLog.i(getSlotId(), "mode=" + mode + " Type=" + type);

        if (type == ImsEventDef.IMS_SCM_TYPE_REGISTRATION) {
            if (mode == ImsEventDef.IMS_SCM_START) {
                mOngoingService |= SERVICE_REG;
                startService();
            } else if (mode == ImsEventDef.IMS_SCM_END) {
                mOngoingService &= ~(SERVICE_REG);
                stopService();
            }
        } else if (type == ImsEventDef.IMS_SCM_TYPE_REG_EVENT) {
            if (mode == ImsEventDef.IMS_SCM_START) {
                mOngoingService |= SERVICE_REG_EVENT;
                startService();
            } else if (mode == ImsEventDef.IMS_SCM_END) {
                mOngoingService &= ~(SERVICE_REG_EVENT);
                stopService();
            }
        }
    }

    private void handleSmsForSCM(int mode) {
        setSysInfoForSCM((mode == ImsEventDef.IMS_SCM_START) ? MODE_START : MODE_END, TYPE_SMS);
    }

    private void startService() {
        if (mIsEndingService) {
            mSCMHandler.removeMessages(EVENT_DELAY_FOR_SERVICE_END);
            mIsEndingService = false;
        }

        if (mServiceStarted) {
            ImsLog.d(getSlotId(), "Service is already started");
            return;
        }

        setSysInfoForSCM(MODE_START, TYPE_REGISTRATION);
        mServiceStarted = true;
    }

    private void stopService() {
        if (!mServiceStarted) {
            return;
        }

        if (mOngoingService != SERVICE_NONE) {
            ImsLog.d(getSlotId(), "Service is running");
            return;
        }

        if (mIsEndingService) {
            mSCMHandler.removeMessages(EVENT_DELAY_FOR_SERVICE_END);
        }

        mIsEndingService = true;
        mSCMHandler.sendEmptyMessageDelayed(EVENT_DELAY_FOR_SERVICE_END, 1000);
    }

    private int getSlotId() {
        return (mVoLteService != null) ? mVoLteService.getSlotID() : 0;
    }

    private class SCMHandler extends Handler {
        public void handleMessage(Message msg) {
            if (msg == null) {
                return;
            }

            ImsLog.i(getSlotId(), "SCMHandler :: what=" + msg.what);

            switch (msg.what) {
                case EVENT_DELAY_FOR_SERVICE_END:
                    mIsEndingService = false;

                    setSysInfoForSCM(MODE_END, TYPE_REGISTRATION);
                    mServiceStarted = false;
                    break;

                case ImsEventDef.IMS_EVENT_SEND_SCM_TO_MODEM: {
                    AsyncResult ar = (AsyncResult)msg.obj;

                    if (ar == null) {
                        return;
                    }

                    Message eventMsg = (Message)ar.result;

                    if (eventMsg == null) {
                        return;
                    }

                    if (isSmsType(eventMsg.arg2)) {
                        handleSmsForSCM(eventMsg.arg1);
                    } else if (isRegSignalType(eventMsg.arg2)){
                        // reg signal
                        handleRegSignalForSCM(eventMsg.arg1, eventMsg.arg2);
                    }
                    break;
                }

                default:
                    break;
            }
        }

        private boolean isCallType(int type) {
            if (type == ImsEventDef.IMS_SCM_TYPE_VOICE
                    || type == ImsEventDef.IMS_SCM_TYPE_VIDEO) {
                return true;
            }

            return false;
        }

        private boolean isSmsType(int type) {
            if (type == ImsEventDef.IMS_SCM_TYPE_SMS) {
                return true;
            }

            return false;
        }

        private boolean isRegSignalType(int type) {
            if (type == ImsEventDef.IMS_SCM_TYPE_REGISTRATION
                    || type == ImsEventDef.IMS_SCM_TYPE_REG_EVENT) {
                return true;
            }

            return false;
        }
    }
}
