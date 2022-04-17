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
#ifndef OS_CONTENT_CURSOR_H_
#define OS_CONTENT_CURSOR_H_

#include "ImsContentCursor.h"
#include "OsContentConstant.h"
#include "OsContentElement.h"

class OsContentValues;

/*
    - OsContentElement List
    - A row of Table
Ex)
    (OsElement)    | (OsElement)    | (OsElement)    | (OsElement)
    Integer             | Integer            | Real                | Text
    10                    | 10                | 0.5                | "hello"
*/



class OsContentCursor
    : public ImsContentCursor
{
public:
    OsContentCursor();
    OsContentCursor(IN const OsContentCursor &objRHS);
    virtual ~OsContentCursor();

public:
    OsContentCursor& operator=(IN const OsContentCursor &objRHS);

public:
    virtual IMS_SINT32 GetInt(IN const AString &strFieldName,
            OUT IMS_BOOL *pbOK = IMS_NULL) const;
    virtual const AString& GetString(IN const AString &strFieldName,
            OUT IMS_BOOL *pbOK = IMS_NULL) const;
    virtual IMS_FLOAT GetFloat(IN const AString &strFieldName,
            OUT IMS_BOOL *pbOK = IMS_NULL) const;

#ifdef __IMS_DB_BINARY_FIELD__
    virtual const ByteArray& GetBinary(IN const AString &strFieldName,
            OUT IMS_BOOL *pbOK = IMS_NULL) const;
    virtual IMS_BOOL GetBinary(IN const AString &strFieldName,
            OUT ByteArray& achOutBinary, OUT IMS_SINT32 &nLength) const;
#endif

    virtual IMS_BOOL PutValue(IN const AString &strFieldName, IN IMS_SINT32 nValue);
    virtual IMS_BOOL PutValue(IN const AString &strFieldName, IN IMS_FLOAT fValue);
    virtual IMS_BOOL PutValue(IN const AString &strFieldName, IN const AString &strValue);

#ifdef __IMS_DB_BINARY_FIELD__
    virtual IMS_BOOL PutValue(IN const AString &strFieldName, IN const ByteArray &objValue);
    virtual IMS_BOOL PutValue(IN const AString &strFieldName,
            IN const ByteArray &objValue, IN IMS_SINT32 nLength);
#endif

    virtual IMS_BOOL Remove(IN const AString &strFieldName);

    virtual IMS_BOOL IsFirstCursor() const;
    virtual IMS_BOOL IsLastCursor() const;

    virtual IContentCursor* MoveToFirst();
    virtual IContentCursor* MoveToLast();
    virtual IContentCursor* MoveToNext();

    virtual void Clear();

    IMSList<OsContentElement>* GetContentList();
    void SetThisCursor(IN OsContentCursor* _pThisCursor);

    // move current cursor position
    void SetContentValues(IN OsContentValues* _pContentValues);

private:
    IMS_BOOL CheckIfSameFieldName(IN const AString &strFieldName);

private:
    OsContentCursor* pThisCursor;
    OsContentValues* pContentValues;

    IMSList<OsContentElement> objElementList;
};

#endif
