package com.android.imsstack.core.agents;

import android.content.Context;
import android.os.Handler;
import android.os.Registrant;
import android.os.RegistrantList;

import com.android.imsstack.core.agents.AgentFactory;
import com.android.imsstack.core.agents.agentif.IIMSPhoneAgent;
import com.android.imsstack.core.agents.agentif.ITRM;
import com.android.imsstack.system.ISystemAPITRM;
import com.android.imsstack.system.SystemInterface;
import com.android.imsstack.util.ImsLog;
import com.android.imsstack.util.MSimUtils;

public class TRMAgent implements ITRM, ISystemAPITRM {
    // Constants--------------------------------------------------
    public static final int SERVICE_NONE = 0;
    public static final int SERVICE_UT = 0x0001;
    public static final int SERVICE_REG = 0x0002;
    public static final int SERVICE_SMS = 0x0004;
    public static final int SERVICE_VOLTE = 0x0008;

    public static final int MODE_END = 0;
    public static final int MODE_START = 1;

    public static final int TRM_TYPE_NONE = 0;
    public static final int TRM_TYPE_VOLTE = 1;
    public static final int TRM_TYPE_SMS = 2;
    public static final int TRM_TYPE_MMS = 3;
    public static final int TRM_TYPE_REGISTRATION = 4;
    public static final int TRM_TYPE_UT = 5;

    // Variables--------------------------------------------------
    private static ITRM mTRMAgent;
    private Context mContext;
    private RegistrantList mTRMRegistrants = new RegistrantList();
    private TRM[] mTRM = null;

    // Public methods --------------------------------------------
    public TRMAgent() {
    }

    public static ITRM getInstance() {
        if (mTRMAgent == null) {
            mTRMAgent = new TRMAgent();
        }

        return mTRMAgent;
    }

    // Interface implementation methods --------------------------
    @Override
    public void init(Context context) {
        mContext = context;

        if (!isTRMSupported()) {
            return;
        }

        SystemInterface.getInstance().setISystemAPITRM(this);

        mTRM = new TRM[MSimUtils.getMaxSimSlot()];

        for (int i = 0; i < MSimUtils.getMaxSimSlot(); i++) {
            mTRM[i] = new TRM();
        }
    }

    @Override
    public void cleanup() {
        SystemInterface.getInstance().setISystemAPITRM(null);
    }

    @Override
    public boolean isServiceAvailable(int slotId, int type) {
        if (isIdle() || (getTopPrioritySlot() == slotId)) {
            return true;
        }

        if (isHighPriortyExistInOtherSlot(slotId, type)) {
            return false;
        }

        return true;
    }

    @Override
    public boolean isSlotAvailable(int slotId) {
        if (isIdle() || (getTopPrioritySlot() == slotId)) {
            return true;
        }

        return false;
    }

    @Override
    public boolean isTRMSupported( ) {
        return MSimUtils.isMultiLteEnabled();
    }

    @Override
    public void setService(int slotId, int type, int mode) {
        SystemInterface.getInstance().notifyTRMServiceChanged(type, mode, slotId);
    }

    @Override
    public void registerForServicePriorityChanged(Handler h, int what, Object obj) {
        mTRMRegistrants.add(new Registrant(h, what, obj));
    }

    @Override
    public void unregisterForServicePriorityChanged(Handler h) {
        mTRMRegistrants.remove(h);
    }

    @Override
    public int setTRM(int trmType, int slotId) {
        IIMSPhoneAgent ipa = (IIMSPhoneAgent)AgentFactory.getAgent(
                AgentFactory.IMS_PHONE, slotId);

        if (ipa != null) {
            ImsLog.i("slot = " + slotId + " , type = " + trmType);
            ipa.setTrm(trmType);

            synchronized(mTRM) {
                mTRM[slotId].setUpdatedService(getServiceType(trmType));
            }

            mTRMRegistrants.notifyResult(Integer.valueOf(slotId));
        } else {
            ImsLog.w("ipa is invalid");
        }

        return 1;
    }

    private int getServiceType(int trmType) {
        int serviceType = SERVICE_NONE;

        switch (trmType) {
            case TRM_TYPE_VOLTE:
                serviceType = SERVICE_VOLTE;
                break;

            case TRM_TYPE_SMS:
                serviceType = SERVICE_SMS;
                break;

            case TRM_TYPE_REGISTRATION:
                serviceType = SERVICE_REG;
                break;

            case TRM_TYPE_UT:
                serviceType = SERVICE_UT;
                break;

            default:
                break;
        }

        return serviceType;
    }

    // Private class   --------------------------------------------
    private class TRM {
        private int updatedService = SERVICE_NONE;

        public int getUpdatedService() {
            return updatedService;
        }

        public void setUpdatedService(int type) {
            updatedService = type;
        }
    }
    // Private methods --------------------------------------------
    private int getTopPrioritySlot() {
        // FIXME: check equal case
        int topSlot = 0;

        synchronized(mTRM) {
            for (int i = 1; i < mTRM.length; i++) {
                if (mTRM[topSlot].getUpdatedService() < mTRM[i].getUpdatedService()) {
                    topSlot = i;
                }
            }
        }

        return topSlot;
    }

    private boolean isHighPriortyExistInOtherSlot(int slotId, int type) {
        synchronized(mTRM) {
            for (int i = 0; i < mTRM.length; i++) {
                if (i == slotId) {
                    continue;
                }

                if (mTRM[i].getUpdatedService() >= type) {
                    return true;
                }
            }
        }

        return false;
    }

    private boolean isIdle() {
        synchronized(mTRM) {
            for (int i = 0; i < mTRM.length; i++) {
                if (mTRM[i].getUpdatedService() != SERVICE_NONE) {
                    return false;
                }
            }
        }

        return true;
    }
    // -----------------------------------------------------------
}
