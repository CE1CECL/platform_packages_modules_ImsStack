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
#ifndef OS_CONTENT_TABLE_H_
#define OS_CONTENT_TABLE_H_

#include "ImsContentTable.h"
#include "OsContentConstant.h"

/*
    --------------------------------------
    FILED_TYPE | FIELD_NAME
    --------------------------------------
Ex)
    Integer |    "Age"
    Text   |    "Name"
*/
class OsContentTable
    : public ImsContentTable
{
public:
    OsContentTable();
    virtual ~OsContentTable();

    OsContentTable& operator=(IN const OsContentTable &objRHS);

public:
    // IContentTable Interface
    virtual IMS_BOOL AddColumnInteger(IN const AString &strFieldName);
    virtual IMS_BOOL AddColumnText(IN const AString &strFieldName);
#ifdef __IMS_DB_BINARY_FIELD__
    virtual IMS_BOOL AddColumnBlob(IN const AString &strFieldName);
    //hsyun - start 110902
    virtual IMS_BOOL AddColumnBlob(IN const AString &strFieldName, IN IMS_SINT32 nSize);
    //hsyun - end
#endif
    virtual IMS_BOOL AddColumnReal(IN const AString &strFieldName);

    virtual IMS_BOOL DeleteColumn(IN const AString &strFieldName);

    virtual void SetPrimaryKey(IN const AString& strFieldName);
    virtual const AString& GetPrimaryKey() const;
    virtual void Clear();

    CONTENT_FIELD_TYPE GetColumnType(IN const AString &strFieldName);
    const IMSList<CONTENT_FIELD>& GetFieldList() const;

private:
    IMS_BOOL CheckIfSameFieldName(IN const AString &strFieldName);

private:
    AString strPrimaryKey;

    IMSList<CONTENT_FIELD> objFieldList;
};

#endif
