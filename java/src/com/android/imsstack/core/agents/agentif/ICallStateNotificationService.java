package com.android.imsstack.core.agents.agentif;

public interface ICallStateNotificationService {
    /*
      set Call State to modem
    */
    void setCallState(int key, int param);
}
