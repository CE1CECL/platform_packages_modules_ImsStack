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

#ifndef _INTERFACE_MEDIA_SESSION_LISTENER_H_
#define _INTERFACE_MEDIA_SESSION_LISTENER_H_

#include "MediaDef.h"
#include "IMMedia.h"

class IMediaSessionListener
{
public:
    virtual void MediaSession_SendEventToUi(IMS_SINT32 nEvent, IMS_SINT32 nResult) = 0;
    virtual IMS_BOOL MediaSession_SendMsgToMediaManager(
            IN IMS_SINT32 eEvent, IN ImsMediaMsgParamBase* param) = 0;
};
#endif /* _INTERFACE_MEDIA_SESSION_LISTENER_H_ */
