package com.android.imsstack.core.agents.agentif;

public interface ILocationAgentManager extends IVoLteAgent {
    /**
     * Returns the location agent if it exists.
     */
    public ILocationAgent getAgent(int slotId);
}
