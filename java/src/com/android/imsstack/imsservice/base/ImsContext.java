package com.android.imsstack.imsservice.base;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;

import com.android.imsstack.core.agents.AgentFactory;
import com.android.imsstack.core.agents.agentif.ISubscription;
import com.android.imsstack.enabler.IContext;
import com.android.imsstack.util.AppContext;
import com.android.imsstack.util.MSimUtils;

import java.util.concurrent.Executor;

public class ImsContext implements IContext {
    private final Context mContext;
    private final Executor mExecutor;
    private final int mSlotId;

    public ImsContext(Context context, Executor executor, int slotId) {
        mContext = context;
        mExecutor = executor;
        mSlotId = slotId;
    }

    /**
     * Utilities for this context.
     */
    @Override
    public Context getContext() {
        return mContext;
    }

    @Override
    public Executor getExecutor() {
        return mExecutor;
    }

    @Override
    public Handler getDefaultHandler() {
        return AppContext.getMainHandler();
    }

    @Override
    public Looper getDefaultLooper() {
        return AppContext.getMainLooper();
    }

    /**
     * Returns the phone id of this context.
     */
    @Override
    public int getPhoneId() {
        // FIXME: phoneId - same as slotId
        return getSlotId();
    }

    /**
     * Returns the slot-id of this context.
     */
    @Override
    public int getSlotId() {
        return mSlotId;
    }

    /**
     * Returns the subscription id of this context.
     */
    @Override
    public int getSubId() {
        ISubscription isub = (ISubscription)AgentFactory.getAgent(AgentFactory.SUBSCRIPTION);
        return (isub != null) ? isub.getSubId(getSlotId()) : MSimUtils.getSubId(getPhoneId());
    }
}
