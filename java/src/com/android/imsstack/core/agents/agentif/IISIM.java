package com.android.imsstack.core.agents.agentif;

public interface IISIM extends IAgent {
    public static final String STATE_NOT_PRESENT = "NOT_PRESENT";
    public static final String STATE_NOT_READY = "NOT_READY";
    public static final String STATE_LOADED = "LOADED";
    public static final String STATE_REFRESH_STARTED = "REFRESH_STARTED";
    public static final String STATE_REFRESH_COMPLETED = "REFRESH_COMPLETED";

    /**
     * Return IMPI value from TelephonyManager. (This method support multi-sim device also)
     */
    String getImpi();

    /**
     * Return Domain value from TelephonyManager. (This method support multi-sim device also)
     */
    String getDomain();

    /**
     * Return IMPU values from TelephonyManager. (This method support multi-sim device also)
     */
    String[] getImpu();

    /**
     * Return Ist value from TelephonyManager. (This method support multi-sim device also)
     */
    String getIst();

    /**
     * Return PCSCF values from TelephonyManager. (This method support multi-sim device also)
     */
    String[] getPcscf();

    /**
     * Return response values from TelephonyManager. (This method support multi-sim device also)
     * This response value will be applied to message which send to server for ISIM AKA auth.
     */
    String getChallengeResponse(String nonce);

    /**
     * Check if GBA is supported.
     */
    boolean isGbaSupported();
}
