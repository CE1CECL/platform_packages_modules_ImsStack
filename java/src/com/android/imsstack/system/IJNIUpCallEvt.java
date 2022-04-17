/*
    Author
    <table>
    date      author                      description
    --------  --------------              ----------
    20131201  joonhun.shin@        Created
    </table>

    Description
 */

package com.android.imsstack.system;

import android.os.Handler;

import com.android.imsstack.core.agents.agentif.ICallInfoService;
import com.android.imsstack.core.agents.agentif.ICallStateNotificationService;


public interface IJNIUpCallEvt {
    /**
     * return slot id
     */
    public int getSlotId();

    // for synchronized operations.
    public void setInterfaceForCallInfoService(ICallInfoService iface);
    public void setInterfaceForCallStateNotificationService(ICallStateNotificationService iface);

    public void registerForNativeBootComplete(Handler h, int what, Object obj);
    public void unregisterForNativeBootComplete(Handler h);

    public void registerForRegiStateChanged(Handler h, int what, Object obj);
    public void unregisterForRegiStateChanged(Handler h);

    public void registerForDebugService(Handler h, int what, Object obj);
    public void unregisterForDebugService(Handler h);

    public void registerForDebugAWTChanged(Handler h, int what, Object obj);
    public void unregisterForDebugAWTChanged(Handler h);

    public void registerForVoLTEIndicatorChanged(Handler h, int what, Object obj);
    public void unregisterForVoLTEIndicatorChanged(Handler h);

    public void registerForVoWIFIIndicatorChanged(Handler h, int what, Object obj);
    public void unregisterForVoWIFIIndicatorChanged(Handler h);

    public void registerForTraceMOCA(Handler h, int what, Object obj);
    public void unregisterForTraceMOCA(Handler h);

    public void registerForShowNotProvisionedNoti(Handler h, int what, Object obj);
    public void unregisterForShowNotProvisionedNoti(Handler h);

    public void registerForDataConnectionChanged(Handler h, int what, Object obj);
    public void unregisterForDataConnectionChanged(Handler h);

    public void registerForISIMReadResult(Handler h, int what, Object obj);
    public void unregisterForISIMReadResult(Handler h);

    public void registerForRegiReportToWFC(Handler h, int what, Object obj);
    public void unregisterForRegiReportToWFC(Handler h);

    public void registerForRegServiceChanged(Handler h, int what, Object obj);
    public void unregisterForRegServiceChanged(Handler h);

    public void registerForRegFailureChanged(Handler h, int what, Object obj);
    public void unregisterForRegFailureChanged(Handler h);

    public void registerForDCNChanged(Handler h, int what, Object obj);
    public void unregisterForDCNChanged(Handler h);

    public void registerForObtainPhoneNumber(Handler h, int what, Object obj);
    public void unregisterForObtainPhoneNumber(Handler h);

    public void registerForMLTMessageChanged(Handler h, int what, Object obj);
    public void unregisterForMLTMessageChanged(Handler h);

    public void registerForSetPdnPreferenceToEpdg(Handler h, int what, Object obj);
    public void unregisterForSetPdnPreferenceToEpdg(Handler h);

    public void registerForSetServiceStatusToEpdg(Handler h, int what, Object obj);
    public void unregisterForSetServiceStatusToEpdg(Handler h);

    public void registerForReportBadNetworkToEpdg(Handler h, int what, Object obj);
    public void unregisterReportBadNetworkToEpdg(Handler h);

    public void registerForRegDestroyed(Handler h, int what, Object obj);
    public void unregisterForRegDestroyed(Handler h);

    public void registerForRegNotifyState(Handler h, int what, Object obj);
    public void unregisterForRegNotifyState(Handler h);

    public void registerForSendDataToModem(Handler h, int what, Object obj);
    public void unregisterForSendDataToModem(Handler h);

    public void registerForSendSCMToModem(Handler h, int what, Object obj);
    public void unregisterForSendSCMToModem(Handler h);

    public void registerForRegistrationChanged(Handler h, int what, Object obj);
    public void unregisterForRegistrationChanged(Handler h);

    public void registerForCallInfo(Handler h, int what, Object obj);
    public void unregisterForCallInfo(Handler h);

    public void registerForCallMediaInfo(Handler h, int what, Object obj);
    public void unregisterForCallMediaInfo(Handler h);

    public void registerForSCMState(Handler h, int what, Object obj);
    public void unregisterForSCMState(Handler h);
}
