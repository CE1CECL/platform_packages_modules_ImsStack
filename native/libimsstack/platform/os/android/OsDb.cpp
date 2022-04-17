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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "sqlite3.h"

#include "IDbListener.h"
#include "OsContentTable.h"
#include "OsDb.h"
#include "ServiceMemory.h"
#include "ServiceSystemTime.h"
#include "ServiceTrace.h"

__IMS_TRACE_TAG_ADAPT__;

// 100 times * 200ms = 20 seconds
#define MAX_RETRY 100
#define TIME_TO_SLEEP_ON_BUSY 200

LOCAL IMS_SINT32 SQLite3CBExecute(IN IMS_PVOID pArg,
        IN IMS_SINT32 nArgc, IN IMS_CHAR **ppArgv, IN IMS_CHAR **ppszColName);
LOCAL IMS_SINT32 SQLiteBusyHandler(IN IMS_PVOID pvData, IN IMS_SINT32 nRetry);

class OsDbPrivate
{
public:
    OsDbPrivate();
    ~OsDbPrivate();

private:
    friend class OsDb;

    sqlite3 *pDB;
};

PUBLIC
OsDbPrivate::OsDbPrivate()
    : pDB(IMS_NULL)
{
}

PUBLIC VIRTUAL
OsDbPrivate::~OsDbPrivate()
{
    if (pDB != IMS_NULL)
    {
        IMS_SINT32 nRC = sqlite3_close(pDB);

        if (nRC != SQLITE_OK)
        {
            IMS_TRACE_E(0, "sqlite3_close() failed - (err code : %d, reason : %s)",
                    nRC, sqlite3_errmsg(pDB), 0);
        }

        pDB = IMS_NULL;
    }
}



PUBLIC
OsDb::OsDb()
    : pDBPrivate (new OsDbPrivate())
    , piDBListener(IMS_NULL)
{
}

PUBLIC
OsDb::~OsDb()
{
    if (pDBPrivate != IMS_NULL)
    {
        delete pDBPrivate;
        pDBPrivate = IMS_NULL;
    }

    piDBListener = IMS_NULL;

#ifdef __IMS_DEBUG__
    IMS_TRACE_D("Destructor :: OsDb", 0, 0, 0);
#endif
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 OsDb::CBExecute(IN IMS_SINT32 nArgc,
        IN IMS_CHAR **ppArgv, IN IMS_CHAR **ppszColName)
{
    if (piDBListener == IMS_NULL)
    {
        IMS_TRACE_E(0, "IDbListener is null", 0, 0, 0);
        return -1;
    }

    return piDBListener->CBExecute(nArgc, ppArgv, ppszColName);
}

/*

Remarks

*/
PUBLIC
OsContentTable* OsDb::CreateTableTemplate(IN const AString &strTable,
        IN const AString &strColumn)
{
    (void) strColumn;
    return CreateTableTemplate(strTable, AString::ConstNull(), AString::ConstNull());
}

/*

Remarks

*/
//hsyun - start 111004
PUBLIC
OsContentTable* OsDb::CreateTableTemplate(IN const AString &strTable,
        IN const AString &strColumn, IN const AString &strValue)
{
    if (pDBPrivate->pDB == IMS_NULL)
    {
        IMS_TRACE_E(0, "DB is null", 0, 0 , 0);
        return IMS_NULL;
    }

    AString strQuery;

    if ((strColumn.GetLength() > 0) && (strValue.GetLength() > 0))
    {
        strQuery.Sprintf("select * from %s where %s='%s'",
                strTable.GetStr(), strColumn.GetStr(), strValue.GetStr());
    }
    else
    {
        strQuery.Sprintf("select * from %s", strTable.GetStr());
    }

#ifdef __IMS_DEBUG__
    IMS_TRACE_D("Query for table template : %s", strQuery.GetStr(), 0, 0);
#endif

    sqlite3_stmt *pstStmt = IMS_NULL;
    const IMS_CHAR *pszLeftover = IMS_NULL;
    // Retry count: 5
    IMS_SINT32 nRetryCount = 5;

    do {
        IMS_SINT32 nRC = sqlite3_prepare_v2(pDBPrivate->pDB,
                strQuery.GetStr(), /*-1*/strQuery.GetLength(), &pstStmt, &pszLeftover);

        nRC = WaitForBusyOrLocked(nRC, nRetryCount, strTable, "sqlite3_prepare_v2");

        if (nRC == IMS_SQLITE_ERROR)
        {
            if (pstStmt != IMS_NULL)
            {
                sqlite3_finalize(pstStmt);
            }
            return IMS_NULL;
        }
        else if (nRC == IMS_SQLITE_RETRY)
        {
            if (pstStmt != IMS_NULL)
            {
                sqlite3_finalize(pstStmt);
            }
            pstStmt = IMS_NULL;
            pszLeftover = IMS_NULL;
            continue;
        }

        if (pstStmt == IMS_NULL)
        {
            IMS_TRACE_E(0, "sqlite3_stmt is null (%s)", strTable.GetStr(), 0, 0);
            return IMS_NULL;
        }

        nRC = sqlite3_step(pstStmt);

        if (nRC == SQLITE_ROW)
        {
            break;
        }

        sqlite3_finalize(pstStmt);

        nRC = WaitForBusyOrLocked(nRC, nRetryCount, strTable, "sqlite3_step");

        if (nRC == IMS_SQLITE_ERROR)
        {
            return IMS_NULL;
        }
        else if (nRC == IMS_SQLITE_RETRY)
        {
            pstStmt = IMS_NULL;
            pszLeftover = IMS_NULL;
            continue;
        }
    } while (nRetryCount > 0);

    IMS_SINT32 nColCount = sqlite3_column_count(pstStmt);

    if (nColCount <= 0)
    {
        sqlite3_finalize(pstStmt);
        IMS_TRACE_E(0, "No columns in table(%s)", strTable.GetStr(), 0, 0);
        return IMS_NULL;
    }

    OsContentTable *pTable = new OsContentTable();

    if (pTable == IMS_NULL)
    {
        sqlite3_finalize(pstStmt);
        return IMS_NULL;
    }

    for (IMS_SINT32 i = 0; i < nColCount; ++i)
    {
        IMS_SINT32 nColType = sqlite3_column_type(pstStmt, i);
        const IMS_CHAR *pszColName = sqlite3_column_name(pstStmt, i);

#ifdef __IMS_DEBUG__
        IMS_TRACE_I("Column at (%d) : [%s : %d]", i, _TRACE_S_(pszColName), nColType);
#endif

        if (nColType == SQLITE_INTEGER)
        {
            pTable->AddColumnInteger(pszColName);
        }
        else if (nColType == SQLITE_FLOAT)
        {
            pTable->AddColumnReal(pszColName);
        }
#ifdef __IMS_DB_BINARY_FIELD__
        else if (nColType == SQLITE_BLOB)
        {
            pTable->AddColumnBlob(pszColName, sqlite3_column_bytes(pstStmt, i));
        }
#endif
        else if (nColType == SQLITE3_TEXT)
        {
            pTable->AddColumnText(pszColName);
        }
        else if (nColType == SQLITE_NULL)
        {
            IMS_TRACE_D("NULL-Column :: (%s)", _TRACE_S_(pszColName), 0, 0);
        }
    }

    sqlite3_finalize(pstStmt);

    return pTable;
}
//hsyun - end

/*

Remarks

    - Execute() Function is returned, only after CBExecute() is returned !!
    - reference to sqlite3_exec() API

*/
PUBLIC VIRTUAL
IMS_SINT32 OsDb::Execute(IN const IMS_CHAR* pszCommand)
{
    if (pDBPrivate->pDB == IMS_NULL)
    {
        IMS_TRACE_E(0, "DB is null", 0, 0 , 0);
        return SQLITE_ERROR;
    }

    IMS_CHAR* pszErrMsg = IMS_NULL;
    IMS_SINT32 nRC = sqlite3_exec(pDBPrivate->pDB,
            pszCommand, SQLite3CBExecute, this, &pszErrMsg);

    if (nRC != SQLITE_OK)
    {
        IMS_TRACE_E(0, "Execute - (err-code=%d, reason=%s)", nRC, pszErrMsg, 0);
        sqlite3_free(pszErrMsg);
    }

    return nRC;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 OsDb::ExecuteGetTable(IN const IMS_CHAR* pszCommand,
        OUT IMS_CHAR*** pppszOut, OUT IMS_SINT32* pnRow, OUT IMS_SINT32* pnColumn,
        OUT IMS_CHAR** ppszError)
{
    if (pDBPrivate->pDB == IMS_NULL)
    {
        IMS_TRACE_E(0, "DB is null", 0, 0, 0);
        return SQLITE_ERROR;
    }

    return sqlite3_get_table(pDBPrivate->pDB,
            pszCommand,   pppszOut, pnRow, pnColumn, ppszError);
}

/*

Remarks

*/
PUBLIC
IMS_BOOL OsDb::Initialize(IN const AString &strDB)
{
    if (pDBPrivate->pDB != IMS_NULL)
    {
        sqlite3_close(pDBPrivate->pDB);
        pDBPrivate->pDB = IMS_NULL;
    }

    IMS_SINT32 nResult = sqlite3_open_v2(strDB.GetStr(),
            &(pDBPrivate->pDB) , SQLITE_OPEN_READWRITE, 0);

    if (nResult != SQLITE_OK)
    {
        IMS_TRACE_E(0, "sqlite3_open() failed - (DB=%s, err-code=%d, reason=%s)",
                strDB.GetStr(), nResult, sqlite3_errmsg(pDBPrivate->pDB));

        sqlite3_close(pDBPrivate->pDB);
        pDBPrivate->pDB = IMS_NULL;

        return IMS_FALSE;
    }

    nResult = sqlite3_busy_handler(pDBPrivate->pDB, SQLiteBusyHandler, pDBPrivate);

    if (nResult != SQLITE_OK)
    {
        IMS_TRACE_E(0, "sqlite3_busy_handler failed - err-code=%d", nResult, 0, 0);
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
void OsDb::SetListener(IN IDbListener* piListener)
{
    piDBListener = piListener;
}

/*

Remarks

*/
PUBLIC GLOBAL
IMS_BOOL OsDb::Exists(IN const AString &strDB)
{
    sqlite3 *pDB = IMS_NULL;
    IMS_SINT32 nRC = sqlite3_open_v2( strDB.GetStr(), &pDB, SQLITE_OPEN_READWRITE, 0 );

    if ( SQLITE_OK != nRC )
    {
        IMS_TRACE_E(0, "sqlite3_open_v2() failed - (DB Path : %s,  err code : %d, reason : %s)",
                strDB.GetStr(), nRC, sqlite3_errmsg(pDB) );

        sqlite3_close( pDB );
        return IMS_FALSE;
    }

#ifdef __IMS_DEBUG__
    IMS_TRACE_D("DB (%s) is already exists", strDB.GetStr(), 0, 0);
#endif

    sqlite3_close( pDB );

    return IMS_TRUE;
}

/*

Remarks

*/
PRIVATE GLOBAL
IMS_SINT32 OsDb::WaitForBusyOrLocked(IN IMS_SINT32 nRC, IN IMS_SINT32& nRetryCount,
        IN const AString& strTable, IN const IMS_CHAR* pszMethod)
{
    if (nRC == SQLITE_OK)
    {
        return IMS_SQLITE_OK;
    }
    else if ((nRC == SQLITE_BUSY)
            || (nRC == SQLITE_LOCKED))
    {
        IMS_TRACE_D("Wait for db access (%d)", nRetryCount, 0, 0);

        --nRetryCount;

        if (nRetryCount > 0)
        {
            IMS_SYS_Sleep(10);
            return IMS_SQLITE_RETRY;
        }

        IMS_TRACE_I("Retry count is over (%s: %s)", pszMethod, strTable.GetStr(), 0);
    }
    else
    {
        IMS_TRACE_E(nRC, "%s (%s) failed (err=%s)",
                pszMethod, strTable.GetStr(), sqlite3_errstr(nRC));
    }

    return IMS_SQLITE_ERROR;
}

/*

Remarks

*/
LOCAL IMS_SINT32 SQLite3CBExecute(IN IMS_PVOID pArg,
        IN IMS_SINT32 nArgc, IN IMS_CHAR **ppArgv, IN IMS_CHAR **ppszColName)
{
    if (pArg == IMS_NULL)
    {
        return (-1);
    }

    IMS_SINT32 nResult = 0;
    OsDb *pDB = reinterpret_cast<OsDb*>(pArg);

    if (pDB != IMS_NULL)
    {
        nResult = pDB->CBExecute(nArgc, ppArgv, ppszColName);
    }

    return nResult;
}

/*

Remarks

*/
LOCAL IMS_SINT32 SQLiteBusyHandler(IN IMS_PVOID pvData, IN IMS_SINT32 nRetry)
{
    (void) pvData;

    if (nRetry < MAX_RETRY)
    {
        IMS_TRACE_I("SQLITE_BUSY %d times, retry again.", nRetry, 0, 0);
        usleep(1000 * TIME_TO_SLEEP_ON_BUSY);
        // Return non-zero to let caller retry again.
        return 1;
    }

    // Return zero to let caller return SQLITE_BUSY immediately.
    IMS_TRACE_E(0, "%d times retried, exit.", nRetry, 0, 0);
    return 0;
}
