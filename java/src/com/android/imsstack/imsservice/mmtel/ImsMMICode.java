/*
    Author
    <table>
    date        author                     description
    --------    -----------------------    ----------
    20170628    kaushal.upadyay@           Created
    </table>

    Description
*/

package com.android.imsstack.imsservice.mmtel;

public class ImsMMICode{
    public static final int ACTION_NONE = -1;
    public static final int ACTION_DEACTIVATE = 0;
    public static final int ACTION_ACTIVATE = 1;
    public static final int ACTION_INTERROGATE = 2;
    public static final int ACTION_REGISTER = 3;
    public static final int ACTION_ERASURE = 4;
    public static final int CATEGORY_NONE = -1;
    public static final int CATEGORY_CF = 0;  // call f/w
    public static final int CATEGORY_CB = 1;  // call bar
    public static final int CATEGORY_CW = 2;  // call waiting
    public static final int CATEGORY_OIP = 3;
    public static final int CATEGORY_OIR = 4;
    public static final int CATEGORY_TIP = 5;
    public static final int CATEGORY_TIR = 6;
    public static final int CATEGORY_MAX = 7;

  // TS 22.030 6.5.2 Every Short String USSD command will end with #-key
        // (known as #-String)
    private static final char END_OF_USSD_COMMAND = '#';
  // From TS 22.030 6.5.2
    private static final String CODE_ACTIVATE = "*";
    private static final String CODE_DEACTIVATE = "#";
    private static final String CODE_INTERROGATE = "*#";
    private static final String CODE_REGISTER = "**";
    private static final String CODE_ERASURE = "##";

    private static final String ACTION_CODE [] = {CODE_DEACTIVATE, CODE_ACTIVATE, CODE_INTERROGATE ,
                                                  CODE_REGISTER, CODE_ERASURE};

    // Call Forwarding
    private static final String SC_CFU = "21";
    private static final String SC_CFB = "67";
    private static final String SC_CFNRY = "61";
    private static final String SC_CFNR = "62";
    // Call Forwarding unconditional Timer
    private static final String SC_CFUT     = "22";
    private static final String SC_CF_All = "002";
    private static final String SC_CF_All_CONDITIONAL = "004";

    // Call Waiting
    private static final String SC_WAIT     = "43";

    // Call Barring
    //TODO : Map call barring properly
    private static final String SC_BAOC         = "33";
    private static final String SC_BAOIC        = "331";
    private static final String SC_BAOICxH      = "332";
    private static final String SC_BAIC         = "35";
    private static final String SC_BAICr        = "351";
    private static final String SC_BA_ALL       = "330";
    private static final String SC_BA_MO        = "333";
    private static final String SC_BA_MT        = "353";

    // Incoming/Anonymous call barring
    private static final String SC_BS_MT        = "156";
    private static final String SC_BAICa        = "157";

    //Called line presentation
    private static final String SC_CLIP    = "30";
    private static final String SC_CLIR    = "31";
    private static final String SC_COLP    = "76";
    private static final String SC_COLR    = "77";

    private static final String SERVICE_CODE[][] = {
      {SC_CFU, SC_CFB, SC_CFNRY, SC_CFNR, SC_CF_All, SC_CF_All_CONDITIONAL},
      {null, SC_BAIC, SC_BAOC, SC_BAOIC, SC_BAOICxH, SC_BAICr, SC_BAICa, SC_BA_ALL, SC_BA_MO,
       SC_BA_MT, SC_BS_MT},
      {SC_WAIT},
      {SC_CLIP},
      {SC_CLIR},
      {SC_COLP},
      {SC_COLR}
    };

    private int mAction;
    private int mServiceClass;
    private int mCondition;
    private String mNumber = null;
    private int mTimeSeconds = 0;

    public ImsMMICode(int action, int serviceClass, int condition) {
        mAction = action;
        mServiceClass = serviceClass;
        mCondition = condition;
    }

    public ImsMMICode(int action, int serviceClass, int condition, String number, int timeSeconds) {
        this(action, serviceClass, condition);
        mNumber = number;
        mTimeSeconds = timeSeconds;
    }

    public String getMMIString() {
        String mmiCode = getActionCode(mAction) + getServiceCode(mServiceClass, mCondition);
        if (mNumber != null){
            mmiCode += "*" + mNumber;
        }
        if (mTimeSeconds != 0){
            mmiCode += "*" + Integer.toString(mTimeSeconds);
        }
        mmiCode += END_OF_USSD_COMMAND;
        return mmiCode;
    }

    private static String getServiceCode(int category, int condition){
        if ((category > CATEGORY_NONE && category < CATEGORY_MAX) &&
                (condition >= 0 && condition < SERVICE_CODE[category].length)){
            return SERVICE_CODE[category][condition];
        }
        return null;
    }

    private static String getActionCode(int action){
        if(action >=0 && action < ACTION_CODE.length){
            return ACTION_CODE[action];
        }
        return null;
    }
}
