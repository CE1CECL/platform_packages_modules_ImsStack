package com.android.imsstack.test;

/**
 * IMS Test Mask
 */
public final class ImsTestMask {
    /**
     * Bitmask: use in java layer for test (ImsTestMode.getExtraTestmask())
     */

    public static final int TEST_MASK_NONE = 0x0000;
    public static final int TEST_MASK_IGNORE_IMPU_VALIDATION = 0x0001;
    public static final int TEST_MASK_ROAMING_CONDITION = 0x0002;
    public static final int TEST_MASK_VOLTE_INDICATOR = 0x0004;
    public static final int TEST_MASK_IMS_STATUS_TO_UICC_OFF = 0x0008;
    public static final int TEST_MASK_NV_SYNC_OFF = 0x0010;
    public static final int TEST_MASK_DISABLE_2SEC_EREG_RETRY = 0x0020;
    public static final int TEST_MASK_HVOLTE_TEST_PLAN = 0x0040;
    public static final int TEST_MASK_IGNORE_FORBIDDEN_RESPONSE = 0x0080;
    public static final int TEST_MASK_UNLOCK_EVS_NEGO_LIMIT = 0x0100;
}
