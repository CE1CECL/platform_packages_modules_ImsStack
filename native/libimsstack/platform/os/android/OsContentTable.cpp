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
#include "OsContentTable.h"
#include "ServiceMemory.h"
#include "ServiceTrace.h"

__IMS_TRACE_TAG_ADAPT__;

PUBLIC
OsContentTable::OsContentTable()
    : strPrimaryKey(AString::ConstNull())
    , objFieldList(IMSList<CONTENT_FIELD>())
{
}

PUBLIC VIRTUAL
OsContentTable::~OsContentTable()
{
    objFieldList.Clear();

#ifdef __IMS_DEBUG__
    IMS_TRACE_D("Destructor::OsContentTable", 0, 0, 0);
#endif
}

/*

Remarks

*/
PUBLIC
OsContentTable& OsContentTable::operator=(IN const OsContentTable& objRHS)
{
    if (this != &objRHS)
    {
        objFieldList = objRHS.objFieldList;
    }

    return (*this);
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL OsContentTable::AddColumnInteger(IN const AString &strFieldName)
{
    if (!CheckIfSameFieldName(strFieldName))
    {
        return IMS_FALSE;
    }

    CONTENT_FIELD stField;

    stField.strFieldName = strFieldName.GetStr();
    stField.eFieldType = CONTENT_FIELD_INTEGER;

    if (!objFieldList.Append(stField))
    {
        return IMS_FALSE;
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL OsContentTable::AddColumnText(IN const AString &strFieldName)
{
    if (!CheckIfSameFieldName(strFieldName))
    {
        return IMS_FALSE;
    }

    CONTENT_FIELD stField;

    stField.strFieldName = strFieldName.GetStr();
    stField.eFieldType = CONTENT_FIELD_TEXT;

    if (!objFieldList.Append(stField))
    {
        return IMS_FALSE;
    }

    return IMS_TRUE;
}

#ifdef __IMS_DB_BINARY_FIELD__
/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL OsContentTable::AddColumnBlob(IN const AString &strFieldName)
{
    if (!CheckIfSameFieldName(strFieldName))
    {
        return IMS_FALSE;
    }

    CONTENT_FIELD stField;

    stField.strFieldName = strFieldName.GetStr();
    stField.eFieldType = CONTENT_FIELD_BLOB;

    if (!objFieldList.Append(stField))
    {
        return IMS_FALSE;
    }

    return IMS_TRUE;
}

/*

Remarks

*/
//hsyun - start
PUBLIC VIRTUAL
IMS_BOOL OsContentTable::AddColumnBlob(IN const AString &strFieldName, IN IMS_SINT32 nSize)
{
    if (!CheckIfSameFieldName(strFieldName))
    {
        return IMS_FALSE;
    }

    CONTENT_FIELD stField;

    stField.strFieldName = strFieldName.GetStr();
    stField.eFieldType = CONTENT_FIELD_BLOB;
    stField.nSize = nSize;

    if (!objFieldList.Append(stField))
    {
        return IMS_FALSE;
    }

    return IMS_TRUE;
}
#endif
//hsyun - end

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL OsContentTable::AddColumnReal(IN const AString &strFieldName)
{
    if (!CheckIfSameFieldName(strFieldName))
    {
        return IMS_FALSE;
    }

    CONTENT_FIELD stField;

    stField.strFieldName = strFieldName.GetStr();
    stField.eFieldType = CONTENT_FIELD_REAL;

    if (!objFieldList.Append(stField))
    {
        return IMS_FALSE;
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL OsContentTable::DeleteColumn(IN const AString &strFieldName)
{
    for (IMS_UINT32 i = 0; i < objFieldList.GetSize(); ++i)
    {
        const CONTENT_FIELD& stField = objFieldList.GetAt(i);

        if (strFieldName.Equals(stField.strFieldName))
        {
            objFieldList.RemoveAt(i);
            return IMS_TRUE;
        }
    }

    return IMS_FALSE;
}

/*

Remarks

*/
PUBLIC VIRTUAL
void OsContentTable::SetPrimaryKey(IN const AString& strFieldName)
{
    strPrimaryKey = strFieldName;
}

/*

Remarks

*/
PUBLIC VIRTUAL
const AString& OsContentTable::GetPrimaryKey() const
{
    return strPrimaryKey;
}

/*

Remarks

*/
PUBLIC VIRTUAL
void OsContentTable::Clear()
{
    objFieldList.Clear();
}

/*

Remarks

*/
PUBLIC
CONTENT_FIELD_TYPE OsContentTable::GetColumnType(IN const AString &strFieldName)
{
    for (IMS_UINT32 i = 0; i < objFieldList.GetSize(); ++i)
    {
        const CONTENT_FIELD& stField = objFieldList.GetAt(i);

        if (strFieldName.Equals(stField.strFieldName))
        {
            return stField.eFieldType;
        }
    }

    return CONTENT_FIELD_INVALID;
}

/*

Remarks

*/
PUBLIC
const IMSList<CONTENT_FIELD>& OsContentTable::GetFieldList() const
{
    return objFieldList;
}

/*

Remarks

*/
PRIVATE
IMS_BOOL OsContentTable::CheckIfSameFieldName(IN const AString &strFieldName)
{
    for (IMS_UINT32 i = 0; i < objFieldList.GetSize(); ++i)
    {
        const CONTENT_FIELD& stField = objFieldList.GetAt(i);

        if (strFieldName.Equals(stField.strFieldName))
        {
            return IMS_FALSE;
        }
    }

    return IMS_TRUE;
}
