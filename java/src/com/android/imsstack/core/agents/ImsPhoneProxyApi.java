package com.android.imsstack.core.agents;

import com.android.imsstack.util.ImsLog;
import com.android.imsstack.util.ImsUtils;

public class ImsPhoneProxyApi {
    public static final int RESULT_NOK = 0;
    public static final int RESULT_OK = 1;
    public static final int RESULT_WOULDBLOCK = 2;

    private final int mSlotId;

    public ImsPhoneProxyApi(int slotId) {
        mSlotId = slotId;
    }

    public void setImsCallStateForMSim(int type, int state, int sysMode, int direction,
        int stateEx) {
        ImsLog.d(mSlotId, "setImsCallStateForMSim - S");

        ImsLog.d(mSlotId, "setImsCallStateForMSim - E");
    }

    public void setScmMode(int type, int mode, int emergency) {
        ImsLog.d(mSlotId, "setScmMode - S");

        ImsLog.d(mSlotId, "setScmMode - E");
    }

    public String[] updateNGetCellInfo(int networkType) {
        return null;
    }

    public static int getNrNetworkMode(int slotId, int timeout) {
        return ImsUtils.getDefaultNrNetworkMode();
    }

    public static int getNrUeCapability(int slotId, int timeout) {
        return ImsUtils.getDefaultNrUeCapability();
    }

    public static int getUeCapabilityVoNr(int slotId, int timeout) {
        return ImsUtils.getDefaultUeCapabilityVoNr();
    }
}
