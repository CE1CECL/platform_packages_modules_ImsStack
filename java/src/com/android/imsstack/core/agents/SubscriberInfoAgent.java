package com.android.imsstack.core.agents;

import com.android.imsstack.core.VoLteFactory;
import com.android.imsstack.core.agents.agentif.ISubscriberInfo;
import com.android.imsstack.core.service.serviceif.IVoLteService;

public class SubscriberInfoAgent {
    public static ISubscriberInfo getInstance(int slotID) {
        IVoLteService vs = VoLteFactory.getInstance().getService(slotID);

        if (vs == null) {
            return null;
        }

        return (ISubscriberInfo)vs.getService(IVoLteService.TYPE_SUBSCRIBEINFO);
    }
}
