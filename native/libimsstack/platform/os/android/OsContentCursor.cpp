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
#include "OsContentCursor.h"
#include "OsContentValues.h"
#include "ServiceMemory.h"
#include "ServiceTrace.h"

__IMS_TRACE_TAG_ADAPT__;

PUBLIC
OsContentCursor::OsContentCursor()
    : pThisCursor(IMS_NULL)
    , pContentValues(IMS_NULL)
    , objElementList(IMSList<OsContentElement>())
{
    pThisCursor = this;
}

PUBLIC
OsContentCursor::OsContentCursor(IN const OsContentCursor &objRHS)
    : pThisCursor(IMS_NULL)
    , pContentValues(objRHS.pContentValues)
    , objElementList(objRHS.objElementList)
{
    pThisCursor = this;
}

PUBLIC VIRTUAL
OsContentCursor::~OsContentCursor()
{
    objElementList.Clear();

#ifdef __IMS_DEBUG__
    IMS_TRACE_D("Destructor :: OsContentCursor", 0, 0, 0);
#endif
}

PUBLIC
OsContentCursor& OsContentCursor::operator=(IN const OsContentCursor &objRHS)
{
    if (this != &objRHS)
    {
        this->pThisCursor = this;
        this->pContentValues = objRHS.pContentValues;
        this->objElementList = objRHS.objElementList;
    }

    return (*this);
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_SINT32 OsContentCursor::GetInt(IN const AString &strFieldName,
        OUT IMS_BOOL *pbOK /* = IMS_NULL */) const
{
    if (pThisCursor == IMS_NULL)
    {
        IMS_TRACE_D("No cursor", 0, 0, 0);

        if (pbOK != IMS_NULL)
        {
            (*pbOK) = IMS_FALSE;
        }

        return (-1);
    }

    IMSList<OsContentElement>* pList = pThisCursor->GetContentList();

    for (IMS_UINT32 i = 0; i < pList->GetSize(); ++i)
    {
        const OsContentElement& objElement = pList->GetAt(i);

        if (strFieldName.Equals(objElement.GetFieldName()))
        {
            if (pbOK != IMS_NULL)
            {
                (*pbOK) = IMS_TRUE;
            }

            return objElement.GetInteger();
        }
    }

    if (pbOK != IMS_NULL)
    {
        (*pbOK) = IMS_FALSE;
    }

    return (-1);
}

/*

Remarks

*/
PUBLIC VIRTUAL
const AString& OsContentCursor::GetString(IN const AString &strFieldName,
        OUT IMS_BOOL *pbOK /* = IMS_NULL */) const
{
    if (pThisCursor == IMS_NULL)
    {
        IMS_TRACE_D("No cursor", 0, 0, 0);

        if (pbOK != IMS_NULL)
        {
            (*pbOK) = IMS_FALSE;
        }

        return AString::ConstNull();
    }

    IMSList<OsContentElement>* pList = pThisCursor->GetContentList();

    for (IMS_UINT32 i = 0; i < pList->GetSize(); ++i)
    {
        const OsContentElement& objElement = pList->GetAt(i);

        if (strFieldName.Equals(objElement.GetFieldName()))
        {
            if (pbOK != IMS_NULL)
            {
                (*pbOK) = IMS_TRUE;
            }

            return objElement.GetText();
        }
    }

    if (pbOK != IMS_NULL)
    {
        (*pbOK) = IMS_FALSE;
    }

    return AString::ConstNull();
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_FLOAT OsContentCursor::GetFloat(IN const AString &strFieldName,
        OUT IMS_BOOL *pbOK /* = IMS_NULL */) const
{
    if (pThisCursor == IMS_NULL)
    {
        IMS_TRACE_D("No cursor", 0, 0, 0);

        if (pbOK != IMS_NULL)
        {
            (*pbOK) = IMS_FALSE;
        }

        return 0;
    }
    IMSList<OsContentElement>* pList = pThisCursor->GetContentList();

    for (IMS_UINT32 i = 0; i < pList->GetSize(); ++i)
    {
        const OsContentElement& objElement = pList->GetAt(i);

        if (strFieldName.Equals(objElement.GetFieldName()))
        {
            if (pbOK != IMS_NULL)
            {
                (*pbOK) = IMS_TRUE;
            }

            return objElement.GetReal();
        }
    }

    if (pbOK != IMS_NULL)
    {
        (*pbOK) = IMS_FALSE;
    }

    return 0;
}

#ifdef __IMS_DB_BINARY_FIELD__
/*

Remarks

*/
PUBLIC VIRTUAL
const ByteArray& OsContentCursor::GetBinary(IN const AString &strFieldName,
        OUT IMS_BOOL *pbOK /* = IMS_NULL */) const
{
    if (pThisCursor == IMS_NULL)
    {
        IMS_TRACE_D("No cursor", 0, 0, 0);

        if (pbOK != IMS_NULL)
        {
            (*pbOK) = IMS_FALSE;
        }

        return ByteArray::ConstNull();
    }

    IMSList<OsContentElement>* pList = pThisCursor->GetContentList();

    for (IMS_UINT32 i = 0; i < pList->GetSize(); ++i)
    {
        const OsContentElement& objElement = pList->GetAt(i);

        if (strFieldName.Equals(objElement.GetFieldName()))
        {
            if (pbOK != IMS_NULL)
            {
                (*pbOK) = IMS_TRUE;
            }

            return objElement.GetBlob();
        }
    }

    if (pbOK != IMS_NULL)
    {
        (*pbOK) = IMS_FALSE;
    }

    return ByteArray::ConstNull();
}

#endif

#ifdef __IMS_DB_BINARY_FIELD__
/*

Remarks

*/
//hsyun - start 110902
PUBLIC VIRTUAL
IMS_BOOL OsContentCursor::GetBinary(IN const AString &strFieldName,
        OUT ByteArray& achOutBinary, OUT IMS_SINT32 &nLength) const
{
    achOutBinary = ByteArray::ConstNull();
    nLength = 0;

    if (pThisCursor == IMS_NULL)
    {
        IMS_TRACE_D("No cursor", 0, 0, 0);
        return IMS_FALSE;
    }

    IMSList<OsContentElement>* pList = pThisCursor->GetContentList();

    for (IMS_UINT32 i = 0; i < pList->GetSize(); ++i)
    {
        OsContentElement &objElement = pList->GetAt(i);

        if (strFieldName.Equals(objElement.GetFieldName()))
        {
            return objElement.GetBlob(achOutBinary, nLength);
        }
    }

    return IMS_FALSE;
}
#endif

//hsyun - end

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL OsContentCursor::PutValue(IN const AString &strFieldName, IN IMS_SINT32 nValue)
{
    if (pThisCursor == IMS_NULL)
    {
        IMS_TRACE_D("No cursor", 0, 0, 0);
        return IMS_FALSE;
    }

    IMSList<OsContentElement>* pList = pThisCursor->GetContentList();

    for (IMS_UINT32 i = 0; i < pList->GetSize(); ++i)
    {
        OsContentElement &objElement = pList->GetAt(i);

        if (strFieldName.Equals(objElement.GetFieldName()))
        {
            objElement.Put(nValue);

            return IMS_TRUE;
        }
    }

    OsContentElement objElement;

    objElement.SetFieldName(strFieldName);
    objElement.Put(nValue);

    return pList->Append(objElement);
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL OsContentCursor::PutValue(IN const AString &strFieldName, IN IMS_FLOAT fValue)
{
    if (pThisCursor == IMS_NULL)
    {
        IMS_TRACE_D("No cursor", 0, 0, 0);
        return IMS_FALSE;
    }

    IMSList<OsContentElement>* pList = pThisCursor->GetContentList();

    for (IMS_UINT32 i = 0; i < pList->GetSize(); ++i)
    {
        OsContentElement &objElement = pList->GetAt(i);

        if (strFieldName.Equals(objElement.GetFieldName()))
        {
            objElement.Put(fValue);

            return IMS_TRUE;
        }
    }

    OsContentElement objElement;

    objElement.SetFieldName(strFieldName);
    objElement.Put(fValue);

    return pList->Append(objElement);
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL OsContentCursor::PutValue(IN const AString &strFieldName, IN const AString& objValue)
{
    if (pThisCursor == IMS_NULL)
    {
        IMS_TRACE_D("No cursor", 0, 0, 0);
        return IMS_FALSE;
    }

    IMSList<OsContentElement>* pList = pThisCursor->GetContentList();

    for (IMS_UINT32 i = 0; i < pList->GetSize(); ++i)
    {
        OsContentElement &objElement = pList->GetAt(i);

        if (strFieldName.Equals(objElement.GetFieldName()))
        {
            objElement.Put(objValue);

            return IMS_TRUE;
        }
    }

    OsContentElement objElement;

    objElement.SetFieldName(strFieldName);
    objElement.Put(objValue);

    return pList->Append(objElement);
}

#ifdef __IMS_DB_BINARY_FIELD__
/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL OsContentCursor::PutValue(IN const AString &strFieldName, IN const ByteArray &objValue)
{
    if (pThisCursor == IMS_NULL)
    {
        IMS_TRACE_D("No cursor", 0, 0, 0);
        return IMS_FALSE;
    }

    IMSList<OsContentElement>* pList = pThisCursor->GetContentList();

    for (IMS_UINT32 i = 0; i < pList->GetSize(); ++i)
    {
        OsContentElement &objElement = pList->GetAt(i);

        if (strFieldName.Equals(objElement.GetFieldName()))
        {
            objElement.Put(objValue);

            return IMS_TRUE;
        }
    }

    OsContentElement objElement;

    objElement.SetFieldName(strFieldName);
    objElement.Put(objValue);

    return pList->Append(objElement);
}

/*

Remarks

*/
//hsyun - start 110902
PUBLIC VIRTUAL
IMS_BOOL OsContentCursor::PutValue(IN const AString &strFieldName,
        IN const ByteArray &objValue, IN IMS_SINT32 nLength)
{
    if (pThisCursor == IMS_NULL)
    {
        IMS_TRACE_D("No cursor", 0, 0, 0);
        return IMS_FALSE;
    }

    if (!CheckIfSameFieldName(strFieldName))
    {
        return IMS_FALSE;
    }

    IMSList<OsContentElement>* pList = pThisCursor->GetContentList();

    OsContentElement objElement;

    objElement.SetFieldName( strFieldName );
    objElement.Put( objValue, nLength );

    return pList->Append( objElement );
}

#endif
//hsyun - end

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL OsContentCursor::Remove(IN const AString& strFieldName)
{
    if (pThisCursor == IMS_NULL)
    {
        IMS_TRACE_D("No cursor", 0, 0, 0);
        return IMS_FALSE;
    }

    IMSList<OsContentElement>* pList = pThisCursor->GetContentList();

    for (IMS_UINT32 i = 0; i < pList->GetSize(); ++i)
    {
        OsContentElement& objElement = pList->GetAt(i);

        if (strFieldName.Equals(objElement.GetFieldName()))
        {
            return pList->RemoveAt(i);
        }
    }

    return IMS_FALSE;
}

/*

Remarks

*/
PUBLIC VIRTUAL
void OsContentCursor::Clear()
{
    if (pThisCursor != IMS_NULL)
    {
        IMSList<OsContentElement>* pList = pThisCursor->GetContentList();
        pList->Clear();
    }

    objElementList.Clear();
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL OsContentCursor::IsFirstCursor() const
{
    if (pContentValues == IMS_NULL)
    {
        return IMS_FALSE;
    }

    return pContentValues->IsFirstCursor();
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL OsContentCursor::IsLastCursor() const
{
    if (pContentValues == IMS_NULL)
    {
        return IMS_FALSE;
    }

    return pContentValues->IsLastCursor();
}

/*

Remarks

*/
PUBLIC VIRTUAL
IContentCursor* OsContentCursor::MoveToFirst()
{
    if (pContentValues == IMS_NULL)
    {
        return IMS_NULL;
    }

    OsContentCursor* pCursor = pContentValues->GetFirstCursor();

    if (pCursor != IMS_NULL)
    {
        pThisCursor = pCursor;
        pThisCursor->SetContentValues(pContentValues);
    }

    return pThisCursor;
}

/*

Remarks

*/
PUBLIC VIRTUAL
IContentCursor* OsContentCursor::MoveToLast()
{
    if (pContentValues == IMS_NULL)
    {
        return IMS_NULL;
    }

    OsContentCursor *pCursor = pContentValues->GetLastCursor();

    if (pCursor != IMS_NULL)
    {
        pThisCursor = pCursor;
        pThisCursor->SetContentValues(pContentValues);
    }

    return pThisCursor;
}

/*

Remarks

*/
PUBLIC VIRTUAL
IContentCursor* OsContentCursor::MoveToNext()
{
    if (pContentValues == IMS_NULL)
    {
        return IMS_NULL;
    }

    OsContentCursor *pCursor = pContentValues->GetNextCursor();

    if (pCursor != IMS_NULL)
    {
        pThisCursor = pCursor;
        pThisCursor->SetContentValues(pContentValues);
    }

    return pThisCursor;
}

/*

Remarks

*/
PUBLIC
IMSList<OsContentElement>* OsContentCursor::GetContentList()
{
    return &objElementList;
}

/*

Remarks

*/
PUBLIC
void OsContentCursor::SetThisCursor(IN OsContentCursor* _pThisCursor)
{
    pThisCursor = _pThisCursor;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL OsContentCursor::CheckIfSameFieldName(IN const AString &strFieldName)
{
    IMSList<OsContentElement>* pList = pThisCursor->GetContentList();

    for (IMS_UINT32 i = 0; i < pList->GetSize(); ++i)
    {
        OsContentElement& objElement = pList->GetAt(i);

        if (strFieldName.Equals(objElement.GetFieldName()))
        {
            return IMS_FALSE;
        }
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
void OsContentCursor::SetContentValues(IN OsContentValues* _pContentValues)
{
    pContentValues = _pContentValues;
}
