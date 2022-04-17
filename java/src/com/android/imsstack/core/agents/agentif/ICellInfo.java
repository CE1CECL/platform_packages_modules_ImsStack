package com.android.imsstack.core.agents.agentif;

import android.content.Context;

public interface ICellInfo extends IAgent {

    /**
     * Returns the most recent cell information with the network type and timestamp (UTC format).
     *  [0] : network type
     *  [1] : timestamp (UTC format)
     *  [2...6] : access network information based on network type
     */
    String[] getAccessNetworkInfo();
    String[] getAccessNetworkInfo(int networkType);

    /**
     * Register and deregister listener to update Network type, Cell information, Time stamp value
     * when related event delivered
     */
    void startTrackingCellInfo(Context context);
    void stopTrackingCellInfo(Context context);
    void setLastCellInfoStorage(boolean bStore);
}
