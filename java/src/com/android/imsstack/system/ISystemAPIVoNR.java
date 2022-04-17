package com.android.imsstack.system;

public interface ISystemAPIVoNR {
    int notifyCallState(int type, int state, int sysMode, int direction);
    int requestCallPreference(int rat, int type);
    int setImsSession(int type, int state);
    int setImsSignalingForUAC(int type);
    int setImsVoice(int state, int sysMode);
    int setUacCheck(int type, int state);
    int setVoice(int state, int emergency);
}
