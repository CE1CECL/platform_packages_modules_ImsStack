package com.android.imsstack.core.agents.agentif;

import android.content.Context;

public interface IAgent {
    void init(Context context);
    void cleanup();
}
