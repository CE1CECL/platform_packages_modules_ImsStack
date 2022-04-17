package com.android.imsstack.test.menu;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.widget.TextView;

import com.android.imsstack.R;
import com.android.imsstack.core.VoLteFactory;
import com.android.imsstack.core.agents.agentif.ILocationAgent;
import com.android.imsstack.core.agents.agentif.ILocationAgentManager;
import com.android.imsstack.util.AppContext;
import com.android.imsstack.util.ImsLog;
import com.android.imsstack.util.MSimUtils;

public class LocationDebugScreen extends Activity {
    // Constants--------------------------------------------------
    private static final int EVENT_LOCATION_PERIODIC_QUERY = 1001;

    private static final int LOCATION_QUERY_INTERVAL = 3000; // 3 secs
    private static final int LOCATION_QUERY_MAX_CNT = 5;

    // Variables--------------------------------------------------
    private TextView mLocationInfoView;

    private int mLocationQueryCount = 0;
    private ILocationAgent mLocationAgent = null;

    protected LocationDebugScreenHandler mLocationDebugScreenHandler =
            new LocationDebugScreenHandler();

    // Static loading materials ----------------------------------
    // Public methods --------------------------------------------
    public LocationDebugScreen() {
        ImsLog.d("");
    }

    // Interface implementation methods --------------------------
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        ImsLog.d("");

        init();

        mLocationDebugScreenHandler.sendEmptyMessageDelayed(EVENT_LOCATION_PERIODIC_QUERY,
                LOCATION_QUERY_INTERVAL);
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    public void init() {
        ImsLog.d("");

        setContentView(R.xml.locationdebugscreen);
        setTitle(R.string.ims_label_location_debug_screen);

        mLocationInfoView = (TextView)findViewById(R.id.location_info);

        ILocationAgentManager lam =(ILocationAgentManager)VoLteFactory.getInstance().getAgent(
                VoLteFactory.AGENT_LOCATION_AGENT_MANAGER);
        mLocationAgent = (lam != null) ? lam.getAgent(MSimUtils.DEFAULT_SLOT_ID) : null;

        if (mLocationAgent != null) {
            mLocationAgent.makeInstantLocationUpdates4Sys();
        }
    }

    // Private/Protected methods ---------------------------------
    private void updateLocation() {
        ImsLog.d("");

        String[] locationInfoArray = null;
        String locationInfo = null;

        if (mLocationAgent != null) {
            locationInfoArray = mLocationAgent.getLastKnownLocation(ILocationAgent.LOCATION_ALL);

            if (locationInfoArray != null) {
                locationInfo = mLocationQueryCount + " try = " + java.util.Arrays.toString(locationInfoArray);

                mLocationInfoView.setText(locationInfo);
            } else {
                if (mLocationQueryCount < LOCATION_QUERY_MAX_CNT) {
                    ImsLog.d("Waiting location acqusition, cnt=" + mLocationQueryCount);
                    mLocationQueryCount++;

                    mLocationDebugScreenHandler.sendEmptyMessageDelayed(
                            EVENT_LOCATION_PERIODIC_QUERY, LOCATION_QUERY_INTERVAL);
                }
            }
        }
    }


    // -----------------------------------------------------------
    private class LocationDebugScreenHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            if (msg == null) {
                ImsLog.e("msg is null");
                return;
            }

            ImsLog.d("message[" + msg.what + "]");

            switch (msg.what) {
                case EVENT_LOCATION_PERIODIC_QUERY:
                    updateLocation();
                    break;
                default :
                    ImsLog.w("Invalid Message");
                    break;
            }
        }
    }
}
