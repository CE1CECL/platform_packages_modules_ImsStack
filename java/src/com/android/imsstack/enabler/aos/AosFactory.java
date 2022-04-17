package com.android.imsstack.enabler.aos;

import java.util.HashMap;
import java.util.Map;

import com.android.imsstack.enabler.aos.service.AosService;
import com.android.imsstack.util.ImsLog;
import com.android.imsstack.util.MSimUtils;

/**
 * Factory class to instantiate AoS components.
 */
public class AosFactory {

    private static AosFactory sFactory = null;

    private Map<Integer, AosService> mAosServices =
            new HashMap<Integer, AosService>(MSimUtils.getMaxSimSlot());

    // TODO : This is temp code for TelephonyCallback
    private AosSettingService aosSettingService = null;

    public static AosFactory getInstance() {
        if (sFactory == null) {
            sFactory = new AosFactory();
        }

        return sFactory;
    }

    public void init() {
        ImsLog.d("");
    }

    public synchronized void start(int slotId) {
        ImsLog.d(slotId, "");

        AosService aosService = new AosService();

        aosService.start(slotId);
        mAosServices.put(slotId, aosService);

        /// TODO : This is temp code for TelephonyCallback
        aosSettingService = new AosSettingService(slotId);
        aosSettingService.start();
    }

    public synchronized void stop(int slotId) {
        ImsLog.d(slotId, "");

        // TODO : This is temp code for TelephonyCallback
        if (aosSettingService != null) {
            aosSettingService.stop();
            aosSettingService = null;
        }

        AosService aosService = (AosService)mAosServices.get(slotId);
        if (aosService != null) {
            aosService.stop();
            mAosServices.remove(slotId);
        }
    }

    public synchronized IAosRegistration getAosRegistration(int slotId) {
        return (IAosRegistration)mAosServices.get(slotId);
    }

    public synchronized IAosInfo getAosInfo(int slotId) {
        return (IAosInfo)mAosServices.get(slotId);
    }
}
