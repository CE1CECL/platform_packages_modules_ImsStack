package com.android.imsstack.core.agents.agentif;

import android.os.Handler;

public interface ISharedState extends IAgent {
    public boolean isNativeBootCompleted();

    public void registerForNativeBootComplete(Handler h, int what, Object obj);
    public void unregisterForNativeBootComplete(Handler h);
}
