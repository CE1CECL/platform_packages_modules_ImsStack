
package com.android.imsstack.test;

public final class SipInfo {
    // SIP message type (Request / Response)
    public static final int MSG_REQ = 0;
    public static final int MSG_RSP = 1;

    // SIP message direction (OUT / IN)
    public static final int DIR_OUT = 0;
    public static final int DIR_IN = 1;

    // SIP method
    //public static final int M_ACK = 0;
    public static final int M_BYE = 1;
    //public static final int M_CANCEL = 2;
    public static final int M_INVITE = 3;
    //public static final int M_OPTIONS = 4;
    public static final int M_REGISTER = 5;
    //public static final int M_PRACK = 6;
    //public static final int M_SUBSCRIBE = 7;
    //public static final int M_NOTIFY = 8;
    //public static final int M_UPDATE = 9;
    //public static final int M_MESSAGE = 10;
    //public static final int M_REFER = 11;
    //public static final int M_PUBLISH = 12;
    //public static final int M_INFO = 13;
    public static final int M_MAX = 15;

    public static final String getMethod(int method) {
        switch (method) {
            case M_BYE:
                return "BYE";
            case M_INVITE:
                return "INVITE";
            case M_REGISTER:
                return "REGISTER";
            default:
                return "Unknown";
        }
    }
}
