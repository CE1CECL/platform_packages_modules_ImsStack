package com.android.imsstack.core.agents.agentif;

import android.os.Handler;

public interface IAlarmTimer extends IAgent {
    /**
     * Return unique timer id which is used for identify alarm timer. (Max value = 0xFFFF)
     */
    int getTimerId();

    /**
     * Register listener to receive alarm timer event
     */
    void registerForTimerExpired(long tid, Handler h, int what, Object obj);

    /**
     * De-register alarm timer listener
     */
    void unregisterForTimerExpired(long tid, Handler h);

    /**
     * Start alarm timer with duration. This API is used for java layer timer event.
     * Please register listener before start timer.
     */
    boolean startTimer(long tid, long duration);

    /**
     * Stop alarm timer before timer expired. This API is used for java layer timer event.
     * Please de-register listener after stop timer.
     */
    void stopTimer(long tid);
}
