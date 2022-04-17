package com.android.imsstack.enabler.mtc.dialogs;

import com.android.imsstack.enabler.mtc.IUMtcService;

public class IUDialogs {

    public static final int EVENT_U2I        = IUMtcService.EVENT_U2I + 60;
    public static final int EVENT_I2U        = IUMtcService.EVENT_I2U + 60;

    // Event : UI to IMS

    // Event : IMS to UI
    public static final int NOTIFY_DIALOG_INFO      = (EVENT_I2U + 1);

};
