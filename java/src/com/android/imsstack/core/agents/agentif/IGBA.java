package com.android.imsstack.core.agents.agentif;

import android.util.Pair;

public interface IGBA extends IAgent {
    public Pair<String, String> getGbaKey(int appType, int gbaMode, boolean isTls, String nafFqdn,
            String securityProtocol, boolean isForced);
}
