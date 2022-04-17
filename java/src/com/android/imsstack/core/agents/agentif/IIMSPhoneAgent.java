package com.android.imsstack.core.agents.agentif;

import android.content.Context;
import android.os.Handler;
import android.telephony.CellInfo;

import java.util.List;

public interface IIMSPhoneAgent extends IAgent {
    void registerForImsPhoneConnectionRestarted(Handler h, int what, Object obj);
    void unregisterForImsPhoneConnectionRestarted(Handler h);
    void registerForLteStateChanged(Handler h, int what, Object obj);
    void unregisterForLteStateChanged(Handler h);
    void registerForModemECCPriority(Handler h, int what, Object obj);
    void unregisterForModemECCPriority(Handler h);
    void registerForUSATEnvelopeResponse(Handler h, int what, Object obj);
    void unregisterForUSATEnvelopeResponse(Handler h);
    void registerForWcdmaVoPSChanged(Handler h, int what, Object obj);
    void unregisterForWcdmaVoPSChanged(Handler h);
    void registerForImsEmcSupportChanged(Handler h, int what, Object obj);
    void unregisterForImsEmcSupportChanged(Handler h);
    void registerForAcBarringForEmergencyChanged(Handler h, int what, Object obj);
    void unregisterForAcBarringForEmergencyChanged(Handler h);

    List<CellInfo> getAllCellInfo();
    boolean isEmergencyAttachSupported();
    boolean isEmergencyBearerServiceSupported();
    boolean isImsVoiceCallSupported();
    int getPlmn2FromSIB1();
    String getApn(String apnType);
    int getCSCallState();
    int getCSCallStateInOtherSlot();
    void getEccPriority();
    int getSignalStrength();
    void sendEnvelopeMessage(int id, String userinfo);

    void setE911CallStateForGPS(int state, int reason);
    void setPeerSimSuspend(boolean enable);

    void setTrm(int type);
    int getPsiRecord();
    void clearNetworkInfo();
    void requestNetworkInfo();
    void sendEnvelope(String data);
    void setImsRegistrationStatus(int state, int services, int detailState,
            int systemMode, int reason);
    void setImsCallStatus(int wholeCallState, int id, int state, int reason,
            int type, int systemMode, int direction, int wholeCallStateEx,
            int emergency);
    void setScmMode(int type, int state, int emergency);

    void updateImsConfig();
}
