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
#ifndef IMS_CONTENT_PROVIDER_H_
#define IMS_CONTENT_PROVIDER_H_

#include "IContentProvider.h"

class ImsContentProvider
    : public IContentProvider
{
public:
    inline ImsContentProvider()
    {}
    inline virtual ~ImsContentProvider()
    {}

public:
    virtual IMS_BOOL Initialize(IN const AString& strDb) = 0;
};

#endif
