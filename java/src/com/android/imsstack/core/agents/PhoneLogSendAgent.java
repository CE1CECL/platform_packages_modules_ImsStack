package com.android.imsstack.core.agents;

import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;

import com.android.imsstack.core.agents.agentif.IVoLteAgent;
import com.android.imsstack.system.JNIUpCallEvtManager;
import com.android.imsstack.system.IJNIUpCallEvt;
import com.android.imsstack.system.ImsEventDef;
import com.android.imsstack.util.ImsLog;

import java.io.IOException;
import java.io.OutputStream;
import java.util.concurrent.ConcurrentHashMap;

public class PhoneLogSendAgent implements IVoLteAgent {
    /** Internal events */
    private final int MSG_SEND_LOG_CMD = 1000;
    private final int MSG_POST_TIME = 2000;
    /** MOCA address */
    private static final String SOCKET_ADDRESS = "Full.MOCA.port";

    private static PhoneLogSendAgent sPhoneLogSendAgent = null;

    private Handler mHandler;
    private ConcurrentHashMap<Integer, TraceMocaHandler> mTraceMocaHandlers
            = new ConcurrentHashMap<Integer, TraceMocaHandler>();
    private LocalSocket mLocalSocket = null;

    public static PhoneLogSendAgent getInstance() {
        if (sPhoneLogSendAgent == null) {
            sPhoneLogSendAgent = new PhoneLogSendAgent();
        }

        return sPhoneLogSendAgent;
    }

    @Override
    public void cleanup() {
        ImsLog.d("size=" + mTraceMocaHandlers.size());

        mTraceMocaHandlers.clear();

        if (mHandler != null) {
            mHandler.removeCallbacksAndMessages(null);
            mHandler = null;
        }
    }

    @Override
    public void start(int slotID) {
        ImsLog.d("size=" + mTraceMocaHandlers.size() + ", slotID=" + slotID);

        if (!mTraceMocaHandlers.containsKey(slotID)) {
            mTraceMocaHandlers.put(slotID, new TraceMocaHandler(slotID));

            IJNIUpCallEvt jniEvt = JNIUpCallEvtManager.getInstance().getJNIUpCallEvt(slotID);

            if (jniEvt != null) {
                jniEvt.registerForTraceMOCA(mTraceMocaHandlers.get(slotID),
                        ImsEventDef.IMS_EVENT_TRACE_MOCA, null);
            }
        }
    }

    @Override
    public void stop(int slotID) {
        ImsLog.d("size=" + mTraceMocaHandlers.size() + ", slotID=" + slotID);

        if (mTraceMocaHandlers.containsKey(slotID)) {
            TraceMocaHandler traceMocaHandler = mTraceMocaHandlers.get(slotID);

            IJNIUpCallEvt jniEvt = JNIUpCallEvtManager.getInstance().getJNIUpCallEvt(slotID);

            if (jniEvt != null) {
                jniEvt.unregisterForTraceMOCA(traceMocaHandler);
            }

            traceMocaHandler.removeCallbacksAndMessages(null);

            traceMocaHandler = null;
            mTraceMocaHandlers.remove(slotID);
        }
    }

    public void sendLogCmd(int nTriggerIndex, int slotID) {
        Message msg = Message.obtain();
        msg.what = MSG_SEND_LOG_CMD;
        msg.arg1 = nTriggerIndex;
        msg.arg2 = slotID;

        if (mHandler != null) {
            mHandler.sendMessageDelayed(msg, MSG_POST_TIME);
        }
    }

    private PhoneLogSendAgent() {
        mHandler = new PhoneLogSendHandler();
    }

    private void processLogCmd(int nTriggerIndex, int slotID) {
        try {
            boolean isConnected = setSocket();

            if (isConnected) {
                String eventNumber = String.valueOf(nTriggerIndex);
                byte[] data = eventNumber.getBytes();

                OutputStream os = mLocalSocket.getOutputStream();
                os.write(data);
                os.flush();

                ImsLog.i("Send log command");
            } else {
                ImsLog.e("Send log command is failed");
            }

            mLocalSocket.close();
        } catch (IOException e) {
            ImsLog.e("Exception Error");
            e.printStackTrace();
        }
    }

    private boolean setSocket() throws IOException {
        try {
            mLocalSocket = new LocalSocket();
            LocalSocketAddress localSocketAddress = new LocalSocketAddress(SOCKET_ADDRESS);

            mLocalSocket.connect(localSocketAddress);

            return mLocalSocket.isConnected();
        } catch (IOException e) {
            ImsLog.i("Exception Error");
            e.printStackTrace();
            return false;
        }
    }

    private class PhoneLogSendHandler extends Handler {
        public void handleMessage(Message msg) {
            if (msg == null) {
                return;
            }

            ImsLog.i("PhoneLogSendHandler :: what=" + msg.what);

            switch (msg.what) {
                case MSG_SEND_LOG_CMD:
                    processLogCmd(msg.arg1, msg.arg2);
                    break;

                default:
                    break;
            }
        }
    };

    private class TraceMocaHandler extends Handler {
        private final int mSlotID;

        public TraceMocaHandler(int slotID) {
            mSlotID = slotID;
        }

        public void handleMessage(Message msg) {
            if (msg == null) {
                return;
            }

            ImsLog.i(mSlotID, "TraceMocaHandler :: what=" + msg.what);

            switch (msg.what) {
                case ImsEventDef.IMS_EVENT_TRACE_MOCA: {
                    AsyncResult ar = (AsyncResult) msg.obj;

                    if (ar == null) {
                        return;
                    }

                    Message eventMsg = (Message) ar.result;

                    if (eventMsg == null) {
                        return;
                    }

                    ImsLog.d(mSlotID, "arg1=" + eventMsg.arg1 + ", arg2=" + eventMsg.arg2);

                    sendLogCmd(eventMsg.arg2, mSlotID);
                    break;
                }

                default:
                    break;
            }
        }
    };
}
