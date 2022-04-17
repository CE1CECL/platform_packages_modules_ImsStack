package com.android.imsstack.enabler.ssc.data;

import com.android.imsstack.enabler.ssc.ESsType;

import java.util.ArrayList;

public class CfServiceData extends SscServiceData {
    private int mNoReplyTimer = -1;
    private ArrayList<SscRuleData> mRuleSet = null;

    public CfServiceData(int slotId, ESsType sstype, int eventNum, int transactionId, int state,
            int condition, int timer, ArrayList<SscRuleData> ruleset) {
        super(slotId, sstype, eventNum, transactionId, state);
        mCondition = condition;
        mNoReplyTimer = timer;
        mRuleSet = ruleset;
    }

    public int getNoReplyTimer() {
        return mNoReplyTimer;
    }

    public void setNoReplyTimer(int timer) {
        this.mNoReplyTimer = timer;
    }

    public ArrayList<SscRuleData> getRuleSet() {
        return mRuleSet;
    }

    public String toString() {
        StringBuffer sb = new StringBuffer();
        if (mRuleSet != null) {
            for(SscRuleData ruleData : mRuleSet) {
                if ((ruleData.getRuleId() == null) || ruleData.getRuleId().isEmpty()) {
                    continue;
                }
                sb.append(ruleData.toString());
            }
        }

        return "\ncommunication-diversion : " + mServiceState + "\n" + "NoReplyTimer : "
                + mNoReplyTimer + "\n" + sb;
    }
}
