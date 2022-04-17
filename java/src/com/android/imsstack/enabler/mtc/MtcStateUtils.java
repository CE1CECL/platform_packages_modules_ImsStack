/*
    Author
    <table>
    date        author                  description
    --------    --------------          ----------
    20131015    hwangoo.park@           Created
    </table>

    Description
*/

package com.android.imsstack.enabler.mtc;

import android.content.Context;
import android.content.ContentResolver;
import android.content.Intent;
import android.database.ContentObserver;

import com.android.imsstack.core.ImsGlobal;
import com.android.imsstack.core.agents.AgentFactory;
import com.android.imsstack.core.agents.agentif.ISubscription;
import com.android.imsstack.provider.ImsStateController;
import com.android.imsstack.util.ImsLog;
import com.android.imsstack.util.MSimUtils;
import com.android.imsstack.util.SettingsUtils;

public class MtcStateUtils {
    public static final int INIT_REG_STATE = 0x01;
    public static final int INIT_CALL_STATE = 0x02;
    public static final int INIT_ALL = INIT_REG_STATE | INIT_CALL_STATE;

    public static final int STATE_INACTIVE = ImsStateController.STATE_INACTIVE;
    public static final int STATE_ACTIVE = ImsStateController.STATE_ACTIVE;

    public static final String SERVICE_VOIP = "VOIP";
    public static final String SERVICE_VT = "VT";
    public static final String SERVICE_UC = "UC";

    public static final String ACTION_CALL_STATE = "com.android.imsstack.action.CALL_STATE";
    public static final String ACTION_REG_STATE = "com.android.imsstack.action.REG_STATE";
    public static final String EXTRA_SUB_ID = "subId";
    public static final String EXTRA_STATE = "state";
    public static final String EXTRA_SVC_TYPE = "svcType";

    private MtcStateUtils() {
    }

    // Invoked on boot-up.
    public static void initializeStateOnceOnBootup(Context context) {
        logi("initializeStateOnceOnBootup");

        ContentResolver cr = context.getContentResolver();

        if (MSimUtils.isMultiSimEnabled()) {
            int phoneCount = MSimUtils.getPhoneCount();
            for (int phoneId = 0; phoneId < phoneCount; ++phoneId) {
                int subId = MSimUtils.getSubId(phoneId);
                ImsStateController.RegState.init(cr, phoneId, subId);
                ImsStateController.VoLteState.init(cr, phoneId, subId);
                ImsStateController.CallState.init(cr, phoneId, subId);
                ImsStateController.RoamingState.init(cr, phoneId, subId);
            }
        } else {
            int subId = MSimUtils.getSubId(MSimUtils.DEFAULT_PHONE_ID);
            ImsStateController.RegState.init(cr, MSimUtils.DEFAULT_PHONE_ID, subId);
            ImsStateController.VoLteState.init(cr, MSimUtils.DEFAULT_PHONE_ID, subId);
            ImsStateController.CallState.init(cr, MSimUtils.DEFAULT_PHONE_ID, subId);
            ImsStateController.RoamingState.init(cr, MSimUtils.DEFAULT_PHONE_ID, subId);
        }
    }

    // When "ims" service is opened or operator is changed
    public static void initializeState(Context context, int slotId) {
        ContentResolver cr = context.getContentResolver();

        int subId = (MSimUtils.isMultiImsEnabled() || MSimUtils.isMultiImsEnabledOnDssv())
                    ? getSubId(slotId) : getActiveSubId();
        int phoneId = getPhoneId(slotId);

        ImsStateController.RegState.init(cr, phoneId, subId);
        ImsStateController.VoLteState.init(cr, phoneId, subId);
        ImsStateController.CallState.init(cr, phoneId, subId);
        ImsStateController.RoamingState.init(cr, phoneId, subId);

        notifyRegState(context, slotId, IUMtcService.SERVICE_NONE, SERVICE_UC);
        notifyCallState(context, slotId, STATE_INACTIVE, SERVICE_VT);
        notifyCallState(context, slotId, STATE_INACTIVE, SERVICE_VOIP);

        boolean isVoLteEnabled = ImsGlobal.isVoLteEnabled(context, slotId);
        boolean isWfcEnabled = ImsGlobal.isWfcEnabled(context, slotId);
        boolean isVtEnabled = ImsGlobal.isVtEnabled(context, slotId);
        boolean isVoLteProvisioningRequired
                = ImsGlobal.isVoLteProvisioningRequired(context, slotId);
        boolean isVtProvisioningRequired = ImsGlobal.isVtProvisioningRequired(context, slotId);
        boolean isWfcProvisioningRequired = ImsGlobal.isWfcProvisioningRequired(context, slotId);
        boolean isImsStateSyncRequired = isVoLteProvisioningRequired || isVtProvisioningRequired;

        logi("initializeState :: slotId=" + slotId
                + ", voLteEnabled=" + isVoLteEnabled
                + ", wfcEnabled=" + isWfcEnabled
                + ", vtEnabled=" + isVtEnabled
                + ", voLteProvisioningRequired=" + isVoLteProvisioningRequired
                + ", vtProvisioningRequired=" + isVtProvisioningRequired);

        // FIXME: apply the operator-specific requirement if needed
        if (isVoLteProvisioningRequired) {
            // VoLTE on/off will be determined by the other parameter (ex. DM)
        } else if (isVoLteEnabled) {
            updateVoLteProvisioned(context, slotId, STATE_ACTIVE);
        } else {
            updateVoLteProvisioned(context, slotId, STATE_INACTIVE);
        }

        if (isVtProvisioningRequired) {
            // VT on/off will be determined by the other parameter (ex. DM)
        } else if (isVtEnabled) {
            updateVtProvisioned(context, slotId, STATE_ACTIVE);
        } else {
            if (!ImsGlobal.isCountry(slotId, "KR")) {
                updateVtProvisioned(context, slotId, STATE_INACTIVE);
            } else {
                updateVtProvisioned(context, slotId, STATE_ACTIVE);
            }
        }

        if (isWfcProvisioningRequired) {
            // WFC on/off will be determined by the other parameter (ex. DM)
        } else if (isWfcEnabled) {
            updateWfcProvisioned(context, slotId, STATE_ACTIVE);
        } else {
            updateWfcProvisioned(context, slotId, STATE_INACTIVE);
        }

        if (isImsStateSyncRequired) {
            logi("initializeState :: ImsStateSyncRequired");
        }
    }

    public static void initializeImsState(Context context, int phoneId, int initFlags) {
        if (context == null) {
            return;
        }

        ContentResolver cr = context.getContentResolver();

        if (phoneId >= MSimUtils.DEFAULT_PHONE_ID) {
            initializeImsStateInternal(cr, phoneId, initFlags);
        } else {
            if (MSimUtils.isMultiSimEnabled()) {
                int phoneCount = MSimUtils.getPhoneCount();
                for (int pid = 0; pid < phoneCount; ++pid) {
                    initializeImsStateInternal(cr, pid, initFlags);
                }
            } else {
                initializeImsStateInternal(cr, MSimUtils.DEFAULT_PHONE_ID, initFlags);
            }
        }

        // FIXME: Is it required to turn off "volte/vt provisioned" state?
    }

    public static void initializeImsStateForSubscriptionChanged(
            Context context, int phoneId) {
        if (context == null) {
            return;
        }

        ContentResolver cr = context.getContentResolver();
        int subId = MSimUtils.getSubId(phoneId);

        ImsStateController.RegState.putSubId(cr, phoneId, subId);
        ImsStateController.VoLteState.putSubId(cr, phoneId, subId);
        ImsStateController.CallState.putSubId(cr, phoneId, subId);
        ImsStateController.RoamingState.putSubId(cr, phoneId, subId);
    }

    public static int getRegisteredServiceType(Context context, int phoneId) {
        if (context == null) {
            return STATE_INACTIVE;
        }

        return ImsStateController.VoLteState.getServiceStatusForPhoneId(
                context.getContentResolver(), phoneId);
    }

    public static boolean isVoLteProvisioned(Context context, int phoneId) {
        if (context == null) {
            return false;
        }

        return ImsStateController.VoLteState.getVoLteProvisionedForPhoneId(
                context.getContentResolver(), phoneId) == STATE_ACTIVE;
    }

    public static boolean isVtProvisioned(Context context, int phoneId) {
        if (context == null) {
            return false;
        }

        return ImsStateController.VoLteState.getVtProvisionedForPhoneId(
                context.getContentResolver(), phoneId) == STATE_ACTIVE;
    }

    public static boolean isWfcProvisioned(Context context, int phoneId) {
        if (context == null) {
            return false;
        }

        return ImsStateController.VoLteState.getWfcProvisionedForPhoneId(
                context.getContentResolver(), phoneId) == STATE_ACTIVE;
    }

    public static void notifyCallState(Context context, int slotId, int state, String svcType) {
        if (context == null) {
            return;
        }

        int subId = getSubId(slotId);

        log("notifyCallState :: slotId=" + slotId + ", subId=" + subId
                + ", state=" + state + ", svcType=" + svcType);

        Intent intent = new Intent(ACTION_CALL_STATE);

        intent.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);
        intent.addFlags(Intent.FLAG_RECEIVER_FOREGROUND);
        intent.addFlags(Intent.FLAG_RECEIVER_INCLUDE_BACKGROUND);
        intent.putExtra(EXTRA_SUB_ID, subId);
        intent.putExtra(EXTRA_STATE, state);
        intent.putExtra(EXTRA_SVC_TYPE, svcType);

        context.sendBroadcast(intent);
    }

    public static void notifyRegState(Context context, int slotId, int state, String svcType) {
        if (context == null) {
            return;
        }

        int subId = getSubId(slotId);

        log("notifyRegState :: slotId=" + slotId + ", subId=" + subId
                + ", state=" + state + ", svcType=" + svcType);

        Intent intent = new Intent(ACTION_REG_STATE);

        intent.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);
        intent.addFlags(Intent.FLAG_RECEIVER_FOREGROUND);
        intent.addFlags(Intent.FLAG_RECEIVER_INCLUDE_BACKGROUND);
        intent.putExtra(EXTRA_SUB_ID, subId);
        intent.putExtra(EXTRA_STATE, state);
        intent.putExtra(EXTRA_SVC_TYPE, svcType);

        context.sendBroadcast(intent);
    }

    public static void registerContentObserver(Context context, ContentObserver observer) {
        if (context == null) {
            return;
        }

        ImsStateController.VoLteState.registerObserver(context.getContentResolver(), observer);
    }

    public static void unregisterContentObserver(Context context, ContentObserver observer) {
        if (context == null) {
            return;
        }

        ImsStateController.VoLteState.unregisterObserver(context.getContentResolver(), observer);
    }

    public static void updateSubId(Context context) {
        if (context == null) {
            return;
        }

        ContentResolver cr = context.getContentResolver();

        if (MSimUtils.isMultiSimEnabled()) {
            int phoneCount = MSimUtils.getPhoneCount();

            for (int phoneId = 0; phoneId < phoneCount; ++phoneId) {
                updateSubIdIfChanged(cr, phoneId, MSimUtils.INVALID_SUB_ID);
            }
        } else {
            updateSubIdIfChanged(cr, MSimUtils.DEFAULT_PHONE_ID, getActiveSubId());
        }
    }

    public static void updateCallState(Context context, int slotId, int state) {
        if (context == null) {
            return;
        }

        ContentResolver cr = context.getContentResolver();

        logi("updateCallState :: state=" + state + ", slotId=" + slotId);

        ImsStateController.VoLteState.putCallStateForPhoneId(cr, state, slotId);
        ImsStateController.CallState.putStateForPhoneId(cr, state, slotId);
    }

    public static void updateRegNetworkType(Context context, int slotId,
            int networkType, boolean updateOnRegistered) {
        if (context == null) {
            return;
        }

        ContentResolver cr = context.getContentResolver();

        if (updateOnRegistered
                && ImsStateController.RegState.getStateForPhoneId(cr, slotId) != STATE_ACTIVE) {
            return;
        }

        logi("updateRegNetworkType :: networkType=" + networkType + ", slotId=" + slotId);

        ImsStateController.RegState.putNetworkTypeForPhoneId(cr, networkType, slotId);
    }

    public static void updateRegState(Context context, int slotId, int state) {
        if (context == null) {
            return;
        }

        logi("updateRegState :: state=" + state + ", slotId=" + slotId);

        ImsStateController.VoLteState.putServiceStatusForPhoneId(
                context.getContentResolver(), state, slotId);
    }

    public static void updateVoLteProvisioned(Context context, int slotId, int provisioned) {
        if (context == null) {
            return;
        }

        logi("updateVoLteProvisioned :: provisioned=" + provisioned + ", slotId=" + slotId);

        ImsStateController.VoLteState.putVoLteProvisionedForPhoneId(
                context.getContentResolver(), provisioned, slotId);

        if (ImsGlobal.isOperator(slotId, "VZW")) {
            SettingsUtils.putAllowVoLteProvisioning(context.getContentResolver(), provisioned);
        }
    }

    public static void updateVtProvisioned(Context context, int slotId, int provisioned) {
        if (context == null) {
            return;
        }

        logi("updateVtProvisioned :: provisioned=" + provisioned + ", slotId=" + slotId);

        ImsStateController.VoLteState.putVtProvisionedForPhoneId(
                context.getContentResolver(), provisioned, slotId);
    }

    public static void updateWfcProvisioned(Context context, int slotId, int provisioned) {
        if (context == null) {
            return;
        }

        logi("updateWfcProvisioned :: provisioned=" + provisioned + ", slotId=" + slotId);

        ImsStateController.VoLteState.putWfcProvisionedForPhoneId(
                context.getContentResolver(), provisioned, slotId);
    }

    private static int getActiveSubId() {
        ISubscription isub = (ISubscription)AgentFactory.getAgent(AgentFactory.SUBSCRIPTION);
        return (isub == null) ? MSimUtils.getImsDefaultSubId() : isub.getActiveSubId();
    }

    private static int getPhoneId(int slotId) {
        ISubscription isub = (ISubscription)AgentFactory.getAgent(AgentFactory.SUBSCRIPTION);
        return (isub == null) ? slotId : isub.getPhoneId(slotId);
    }

    private static int getSubId(int slotId) {
        ISubscription isub = (ISubscription)AgentFactory.getAgent(AgentFactory.SUBSCRIPTION);
        return (isub == null) ? MSimUtils.INVALID_SUB_ID : isub.getSubId(slotId);
    }

    private static void initializeImsStateInternal(ContentResolver cr, int phoneId, int initFlags) {
        if ((initFlags & INIT_CALL_STATE) != 0) {
            ImsStateController.initCallState(cr, phoneId);
        }

        if ((initFlags & INIT_REG_STATE) != 0) {
            ImsStateController.initRegState(cr, phoneId);
        }
    }

    private static void updateSubIdIfChanged(ContentResolver cr, int phoneId, int subId) {
        if (subId == MSimUtils.INVALID_SUB_ID) {
            subId = MSimUtils.getSubId(phoneId);
        }

        if (!MSimUtils.isValidSubId(subId)) {
            if (!MSimUtils.isMultiSimEnabled()) {
                return;
            }

            // Update subId for multi-SIM even though it's invalid.
            // Case: change same SIM card from SIM1 to SIM2 or vice versa.
        }

        if (MSimUtils.hasIccCard(phoneId)
                && MSimUtils.isDummySubId(subId)) {
            return;
        }

        int subIdForRegState = ImsStateController.RegState.getSubId(cr, phoneId);
        int subIdForVoLteState = ImsStateController.VoLteState.getSubId(cr, phoneId);
        int subIdForCallState = ImsStateController.CallState.getSubId(cr, phoneId);
        int subIdForRoamingState = ImsStateController.RoamingState.getSubId(cr, phoneId);

        if (subId != subIdForRegState) {
            ImsStateController.RegState.putSubId(cr, phoneId, subId);
        }

        if (subId != subIdForVoLteState) {
            ImsStateController.VoLteState.putSubId(cr, phoneId, subId);
        }

        if (subId != subIdForCallState) {
            ImsStateController.CallState.putSubId(cr, phoneId, subId);
        }

        if (subId != subIdForRoamingState) {
            ImsStateController.RoamingState.putSubId(cr, phoneId, subId);
        }

        logi("updateSubIdIfChanged :: subId=" + subId + ", subIdForReg=" + subIdForRegState
                + ", subIdForVoLte=" + subIdForVoLteState + ", subIdForCall=" + subIdForCallState
                + ", subIdForRoaming=" + subIdForRoamingState);
    }

    private static void log(String s) {
        ImsLog.d("[GII-MTC] " + s);
    }

    private static void logi(String s) {
        ImsLog.i("[GII-MTC] " + s);
    }
}
