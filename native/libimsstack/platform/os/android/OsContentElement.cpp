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
#include "OsContentElement.h"
#include "ServiceMemory.h"
#include "ServiceTrace.h"

__IMS_TRACE_TAG_ADAPT__;

PUBLIC
OsContentElement::OsContentElement()
    : nValue(-1)
    , fValue(0)
    , strValue(AString::ConstNull())
#ifdef __IMS_DB_BINARY_FIELD__
    , objBlob(ByteArray::ConstNull())
    , nBlobLength(0)
#endif
{
    stContentField.eFieldType = CONTENT_FIELD_INVALID;
    stContentField.strFieldName = AString::ConstNull();
}

PUBLIC
OsContentElement::OsContentElement(IN const OsContentElement &objRHS)
    : nValue( objRHS.nValue )
    , fValue( objRHS.fValue )
    , strValue( objRHS.strValue )
#ifdef __IMS_DB_BINARY_FIELD__
    , objBlob( objRHS.objBlob )
    , nBlobLength(objRHS.nBlobLength)
#endif
{
    stContentField.eFieldType = objRHS.stContentField.eFieldType;
    stContentField.strFieldName = objRHS.stContentField.strFieldName;
}

PUBLIC VIRTUAL
OsContentElement::~OsContentElement()
{
}

/*

Remarks

*/
PUBLIC
OsContentElement& OsContentElement::operator=(IN const OsContentElement &objRHS)
{
    if (this != &objRHS)
    {
        stContentField.eFieldType = objRHS.stContentField.eFieldType;
        stContentField.strFieldName = objRHS.stContentField.strFieldName;

        nValue = objRHS.nValue;
        fValue = objRHS.fValue;
        strValue = objRHS.strValue;
#ifdef __IMS_DB_BINARY_FIELD__
        objBlob = objRHS.objBlob;
        nBlobLength = objRHS.nBlobLength;
#endif
    }

    return (*this);
}


#ifdef __IMS_DB_BINARY_FIELD__

/*

Remarks

*/
//hsyun - start 110902
PUBLIC
IMS_BOOL OsContentElement::GetBlob(OUT ByteArray& achOutBinary, OUT IMS_SINT32 &nLength)
{
    achOutBinary = objBlob;
    nLength = nBlobLength;
    return IMS_TRUE;
}
#endif

//hsyun - end

/*

Remarks

*/
PUBLIC
void OsContentElement::SetFieldName(IN const AString& strFieldName)
{
    stContentField.strFieldName = strFieldName;
}

/*

Remarks

*/
PUBLIC
void OsContentElement::SetFieldType(IN CONTENT_FIELD_TYPE eFieldType)
{
    stContentField.eFieldType = eFieldType;
}

/*

Remarks

*/
PUBLIC
void OsContentElement::Put(IN IMS_SINT32 nValue)
{
    stContentField.eFieldType = CONTENT_FIELD_INTEGER;
    this->nValue = nValue;
}

/*

Remarks

*/
PUBLIC
void OsContentElement::Put(IN IMS_FLOAT fValue)
{
    stContentField.eFieldType = CONTENT_FIELD_REAL;
    this->fValue = fValue;
}

/*

Remarks

*/
PUBLIC
void OsContentElement::Put(IN const AString &strValue)
{
    stContentField.eFieldType = CONTENT_FIELD_TEXT;
    this->strValue = strValue;
}

#ifdef __IMS_DB_BINARY_FIELD__
/*

Remarks

*/
PUBLIC
void OsContentElement::Put(IN const ByteArray &objBlob)
{
    stContentField.eFieldType = CONTENT_FIELD_BLOB;
    this->objBlob = objBlob;
}
#endif

#ifdef __IMS_DB_BINARY_FIELD__
/*

Remarks

*/
//hsyun - start 110902
PUBLIC
void OsContentElement::Put(IN const ByteArray &objBlob , IN IMS_SINT32 nLength)
{
    stContentField.eFieldType = CONTENT_FIELD_BLOB;
    this->objBlob = objBlob;
    this->nBlobLength = nLength;
}
#endif
//hsyun - end
