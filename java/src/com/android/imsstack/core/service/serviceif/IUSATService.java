package com.android.imsstack.core.service.serviceif;

public interface IUSATService extends IService {

    public static final int INVALID_ID = -1;

    public static final int DISCONNECT_REASON_UNKNOWN = 0;
    public static final int DISCONNECT_REASON_USER = 1;
    public static final int DISCONNECT_REASON_NW = 2;
    public static final int DISCONNECT_REASON_HOLD_FAIL = 3;
    public static final int DISCONNECT_REASON_TERMINATED_FAIL = 4;

    public interface USATListener {
        void onNotifyCallAllowed(int response, String modifiedInfo);
    }

    void sendActiveCallStateToUICC(int reason);
    boolean isUICCSetupCall();
    boolean isUSATSupported();
    int isCallAllowedByUSAT(String targetNumber, USATListener listener);
    void abortTransaction(int transactionId);
}
