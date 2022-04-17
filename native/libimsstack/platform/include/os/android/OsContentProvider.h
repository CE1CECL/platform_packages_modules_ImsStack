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
#ifndef OS_CONTENT_PROVIDER_H_
#define OS_CONTENT_PROVIDER_H_

#include "IMSList.h"
#include "IMSMap.h"

#include "IDbListener.h"
#include "ImsContentProvider.h"

#include "OsContentElement.h"
#include "OsContentTable.h"
#include "OsContentCursor.h"
#include "OsContentValues.h"

class OsDb;

class OsContentProvider
    : public IDbListener
    , public ImsContentProvider
{
public:
    OsContentProvider();
    virtual ~OsContentProvider();

private:
    OsContentProvider(IN const OsContentProvider &objRHS);
    OsContentProvider& operator=(IN const OsContentProvider &objRHS);

public:
    //4 NOTICE : Table should be destructed after calling ManagedQuery() method.

    // IContentProvider class
    virtual IMS_BOOL CreateTable(IN const AString& strTable, IN IContentTable* piTable);
    virtual IContentTable* CreateTableEx(IN const AString &strTable,
            IN const AString &strColumn = AString::ConstNull());
    //hsyun - start 111004
    virtual IContentTable* CreateTableEx(IN const AString &strTable,
            IN const AString &strColumn, IN const AString &strValue);
    //hsyun - end
    virtual void DropTable(IN const AString& strTable);
    virtual IMS_BOOL AddTable(IN const AString& strTable, IN IContentTable* piContentTable);
    virtual void RemoveTable(IN const AString& strTable);

    // "insert" operations
    virtual IMS_BOOL InsertData(IN const AString& strTable, IN IContentCursor* piCursor);

    // "update" operations
    virtual IMS_BOOL UpdateData(IN const AString& strTable,
            IN IContentCursor* piCursorSet, IN IContentCursor* piCursorWhere);

    virtual IMS_BOOL UpdateData(IN const AString& strTable,
            IN IContentCursor* piCursorSet, IN const AString& strWhere);

    // "delete" operations
    virtual IMS_BOOL DeleteData(IN const AString& strTable, IN const AString& strWhere);
    virtual IMS_BOOL DeleteData(IN const AString& strTable, IN IContentCursor* piCursor);

    // "select" operations
    //4 "select * from strTable where strWhere"
    virtual IContentCursor* ManagedQuery(IN const AString& strTable,
            IN const AString& strWhere);
    //4 "select objColumnNames from strTable where strWhere"
    virtual IContentCursor* ManagedQuery(IN const AString& strTable,
            IN const IMSList<AString>& objColumnNames, IN const AString& strWhere);
    //4 "select * from strTable where objCursor"
    virtual IContentCursor* ManagedQuery(IN const AString& strTable,
            IN IContentCursor* piCursor);

    //4 "select objColumnNames from strTable where objCursor"
    virtual IContentCursor* ManagedQuery(IN const AString& strTable,
            IN const IMSList<AString>& objColumnNames, IN IContentCursor* piCursor);

    virtual void SetQueryOption(IN QueryOption _eQueryOption);

    // ImsContentProvider class
    virtual IMS_BOOL Initialize(IN const AString &strDB);

private:
    // OsDb Functions
    IMS_BOOL Execute(IN const IMS_CHAR* pszCommand, IN SQL_QUERY_TYPE eQueryType);
    virtual IMS_SINT32 CBExecute(IN IMS_SINT32 nArgc,
            IN IMS_CHAR **pArgv, IN IMS_CHAR **pszColName);

    static const IMS_CHAR* GetType(IN CONTENT_FIELD_TYPE eFieldType);

private:
    OsDb* pAndroidDB;

    // tables of db
    IMSMap<AString, IContentTable*> objTables;
    // for checking if correct result incomes or not, compared with expected query result
    OsContentTable objExpectedTable;
    // query result
    OsContentValues objContentValues;

    // query option
    QueryOption eQueryOption;
};

#endif
