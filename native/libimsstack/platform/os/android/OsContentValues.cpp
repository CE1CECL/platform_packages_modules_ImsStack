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
#include "OsContentValues.h"
#include "ServiceMemory.h"
#include "ServiceTrace.h"

__IMS_TRACE_TAG_ADAPT__;

PUBLIC
OsContentValues::OsContentValues()
    : nCursorIndex(-1)
    , objCursorList(IMSList<OsContentCursor>())
{
}

PUBLIC VIRTUAL
OsContentValues::~OsContentValues()
{
    objCursorList.Clear();

#ifdef __IMS_DEBUG__
    IMS_TRACE_D("Destructor :: OsContentValues", 0, 0, 0);
#endif
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 OsContentValues::GetCursorSize() const
{
    return objCursorList.GetSize();
}

/*

Remarks

*/
PUBLIC
IMS_BOOL OsContentValues::IsFirstCursor() const
{
    return (nCursorIndex == 0);
}

/*

Remarks

*/
PUBLIC
IMS_BOOL OsContentValues::IsLastCursor() const
{
    return ((GetCursorSize() - 1) == nCursorIndex);
}

/*

Remarks

*/
PUBLIC
OsContentCursor* OsContentValues::GetNextCursor()
{
    if ((nCursorIndex + 1) >= GetCursorSize())
    {
        IMS_TRACE_D("No more cursor", 0, 0, 0);
        return IMS_NULL;
    }

    if (GetCursorSize() == 0)
    {
        IMS_TRACE_D("No cursor", 0, 0, 0);
        return IMS_NULL;
    }

    return SetCurrentCursorIndex(nCursorIndex + 1);
}

/*

Remarks

*/
PUBLIC
OsContentCursor* OsContentValues::GetFirstCursor()
{
    if (GetCursorSize() == 0)
    {
        IMS_TRACE_D("No cursor", 0, 0, 0);
        return IMS_NULL;
    }

    return SetCurrentCursorIndex(0);
}

/*

Remarks

*/
PUBLIC
OsContentCursor* OsContentValues::GetLastCursor()
{
    if (GetCursorSize() == 0)
    {
        IMS_TRACE_D("No cursor", 0, 0, 0);
        return IMS_NULL;
    }

    return SetCurrentCursorIndex(GetCursorSize() - 1);
}

/*

Remarks

*/
PUBLIC
OsContentCursor* OsContentValues::SetCurrentCursorIndex(IN IMS_SINT32 nIndex)
{
    OsContentCursor *pCursor = GetCursor(nIndex);

    if (pCursor == IMS_NULL)
    {
        return IMS_NULL;
    }

    nCursorIndex = nIndex;

    pCursor->SetContentValues(this);

    return pCursor;
}

/*

Remarks

*/
PUBLIC
OsContentCursor* OsContentValues::GetCursor(IN IMS_SINT32 nIndex)
{
    if ((nIndex < 0) || (nIndex >= GetCursorSize()))
    {
        IMS_TRACE_D("Out of index(size=%d, index=%d)", objCursorList.GetSize(), nIndex, 0);
        return IMS_NULL;
    }

    OsContentCursor &objCursor = objCursorList.GetAt(nIndex);

    return &objCursor;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL OsContentValues::InsertCursor(IN OsContentCursor& objCursor)
{
    if (!objCursorList.Append(objCursor))
    {
        return IMS_FALSE;
    }

    SetCurrentCursorIndex(GetCursorSize() - 1);

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL OsContentValues::DeleteCursor(IN IMS_SINT32 nIndex)
{
    if (!objCursorList.RemoveAt(nIndex))
    {
        return IMS_FALSE;
    }

    if (nCursorIndex == nIndex)
    {
        nCursorIndex = nCursorIndex - 1;
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
void OsContentValues::Clear()
{
    objCursorList.Clear();
}
