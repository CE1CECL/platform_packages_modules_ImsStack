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
#ifndef OS_DB_H_
#define OS_DB_H_

#include "AString.h"

class OsDbPrivate;
class OsContentTable;
class IDbListener;

class OsDb
{
public:
    OsDb();
    ~OsDb();

private:
    OsDb(IN const OsDb& objRHS);
    OsDb& operator=(IN const OsDb& objRHS);

public:
    IMS_SINT32 CBExecute(IN IMS_SINT32 nArgc,
            IN IMS_CHAR **ppArgv, IN IMS_CHAR **ppszColName);

    OsContentTable* CreateTableTemplate(IN const AString &strTable,
            IN const AString &strColumn);

    //hsyun - start 111004
    OsContentTable* CreateTableTemplate(IN const AString &strTable,
            IN const AString &strColumn, IN const AString &strValue);
    //hsyun - end

    IMS_SINT32 Execute(IN const IMS_CHAR* pszCommand);

    IMS_SINT32 ExecuteGetTable(IN const IMS_CHAR* pszCommand,
        OUT IMS_CHAR*** pppszOut, OUT IMS_SINT32* pnRow, OUT IMS_SINT32* pnColumn,
        OUT IMS_CHAR** ppszError);

    IMS_BOOL Initialize(IN const AString &strDB);
    void SetListener(IN IDbListener* piListener);

    static IMS_BOOL Exists(IN const AString &strDB);

private:
    static IMS_SINT32 WaitForBusyOrLocked(IN IMS_SINT32 nRC, IN IMS_SINT32& nRetryCount,
            IN const AString& strTable, IN const IMS_CHAR* pszMethod);

private:
    enum
    {
        IMS_SQLITE_ERROR = (-1),
        IMS_SQLITE_OK = 0,
        IMS_SQLITE_RETRY = 1
    };

    OsDbPrivate* pDBPrivate;
    IDbListener* piDBListener;
};

#endif
