package com.android.imsstack.core.agents.agentif;

public interface IPhoneStateNotifier {
    /**
     * Returns the current events to be monitored.
     */
    public int getEvents();

    /**
     * Sets the events which are interested in.
     */
    public void setEvents(int events);

    /**
     * Sets the listener to get the events.
     */
    public void setListener(ImsPhoneStateListener listener);
}
