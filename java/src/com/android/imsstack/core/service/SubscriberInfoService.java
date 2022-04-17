package com.android.imsstack.core.service;

import android.content.Context;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;

import com.android.imsstack.core.ImsGlobal;
import com.android.imsstack.core.agents.AgentFactory;
import com.android.imsstack.core.agents.agentif.ISubscriberInfo;
import com.android.imsstack.core.config.ImsDbController;
import com.android.imsstack.core.config.ProviderInterface;
import com.android.imsstack.core.service.serviceif.IService;
import com.android.imsstack.core.service.serviceif.IVoLteService;
import com.android.imsstack.system.ImsEventDef;
import com.android.imsstack.system.IJNIUpCallEvt;
import com.android.imsstack.system.JNIUpCallEvtManager;
import com.android.imsstack.util.DBUtils;
import com.android.imsstack.util.ImsLog;

public class SubscriberInfoService implements ISubscriberInfo, IService {
    private IVoLteService mVoLteService = null;
    private Handler mSubscriberInfoHandler = null;

    public SubscriberInfoService() {
    }

    @Override
    public boolean start(IVoLteService voLteService) {
        mVoLteService = voLteService;

        ImsLog.i(getSlotId(), "");

        mSubscriberInfoHandler = new SubscriberInfoHandler();

        IJNIUpCallEvt jniEvt = JNIUpCallEvtManager.getInstance().getJNIUpCallEvt(getSlotId());

        if (jniEvt != null) {
            jniEvt.registerForISIMReadResult(mSubscriberInfoHandler,
                    ImsEventDef.IMS_EVENT_ISIM_STATE, null);
        }

        AgentFactory.setAgentForMIms(this, AgentFactory.SUBSCRIBER_INFO, getSlotId());

        return true;
    }

    @Override
    public void cleanup(Context context) {
        ImsLog.d(getSlotId(), "");

        if (mSubscriberInfoHandler != null) {
            IJNIUpCallEvt jniEvt = JNIUpCallEvtManager.getInstance().getJNIUpCallEvt(getSlotId());

            if (jniEvt != null) {
                jniEvt.unregisterForISIMReadResult(mSubscriberInfoHandler);
            }

            mSubscriberInfoHandler.removeCallbacksAndMessages(null);
            mSubscriberInfoHandler = null;
        }

        AgentFactory.setAgentForMIms(null, AgentFactory.SUBSCRIBER_INFO, getSlotId());
    }

    @Override
    public void update(Context context) {
    }

    @Override
    public void init(Context context) {
    }

    @Override
    public void cleanup() {
    }

    @Override
    public boolean isImsOn() {
        return isFeatureEnabled(ProviderInterface.Subscriber.AdminFeatures.IMS);
    }

    @Override
    public boolean isIsimOn() {
        return isFeatureEnabled(ProviderInterface.Subscriber.AdminFeatures.ISIM);
    }

    @Override
    public boolean isUsimOn() {
        return isFeatureEnabled(ProviderInterface.Subscriber.AdminFeatures.USIM);
    }

    @Override
    public boolean isDebugOn() {
        return isFeatureEnabled(ProviderInterface.Subscriber.AdminFeatures.DEBUG);
    }

    @Override
    public boolean isDMOn() {
        return isFeatureEnabled(ProviderInterface.Subscriber.AdminFeatures.DM);
    }

    @Override
    public boolean isTestMode() {
        return isFeatureEnabled(ProviderInterface.Subscriber.AdminFeatures.TESTMODE);
    }

    @Override
    public boolean isTestModeForGCF() {
        return isFeatureEnabled(ProviderInterface.Subscriber.AdminFeatures.TESTMODE_GCF);
    }

    @Override
    public boolean isVoLTEServiceOn() {
        return getServiceEnabled(ProviderInterface.Subscriber.AdminServices.VOLTE);
    }

    @Override
    public boolean isViLTEServiceOn() {
        return getServiceEnabled(ProviderInterface.Subscriber.AdminServices.VILTE);
    }

    @Override
    public boolean isVoWiFiServiceOn() {
        return getServiceEnabled(ProviderInterface.Subscriber.AdminServices.VOWIFI);
    }

    @Override
    public boolean isViWiFiServiceOn() {
        return getServiceEnabled(ProviderInterface.Subscriber.AdminServices.VIWIFI);
    }

    private boolean isFeatureEnabled(int feature) {
        int adminFeatures = ImsDbController.Subscriber.getAdminFeatures(getSlotId());

        return ImsDbController.isAdminFeatureEnabled(adminFeatures, feature);
    }

    private void setISIMReadResult(int isimReadResult) {
        int mISIMReadResult = isimReadResult;

        ImsLog.i(getSlotId(), "ISIMReadResult=" + mISIMReadResult);

        if (mISIMReadResult == ImsEventDef.IMS_ISIM_STATE_INVALID) {
            ImsDbController.Subscriber.setAdminFeatures(getSlotId(),
                    ProviderInterface.Subscriber.AdminFeatures.USIM,
                    ProviderInterface.Subscriber.AdminFeatures.ISIM);
        }
    }

    private boolean getServiceEnabled(int service) {
        Context c = getContext();

        if (c == null) {
            return false;
        }

        int services = DBUtils.CP.getHex(getSlotId(), c.getContentResolver(),
                ProviderInterface.Subscriber.CONTENT_URI,
                ProviderInterface.Subscriber.SERVICES, 0);

        return ((services & service) == service);
    }

    private Context getContext() {
        return (mVoLteService != null) ? mVoLteService.getContext() : null;
    }

    private int getSlotId() {
        return (mVoLteService != null) ? mVoLteService.getSlotID() : 0;
    }

    private class SubscriberInfoHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            if (msg == null) {
                return;
            }

            ImsLog.i(getSlotId(), "SubscriberInfoHandler :: what=" + msg.what);

            switch (msg.what) {
                case ImsEventDef.IMS_EVENT_ISIM_STATE:
                    if (ImsGlobal.isCountry(getSlotId(), "KR")
                            && !ImsGlobal.isOperator(getSlotId(), "LGU")) {
                        AsyncResult ar = (AsyncResult) msg.obj;

                        if (ar == null) {
                            return;
                        }

                        Message eventMsg = (Message) ar.result;

                        if (eventMsg == null) {
                            return;
                        }

                        setISIMReadResult(eventMsg.arg1);
                    }
                    break;

                default:
                    break;
            }
        }
    }
}
