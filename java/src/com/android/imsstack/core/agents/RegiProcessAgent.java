package com.android.imsstack.core.agents;

import com.android.imsstack.core.VoLteFactory;
import com.android.imsstack.core.agents.agentif.IRegiProcess;
import com.android.imsstack.core.service.serviceif.IVoLteService;

public class RegiProcessAgent {
    public static IRegiProcess getInstance(int slotID) {
        IVoLteService vs = VoLteFactory.getInstance().getService(slotID);

        if (vs == null) {
            return null;
        }

        return (IRegiProcess)vs.getService(IVoLteService.TYPE_REGIPROCESS);
    }
}
