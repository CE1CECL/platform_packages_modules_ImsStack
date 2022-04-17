package com.android.imsstack.core.agents;

import android.content.Context;

import com.android.imsstack.core.agents.agentif.ILocationAgent;
import com.android.imsstack.core.agents.agentif.ILocationAgentManager;
import com.android.imsstack.core.agents.agentif.IVoLteAgent;
import com.android.imsstack.util.MSimUtils;

public final class LocationAgentManager implements ILocationAgentManager {
    private final Context mContext;
    private LocationAgent[] mAgents = new LocationAgent[MSimUtils.getMaxSimSlot()];

    public LocationAgentManager(Context context) {
        mContext = context;
    }

    @Override
    public void cleanup() {
        synchronized (mAgents) {
            for (int i = 0; i < mAgents.length; i++) {
                if (mAgents[i] != null) {
                    mAgents[i].dispose();
                    mAgents[i] = null;
                }
            }
        }
    }

    @Override
    public void start(int slotId) {
        // LocationAgent will be instantiated by init(...) method.
    }

    @Override
    public void stop(int slotId) {
        if (slotId < 0 || slotId >= mAgents.length) {
            return;
        }

        synchronized (mAgents) {
            if (mAgents[slotId] != null) {
                mAgents[slotId].dispose();
                mAgents[slotId] = null;
            }
        }
    }

    @Override
    public ILocationAgent getAgent(int slotId) {
        if (slotId < 0 || slotId >= mAgents.length) {
            return null;
        }

        synchronized (mAgents) {
            if (mAgents[slotId] == null) {
                mAgents[slotId] = new LocationAgent(mContext, slotId);
            }

            return mAgents[slotId];
        }
    }
}
