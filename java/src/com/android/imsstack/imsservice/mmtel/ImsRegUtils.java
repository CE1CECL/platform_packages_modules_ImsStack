/*
    Author
    <table>
    date        author                  description
    --------    --------------          ----------
    20151209    hwangoo.park@           Created
    </table>

    Description
*/

package com.android.imsstack.imsservice.mmtel;

import android.content.Context;
import android.telephony.TelephonyManager;

import com.android.imsstack.core.agents.AgentFactory;
import com.android.imsstack.core.agents.agentif.IWifiState;
import com.android.imsstack.enabler.IBaseContext;
import com.android.imsstack.provider.ImsStateController;

/**
 * IMS registration related utility methods.
 */
public class ImsRegUtils {
    public static boolean isImsRegisteredOnWifi(IBaseContext context) {
        IWifiState ws = (IWifiState)AgentFactory.getAgent(AgentFactory.WIFI_STATE);
        boolean isWifiConnected = (ws != null) ? ws.isWifiConnected() : false;
        int networkType = ImsStateController.RegState.getNetworkTypeForPhoneId(
                context.getContext().getContentResolver(), context.getPhoneId());

        return isWifiConnected
                && (networkType == TelephonyManager.NETWORK_TYPE_IWLAN);
    }
}
