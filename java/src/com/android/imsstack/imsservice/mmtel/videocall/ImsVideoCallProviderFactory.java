/*
    Author
    <table>
    date        author                  description
    --------    --------------          ----------
    20160309    hwangoo.park@           Created
    </table>

    Description
*/

package com.android.imsstack.imsservice.mmtel.videocall;

import com.android.imsstack.enabler.mtc.MtcMediaSession;
import com.android.imsstack.imsservice.mmtel.call.IVideoCallSession;
import com.android.imsstack.imsservice.mmtel.videocall.base.ImsVideoCallProviderBase;

public class ImsVideoCallProviderFactory {

    public static ImsVideoCallProviderBase createVideoCallProvider(IVideoCallSession callSession,
            MtcMediaSession mediaSession) {
        return new ImsVideoCallProviderImpl(callSession, mediaSession);
    }
}
