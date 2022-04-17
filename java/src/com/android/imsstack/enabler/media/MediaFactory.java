/**
 * Copyright (C) 2022 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.imsstack.enabler.media;

import com.android.imsstack.enabler.IBaseContext;
import com.android.imsstack.enabler.media.audio.MediaAudioSession;
import com.android.imsstack.enabler.media.base.MediaSessionBase;
import com.android.imsstack.enabler.mtc.MtcMediaSession;

public class MediaFactory {

    public static MediaSessionBase createMediaSession(
        IBaseContext context, MtcMediaSession mtcMediaSession) {
        // TODO: To be updated for VideoSession and RTTSession
        return new MediaAudioSession(context, mtcMediaSession);
    }

}
