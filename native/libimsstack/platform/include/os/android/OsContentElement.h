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
#ifndef OS_CONTENT_ELEMENT_H_
#define OS_CONTENT_ELEMENT_H_

#include "ByteArray.h"
#include "OsContentConstant.h"

/*
    --------------------------------------
    FILED_TYPE | FIELD_NAME | FIELD_VALUE
    --------------------------------------
Ex)
    Integer | "Age" | 30
*/
class OsContentElement
{
public:
    OsContentElement();
    OsContentElement(IN const OsContentElement &objRHS);
    virtual ~OsContentElement();

public:
    OsContentElement& operator=(IN const OsContentElement &objRHS);

public:
    inline CONTENT_FIELD_TYPE GetFieldType() const
    { return stContentField.eFieldType; }
    inline const AString& GetFieldName() const
    { return stContentField.strFieldName;}

    inline IMS_SINT32 GetInteger() const { return nValue; };
    inline IMS_FLOAT GetReal() const { return fValue; };
    inline const AString& GetText() const { return strValue; };
#ifdef __IMS_DB_BINARY_FIELD__
    inline const ByteArray& GetBlob() const { return objBlob; };

    IMS_BOOL GetBlob(OUT ByteArray& achOutBinary, OUT IMS_SINT32 &nLength);
#endif
    void SetFieldName(IN const AString& strFieldName);
    void SetFieldType(IN CONTENT_FIELD_TYPE eFieldType);

    void Put(IN IMS_SINT32 nValue);
    void Put(IN IMS_FLOAT fValue);
    void Put(IN const AString &strValue);
#ifdef __IMS_DB_BINARY_FIELD__
    void Put(IN const ByteArray &objBlob);

    void Put(IN const ByteArray &objBlob, IN IMS_SINT32 nLength);
#endif
public:
    CONTENT_FIELD stContentField;

    IMS_SINT32 nValue;
    IMS_FLOAT fValue;
    AString strValue;
#ifdef __IMS_DB_BINARY_FIELD__
    ByteArray objBlob;
    IMS_SINT32 nBlobLength;
#endif
};

#endif
