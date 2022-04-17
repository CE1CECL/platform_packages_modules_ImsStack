package com.android.imsstack.core.agents.agentif;

import android.os.Handler;

import com.android.imsstack.core.service.serviceif.IService;

public interface IRegiProcess extends IService {
    public static final int REGSTATE_IDLE = 0;
    public static final int REGSTATE_REGISTERING = 1;
    public static final int REGSTATE_WIFI_REGISTERED = 2;
    public static final int REGSTATE_LTE_REGISTERED = 3;
    public static final int REGSTATE_FAILED = 4;

    public static final int NETWORK_MODE_INVALID = -1;

    void registerForRegServiceChanged(Handler h, int what, Object obj);
    void unregisterForRegServiceChanged(Handler h);
    void registerForRegStateChanged(Handler h, int what, Object obj);
    void unregisterForRegStateChanged(Handler h);
    int getChangedNetworkMode();
    int[] getRegistration();
    int getRegServiceState();
    int getRegServiceStateForWiFi();
    int getVoipRegState();
    void updateCurrentRegService(int type);
    void setAndUpdateRegService(int regServiceState, int type);
    void setBlockRegStateNotification (boolean blockRegStateNoti);
    boolean isRegistered();
    void setChangedNetworkMode(int networkMode);
    boolean isHandleNetworkModeRequired();
    void setHandleNetworkModeRequired(boolean handleNetMode);
    void handleChangeNetworkMode();
    void sendRegStateIntentForSPR(int state, int reason);
    /**
     * Updates the registration status based on the PDN control.
     * By this change, the updated states will be sent to the modem.
     * If detailState is IMS_REGISTRATION_INVALID, it needs to be ignored.
     */
    void updateRegistrationStatus(int reason, int detailState);
    void updateCurrentRegistration();
}
