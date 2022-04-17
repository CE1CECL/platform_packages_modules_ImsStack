/*
    Author
    <table>
    date        author                  description
    --------    --------------          ----------
    20141129    hwangoo.park@           Created
    </table>

    Description
*/

package com.android.imsstack.enabler.mtc.reg;

public class ImsServiceState {
    // IUIMS::APP_XXX
    public int mApp = (-1);
    // IUMtcService.SERVICE_XXX
    public int mServiceType = 0;
    // Additional information for each service types
    // As of now, it's only for SERVICE_EMERGENCY
    public int mExtraState = (-1);
    // Reason code if the service is not available
    public int mReason = 0;

    public ImsServiceState() {
    }

    public ImsServiceState(int app, int serviceType) {
        mApp = app;
        mServiceType = serviceType;
    }

    public ImsServiceState(int app, int serviceType, int extraState, int reason) {
        this(app, serviceType);

        mExtraState = extraState;
        mReason = reason;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();

        sb.append("[ ImsServiceState: app=");
        sb.append(mApp);
        sb.append(", serviceType=");
        sb.append(mServiceType);
        sb.append(", extraState=");
        sb.append(mExtraState);
        sb.append(", reason=");
        sb.append(mReason);
        sb.append(" ]");

        return sb.toString();
    }
}
