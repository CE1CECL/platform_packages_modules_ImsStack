package com.android.imsstack.core.agents.agentif;

public interface ITelephonyState extends IAgent {
    /**
     * Returns CS call state via IMSPhone
     */
    int getCallState();

    /**
     * Returns Call state via TelephonyManager. (Multi-Sim supported also)
     */
    int getCallState(boolean fromSim);

    /**
     * Returns DataState via TelephonyManager.
     */
    int getDataState();

    /**
     * Returns SimState via TelephonyManager. (Multi-Sim supported also)
     */
    int getSimState();

    /**
     * Returns NetworkType via TelephonyManager.
     */
    int getNetworkType();

    /**
     * Returns Voice NetworkType via TelephonyManager.
     */
    int getVoiceNetworkType();

}
