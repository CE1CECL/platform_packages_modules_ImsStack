/*
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

#define LOG_TAG "ImsStack"
#define LOG_NIDEBUG 0
#define LOG_NDDEBUG 0

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <utils/Log.h>
#include <nativehelper/JNIHelp.h>

#if defined(ALOGD)
#define IMS_LOGD            ALOGD
#elif defined(LOGD)
#define IMS_LOGD            LOGD
#else
#warning LOGD macro is not defined
#endif

#if defined(ALOGE)
#define IMS_LOGE            ALOGE
#elif defined(LOGE)
#define IMS_LOGE            LOGE
#else
#warning LOGE macro is not defined
#endif

extern jint IMSInterface_OnLoad(JavaVM* vm, JNIEnv* env);

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    (void) reserved;

    IMS_LOGD("JNI_OnLoad: libimsstack");

    JNIEnv* env = NULL;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK)
    {
        IMS_LOGE("JNI_OnLoad: GetEnv failed");
        return (-1);
    }

    assert(env != NULL);

    if (IMSInterface_OnLoad(vm, env) < 0)
    {
        IMS_LOGE("JNI_OnLoad: IMSInterface_OnLoad failed");
        return (-1);
    }

    return JNI_VERSION_1_4;
}
