package com.android.imsstack.util;

import com.android.imsstack.core.agents.AgentFactory;
import com.android.imsstack.core.agents.agentif.IIMSPhoneAgent;

public final class ImsPhoneUtils {
     public static void setE911CallStateForGPS(int slotId, int state, int reason) {
        IIMSPhoneAgent ipa = (IIMSPhoneAgent)AgentFactory.getAgent(
                AgentFactory.IMS_PHONE, slotId);

        if (ipa != null) {
            ipa.setE911CallStateForGPS(state, reason);
        }
    }
}
