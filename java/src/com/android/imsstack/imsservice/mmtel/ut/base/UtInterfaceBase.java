/*
    Author
    <table>
    date        author                  description
    --------    --------------          ----------
    20151009    hwangoo.park@           Created
    </table>

    Description
*/

package com.android.imsstack.imsservice.mmtel.ut.base;

import android.content.Context;
import android.os.Bundle;
import android.telephony.ims.ImsReasonInfo;

import com.android.imsstack.imsservice.mmtel.ut.base.IUtServiceStateListener;
import com.android.imsstack.imsservice.mmtel.ut.base.UtListener;

public class UtInterfaceBase implements UtInterface {
    protected UtListener mUtListener = null;
    private IUtServiceStateListener mUtServiceStateListener = null;

    @Override
    public boolean isUtAvailable() {
        return false;
    }

    @Override
    public void start(Context context) {
        // no-op
    }

    @Override
    public void setListener(UtListener listener) {
        mUtListener = listener;
    }

    @Override
    public void setServiceStateListener(IUtServiceStateListener listener) {
        mUtServiceStateListener = listener;
    }

    @Override
    public void onServiceStateChanged() {
        if (mUtServiceStateListener != null) {
            mUtServiceStateListener.onServiceStateChanged();
        }
    }

    /**
     * Implementation of IImsUt.
     */
    @Override
    public void close() {
        // no-op
    }

    @Override
    public int queryCallBarring(int condition) {
        return (ImsReasonInfo.CODE_UT_OPERATION_NOT_ALLOWED * (-1));
    }

    @Override
    public int queryCallBarringForServiceClass(int condition, int serviceClass) {
        return (ImsReasonInfo.CODE_UT_OPERATION_NOT_ALLOWED * (-1));
    }

    @Override
    public int queryCallForward(int condition, String number) {
        return (ImsReasonInfo.CODE_UT_OPERATION_NOT_ALLOWED * (-1));
    }

    @Override
    public int queryCallWaiting() {
        return (ImsReasonInfo.CODE_UT_OPERATION_NOT_ALLOWED * (-1));
    }

    @Override
    public int queryCLIR() {
        return (ImsReasonInfo.CODE_UT_OPERATION_NOT_ALLOWED * (-1));
    }

    @Override
    public int queryCLIP() {
        return (ImsReasonInfo.CODE_UT_OPERATION_NOT_ALLOWED * (-1));
    }

    @Override
    public int queryCOLR() {
        return (ImsReasonInfo.CODE_UT_OPERATION_NOT_ALLOWED * (-1));
    }

    @Override
    public int queryCOLP() {
        return (ImsReasonInfo.CODE_UT_OPERATION_NOT_ALLOWED * (-1));
    }

    @Override
    public int transact(Bundle ssInfo) {
        return (ImsReasonInfo.CODE_UT_OPERATION_NOT_ALLOWED * (-1));
    }

    @Override
    public int updateCallBarring(int condition, int action, String[] barringList) {
        return (ImsReasonInfo.CODE_UT_OPERATION_NOT_ALLOWED * (-1));
    }

    @Override
    public int updateCallBarringForServiceClass(int cbType, int action, String[] barringList,
            int serviceClass) {
        return (ImsReasonInfo.CODE_UT_OPERATION_NOT_ALLOWED * (-1));
    }

    @Override
    public int updateCallBarringWithPassword(int condition, int action, String[] barringList,
            int serviceClass, String password) {
        return (ImsReasonInfo.CODE_UT_OPERATION_NOT_ALLOWED * (-1));
    }

    @Override
    public int updateCallForward(int action, int condition, String number, int serviceClass,
            int timeSeconds) {
        return (ImsReasonInfo.CODE_UT_OPERATION_NOT_ALLOWED * (-1));
    }

    @Override
    public int updateCallWaiting(boolean enable, int serviceClass) {
        return (ImsReasonInfo.CODE_UT_OPERATION_NOT_ALLOWED * (-1));
    }

    @Override
    public int updateCLIR(int clirMode) {
        return (ImsReasonInfo.CODE_UT_OPERATION_NOT_ALLOWED * (-1));
    }

    @Override
    public int updateCLIP(boolean enable) {
        return (ImsReasonInfo.CODE_UT_OPERATION_NOT_ALLOWED * (-1));
    }

    @Override
    public int updateCOLR(int presentation) {
        return (ImsReasonInfo.CODE_UT_OPERATION_NOT_ALLOWED * (-1));
    }

    @Override
    public int updateCOLP(boolean enable) {
        return (ImsReasonInfo.CODE_UT_OPERATION_NOT_ALLOWED * (-1));
    }
}
