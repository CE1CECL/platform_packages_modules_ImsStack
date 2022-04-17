/**
 * IMSSettings
 * Role
 *         Setting menu factory which is requested by operator.
 *         Send intent to target setting menu based on Operator & Country Information in java layer.
 */

package com.android.imsstack.test.menu;

import android.app.Activity;
import android.view.View;
import android.widget.ListView;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;

import com.android.imsstack.R;
import com.android.imsstack.core.OperatorInfo;
import com.android.imsstack.util.ImsLog;
import com.android.imsstack.util.MSimUtils;

import java.util.ArrayList;

public class SettingSlot {

    private ArrayList<String> mSlotList = null;
    private ArrayAdapter<String> mSlotListAdapter = null;
    private ListView mSlotListView = null;

    private Activity mActivity = null;
    private SlotListener mListener = null;

    public SettingSlot(Activity activity) {
        ImsLog.d("");

        mActivity = activity;

        if (mActivity == null) {
            ImsLog.e("Activity is null");
        }
    }

    public SettingSlot(Activity activity, ListView listView) {
        ImsLog.d("");

        mActivity = activity;
        mSlotListView = listView;

        if (mActivity == null || mSlotListView == null) {
            ImsLog.e("Activity/listView is null");
        }
    }

    public static class SlotListener {
        public void onSlot(int slotID) {
            // no-op
        }
    }

    /* ---------------------------------------------------------------------------------------------
        private methods
    --------------------------------------------------------------------------------------------- */
    public void setSlotList(SettingSlot.SlotListener listener) {
        ImsLog.d("");

        mSlotList = new ArrayList<String>();
        mListener = listener;

        int nSlot = MSimUtils.getMaxSimSlot();
        for (int index = 0; index < nSlot; index++) {
            String operator = OperatorInfo.getOperator(index);
            if (!operator.isEmpty()) {
                mSlotList.add("Slot - " + index + " : " + operator + " / " + OperatorInfo.getCountry(index));
            }
        }

        mSlotListAdapter = new ArrayAdapter<String>(mActivity, android.R.layout.simple_list_item_1, mSlotList);

        if (mSlotListView == null) {
            mActivity.setContentView(R.xml.multi_slot);
            mSlotListView = (ListView)mActivity.findViewById(R.id.multi_slot_listview);

        }

        mSlotListView.setAdapter(mSlotListAdapter);
        mSlotListView.setOnItemClickListener(mSlotListener);


    }

    /* ---------------------------------------------------------------------------------------------
        Handler - OnItemClickListener
    --------------------------------------------------------------------------------------------- */
    AdapterView.OnItemClickListener mSlotListener = new AdapterView.OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
            if (mSlotList.get(position).contains("Slot - 0")) {
                mListener.onSlot(0);
            } else if (mSlotList.get(position).contains("Slot - 1")) {
                mListener.onSlot(1);
            } else {
                mListener.onSlot(0);
            }
        }
    };

}
