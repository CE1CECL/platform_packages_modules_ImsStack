package com.android.imsstack.external.data;

public final class DataPhoneConstants {
    public static final String ACTION_VOLTE_LTE_STATE_INFO =
            "com.android.imsstack.external.data.action.VOLTE_LTE_STATE_INFO";
    public static final String LTE_UPDATE_RESULT = "extra_result";
    public static final String LTE_STATE_INFO = "stateInfo";
    public static final String LTE_DETACH_CAUSE = "extra_cause";
    public static final String EMC_FAIL_CAUSE = "failCause";
    public static final String DATA_SMCAUSE = "smcause";

    public enum EmcFailCause {
        EMC_FAIL_CAUSE(0);

        private final int mCause;

        private EmcFailCause(int cause) {
            mCause = cause;
        }

        public int getCode() {
            return mCause;
        }

        public static EmcFailCause fromInt(int cause) {
            EmcFailCause[] failCauses = EmcFailCause.values();

            for (EmcFailCause efc : failCauses) {
                if (efc.getCode() == cause) {
                    return efc;
                }
            }

            return null;
        }
    }

    public enum LteStateInfo {
        UNKNOWN(0),
        NORMAL_ATTACHED(1),
        NORMAL_DETACHED(2),
        REATTACH_REQUIRED(3),
        EPS_ONLY_ATTACHED(4),
        EMERGENCY_ATTACHED(5);

        private final int mCode;

        private LteStateInfo(int code) {
            mCode = code;
        }

        public int getCode() {
            return mCode;
        }

        public static LteStateInfo fromInt(int code) {
            LteStateInfo[] stateInfos = LteStateInfo.values();

            for (LteStateInfo lsi : stateInfos) {
                if (lsi.getCode() == code) {
                    return lsi;
                }
            }

            return null;
        }
    }
}
