/*
    Author
    <table>
    date        author                  description
    --------    --------------          ----------
    20151009    hwangoo.park@           Created
    </table>

    Description
*/

package com.android.imsstack.imsservice.mmtel.ut;

import com.android.imsstack.enabler.ssc.SscServiceImpl;
import com.android.imsstack.imsservice.mmtel.ut.base.UtInterface;
import com.android.imsstack.util.MSimUtils;

public final class UtFactory {
    private static UtFactory sUtFactory = new UtFactory();
    private final UtInterface[] mUtInterface = new UtInterface[MSimUtils.getMaxSimSlot()];

    private UtFactory() {
        for (int i = 0; i < mUtInterface.length; i++) {
            mUtInterface[i] = null;
        }
    }

    public static UtFactory getInstance() {
        return sUtFactory;
    }

    public UtInterface getUtInterface(int slotId) {
        if (slotId < 0 || slotId >= mUtInterface.length) {
            return null;
        }

        if (mUtInterface[slotId] == null) {
            mUtInterface[slotId] = new SscServiceImpl(slotId);
        }

        return mUtInterface[slotId];
    }

    public void releaseUtInterface(int slotId) {
        if (slotId < 0 || slotId >= mUtInterface.length) {
            return;
        }

        if (mUtInterface[slotId] == null) {
            return;
        }

        mUtInterface[slotId].close();
        mUtInterface[slotId] = null;
    }
}
