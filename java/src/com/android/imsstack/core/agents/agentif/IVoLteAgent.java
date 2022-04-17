package com.android.imsstack.core.agents.agentif;

public interface IVoLteAgent {
    void cleanup();
    void start(int slotId);
    void stop(int slotId);
}
