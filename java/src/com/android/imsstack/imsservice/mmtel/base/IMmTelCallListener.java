package com.android.imsstack.imsservice.mmtel.base;

import android.telephony.ims.stub.ImsCallSessionImplBase;

import com.android.imsstack.external.ims.ImsDialogState;

/**
 * Listener for MMTel call events.
 */
public interface IMmTelCallListener {
    /**
     * This is invoked when incoming call is received.
     */
    public void onIncomingCallReceived(ImsCallSessionImplBase session);

    /**
     * This is invoked when IMS dialog state is changed.
     */
    public void onImsDialogStateChanged(ImsDialogState dialogState);
}
