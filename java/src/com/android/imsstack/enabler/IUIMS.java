/*
    Author
    <table>
    date      author                description
    --------  --------------        ----------
    20110423  sukhwan.mun@           Created
    </table>

    Description

*/
package com.android.imsstack.enabler;

public class IUIMS
{
    public static final int APP_BASE = 0;
    public static final int APP_MTC = 1;
    public static final int APP_MTS = 2;
    public static final int APP_UCE = 3;
    public static final int APP_SIP_DELEGATE = 4;
    public static final int APP_MAX = 5;
    public static final int APP_UNKNOWN = APP_MAX + 1;

    // System interface
    public static final int SYSTEM_INTERFACE = 51;

    // AOS
    public static final int AOS_SERVICE = 61;

    // MTC
    public static final int MTC_CALL = 91;

    public static final int MTS_BASE = 400;
    public static final int MTS_EMERGENCY_SERVICE = (MTS_BASE + 1);

    //// Bitmask to composite each services
    public static final int M_APP_UC = 0x00000001;
    public static final int M_APP_SMS = 0x00000002;
    public static final int M_APP_VT = 0x00000004;
    public static final int M_SERVICE_ALL = 0xFFFFFFFF;
}
