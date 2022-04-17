package com.android.imsstack.system;

import com.android.imsstack.core.agents.dcmif.IDCUtil;

import java.io.FileDescriptor;

public interface ISystemAPINetwork {

    int activateDataConnection4Sys(int apnType, int ipcanType);
    int deactivateDataConnection4Sys(int apnType, int ipcanType);

    IDCUtil.AccessNetworkInfo getAccessNetworkInfo4Sys(int defaultNetworkType);
    String getApnName4Sys(int apnType);
    int getDataConnectionState4Sys(int apnType);
    String[] getHostByName4Sys(int apnType, int ipVersion, String host);
    int getIfaceId4Sys(int apnType);
    String getIfaceName4Sys(int apnType);
    int getIpcanCategory4Sys(int apnType);
    String[] getLastAccessNetworkInfo4Sys(int networkType);
    String getLocalAddress4Sys(int apnType, int ipVersion);
    int getLteRsrpStrength4Sys();
    String[] getPcscfAddresses4Sys(int apnType, int ipVersion);
    int getRoamingState4Sys();
    int getVoiceRoamingType4Sys();
    int getDataRoamingType4Sys();
    int getServiceState4Sys();
    int getVoiceServiceState4Sys();
    int isLteEmergencyOnly4Sys();
    int isEmergencyAttachSupported4Sys();
    int sendPingToHostAddress4Sys(int apnType, String hostAddress);
    int getMocnPlmnInfo4Sys();
    boolean isMobileDataEnabled();
    int getMtu4Sys(int apnType);
    int bindSocket(int apnType, FileDescriptor sockFd);
}
