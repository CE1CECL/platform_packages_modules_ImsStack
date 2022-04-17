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

import android.telephony.ims.ImsCallForwardInfo;
import android.telephony.ims.ImsReasonInfo;
import android.telephony.ims.ImsSsInfo;

public class UtListener {
    public void utConfigurationUpdated(final int id) {
        // no-op
    }

    public void utConfigurationUpdateFailed(final int id, final ImsReasonInfo reasonInfo) {
        // no-op
    }

    public void lineIdentificationSupplementaryServiceResponse(final int id, ImsSsInfo ssInfo) {
        // no-op
    }

    public void utConfigurationCallBarringQueried(final int id, final ImsSsInfo[] cbInfo) {
        // no-op
    }

    public void utConfigurationCallForwardQueried(final int id, final ImsCallForwardInfo[] cfInfo) {
        // no-op
    }

    public void utConfigurationCallWaitingQueried(final int id, final ImsSsInfo[] cwInfo) {
        // no-op
    }

    public void utConfigurationQueryFailed(final int id, final ImsReasonInfo reasonInfo) {
        // no-op
    }
}
