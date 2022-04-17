package com.android.imsstack.core.agents.dcmif;

import android.telephony.TelephonyManager;

public enum EDataState {

    // Enumerations ----------------------------------------------
    DATA_STATE_DISCONNECTED     (0),
    DATA_STATE_CONNECTED        (2),
    DATA_STATE_CONNECT_FAILED   (4),
    DATA_STATE_IP_CHANGED       (5),
    DATA_STATE_PCSCF_CHANGED    (6);

    // Variables--------------------------------------------------
    private final int mState;

    // Private/Protected methods ---------------------------------
    private EDataState(int mState) {
        this.mState = mState;
    }

    // Public methods --------------------------------------------
     /**
     * convertToDataType
     *
     */
    public static int convertFromTMtoImsType(int nTelephonyMngrType) {
        if (nTelephonyMngrType == TelephonyManager.DATA_CONNECTED
        || nTelephonyMngrType == TelephonyManager.DATA_SUSPENDED) {

            return DATA_STATE_CONNECTED.getState();
        }

        return DATA_STATE_DISCONNECTED.getState();
    }

    public static int convertFromImsTypeToTM(EDataState dataState) {
        if (dataState == EDataState.DATA_STATE_CONNECTED) {
            return TelephonyManager.DATA_CONNECTED;
        }
        return TelephonyManager.DATA_DISCONNECTED;

    }

    public static EDataState convertIntTypeToEnum(int nDataState) {

        if (DATA_STATE_CONNECTED.getState() == nDataState) {
            return EDataState.DATA_STATE_CONNECTED;
        } else if (DATA_STATE_DISCONNECTED.getState() == nDataState) {
            return EDataState.DATA_STATE_DISCONNECTED;
        } else if (DATA_STATE_CONNECT_FAILED.getState() == nDataState) {
            return EDataState.DATA_STATE_CONNECT_FAILED;
        } else if (DATA_STATE_IP_CHANGED.getState() == nDataState) {
            return EDataState.DATA_STATE_IP_CHANGED;
        }

        return EDataState.DATA_STATE_DISCONNECTED;
    }

    public int getState() {
        return this.mState;
    }
}
