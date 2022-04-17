package com.android.imsstack.enabler.ssc.data;

import com.android.imsstack.enabler.ssc.ESsType;

import java.util.ArrayList;

public class CbServiceData extends SscServiceData {
    public ArrayList<SscRuleData> mRuleSet = null;

    public CbServiceData(int slotId, ESsType sstype, int eventNum, int transactionId, int state,
            int condition, ArrayList<SscRuleData> ruleSet) {
        super(slotId, sstype, eventNum, transactionId, state);
        mCondition = condition;
        mRuleSet = ruleSet;
    }

    public void setRuleSet(ArrayList<SscRuleData> ruleSet) {
        this.mRuleSet = ruleSet;
    }

    public ArrayList<SscRuleData> getRuleSet() {
        return this.mRuleSet;
    }

    public String toString() {
        StringBuffer sb = new StringBuffer();
        if (mRuleSet != null) {
            for (SscRuleData ruleData : mRuleSet) {
                if ((ruleData.getRuleId() == null) || ruleData.getRuleId().isEmpty()) {
                    continue;
                }
                sb.append(ruleData.toString());
            }
        }
        return super.toString() + ", IsEnabled : " + mServiceState + ", mCondition : " + mCondition;
    }
}
