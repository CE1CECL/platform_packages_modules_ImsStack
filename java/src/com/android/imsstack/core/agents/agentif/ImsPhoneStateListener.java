package com.android.imsstack.core.agents.agentif;

import android.telephony.CellInfo;
import android.telephony.PreciseCallState;
import android.telephony.PreciseDataConnectionState;
import android.telephony.ServiceState;
import android.telephony.SignalStrength;

import com.android.imsstack.core.agents.dcmif.EApnType;

import java.util.List;

public class ImsPhoneStateListener {
    public static final int LISTEN_NONE = 0;
    public static final int LISTEN_SERVICE_STATE = 0x00000001;
    public static final int LISTEN_CALL_STATE = 0x00000002;
    public static final int LISTEN_PRECISE_CALL_STATE = 0x00000004;
    public static final int LISTEN_SRVCC_STATE = 0x00000008;
    public static final int LISTEN_CELL_INFO = 0x00000010;
    public static final int LISTEN_SIGNAL_STRENGTHS = 0x00000020;
    public static final int LISTEN_PCSCF_ADDRESS_INFO = 0x00000040;
    public static final int LISTEN_PRECISE_DATA_CONNECTION_STATE = 0x00000080;

    /**
     * Invokes when service state is changed.
     */
    public void onServiceStateChanged(ServiceState serviceState) {
        // no-op
    }

    /**
     * Invokes when call state is changed.
     */
    public void onCallStateChanged(int state, String incomingNumber) {
        // no-op
    }

    /**
     * Invokes when precise call state is changed.
     */
    public void onPreciseCallStateChanged(PreciseCallState callState) {
        // no-op
    }

    /**
     * Invokes when SRVCC state is changed.
     */
    public void onSrvccStateChanged(int state) {
        // no-op
    }

    /**
     * Invokes when cell info. is changed.
     */
    public void onCellInfoChanged(List<CellInfo> cellInfo) {
        // no-op
    }

    /**
     * Invokes when signal strengths is changed.
     */
    public void onSignalStrengthsChanged(SignalStrength signalStrength) {
        // no-op
    }

    /**
     * Invokes when pdn handover info is changed.
     */
    public void onPcscfUpdated(List<String> pcscf) {
        // no-op
    }

    /**
     * Invokes when precise data connection state is changed.
     */
    public void onPreciseDataConnectionStateChanged(
            PreciseDataConnectionState dataConnectionState)  {
        // no-op
    }

    protected static boolean isApnTypeIms(String apnSettingType) {
        int apnType = EApnType.getTypeFromApnSettingType(apnSettingType);
        return EApnType.IMS.getType() == apnType;
    }
}
