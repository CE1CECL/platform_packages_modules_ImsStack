package com.android.imsstack.core.service;

import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.database.ContentObserver;
import android.media.AudioManager;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.os.Registrant;
import android.os.RegistrantList;
import android.provider.Settings;
import android.telephony.TelephonyManager;

import com.android.imsstack.core.ImsGlobal;
import com.android.imsstack.core.UICCHelper;
import com.android.imsstack.core.agents.AgentFactory;
import com.android.imsstack.core.agents.IIMSPhoneGov;
import com.android.imsstack.core.agents.IntentWrapper;
import com.android.imsstack.core.agents.agentif.ICallSetting;
import com.android.imsstack.core.agents.agentif.IIMSPhoneAgent;
import com.android.imsstack.core.agents.agentif.IRegiProcess;
import com.android.imsstack.core.agents.agentif.ITelephonySubscriber;
import com.android.imsstack.core.agents.agentif.IWifiState;
import com.android.imsstack.core.agents.dcm.DCFactory;
import com.android.imsstack.core.agents.dcmif.ApnStateListener;
import com.android.imsstack.core.agents.dcmif.EApnType;
import com.android.imsstack.core.agents.dcmif.IApn;
import com.android.imsstack.core.agents.dcmif.IDCApn;
import com.android.imsstack.core.agents.dcmif.IDCNetWatcher;
import com.android.imsstack.core.config.FeatureConfig;
import com.android.imsstack.core.service.serviceif.IVoLteService;
import com.android.imsstack.enabler.IUIMS;
import com.android.imsstack.enabler.mtc.Call;
import com.android.imsstack.enabler.mtc.CallStateListener;
import com.android.imsstack.enabler.mtc.ICallStateTracker;
import com.android.imsstack.external.ims.ImsStateProvider;
import com.android.imsstack.internal.imsservice.CallUtils;
import com.android.imsstack.provider.ImsStateController;
import com.android.imsstack.system.IJNIUpCallEvt;
import com.android.imsstack.system.ImsEventDef;
import com.android.imsstack.system.JNIUpCallEvtManager;
import com.android.imsstack.test.ImsTestMode;
import com.android.imsstack.util.AppContext;
import com.android.imsstack.util.FeatureUtils;
import com.android.imsstack.util.ImsLog;
import com.android.imsstack.util.ImsProperties;
import com.android.imsstack.util.MSimUtils;

public class RegiProcessService implements IRegiProcess {
    /** Intent state type for VZW */
    private static final int NOT_REGISTERED = 0;
    private static final int WIFI_REGISTERED = 1;
    private static final int LTE_REGISTERED = 2;
    /** System mode */
    private static final int SYS_MODE_NONE = 0x00;
    private static final int SYS_MODE_HRPD = 0x02;
    private static final int SYS_MODE_UMTS = 0x05;
    private static final int SYS_MODE_WLAN = 0x06;
    private static final int SYS_MODE_LTE = 0x08;

    private IVoLteService mVoLteService = null;
    private Handler mRegiProcessHandler = null;
    private int[] mRegInfo = new int[6];
    private int mRegServiceState = 0;
    private int mRegServiceStateForWiFi = 0;
    private int mVoipRegState = (-1);
    private boolean mBlockRegStateNoti = false;
    private boolean mIsRegistered = false;
    private boolean mIsVoipRegistered = false;
    private boolean mIsRegStateIntentRequired = false;
    private boolean mIshandleNetworkModeRequired = false;
    // NetworkTypeChange
    private int mChangedNetworkMode = NETWORK_MODE_INVALID;
    private ImsApnStateListener mApnStateListener = null;
    private RegistrantList mRegServiceRegistrants = new RegistrantList();
    // for updating Reg state and network type
    private RegistrantList mRegStateRegistrants = new RegistrantList();
    // for checking call state for hVoLTE supported operator
    private boolean mUpdatePended = false;
    private RegiProcessAgentCallStateListener mCallListener = null;
    // for checking roaming feature
    private boolean mRoamingEnabled = true;
    private ContentObserver mContentObserver = null;

    public RegiProcessService() {
    }

    @Override
    public boolean start(IVoLteService voLteService) {
        mVoLteService = voLteService;

        ImsLog.i(getSlotId(), "");

        setApnStateListener();

        // Update ImsStateProvider for reg state
        updateRegStateAndNetworkType(0, 0);

        // Update Reg state on internal IMS module
        notifyRegState(0, 0);

        sendRegStateIntentForVZW();

        if (isHVolteSupportedOperator()){
            mCallListener = new RegiProcessAgentCallStateListener();
            CallUtils.addCallStateListener(getSlotId(), mCallListener);
        }

        Context c = getContext();

        if (c != null) {
            ContentResolver cr = c.getContentResolver();

            if (cr != null) {
                // To prevent user setting is disabled continuously
                Settings.Global.putInt(cr, "preferred_network_mode_changed", 1);

                // register roaming feature observer
                mContentObserver = new ContentObserver(new Handler()) {
                    @Override
                    public void onChange(boolean bChange) {
                        super.onChange(bChange);
                        mRoamingEnabled = ImsStateController.RoamingState.getVoLteRoamingForPhoneId(
                                cr, getSlotId()) == ImsStateController.STATE_ACTIVE ? true : false;
                    }
                };

                ImsStateProvider.RoamingState.registerObserver(cr, mContentObserver);
                mRoamingEnabled = ImsStateController.RoamingState.getVoLteRoamingForPhoneId(
                        cr, getSlotId()) == ImsStateController.STATE_ACTIVE ? true : false;
            }
        }

        mRegiProcessHandler = new RegiProcessHandler();

        IJNIUpCallEvt jniEvt = JNIUpCallEvtManager.getInstance().getJNIUpCallEvt(getSlotId());

        if (jniEvt != null) {
            jniEvt.registerForRegiStateChanged(mRegiProcessHandler,
                    ImsEventDef.IMS_EVENT_REG_STATE, null);
            jniEvt.registerForRegiReportToWFC(mRegiProcessHandler,
                    ImsEventDef.IMS_EVENT_REGI_REPORT_TO_WFC, null);
            jniEvt.registerForRegServiceChanged(mRegiProcessHandler,
                    ImsEventDef.IMS_EVENT_REG_SERVICE, null);
            jniEvt.registerForRegFailureChanged(mRegiProcessHandler,
                    ImsEventDef.IMS_EVENT_REG_FAILURE, null);
            jniEvt.registerForRegDestroyed(mRegiProcessHandler,
                    ImsEventDef.IMS_EVENT_REG_DESTROYED, null);
            jniEvt.registerForRegNotifyState(mRegiProcessHandler,
                    ImsEventDef.IMS_EVENT_NOTIFY_STATE, null);
            jniEvt.registerForRegistrationChanged(mRegiProcessHandler,
                    ImsEventDef.IMS_EVENT_REGISTRATION, null);
            if (ImsGlobal.isOperator(getSlotId(), "SPR")) {
                jniEvt.registerForVoLTEIndicatorChanged(mRegiProcessHandler,
                        ImsEventDef.IMS_EVENT_VOLTE_INDICATOR, null);
            }
        }

        initRegInfo();

        return true;
    }

    @Override
    public void cleanup(Context context) {
        ImsLog.i(getSlotId(), "");

        if (mRegiProcessHandler != null) {
            IJNIUpCallEvt jniEvt = JNIUpCallEvtManager.getInstance().getJNIUpCallEvt(getSlotId());

            if (jniEvt != null) {
                jniEvt.unregisterForRegiStateChanged(mRegiProcessHandler);
                jniEvt.unregisterForRegiReportToWFC(mRegiProcessHandler);
                jniEvt.unregisterForRegServiceChanged(mRegiProcessHandler);
                jniEvt.unregisterForRegFailureChanged(mRegiProcessHandler);
                jniEvt.unregisterForRegDestroyed(mRegiProcessHandler);
                jniEvt.unregisterForRegNotifyState(mRegiProcessHandler);
                jniEvt.unregisterForRegistrationChanged(mRegiProcessHandler);
                jniEvt.unregisterForVoLTEIndicatorChanged(mRegiProcessHandler);
            }

            mRegiProcessHandler.removeCallbacksAndMessages(null);
            mRegiProcessHandler = null;
        }

        if (mIsRegistered) {
            mIsRegistered = false;

            // Update ImsStateProvider for reg state
            updateRegStateAndNetworkType(0, getRegisteredNetworkType());

            // Update Reg state on internal IMS module
            notifyRegState(0, getRegisteredNetworkType());
        }

        clearApnStateListener();

        if (mCallListener != null) {
            CallUtils.removeCallStateListener(getSlotId(), mCallListener);
            mCallListener = null;
        }

        ContentResolver cr = context.getContentResolver();

        if (cr != null) {
            ImsStateProvider.RoamingState.unregisterObserver(cr, mContentObserver);
        }
    }

    @Override
    public void update(Context context) {
    }

    @Override
    public void registerForRegServiceChanged(Handler h, int what, Object obj) {
        mRegServiceRegistrants.add(new Registrant(h, what, obj));
    }

    @Override
    public void unregisterForRegServiceChanged(Handler h) {
        mRegServiceRegistrants.remove(h);
    }

    @Override
    public void registerForRegStateChanged(Handler h, int what, Object obj) {
        mRegStateRegistrants.add(new Registrant(h, what, obj));
    }

    @Override
    public void unregisterForRegStateChanged(Handler h) {
        mRegStateRegistrants.remove(h);
    }

    @Override
    public int getChangedNetworkMode() {
        return mChangedNetworkMode;
    }

    @Override
    public int[] getRegistration() {
        return mRegInfo;
    }

    @Override
    public int getRegServiceState() {
        return mRegServiceState;
    }

    @Override
    public int getRegServiceStateForWiFi() {
        return mRegServiceStateForWiFi;
    }

    @Override
    public int getVoipRegState() {
        return mVoipRegState;
    }

    @Override
    public void updateCurrentRegService(int type) {
        setRegServiceToModem(mRegServiceState, type);
    }

    @Override
    public void setAndUpdateRegService(int regServiceState, int type) {
        setRegServiceToModem(regServiceState, type);
    }

    @Override
    public void setBlockRegStateNotification (boolean blockRegStateNoti) {
        mBlockRegStateNoti = blockRegStateNoti;
    }

    @Override
    public boolean isRegistered() {
        return mIsRegistered;
    }

    @Override
    public void setChangedNetworkMode(int networkMode) {
        mChangedNetworkMode = networkMode;
    }

    @Override
    public boolean isHandleNetworkModeRequired() {
        if (ImsGlobal.isOperatorCountry(getSlotId(), "TMO", "US")
                || ImsGlobal.isOperatorCountry(getSlotId(), "TRF", "US")
                || ImsGlobal.isOperatorCountry(getSlotId(), "MPCS", "US")
                || mIshandleNetworkModeRequired) {
            return true;
        }

        return false;
    }

    @Override
    public void setHandleNetworkModeRequired(boolean handleNetMode) {
        mIshandleNetworkModeRequired = handleNetMode;
    }

    @Override
    public void handleChangeNetworkMode() {
        Context c = getContext();

        if (c != null) {
            ContentResolver cr = c.getContentResolver();

            if (cr != null) {
                TelephonyManager tm = AppContext.getTelephonyManager();

                if (tm != null) {
                    tm.setPreferredNetworkType(MSimUtils.getSubId(getSlotId()),
                            mChangedNetworkMode);
                }

                Settings.Global.putInt(cr,
                        Settings.Global.PREFERRED_NETWORK_MODE + MSimUtils.getSubId(getSlotId()),
                        mChangedNetworkMode);
                Settings.Global.putInt(cr, "preferred_network_mode_changed", 1);
                Settings.Global.putInt(cr, "user_selected_network_mode", 1);
            }
        }

        mChangedNetworkMode = NETWORK_MODE_INVALID;
    }

    @Override
    public void sendRegStateIntentForSPR(int state, int reason) {
        ImsLog.i(getSlotId(), "state=" + state + ", reason=" + reason);

        if (state == REGSTATE_WIFI_REGISTERED || state == REGSTATE_LTE_REGISTERED) {
            if (getNetworkTypeForImsApn() == TelephonyManager.NETWORK_TYPE_IWLAN) {
                mVoipRegState = REGSTATE_WIFI_REGISTERED;
            } else {
                mVoipRegState = REGSTATE_LTE_REGISTERED;
            }
        } else {
            if (getVoipRegState() == state) {
                return;
            }
            mVoipRegState = state;
        }

        String regState = "";
        String regFailureReason = "";
        String regNetwork = "";

        if (mVoipRegState == REGSTATE_IDLE) {
            regState = "IDLE";
            regFailureReason = "";
        } else if (mVoipRegState == REGSTATE_REGISTERING) {
            regState = "REGISTERING";
            regFailureReason = "";
        } else if (mVoipRegState == REGSTATE_WIFI_REGISTERED) {
            regState = "REGISTERED";
            regNetwork = "WIFI";
        } else if (mVoipRegState == REGSTATE_LTE_REGISTERED) {
            regState = "REGISTERED";
            regNetwork = "LTE";
        } else {
            regState = "REG_FAILED";
            if (reason == 424) {
                regFailureReason = "LOCATION_FAIL";
            } else if (reason == 606) {
                regFailureReason = "BLOCKEDLIST_COUNTRY";
            } else {
                regFailureReason = "GENERIC";
            }
        }

        Context context = getContext();
        if (context != null) {
            Intent intent = new Intent("IMS_REGISTRATION_STATE");
            intent.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);
            intent.putExtra("state", regState);
            intent.putExtra("reason", regFailureReason);
            intent.putExtra("network", regNetwork);
            context.sendStickyBroadcast(intent);

            IntentWrapper.sendIntent(context, ImsEventDef.ACTION_VOWIFI_REGISTRATION_STATE,
                    "state", (mVoipRegState == REGSTATE_WIFI_REGISTERED) ? 1 : 0, false);
        }

        // For StatsService
        if (mVoipRegState == REGSTATE_IDLE) {
            regState = "IDLE";
            regFailureReason = "";
        } else if (mVoipRegState == REGSTATE_WIFI_REGISTERED) {
            regState = "REGISTERED";
            regNetwork = "EPDG";
        } else if (mVoipRegState == REGSTATE_LTE_REGISTERED) {
            regState = "REGISTERED";
            regNetwork = "MOBILE";
        } else {
            regState = "REG_FAILED";
            regNetwork = (getNetworkTypeForImsApn() == TelephonyManager.NETWORK_TYPE_IWLAN) ?
                    "EPDG" : "MOBILE";
            regFailureReason = Integer.toString(reason);
        }

        if (context != null) {
            Intent intentForStatsService = new Intent(ImsEventDef.ACTION_WFC_REGISTRATION_STATE);
            intentForStatsService.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);
            intentForStatsService.putExtra("state", regState);
            intentForStatsService.putExtra("reason", regFailureReason);
            intentForStatsService.putExtra("rat", regNetwork);
            intentForStatsService.setPackage(context.getPackageName());
            context.sendBroadcast(intentForStatsService);
        }
    }

    @Override
    public void updateRegistrationStatus(int reason, int detailState) {
        int[] regInfo = getRegistration();

        if (detailState != ImsEventDef.IMS_REGISTRATION_INVALID) {
            regInfo[2] = detailState;
        }

        regInfo[4] = reason;

        IIMSPhoneAgent ip = IIMSPhoneGov.getInstance(getSlotId());

        if (ip != null) {
            ip.setImsRegistrationStatus(regInfo[0],
                    regInfo[1], regInfo[2], regInfo[3], regInfo[4]);
        }
    }

    @Override
    public void updateCurrentRegistration() {
        if (isHVolteSupportedOperator()) {
            // FIX_ME : update operation
            return;
        }

        ImsLog.d(getSlotId(), "reg state = " + mRegInfo[0]);

        // check not registered state
        if (mRegInfo[0] == 0) {
            return;
        }

        // system mode
        mRegInfo[3] = getSystemMode();

        IIMSPhoneAgent ip = IIMSPhoneGov.getInstance(getSlotId());

        if (ip != null) {
            ip.setImsRegistrationStatus(mRegInfo[0],
                    mRegInfo[1], mRegInfo[2], mRegInfo[3], mRegInfo[4]);
        }
    }

    private int getSystemMode() {
        IDCNetWatcher dcnw = (IDCNetWatcher)DCFactory.getDC(
                DCFactory.NETWORK_WATCHER, getSlotId());

        if (dcnw == null) {
            return SYS_MODE_NONE;
        }

        int networkType = dcnw.getNetworkType();
        int sysMode = SYS_MODE_NONE;

        switch (networkType) {
            case TelephonyManager.NETWORK_TYPE_LTE:
                sysMode = SYS_MODE_LTE;
                break;

            case TelephonyManager.NETWORK_TYPE_CDMA:
            case TelephonyManager.NETWORK_TYPE_EVDO_0:
            case TelephonyManager.NETWORK_TYPE_EVDO_A:
            case TelephonyManager.NETWORK_TYPE_1xRTT:
            case TelephonyManager.NETWORK_TYPE_EVDO_B:
            case TelephonyManager.NETWORK_TYPE_EHRPD:
                sysMode = SYS_MODE_HRPD;
                break;

            case TelephonyManager.NETWORK_TYPE_GPRS:
            case TelephonyManager.NETWORK_TYPE_EDGE:
            case TelephonyManager.NETWORK_TYPE_UMTS:
                sysMode = SYS_MODE_UMTS;
                break;

            default:
                break;
        }

        return sysMode;
    }

    private void initRegInfo() {
        // state
        mRegInfo[0] = 0;
        // services
        mRegInfo[1] = 0;
        // detail state
        mRegInfo[2] = 0;
        // system mode
        mRegInfo[3] = 0;
        // reason
        mRegInfo[4] = ImsEventDef.IMS_REGISTRATION_REASON_IMS_INITIALIZATION;
        // slot id
        mRegInfo[5] = getSlotId();

        if (isInitRegInfoRequired()) {
            IIMSPhoneAgent ip = IIMSPhoneGov.getInstance(getSlotId());

            if (ip != null) {
                ip.setImsRegistrationStatus(mRegInfo[0],
                        mRegInfo[1], mRegInfo[2], mRegInfo[3], mRegInfo[4]);
            }
        }
    }

    private boolean isVoipRegistered() {
        return mIsVoipRegistered;
    }

    private void processIpcanChanged(int ipcanType) {
        if (ImsGlobal.isOperator(getSlotId(), "VZW")) {
            mIsRegStateIntentRequired = true;
        } else if (ImsGlobal.isOperator(getSlotId(), "SPR")) {
            if (isVoipRegistered()) {
                if (ipcanType == TelephonyManager.NETWORK_TYPE_IWLAN) {
                    ICallSetting cs = (ICallSetting)AgentFactory.getAgent(
                            AgentFactory.CALL_SETTING, getSlotId());

                    if (cs != null && cs.isWfcEnabled()) {
                        sendRegStateIntentForSPR(REGSTATE_WIFI_REGISTERED, 0);
                    }
                } else {
                    sendRegStateIntentForSPR(REGSTATE_LTE_REGISTERED, 0);
                }
            }
        }
    }

    private void processRegiState(int appType, boolean isRegistered) {
        if (appType == IUIMS.APP_MAX) {
            mIsRegistered = isRegistered;

            // Update ImsStateProvider for reg state
            updateRegStateAndNetworkType(isRegistered ? 1 : 0, getRegisteredNetworkType());

            // Update Reg state on internal IMS module
            notifyRegState(isRegistered ? 1 : 0, getRegisteredNetworkType());
        } else if (appType == IUIMS.APP_MTC) {
            // no-op
        }
    }

    private void processSendPendedUpdate() {
        if (!mUpdatePended) {
            return;
        }

        int sysMode = getSystemMode();

        if (sysMode != SYS_MODE_LTE && sysMode != SYS_MODE_HRPD) {
            sysMode = SYS_MODE_NONE;
        }

        setRegistrationForVZW(sysMode, mRegServiceState);

        if (FeatureConfig.isEnabled(getSlotId(), FeatureConfig.VOWIFI)) {
            mRegServiceRegistrants.notifyResult(Integer.valueOf(mRegServiceState));
        }

        mUpdatePended = false;
    }

    private void setRegiState(Message msg) {
        int appType = msg.arg1;
        int regState = msg.arg2;
        boolean isRegistered = (msg.arg2 == ImsEventDef.IMS_REG_ON);

        ImsLog.d(getSlotId(), "appType=" + appType + ", regState=" + regState
                + ", isRegistered=" + isRegistered);

        processRegiState(appType, isRegistered);

        if (ImsGlobal.isOperator(getSlotId(), "ATT")) {
            setRegiStateForATT(appType, regState, isRegistered);
        } else if (ImsGlobal.isOperatorCountry(getSlotId(), "TMO", "US")
                || ImsGlobal.isOperatorCountry(getSlotId(), "MPCS", "US")
                || ImsGlobal.isOperatorCountry(getSlotId(), "TRF", "US")) {
            setRegiStateForTMUS(appType, regState, isRegistered);
        } else if (ImsGlobal.isOperator(getSlotId(), "VZW")) {
            setRegiStateForVZW(appType, regState, isRegistered);
        } else if (ImsGlobal.isCountry(getSlotId(), "JP")) {
            setRegiStateForJP(appType, regState, isRegistered);
        } else if (ImsGlobal.isOperator(getSlotId(), "SPR")) {
            setRegiStateForSPR(appType, regState, isRegistered);
        } else {
            setRegiStateForGlobal(appType, regState, isRegistered);
        }
    }

    private void setRegiStateForATT(int appType, int regState, boolean isRegistered) {
        // FIXME: Is this for SMS only service??
        // FIXME: Is this really need for all operator??
        if (appType == IUIMS.APP_MTS) {
            if (regState != ImsEventDef.IMS_REG_OFF_DONE) {
                IntentWrapper.sendIntent(getContext(), ImsEventDef.ACTION_SVC_STATUS_IND,
                        "state", isRegistered ? 0 : 1, true);
            }
        } else if (appType == IUIMS.APP_MAX) {
            IntentWrapper.sendIntent(getContext(), ImsEventDef.ACTION_SVC_STATUS_IND,
                    "state", isRegistered ? 0 : 1, true);
        }
    }

    private void setRegiStateForTMUS(int appType, int regState, boolean isRegistered) {
        // FIXME: Is this for SMS only service??
        // FIXME: Is this really need for all operator??
        if (appType == IUIMS.APP_MTS) {
            if (regState != ImsEventDef.IMS_REG_OFF_DONE) {
                IntentWrapper.sendIntent(getContext(), ImsEventDef.ACTION_SVC_STATUS_IND,
                        "state", isRegistered ? 0 : 1, true);
            }
        } else if (appType == IUIMS.APP_MAX) {
            IntentWrapper.sendIntent(getContext(), ImsEventDef.ACTION_SVC_STATUS_IND,
                    "state", isRegistered ? 0 : 1, true);

            if (!isRegistered && mChangedNetworkMode != NETWORK_MODE_INVALID) {
                handleChangeNetworkMode();
            }
        } else if (appType == IUIMS.APP_MTC) {
            if (FeatureConfig.isEnabled(getSlotId(), FeatureConfig.VOWIFI)) {
                if (isEpdgEnabled() && (regState == ImsEventDef.IMS_REG_ON)) {
                    IntentWrapper.sendIntent(getContext(),
                            ImsEventDef.ACTION_VOWIFI_REGISTRATION_STATE, "state", 1, false);
                } else {
                    IntentWrapper.sendIntent(getContext(),
                            ImsEventDef.ACTION_VOWIFI_REGISTRATION_STATE, "state", 0, false);
                }
            }
        }
    }

    private void setRegiStateForGlobal(int appType, int regState, boolean isRegistered) {
        if (appType == IUIMS.APP_MTS) {
            // no-op
        } else if (appType == IUIMS.APP_MAX) {
            if (!isRegistered && mChangedNetworkMode != NETWORK_MODE_INVALID) {
                handleChangeNetworkMode();
            }
        } else if (appType == IUIMS.APP_MTC) {
            if (FeatureConfig.isEnabled(getSlotId(), FeatureConfig.VOWIFI)) {
                if (isEpdgEnabled() && (regState == ImsEventDef.IMS_REG_ON)) {
                    IntentWrapper.sendIntent(getContext(),
                            ImsEventDef.ACTION_VOWIFI_REGISTRATION_STATE, "state", 1,
                                    MSimUtils.PHONE_KEY, getSlotId(), false);
                } else {
                    IntentWrapper.sendIntent(getContext(),
                            ImsEventDef.ACTION_VOWIFI_REGISTRATION_STATE, "state", 0,
                                    MSimUtils.PHONE_KEY, getSlotId(), false);
                }
            }
            // set rtt call type
            if (ImsProperties.isChipVendorMtk()) {
                Context c = getContext();
                if (c != null) {
                    AudioManager audioManager = c.getSystemService(AudioManager.class);
                    if (audioManager != null) {
                        ImsLog.d(getSlotId(), "Set audioManager with regState =" + regState);
                        if (regState == ImsEventDef.IMS_REG_ON) {
                            audioManager.setParameters("rtt_call_type=2");
                        } else {
                            audioManager.setParameters("rtt_call_type=0");
                        }
                    }
                }
            }
        }
    }

    private void setRegiStateForVZW(int appType, int regState, boolean isRegistered) {
        if (appType == IUIMS.APP_MTC) {
            // TODO: need to distinguish a voice call only
            if (isVoipRegistered() != isRegistered) {
                mIsVoipRegistered = isRegistered;
                mIsRegStateIntentRequired = true;
            } else {
                sendRegStateIntentForVZW();
            }
        }
    }

    private void setRegiStateForJP(int appType, int regState, boolean isRegistered) {
        if (appType == IUIMS.APP_MAX) {
            if (!isRegistered && mChangedNetworkMode != NETWORK_MODE_INVALID) {
                handleChangeNetworkMode();
            }
        }
    }

    private void setRegiStateForSPR(int appType, int regState, boolean isRegistered) {
        if (appType == IUIMS.APP_MTC) {
            if (isRegistered) {
                sendRegStateIntentForSPR(REGSTATE_REGISTERING, 0);
                if (getNetworkTypeForImsApn() == TelephonyManager.NETWORK_TYPE_IWLAN) {
                    sendRegStateIntentForSPR(REGSTATE_WIFI_REGISTERED, 0);
                } else {
                    sendRegStateIntentForSPR(REGSTATE_LTE_REGISTERED, 0);
                }
            } else {
                sendRegStateIntentForSPR(REGSTATE_IDLE, 0);
            }
            mIsVoipRegistered = isRegistered;
        }
    }

    private void setRegiReportToWFC(Message msg) {
        ImsLog.d(getSlotId(), "wParam=" + msg.arg1
                + ", lParam=" + String.format("0x%08X", msg.arg2));

        if (ImsGlobal.isOperator(getSlotId(), "SPR")) {
            if (msg.arg1 == ImsEventDef.REPORT_REG_STATUS_REGISTERING) {
                sendRegStateIntentForSPR(REGSTATE_REGISTERING, 0);
            } else if (msg.arg1 == ImsEventDef.REPORT_REG_STATUS_DEREGISTERING) {
                sendRegStateIntentForSPR(REGSTATE_IDLE, 0);
            }
        } else {
            if (msg.arg1 == ImsEventDef.REPORT_REG_STATUS_NOT_READY) {
                sendRegNotReadyIntent(msg.arg2);
            } else {
                sendRegStateIntent(msg.arg1, msg.arg2);
            }
        }
    }

    private void sendRegNotReadyIntent(int lParam) {
        String reason ="";
        int lParamLower = (lParam & 0x0000ffff);

        if (lParamLower == ImsEventDef.REASON_PCSCF_FAILED) {
            reason = "GET_PCSCF_IP_FAILED";
        } else if (lParamLower == ImsEventDef.REASON_LOCAL_IP_FAILED) {
            reason = "GET_LOCAL_IP_FAILED";
        } else if (lParamLower == ImsEventDef.REASON_NO_USER_INFO) {
            reason = "NO_USER_INFO";
        } else if (lParamLower == ImsEventDef.REASON_NO_SERVICE_RAT) {
            reason = "OUT_OF_SERVICE";
        } else if (lParamLower == ImsEventDef.REASON_IMS_DISABLED) {
            reason = "IMS_DISABLED";
        } else if (lParamLower == ImsEventDef.REASON_TTY_ENABLED) {
            reason = "TTY_ENABLED";
        } else if (lParamLower == ImsEventDef.REASON_GBA_NOT_SUPPORTED) {
            reason = "GBA_NOT_SUPPORTED";
        } else if (lParamLower == ImsEventDef.REASON_AIRPLANE_MODE_ENABLED) {
            reason = "AIRPLANE_MODE_ENABLED";
        } else {
            ImsLog.e(getSlotId(), "invalid cause");
            return;
        }

        IntentWrapper.sendIntent(getContext(), ImsEventDef.ACTION_WFC_REGISTRATION_NOREADY,
                "reason", reason, false);
    }

    private void sendRegStateIntent(int wParam, int lParam) {
        String state = "";

        if (wParam == ImsEventDef.REPORT_REG_STATUS_IDLE) {
            state = "IDLE";
        } else if (wParam == ImsEventDef.REPORT_REG_STATUS_REGISTERING) {
            state = "REGISTERING";
        } else if (wParam == ImsEventDef.REPORT_REG_STATUS_REGFAILED) {
            state = "REG_FAILED";
        } else if (wParam == ImsEventDef.REPORT_REG_STATUS_REGISTERED) {
            state = "REGISTERED";
        } else if (wParam == ImsEventDef.REPORT_REG_STATUS_REFRESHING) {
            state = "REFRESHING";
        } else if (wParam == ImsEventDef.REPORT_REG_STATUS_REFRESHFAILED) {
            state = "REFRESH_FAILED";
        } else if (wParam == ImsEventDef.REPORT_REG_STATUS_DEREGISTERING) {
            state = "DEREGISTERING";
        } else {
            ImsLog.e(getSlotId(), "unexpected reg state");
            return;
        }

        String rat = "";
        int lParamUpper = (lParam & 0xffff0000);

        if (lParamUpper == ImsEventDef.IPCAN_CAT_MOBILE) {
            rat = "MOBILE";
        } else if (lParamUpper == ImsEventDef.IPCAN_CAT_EPDG) {
            rat = "EPDG";
        } else {
            ImsLog.e(getSlotId(), "unexpected ipcan");
            return;
        }

        String reason = "";
        int lParamLower = (lParam & 0x0000ffff);

        if (lParamLower == ImsEventDef.REASON_IPSEC_FAILED) {
            reason = "ESP_IPSEC_FAILED";
        } else if (lParamLower == ImsEventDef.REASON_TXN_TIMEOUT) {
            reason = "TRANSACTION_TIMEOUT";
        } else if (lParamLower == ImsEventDef.REASON_NOTIFICATION_TERMINATED) {
            reason = "NOTIFY_TERMINATED";
        } else if (lParamLower == ImsEventDef.REASON_SUBSCRIPTION_FORBIDDEN) {
            reason = "SUBSCRIBE_FORBIDDEN";
        } else if (lParamLower == ImsEventDef.REASON_ALL_OTHER_FAILURES) {
            reason = "ALL_OTHER_FAILURES";
        } else if (lParamLower == ImsEventDef.REASON_SUBSCRIPTION_FORBIDDEN_TERMINATE) {
            reason = "SUBSCRIBE_FORBIDDEN_TERMINATE";
        } else {
            try {
                reason = Integer.toString(lParamLower);
            } catch (Exception e) {
                ImsLog.e(getSlotId(), e.toString());
                return;
            }
        }

        IntentWrapper.sendIntent(getContext(), ImsEventDef.ACTION_WFC_REGISTRATION_STATE,
                "state", state, "rat", rat, "reason", reason, false);
    }

    private void sendRegStateIntentForVZW() {
        if (!ImsGlobal.isOperator(getSlotId(), "VZW")) {
            return;
        }

        if (!mIsRegStateIntentRequired) {
            return;
        }

        int state = NOT_REGISTERED;

        if (mIsVoipRegistered) {
            if (getNetworkTypeForImsApn() == TelephonyManager.NETWORK_TYPE_IWLAN) {
                state = WIFI_REGISTERED;
            } else {
                state = LTE_REGISTERED;
            }
        }

        if (getVoipRegState() == state) {
            return;
        }

        Context c = getContext();

        Intent i = new Intent("com.verizon.provider.ACTION_SETTING_CHANGED");
        i.putExtra("setting", "ims_reg_status");
        i.putExtra("previous_value", mVoipRegState > NOT_REGISTERED ? "1" : "0");
        i.putExtra("new_value", state > NOT_REGISTERED ? "1" : "0");
        i.addFlags(Intent.FLAG_RECEIVER_INCLUDE_BACKGROUND);

        if (c != null) {
            c.sendBroadcastAsUser(i, android.os.UserHandle.CURRENT,
                    "com.verizon.settings.permission.RECEIVE_UPDATED_SETTING");
        }

        mVoipRegState = state;

        IDCNetWatcher dcnw = (IDCNetWatcher)DCFactory.getDC(
                DCFactory.NETWORK_WATCHER, getSlotId());
        if (dcnw != null && !dcnw.isRoaming() && isVZWSim()) {
            String voipState = "";

            if (state == NOT_REGISTERED) {
                voipState = "not_registered";
            } else if (state == WIFI_REGISTERED) {
                voipState = "wifi_registered";
            } else if (state == LTE_REGISTERED) {
                voipState = "lte_registered";
            }

            if (!voipState.isEmpty()) {
                IntentWrapper.sendIntent(c, "com.verizon.net.IMS_REGISTRATION",
                        "state", voipState, true);
            }
        }

        // Update ERI for VoWiFi
        IntentWrapper.sendIntent(c, ImsEventDef.ACTION_VOWIFI_REGISTRATION_STATE,
                "state", (state == WIFI_REGISTERED) ? 1 : 0, false);

        mIsRegStateIntentRequired = false;
    }

    private void setRegServiceToModem(int service, int type) {
        // update the registered services to modem for hVoLTE device
        if (isHVolteSupportedOperator() == false) {
            return;
        }

        ImsLog.d(getSlotId(), "service=" + service + ", type=" + type);

        if (mBlockRegStateNoti) {
            ImsLog.d(getSlotId(), "Reg state notification is blocked");
            return;
        }

        // do not update the res service, only sync up the state
        if (type == ImsEventDef.IMS_REG_SERVICE_TYPE_SYNCUP) {
            mRegServiceState = service;
            return;
        }

        int sysMode = SYS_MODE_NONE;
        boolean isRoaming = false;

        if (ImsTestMode.getInstance().getTestMode(getSlotId()).isCallOverWifiEnabled()
                || (service == ImsEventDef.IMS_REG_SERVICE_VOIP
                        && type == ImsEventDef.IMS_REG_SERVICE_TYPE_UPDATE)) {
            sysMode = SYS_MODE_LTE;
        } else {
            IDCNetWatcher dcnw = (IDCNetWatcher)DCFactory.getDC(
                    DCFactory.NETWORK_WATCHER, getSlotId());
            if (dcnw != null) {
                isRoaming = dcnw.isRoaming();
            }
        }

        if (sysMode == SYS_MODE_NONE) {
            sysMode = getSystemMode();
        }

        if (sysMode != SYS_MODE_LTE
            && sysMode != SYS_MODE_HRPD) {
            sysMode = SYS_MODE_NONE;
        }

        if (getNetworkTypeForImsApn() == TelephonyManager.NETWORK_TYPE_IWLAN) {
            Context c = getContext();

            sysMode = SYS_MODE_WLAN;
            mRegServiceStateForWiFi = service;

            if (isRegistered()) {
                if (service != ImsEventDef.IMS_REG_SERVICE_VOIP) {
                    if (FeatureUtils.isCdmaLessSupported(c) || (mRoamingEnabled && isRoaming)) {
                        ImsLog.d(getSlotId(), "No update ims reg service to prevent PLMN backoff");
                        return;
                    }
                }
            } else {
                if (FeatureUtils.isCdmaLessSupported(c)) {
                    if (type == ImsEventDef.IMS_REG_SERVICE_TYPE_NORMAL) {
                        ImsLog.d(getSlotId(), "No update ims reg service " +
                                "when reg is failing over WiFi");
                        return;
                    } else {
                        ImsLog.d(getSlotId(), "Force update ims reg service to VOIP " +
                                "when reg is failing over WiFi");
                        mRegServiceStateForWiFi = ImsEventDef.IMS_REG_SERVICE_VOIP;
                    }
                }

                mRegServiceState = service; // Sync up reg state for LTE
            }

            setRegistrationForVZW(sysMode, mRegServiceStateForWiFi);

            mRegServiceRegistrants.notifyResult(Integer.valueOf(mRegServiceStateForWiFi));

            ImsLog.d(getSlotId(), "on wifi :: service=" + mRegServiceStateForWiFi
                    + ", sysMode=" + sysMode);
        } else {
            mRegServiceStateForWiFi = 0;

            // check valid combination
            if (sysMode == SYS_MODE_HRPD) {
                if (service != ImsEventDef.IMS_REG_SERVICE_NONE
                        && service != ImsEventDef.IMS_REG_SERVICE_SMS) {
                    return;
                }
            }

            mRegServiceState = service;

            // Check existing calls
            if (mRegServiceState == ImsEventDef.IMS_REG_SERVICE_NONE) {
                ICallStateTracker icst = CallUtils.getCallStateTracker(getSlotId());

                if ((icst != null) && icst.hasEstablishedCall()) {
                    ImsLog.d(getSlotId(), "Update is pended due to existing calls");
                    mUpdatePended = true;
                    return;
                }
            }

            if (mUpdatePended) {
                mUpdatePended = false;
            }

            setRegistrationForVZW(sysMode, mRegServiceState);

            mRegServiceRegistrants.notifyResult(Integer.valueOf(mRegServiceState));
        }
    }

    private void setRegFailure(int respCode) {
        ImsLog.d(getSlotId(), "responseCode=" + respCode);

        if (ImsGlobal.isOperator(getSlotId(), "SPR")) {
            sendRegStateIntentForSPR(REGSTATE_REGISTERING, 0);
            sendRegStateIntentForSPR(REGSTATE_FAILED, respCode);
        } else if (ImsGlobal.isOperator(getSlotId(), "VZW")
                    || ImsGlobal.isOperator(getSlotId(), "ATT")) {
            String imsStatus = UICCHelper.getStringForIMSRegEvent(respCode,
                    getSlotId(), ImsEventDef.IMS_REG_NOTIFY_STATE_INVALID);
            if (imsStatus != null) {
                IIMSPhoneAgent ip = IIMSPhoneGov.getInstance(getSlotId());

                if (ip != null) {
                    ip.sendEnvelope(imsStatus);
                }
            }
        }
    }

    private void setRegDestroyed(int reason) {
        ImsLog.d(getSlotId(), "reason=" + reason);
    }

    private void setRegistration(int param1, int param2) {
        ImsLog.d(getSlotId(), "param1=" + String.format("0x%08X", param1)
                + ", param2=" + String.format("0x%08X", param2));

        // state
        mRegInfo[0] = (param1 >> 16) & 0xFFFF;
        // services
        mRegInfo[1] = param1 & 0xFFFF;
        // detail state
        mRegInfo[2] = param2 & 0xFFFF;
        // system mode
        mRegInfo[3] = getSystemMode();
        // reason
        mRegInfo[4] = (param2 >> 16) & 0xFFFF;
        // slot id
        mRegInfo[5] = getSlotId();

        IIMSPhoneAgent ip = IIMSPhoneGov.getInstance(getSlotId());

        if (ip != null) {
            ip.setImsRegistrationStatus(mRegInfo[0],
                    mRegInfo[1], mRegInfo[2], mRegInfo[3], mRegInfo[4]);
        }
    }

    private void setRegistrationForVZW(int sysMode, int services) {
        ImsLog.d(getSlotId(), "sysMode=" + sysMode + ", services=" + services);

        // state
        mRegInfo[0] = (services != 0) ? 1 : 0;
        // services
        mRegInfo[1] = 0;
        if (services == ImsEventDef.IMS_REG_SERVICE_SMS) {
            mRegInfo[1] = ImsEventDef.IMS_REGISTRATION_SERVICE_SMS;
        } else if (services == ImsEventDef.IMS_REG_SERVICE_VOIP) {
            mRegInfo[1] = ImsEventDef.IMS_REGISTRATION_SERVICE_SMS
                    | ImsEventDef.IMS_REGISTRATION_SERVICE_VOICE;
        }
        // detail state (0 : offline , 2 : registered)
        mRegInfo[2] = (services != 0) ? 2 : 0;
        // system mode
        mRegInfo[3] = sysMode;
        // reason (0 : unspecified)
        mRegInfo[4] = 0;
        // slot id
        mRegInfo[5] = getSlotId();

        IIMSPhoneAgent ip = IIMSPhoneGov.getInstance(getSlotId());

        if (ip != null) {
            ip.setImsRegistrationStatus(mRegInfo[0],
                    mRegInfo[1], mRegInfo[2], mRegInfo[3], mRegInfo[4]);
        }
    }

    private void setRegNotifyState(int reason) {
        ImsLog.d(getSlotId(), "reason=" + reason);

        if (!ImsGlobal.isOperator(getSlotId(), "VZW")
            && !ImsGlobal.isOperator(getSlotId(), "ATT")) {
            return;
        }

        if (isRegistered()) {
            String imsStatus = UICCHelper.getStringForIMSRegEvent(200, getSlotId(), reason);

            if (imsStatus != null) {
                IIMSPhoneAgent ip = IIMSPhoneGov.getInstance(getSlotId());

                if (ip != null) {
                    ip.sendEnvelope(imsStatus);
                }
            }
        }
    }

    private void clearApnStateListener() {
        if (mApnStateListener == null) {
            return;
        }

        IDCApn dcApn = (IDCApn)DCFactory.getDC(DCFactory.APN, getSlotId());
        IApn apn = (dcApn != null) ? dcApn.getApnControl(EApnType.IMS.getType()) : null;

        if (apn != null) {
            apn.removeListener(mApnStateListener);
        }

        mApnStateListener = null;
    }

    private void setApnStateListener() {
        IDCApn dcApn = (IDCApn)DCFactory.getDC(DCFactory.APN, getSlotId());
        IApn apn = (dcApn != null) ? dcApn.getApnControl(EApnType.IMS.getType()) : null;

        if (apn != null) {
            mApnStateListener = new ImsApnStateListener();
            apn.addListener(mApnStateListener);
        }
    }

    private int getNetworkTypeForImsApn() {
        IDCApn dcApn = (IDCApn)DCFactory.getDC(DCFactory.APN, getSlotId());
        IApn apn = (dcApn != null) ? dcApn.getApnControl(EApnType.IMS.getType()) : null;

        if (apn != null && apn.getIpcanCategory() == IApn.IPCAN_CATEGORY_WLAN) {
            return TelephonyManager.NETWORK_TYPE_IWLAN;
        }

        // TelephonyManager.NETWORK_TYPE_LTE
        return 0;
    }

    private int getRegisteredNetworkType() {
        return getNetworkTypeForImsApn();
    }

    private void updateCallStateForWifi(int networkType) {
        Context c = getContext();

        if (c == null) {
            return;
        }

        ContentResolver cr = c.getContentResolver();
        if (ImsStateController.CallState.getStateForPhoneId(cr, getSlotId())
                == ImsStateController.STATE_INACTIVE) {
            return;
        }

        ImsLog.d(getSlotId(), "networkType=" + networkType);

        if (networkType == TelephonyManager.NETWORK_TYPE_IWLAN) {
            ImsStateController.CallState.putConnectedCallOnWifiForPhoneId(
                    cr, ImsStateController.STATE_ACTIVE, getSlotId());
        } else {
            ImsStateController.CallState.putConnectedCallOnWifiForPhoneId(
                    cr, ImsStateController.STATE_INACTIVE, getSlotId());
        }
    }

    private void updateRegNetworkType(int networkType, boolean updateOnRegistered) {
        Context c = getContext();

        if (c == null) {
            return;
        }

        ContentResolver cr = c.getContentResolver();
        if (updateOnRegistered && ImsStateController.RegState.getStateForPhoneId(cr, getSlotId())
                == ImsStateController.STATE_INACTIVE) {
            return;
        }

        ImsLog.d(getSlotId(), "networkType=" + networkType);

        ImsStateController.RegState.putNetworkTypeForPhoneId(cr, networkType, getSlotId());
    }

    private void updateRegStateAndNetworkType(int regState, int networkType) {
        Context c = getContext();

        if (c == null) {
            return;
        }

        ContentResolver cr = c.getContentResolver();
        ImsLog.d(getSlotId(), "regState=" + regState + ", networkType=" + networkType);

        ImsStateController.RegState.putStateForPhoneId(cr, regState, getSlotId());
        ImsStateController.RegState.putNetworkTypeForPhoneId(cr, networkType, getSlotId());
    }

    private void notifyRegState(int regState, int networkType) {
        Message msg = Message.obtain();
        msg.what = 0;
        msg.arg1 = regState;
        msg.arg2 = networkType;

        ImsLog.i(getSlotId(), "regState=" + msg.arg1 + ", networkType=" + msg.arg2);

        mRegStateRegistrants.notifyResult(msg);
    }

    private Context getContext() {
        return (mVoLteService != null) ? mVoLteService.getContext() : null;
    }

    private int getSlotId() {
        return (mVoLteService != null) ? mVoLteService.getSlotID() : 0;
    }

    private boolean isHVolteSupportedOperator() {
        if (ImsGlobal.isOperator(getSlotId(), "VZW")
                || ImsGlobal.isOperator(getSlotId(), "USC")
                || ImsGlobal.isOperator(getSlotId(), "SPR")
                || ImsGlobal.isOperator(getSlotId(), "ACG")
                || ImsGlobal.isOperatorCountry(getSlotId(), "CT", "CN")) {
            return true;
        }

        return false;
    }

    private boolean isInitRegInfoRequired() {
        if (ImsGlobal.isOperator(getSlotId(), "VZW")
                || ImsGlobal.isOperatorCountry(getSlotId(), "CT", "CN")
                || ImsGlobal.isOperator(getSlotId(), "XPM")) {
            // for some hVoLTE operator, it is not required.
            return false;
        }

        return true;
    }

    private boolean isVZWSim() {
        ITelephonySubscriber ts = (ITelephonySubscriber)AgentFactory.getAgent(
            AgentFactory.TELEPHONY_SUBSCRIBER, getSlotId());

        if (ts != null) {
            String mccmnc = ts.getMccMnc(true);

            if (mccmnc != null && mccmnc.equals("311480")) {
                return true;
            }
        }

        return false;
    }

    private boolean isEpdgEnabled() {
        IWifiState ws = (IWifiState)AgentFactory.getAgent(AgentFactory.WIFI_STATE);
        if (ws != null && !ws.isWifiConnected()) {
            return false;
        }

        IDCApn dcApn = (IDCApn)DCFactory.getDC(DCFactory.APN, getSlotId());
        IApn apn = (dcApn != null) ? dcApn.getApnControl(EApnType.IMS.getType()) : null;

        if (apn != null && apn.getIpcanCategory() == IApn.IPCAN_CATEGORY_WLAN) {
            return true;
        }

        return false;
    }

    private class RegiProcessHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            if (msg == null) {
                return;
            }

            ImsLog.i(getSlotId(), "RegiProcessHandler :: what=" + msg.what);

            AsyncResult ar = (AsyncResult) msg.obj;

            if (ar == null) {
                return;
            }

            Message eventMsg = (Message) ar.result;

            if (eventMsg == null) {
                return;
            }

            switch(eventMsg.what) {
                /** Common */
                case ImsEventDef.IMS_EVENT_REG_STATE:
                    setRegiState(eventMsg);
                    break;

                /** Common */
                case ImsEventDef.IMS_EVENT_REGISTRATION:
                    setRegistration(eventMsg.arg1, eventMsg.arg2);
                    break;

                /** Common */
                case ImsEventDef.IMS_EVENT_REGI_REPORT_TO_WFC:
                    setRegiReportToWFC(eventMsg);
                    break;

                /** hVoLTE supported only (SPR / USC / VZW / ACG / CT) */
                case ImsEventDef.IMS_EVENT_REG_SERVICE:
                    sendRegStateIntentForVZW();
                    setRegServiceToModem(eventMsg.arg1, eventMsg.arg2);
                    break;

                /** KDDI / SPR / VZW only */
                case ImsEventDef.IMS_EVENT_REG_FAILURE:
                    setRegFailure(eventMsg.arg1);
                    break;

                /** VZW only */
                case ImsEventDef.IMS_EVENT_REG_DESTROYED:
                    setRegDestroyed(eventMsg.arg1);
                    break;

                /** VZW only */
                case ImsEventDef.IMS_EVENT_NOTIFY_STATE:
                    setRegNotifyState(eventMsg.arg1);
                    break;

                /** SPR only */
                case ImsEventDef.IMS_EVENT_VOLTE_INDICATOR:
                    sendRegStateIntentForSPR(mVoipRegState, 0);
                    break;

                default:
                    break;
            }
        }
    }

    private class ImsApnStateListener extends ApnStateListener {
        @Override
        public void onIpcanCategoryChanged(int apnType, int ipcanCategory) {
            ImsLog.d(getSlotId(), "onIpcanCategoryChanged :: apnType="
                    + apnType + ", ipcanCategory=" + ipcanCategory);

            if (apnType != EApnType.IMS.getType()) {
                return;
            }

            final int networkType = (ipcanCategory == IApn.IPCAN_CATEGORY_WLAN) ?
                    TelephonyManager.NETWORK_TYPE_IWLAN : 0;

            if (mRegiProcessHandler != null) {
                mRegiProcessHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        updateRegNetworkType(networkType, true);
                        updateCallStateForWifi(networkType);
                        processIpcanChanged(networkType);
                    }
                });
            } else {
                updateRegNetworkType(networkType, true);
                updateCallStateForWifi(networkType);
                processIpcanChanged(networkType);
            }

            // Update Reg state on internal IMS module
            notifyRegState(mIsRegistered ? 1 : 0, networkType);
        }
    }

    private class RegiProcessAgentCallStateListener extends CallStateListener {
        public RegiProcessAgentCallStateListener() {

        }

        @Override
        public void onCallDestroyed(Call call) {
            if (mUpdatePended) {
                processSendPendedUpdate();
            }
        }

        @Override
        public void onCallTerminated(Call call) {
            if (mUpdatePended) {
                processSendPendedUpdate();
            }
        }
    }
}
