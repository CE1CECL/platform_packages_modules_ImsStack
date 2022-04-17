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

#ifndef _CODEC_CONFIG_FACTORY_H_
#define _CODEC_CONFIG_FACTORY_H_

#include "AString.h"

class CodecConfig;

class CodecConfigFactory
{
private:
    CodecConfigFactory();

public:
    static CodecConfig* CreateACodecConfig(IN CONST AString &strName, IN CONST IMS_SINT32 nIndex);
    static CodecConfig* CreateVCodecConfig(IN CONST AString &strName, IN CONST IMS_SINT32 nIndex);
    static CodecConfig* CreateTCodecConfig(IN CONST AString &strName, IN CONST IMS_SINT32 nIndex);
};
#endif                                              // _CODEC_CONFIG_FACTORY_H_
