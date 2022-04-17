package com.android.imsstack.enabler.ssc;

import android.os.Handler;

/**
 * SscTransactionBuilder
 *
 * This class make SscTransaction instance based on configurable parameter and setting value.
 * @Param
 *     + Get / Put Transaction Type from input parameter
 *  + PDN Type (XCAP PDN / Internet PDN ...) from DB
 *     + Http Connection Type (HTTP / HTTPS ) from DB
 *  + XML Parser, if operator specific class is exist
 *  + XML Creator, if operator specific class is exist
 *  + XML PathFinder, if operator specific class is exist
 *  + XML Doc Validator, if operator specific class is exist

 * @author kyubo.lim
 *
 */
public class SscTransactionFactory {
    public static SscTransaction getSscTransaction(int slotId, Handler callbackHandler) {
        return new SscTransaction(slotId, callbackHandler);
    }
}
