package com.android.imsstack.core;

import android.content.Context;

import com.android.imsstack.core.agents.AgentFactory;
import com.android.imsstack.core.agents.agentif.IIMSPhoneAgent;
import com.android.imsstack.core.agents.agentif.IRilCommand;
import com.android.imsstack.core.agents.agentif.ISubscription;
import com.android.imsstack.util.ImsLog;
import com.android.imsstack.util.MSimUtils;

/**
 * This class provides the inter-communication between the application and RIL.
 */
public class RilCommandProxy implements IRilCommand {
    private static boolean sMultiSimEnabled = false;
    private static boolean sMultiLteEnabled = false;
    private static RilCommandProxy sRilCommandProxy = null;

    public static RilCommandProxy getInstance(Context context) {

        if (sRilCommandProxy == null) {
            sRilCommandProxy = new RilCommandProxy(context);
        }

        return sRilCommandProxy;
    }

    /* ---------------------------------------------------------------------------------------------
        implements IVoLteAgent
    --------------------------------------------------------------------------------------------- */
    @Override
    public void cleanup() {
    }

    @Override
    public void start(int slotID) {
    }

    @Override
    public void stop(int slotID) {
    }

    /* ---------------------------------------------------------------------------------------------
        implements IRilCommand
    --------------------------------------------------------------------------------------------- */
    public void disableTuneAway() {
        if (!isRilCommandRequired()) {
            return;
        }

        int slotId = getActiveSlotId();

        if (slotId < 0 || slotId >= MSimUtils.getMaxSimSlot()) {
            return;
        }

        if (isSimAbsent(getOtherSlotId(slotId))) {
            // No need to disable tune-away if other SIM is absent.
            return;
        }

        IIMSPhoneAgent ipa = (IIMSPhoneAgent)AgentFactory.getAgent(AgentFactory.IMS_PHONE, slotId);
        if (ipa != null) {
            ipa.setPeerSimSuspend(false);
        }

        ImsLog.d("Peer SIM paging monitoring disabled");
    }

    public void enableTuneAway() {
        if (!isRilCommandRequired()) {
            return;
        }

        int slotId = getActiveSlotId();

        if (slotId < 0 || slotId >= MSimUtils.getMaxSimSlot()) {
            return;
        }

        if (isSimAbsent(getOtherSlotId(slotId))) {
            // No need to enable tune-away if other SIM is absent.
            return;
        }

        IIMSPhoneAgent ipa = (IIMSPhoneAgent)AgentFactory.getAgent(AgentFactory.IMS_PHONE, slotId);

        if (ipa != null) {
            ipa.setPeerSimSuspend(true);
        }

        ImsLog.d("Peer SIM paging monitoring enabled");
    }

    private static int getActiveSlotId() {
        ISubscription isub = (ISubscription)AgentFactory.getAgent(AgentFactory.SUBSCRIPTION);

        if (isub != null) {
            return isub.getSlotId();
        } else {
            return MSimUtils.getSlotId(MSimUtils.getDefaultDataSubId());
        }
    }

    private static int getOtherSlotId(int slotId) {
        return (slotId == 0) ? 1 : 0;
    }

    private static boolean isSimAbsent(int slotId) {
        ISubscription isub = (ISubscription)AgentFactory.getAgent(AgentFactory.SUBSCRIPTION);
        return (isub != null) ? isub.isSimAbsent(slotId) : false;
    }

    private static boolean isRilCommandRequired() {
        return (sMultiSimEnabled && !sMultiLteEnabled);
    }

    /* ---------------------------------------------------------------------------------------------
        private methods
    --------------------------------------------------------------------------------------------- */
    private RilCommandProxy(Context context) {
        ImsLog.d("");

        sMultiSimEnabled = MSimUtils.isMultiSimEnabled();
        sMultiLteEnabled = MSimUtils.isMultiLteEnabled();
    }
}
