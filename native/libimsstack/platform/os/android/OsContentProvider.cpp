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
#include <stdlib.h>
#include <unistd.h>

#include "IMSStrLib.h"
#include "OsContentProvider.h"
#include "OsDb.h"
#include "ServiceMemory.h"
#include "ServiceTrace.h"

__IMS_TRACE_TAG_ADAPT__;

PUBLIC
OsContentProvider::OsContentProvider()
    : pAndroidDB (IMS_NULL)
    , objTables(IMSMap<AString, IContentTable*>())
    , eQueryOption(OPTION_NULL)
{
}

PUBLIC VIRTUAL
OsContentProvider::~OsContentProvider()
{
    objTables.Clear();

    if (pAndroidDB != IMS_NULL)
    {
        delete pAndroidDB;
        pAndroidDB = IMS_NULL;
    }

#ifdef __IMS_DEBUG__
    IMS_TRACE_D("Destructor::OsContentProvider", 0, 0, 0);
#endif
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL OsContentProvider::CreateTable(IN const AString& strTable, IN IContentTable* piTable)
{
    OsContentTable* pTable = DYNAMIC_CAST(OsContentTable*, piTable);
    const IMSList<CONTENT_FIELD>& objContentFields = pTable->GetFieldList();
    const AString &strPrimaryKey = pTable->GetPrimaryKey();

    AString strSQL("");

    //4 "create table students (test1 integer, test2 integer, test3 integer)"
    strSQL.Sprintf("create table %s (", strTable.GetStr());

    IMS_UINT32 nFieldCount = objContentFields.GetSize();

    for (IMS_UINT32 i = 0; i < nFieldCount; ++i )
    {
        const CONTENT_FIELD& stField = objContentFields.GetAt(i);

        strSQL.Append(" ");
        strSQL.Append(stField.strFieldName);
        strSQL.Append(" ");
        strSQL.Append(GetType(stField.eFieldType));

        // If a primary key exists, set primary key
        if (stField.strFieldName.Equals(strPrimaryKey))
        {
            strSQL.Append(" primary key");
        }

        if (i < (nFieldCount - 1))
        {
            strSQL.Append(",");
        }
    }
    strSQL.Append(" )");

#ifdef __IMS_DEBUG__
    IMS_TRACE_I("CreateTable - %s", strSQL.GetStr(), 0, 0);
#endif

    Execute(strSQL.GetStr(), SQL_QUERY_CREATE);

    if (objTables.GetIndexOfKey(strTable) < 0)
    {
        if (!objTables.Add(strTable, piTable))
        {
            return IMS_FALSE;
        }
    }
    else
    {
        IMS_TRACE_D("Table(%s) already exists", strTable.GetStr(), 0, 0);
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL
IContentTable* OsContentProvider::CreateTableEx(IN const AString &strTable,
        IN const AString &strColumn /* = AString::ConstNull() */)
{
    return CreateTableEx(strTable, strColumn, AString::ConstNull());
}

/*

Remarks

*/
//hsyun - start 111004
PUBLIC VIRTUAL
IContentTable* OsContentProvider::CreateTableEx(IN const AString &strTable,
        IN const AString &strColumn, IN const AString &strValue)
{
    if (pAndroidDB == IMS_NULL)
    {
        IMS_TRACE_E(0, "No DB", 0, 0, 0);
        return IMS_NULL;
    }

    OsContentTable *pTable = pAndroidDB->CreateTableTemplate(strTable, strColumn, strValue);

    if (pTable == IMS_NULL)
    {
        IMS_TRACE_E(0, "Creating a table (%s:%s=%s) template failed",
                strTable.GetStr(), strColumn.GetStr(), strValue.GetStr());
        return IMS_NULL;
    }

    return pTable;
}
//hsyun - end

/*

Remarks

*/
PUBLIC VIRTUAL
void OsContentProvider::DropTable(IN const AString& strTable)
{
    AString strSQL;

    //4 "drop table students"
    strSQL.Sprintf("drop table %s", strTable.GetStr());

#ifdef __IMS_DEBUG__
    IMS_TRACE_I("DropTable - %s", strSQL.GetStr(), 0, 0);
#endif

    Execute(strSQL.GetStr(), SQL_QUERY_DROP);

    if (objTables.GetIndexOfKey(strTable) >= 0)
    {
        objTables.Remove(strTable);
    }
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL OsContentProvider::AddTable(IN const AString& strTable,
        IN IContentTable* piContentTable)
{
    if (objTables.GetIndexOfKey(strTable) >= 0)
    {
        IMS_TRACE_D("DB table(%s) already exists", strTable.GetStr(), 0, 0);
        return IMS_TRUE;
    }

    if (!objTables.Add(strTable, piContentTable))
    {
        return IMS_FALSE;
    }

#ifdef __IMS_DEBUG__
    IMS_TRACE_D("AddTable - %s", strTable.GetStr(), 0, 0);
#endif

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL
void OsContentProvider::RemoveTable(IN const AString& strTable)
{
    if (objTables.GetIndexOfKey(strTable) < 0)
    {
        return;
    }

#ifdef __IMS_DEBUG__
    IMS_TRACE_D("RemoveTable - %s", strTable.GetStr(), 0, 0);
#endif

    objTables.Remove(strTable);
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL OsContentProvider::InsertData(IN const AString& strTable,
        IN IContentCursor* piCursor)
{
    OsContentCursor* pCursor = DYNAMIC_CAST(OsContentCursor*, piCursor);
    IMSList<OsContentElement>* pContentElements
            = (pCursor != IMS_NULL) ? pCursor->GetContentList() : IMS_NULL;
    IMS_UINT32 nElementCount = (pContentElements != IMS_NULL) ? pContentElements->GetSize() : 0;

    if (nElementCount == 0)
    {
        IMS_TRACE_E(0, "No columns to be inserted - %s", strTable.GetStr(), 0, 0);
        return IMS_FALSE;
    }

    AString strSQL;

    //4 "insert into students (test1, test2, test3) values (100, 200, 'hello')"
    strSQL.Sprintf("insert into %s (", strTable.GetStr());

    for (IMS_UINT32 i = 0; i < nElementCount; ++i)
    {
        const OsContentElement& objElement = pContentElements->GetAt(i);
        const AString& strField = objElement.GetFieldName();

        strSQL.Append(" ");
        strSQL.Append(strField);

        if (i < (nElementCount - 1))
        {
            strSQL.Append(",");
        }
    }

    strSQL.Append(" ) values (");

    AString strValue;

    for (IMS_UINT32 i = 0; i < nElementCount; ++i)
    {
        const OsContentElement& objElement = pContentElements->GetAt(i);
        CONTENT_FIELD_TYPE eField = objElement.GetFieldType();

        strSQL.Append(" ");

        if (eField == CONTENT_FIELD_INTEGER)
        {
            strValue.Sprintf("%d", objElement.GetInteger());
            strSQL.Append(strValue);
        }
        else if (eField == CONTENT_FIELD_REAL)
        {
            strValue.Sprintf("%f", objElement.GetReal());
            strSQL.Append(strValue);
        }
        else if (eField == CONTENT_FIELD_TEXT)
        {
            strValue.Sprintf("'%s'", objElement.GetText().GetStr());
            strSQL.Append(strValue);
        }
#ifdef __IMS_DB_BINARY_FIELD__
        else if (eField == CONTENT_FIELD_BLOB)
        {
            const ByteArray &objBlob = objElement.GetBlob();
            strValue.FromRawData(reinterpret_cast<const IMS_CHAR *>(
                    objBlob.GetData()), objBlob.GetLength());
            strSQL.Append(strValue);
        }
#endif

        if (i < (nElementCount - 1))
        {
            strSQL.Append(",");
        }
    }

    strSQL.Append(" )");

#ifdef __IMS_DEBUG__
    IMS_TRACE_I("InsertData - %s", strSQL.GetStr(), 0, 0);
#endif

    return Execute(strSQL.GetStr(), SQL_QUERY_INSERT);
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL OsContentProvider::UpdateData(IN const AString& strTable,
        IN IContentCursor* piCursorSet, IN IContentCursor* piCursorWhere)
{
    OsContentCursor* pCursorWhere = DYNAMIC_CAST(OsContentCursor*, piCursorWhere);
    IMSList<OsContentElement>* pContentElements
            = (pCursorWhere != IMS_NULL) ? pCursorWhere->GetContentList() : IMS_NULL;
    IMS_UINT32 nElementCount = (pContentElements != IMS_NULL) ? pContentElements->GetSize() : 0;
    AString strWhere(AString::ConstNull());

    if (nElementCount != 0)
    {
        AString strValue;

        for (IMS_UINT32 i = 0; i < nElementCount; i++)
        {
            const OsContentElement &objElement = pContentElements->GetAt(i);

            strWhere.Append(objElement.GetFieldName());
            strWhere.Append("=");

            if (objElement.GetFieldType() == CONTENT_FIELD_INTEGER)
            {
                strValue.Sprintf("%d", objElement.GetInteger());
                strWhere.Append(strValue);
            }
            else if (objElement.GetFieldType() == CONTENT_FIELD_TEXT)
            {
                strValue.Sprintf("'%s'", objElement.GetText().GetStr());
                strWhere.Append(strValue);
            }
            else if (objElement.GetFieldType() == CONTENT_FIELD_REAL)
            {
                strValue.Sprintf("%f", objElement.GetReal());
                strWhere.Append(strValue);
            }
#ifdef __IMS_DB_BINARY_FIELD__
            else if (objElement.GetFieldType() == CONTENT_FIELD_BLOB)
            {
                const ByteArray &objBlob = objElement.GetBlob();

                strValue.FromRawData(reinterpret_cast<const IMS_CHAR *>(
                        objBlob.GetData()), objBlob.GetLength());
                strWhere.Append(strValue);
            }
#endif

            if (i < (nElementCount - 1))
            {
                strWhere.Append(" and ");
            }
        }
    }

    return UpdateData(strTable, piCursorSet, strWhere);
}

/*

Remarks

*/
PUBLIC
IMS_BOOL OsContentProvider::UpdateData(IN const AString& strTable,
        IN IContentCursor* piCursorSet, IN const AString& strWhere)
{
    OsContentCursor *pCursorSet = DYNAMIC_CAST(OsContentCursor*, piCursorSet);
    IMSList<OsContentElement>* pContentElements
            = (pCursorSet != IMS_NULL) ? pCursorSet->GetContentList() : IMS_NULL;
    IMS_UINT32 nElementCount = (pContentElements != IMS_NULL) ? pContentElements->GetSize() : 0;

    if (nElementCount == 0)
    {
        IMS_TRACE_E(0, "No columns to be updated - %s", strTable.GetStr(), 0, 0);
        return IMS_FALSE;
    }

    AString strSQL;

    //4 "update strTable set objSet where strWhere"
    strSQL.Sprintf("update %s set ", strTable.GetStr());

    AString strValue;
    AString strSet;

    for (IMS_UINT32 i = 0; i < nElementCount; i++)
    {
        const OsContentElement &objElement = pContentElements->GetAt(i);

        strSet.Append(objElement.GetFieldName());
        strSet.Append("=");

        if (objElement.GetFieldType() == CONTENT_FIELD_INTEGER)
        {
            strValue.Sprintf("%d", objElement.GetInteger());
            strSet.Append(strValue);
        }
        else if (objElement.GetFieldType() == CONTENT_FIELD_TEXT)
        {
            strValue.Sprintf("'%s'", objElement.GetText().GetStr());
            strSet.Append(strValue);
        }
        else if (objElement.GetFieldType() == CONTENT_FIELD_REAL)
        {
            strValue.Sprintf("%f", objElement.GetReal());
            strSet.Append(strValue);
        }
#ifdef __IMS_DB_BINARY_FIELD__
        else if (objElement.GetFieldType() == CONTENT_FIELD_BLOB)
        {
            const ByteArray &objBlob = objElement.GetBlob();

            strValue.FromRawData(reinterpret_cast<const IMS_CHAR *>(
                    objBlob.GetData()), objBlob.GetLength());
            strSet.Append(strValue);
        }
#endif

        if (i < (nElementCount - 1))
        {
            strSet.Append(", ");
        }
    }

    strSQL.Append(strSet);

    if (strWhere.GetLength() > 0)
    {
        strSQL.Append(" where ");
        strSQL.Append(strWhere);
    }

#ifdef __IMS_DEBUG__
    IMS_TRACE_I("UpdateData - %s", strSQL.GetStr(), 0, 0);
#endif

    return Execute(strSQL.GetStr(), SQL_QUERY_UPDATE);
}

/*

Remarks

*/
PUBLIC
IMS_BOOL OsContentProvider::DeleteData(IN const AString& strTable,
        IN const AString& strWhere)
{
    AString strSQL;

    //4 "delete from students where Test1 = 100"
    strSQL.Sprintf("delete from %s ", strTable.GetStr());

    if (strWhere.GetLength() > 0)
    {
        strSQL.Append(" where ");
        strSQL.Append(strWhere);
    }

#ifdef __IMS_DEBUG__
    IMS_TRACE_I("DeleteData - %s", strSQL.GetStr(), 0, 0);
#endif

    return Execute(strSQL.GetStr(), SQL_QUERY_DELETE);
}

/*

Remarks

*/
PUBLIC
IMS_BOOL OsContentProvider::DeleteData(IN const AString& strTable,
        IN IContentCursor* piCursor)
{
    OsContentCursor* pCursor = DYNAMIC_CAST(OsContentCursor*, piCursor);
    IMSList<OsContentElement>* pContentElements
            = (pCursor != IMS_NULL) ? pCursor->GetContentList() : IMS_NULL;
    IMS_UINT32 nElementCount = (pContentElements != IMS_NULL) ? pContentElements->GetSize() : 0;
    AString strWhere(AString::ConstNull());

    if (nElementCount != 0)
    {
        AString strValue;

        for (IMS_UINT32 i = 0; i < nElementCount; i++)
        {
            const OsContentElement &objElement = pContentElements->GetAt(i);

            strWhere.Append(objElement.GetFieldName());
            strWhere.Append("=");

            if (objElement.GetFieldType() == CONTENT_FIELD_INTEGER)
            {
                strValue.Sprintf("%d", objElement.GetInteger());
                strWhere.Append(strValue);
            }
            else if (objElement.GetFieldType() == CONTENT_FIELD_TEXT)
            {
                strValue.Sprintf("'%s'", objElement.GetText().GetStr());
                strWhere.Append(strValue);
            }
            else if (objElement.GetFieldType() == CONTENT_FIELD_REAL)
            {
                strValue.Sprintf("%f", objElement.GetReal());
                strWhere.Append(strValue);
            }
#ifdef __IMS_DB_BINARY_FIELD__
            else if (objElement.GetFieldType() == CONTENT_FIELD_BLOB)
            {
                const ByteArray &objBlob = objElement.GetBlob();

                strValue.FromRawData(reinterpret_cast<const IMS_CHAR *>(
                        objBlob.GetData()), objBlob.GetLength());
                strWhere.Append(strValue);
            }
#endif

            if (i < (nElementCount - 1))
            {
                strWhere.Append(" and ");
            }
        }
    }

    return DeleteData(strTable, strWhere);
}

/*

Remarks

*/
PUBLIC
IContentCursor* OsContentProvider::ManagedQuery(IN const AString& strTable,
        IN const AString& strWhere)
{
    if (objTables.GetIndexOfKey(strTable) < 0)
    {
        IMS_TRACE_E(0, "No table(%s)", strTable.GetStr(), 0, 0);
        return IMS_NULL;
    }

    OsContentTable* pTable = DYNAMIC_CAST(OsContentTable*, objTables.GetValue(strTable));

    if (pTable == IMS_NULL)
    {
        IMS_TRACE_E(0, "No table(%s)", strTable.GetStr(), 0, 0);
        return IMS_NULL;
    }

    const IMSList<CONTENT_FIELD>& objFields = pTable->GetFieldList();

    for (IMS_UINT32 i = 0; i < objFields.GetSize(); ++i)
    {
        const CONTENT_FIELD &stField = objFields.GetAt(i);

        if (stField.eFieldType == CONTENT_FIELD_INTEGER)
        {
            objExpectedTable.AddColumnInteger(stField.strFieldName);
        }
        else if (stField.eFieldType == CONTENT_FIELD_REAL)
        {
            objExpectedTable.AddColumnReal(stField.strFieldName);
        }
        else if (stField.eFieldType == CONTENT_FIELD_TEXT)
        {
            objExpectedTable.AddColumnText(stField.strFieldName);
        }
#ifdef __IMS_DB_BINARY_FIELD__
        else if (stField.eFieldType == CONTENT_FIELD_BLOB)
        {
            //hsyun - start 110902
            //objExpectedTable.AddColumnBlob(stField.strFieldName);
            objExpectedTable.AddColumnBlob(stField.strFieldName, stField.nSize);
            //hsyun - end
        }
#endif
        else
        {
            IMS_TRACE_E(0, "Invalid content field type(%d)", stField.eFieldType, 0, 0);
            return IMS_NULL;
        }
    }

    //4 "select *"
    AString strSQL;

    strSQL.Append("select * ");

    //4 "from students"
    strSQL.Append("from ");
    strSQL.Append(strTable);

    //4 "where test1 = 100"
    if (strWhere.GetLength() > 0)
    {
        strSQL.Append(" where ");
        strSQL.Append(strWhere);
    }

    // Add option query
    if (eQueryOption == OPTION_COLLATE_NOCASE)
    {
        strSQL.Append(" collate nocase ");
    }

    IMS_TRACE_D("ManagedQuery - %s", strSQL.GetStr(), 0, 0);

    if (Execute(strSQL.GetStr(), SQL_QUERY_SELECT))
    {
        if (objContentValues.GetCursorSize() > 0)
        {
            OsContentCursor *pCursor = objContentValues.SetCurrentCursorIndex(0);

            if (pCursor != IMS_NULL)
            {
                pCursor->SetThisCursor(pCursor);
            }

            return pCursor;
        }
    }

    return IMS_NULL;
}

/*

Remarks

*/
PUBLIC
IContentCursor* OsContentProvider::ManagedQuery(IN const AString& strTable,
        IN const IMSList<AString>& objColumnNames, IN const AString& strWhere)
{
    if (objTables.GetIndexOfKey(strTable) < 0)
    {
        IMS_TRACE_E(0, "No table(%s)", strTable.GetStr(), 0, 0);
        return IMS_NULL;
    }

    OsContentTable* pTable = DYNAMIC_CAST(OsContentTable*, objTables.GetValue(strTable));

    if (pTable == IMS_NULL)
    {
        IMS_TRACE_E(0, "No table(%s)", strTable.GetStr(), 0, 0);
        return IMS_NULL;
    }

    AString strSelect;
    IMS_UINT32 nColumnCount = objColumnNames.GetSize();

    for (IMS_UINT32 i = 0; i < nColumnCount; ++i)
    {
        const AString &strField = objColumnNames.GetAt(i);
        CONTENT_FIELD_TYPE enFieldType = pTable->GetColumnType(strField);

        if (enFieldType == CONTENT_FIELD_INTEGER)
        {
            objExpectedTable.AddColumnInteger(strField);
        }
        else if (enFieldType == CONTENT_FIELD_REAL)
        {
            objExpectedTable.AddColumnReal(strField);
        }
        else if (enFieldType == CONTENT_FIELD_TEXT)
        {
            objExpectedTable.AddColumnText(strField);
        }
#ifdef __IMS_DB_BINARY_FIELD__
        else if (enFieldType == CONTENT_FIELD_BLOB)
        {
            objExpectedTable.AddColumnBlob(strField);
        }
#endif
        else
        {
            IMS_TRACE_E(0, "Invalid content field type(%s)", strField.GetStr(), 0, 0);
            return IMS_NULL;
        }

        strSelect.Append(strField);

        if (i < (nColumnCount - 1))
        {
            strSelect.Append(", ");
        }
    }

    strSelect.Append(" ");

    AString strSQL;

    //4 "select ( test1, test2 )"
    strSQL.Append("select ");
    strSQL.Append(strSelect);

    //4 "from students"
    strSQL.Append("from ");
    strSQL.Append(strTable);

    //4 "where test1 = 100"
    if (strWhere.GetLength() > 0)
    {
        strSQL.Append(" where ");
        strSQL.Append(strWhere);
    }

    // Add option query
    if (eQueryOption == OPTION_COLLATE_NOCASE)
    {
        strSQL.Append(" collate nocase ");
    }

    IMS_TRACE_D("ManagedQuery - %s", strSQL.GetStr(), 0, 0);

    if (Execute(strSQL.GetStr(), SQL_QUERY_SELECT))
    {
        if (objContentValues.GetCursorSize() > 0)
        {
            OsContentCursor *pCursor = objContentValues.SetCurrentCursorIndex(0);

            if (pCursor != IMS_NULL)
            {
                pCursor->SetThisCursor(pCursor);
            }

            return pCursor;
        }
    }

    return IMS_NULL;
}

/*

Remarks

*/
PUBLIC
IContentCursor* OsContentProvider::ManagedQuery(IN const AString& strTable,
        IN IContentCursor* piCursor)
{
    OsContentCursor* pCursor = DYNAMIC_CAST(OsContentCursor*, piCursor);
    IMSList<OsContentElement>* pContentElements
            = (pCursor != IMS_NULL) ? pCursor->GetContentList() : IMS_NULL;
    IMS_UINT32 nElementCount = (pContentElements != IMS_NULL) ? pContentElements->GetSize() : 0;
    AString strWhere(AString::ConstNull());

    if (nElementCount != 0)
    {
        AString strValue;

        for (IMS_UINT32 i = 0; i < nElementCount; i++)
        {
            const OsContentElement &objElement = pContentElements->GetAt(i);

            strWhere.Append(objElement.GetFieldName());
            strWhere.Append("=");

            if (objElement.GetFieldType() == CONTENT_FIELD_INTEGER)
            {
                strValue.Sprintf("%d", objElement.GetInteger());
                strWhere.Append(strValue);
            }
            else if (objElement.GetFieldType() == CONTENT_FIELD_TEXT)
            {
                strValue.Sprintf("'%s'", objElement.GetText().GetStr());
                strWhere.Append(strValue);
            }
            else if (objElement.GetFieldType() == CONTENT_FIELD_REAL)
            {
                strValue.Sprintf("%f", objElement.GetReal());
                strWhere.Append(strValue);
            }
#ifdef __IMS_DB_BINARY_FIELD__
            else if (objElement.GetFieldType() == CONTENT_FIELD_BLOB)
            {
                const ByteArray &objBlob = objElement.GetBlob();

                strValue.FromRawData(reinterpret_cast<const IMS_CHAR *>(
                        objBlob.GetData()), objBlob.GetLength());
                strWhere.Append(strValue);
            }
#endif
            else
            {
                IMS_TRACE_E(0, "Invalid content field type(%d)", objElement.GetFieldType(), 0, 0);
            }

            if (i < (nElementCount - 1))
            {
                strWhere.Append(" and ");
            }
        }
    }

    return ManagedQuery(strTable, strWhere);
}

/*

Remarks

*/
PUBLIC
IContentCursor* OsContentProvider::ManagedQuery(IN const AString& strTable,
        IN const IMSList<AString>& objColumnNames, IN IContentCursor* piCursor)
{
    OsContentCursor* pCursor = DYNAMIC_CAST(OsContentCursor*, piCursor);
    IMSList<OsContentElement>* pContentElements
            = (pCursor != IMS_NULL) ? pCursor->GetContentList() : IMS_NULL;
    IMS_UINT32 nElementCount = (pContentElements != IMS_NULL) ? pContentElements->GetSize() : 0;
    AString strWhere(AString::ConstNull());

    if (nElementCount != 0)
    {
        AString strValue;

        for (IMS_UINT32 i = 0; i < nElementCount; i++)
        {
            const OsContentElement &objElement = pContentElements->GetAt(i);

            strWhere.Append(objElement.GetFieldName());
            strWhere.Append("=");

            if (objElement.GetFieldType() == CONTENT_FIELD_INTEGER)
            {
                strValue.Sprintf("%d", objElement.GetInteger());
                strWhere.Append(strValue);
            }
            else if (objElement.GetFieldType() == CONTENT_FIELD_TEXT)
            {
                strValue.Sprintf("'%s'", objElement.GetText().GetStr());
                strWhere.Append(strValue);
            }
            else if (objElement.GetFieldType() == CONTENT_FIELD_REAL)
            {
                strValue.Sprintf("%f", objElement.GetReal());
                strWhere.Append(strValue);
            }
#ifdef __IMS_DB_BINARY_FIELD__
            else if (objElement.GetFieldType() == CONTENT_FIELD_BLOB)
            {
                const ByteArray &objBlob = objElement.GetBlob();

                strValue.FromRawData(reinterpret_cast<const IMS_CHAR *>(
                        objBlob.GetData()), objBlob.GetLength());
                strWhere.Append(strValue);
            }
#endif

            if (i < (nElementCount - 1))
            {
                strWhere.Append(" and ");
            }
        }
    }

    return ManagedQuery(strTable, objColumnNames, strWhere);
}

/*

Remarks

*/
PUBLIC VIRTUAL
void OsContentProvider::SetQueryOption(IN QueryOption _eQueryOption)
{
    this->eQueryOption = _eQueryOption;
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL OsContentProvider::Initialize(IN const AString &strDB)
{
    if (pAndroidDB != IMS_NULL)
    {
        IMS_TRACE_D("DB (%s) is already exists", strDB.GetStr(), 0, 0);
        return IMS_TRUE;
    }

#ifdef __IMS_DEBUG__
    IMS_TRACE_D("DB (%s) is opening ...", strDB.GetStr(), 0, 0);
#endif

    pAndroidDB = new OsDb();

    if (pAndroidDB == IMS_NULL)
    {
        return IMS_FALSE;
    }

    if (!pAndroidDB->Initialize(strDB))
    {
        IMS_TRACE_E(0, "Initializing DB (%s) failed", strDB.GetStr(), 0, 0);
        return IMS_FALSE;
    }

    // Set listener for sql callback
    pAndroidDB->SetListener(this);

    return IMS_TRUE;
}

/*

Remarks

*/
PRIVATE
IMS_BOOL OsContentProvider::Execute(IN const IMS_CHAR* pszCommand,
        IN SQL_QUERY_TYPE eQueryType)
{
    if (pAndroidDB == IMS_NULL)
    {
        IMS_TRACE_E(0, "OsDb is null(QueryType=%d)", eQueryType, 0, 0);
        return IMS_FALSE;
    }

    objContentValues.Clear();

    //3 NOTICE !!
    //4 Execute() Function is returned, only after CBExecute() is returned !!
    //4 reference to sqlite3_exec() API
    IMS_SINT32 nResult = pAndroidDB->Execute(pszCommand);

    // Reset query option
    eQueryOption = OPTION_NULL;

    return (nResult == 0);
}

/*

Remarks

*/
PRIVATE VIRTUAL
IMS_SINT32 OsContentProvider::CBExecute(IN IMS_SINT32 nArgc,
        IN IMS_CHAR **pArgv, IN IMS_CHAR **pszColName)
{
    OsContentCursor objResultCursor;

    for (IMS_SINT32 i = 0; i < nArgc; i++)
    {
        AString strColName(pszColName[i]);
        CONTENT_FIELD_TYPE enFieldType = objExpectedTable.GetColumnType(strColName);

        if (enFieldType == CONTENT_FIELD_INTEGER)
        {
            objResultCursor.PutValue(strColName, IMS_Atoi(pArgv[i]));
        }
        else if (enFieldType == CONTENT_FIELD_TEXT)
        {
            objResultCursor.PutValue(strColName, AString(pArgv[i]));
        }
        else if (enFieldType == CONTENT_FIELD_REAL)
        {
            objResultCursor.PutValue(strColName, static_cast<IMS_FLOAT>(atof(pArgv[i])));
        }
#ifdef __IMS_DB_BINARY_FIELD__
        else if (enFieldType == CONTENT_FIELD_BLOB)
        {
            ByteArray objBlob(reinterpret_cast<IMS_BYTE*>(pArgv[i]), 0xff);

            objResultCursor.PutValue(strColName,
                    objBlob, objExpectedTable.GetFieldList().GetAt(i).nSize);
        }
#endif
        else
        {
            IMS_TRACE_D("InvalidContent :: (%s at %d)", pszColName[i], i, 0);
        }
    }

    objContentValues.InsertCursor(objResultCursor);

    return 0;
}

/*

Remarks
*/
PRIVATE GLOBAL
const IMS_CHAR* OsContentProvider::GetType(IN CONTENT_FIELD_TYPE eFieldType)
{
    if (eFieldType == CONTENT_FIELD_INTEGER)
    {
        return "integer";
    }
    else if (eFieldType == CONTENT_FIELD_TEXT)
    {
        return "text";
    }
    else if (eFieldType == CONTENT_FIELD_REAL)
    {
        return "real";
    }
#ifdef __IMS_DB_BINARY_FIELD__
    else if (eFieldType == CONTENT_FIELD_BLOB)
    {
        return "blob";
    }
#endif

    return IMS_NULL;
}
