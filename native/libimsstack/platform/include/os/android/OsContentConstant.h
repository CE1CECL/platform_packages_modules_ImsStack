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
#ifndef OS_CONTENT_CONSTANT_H_
#define OS_CONTENT_CONSTANT_H_

#include "AString.h"

#define MAX_FIELD_NAME 30
#define MAX_SQLQUERY_LENGTH 512
#define DEFAULT_DATABASE_NAME     "AndroidDefault.db"

typedef enum
{
    CONTENT_ORDER_INVALID = -1,
    CONTENT_ORDER_DESCEND = 0,
    CONTENT_ORDER_ASCEND,
    CONTENT_ORDER_MAX
} CONTENT_ORDER_TYPE;

typedef enum
{
    SQL_QUERY_INVALID = -1,
    SQL_QUERY_CREATE,
    SQL_QUERY_DROP,
    SQL_QUERY_INSERT,
    SQL_QUERY_DELETE,
    SQL_QUERY_UPDATE,
    SQL_QUERY_SELECT,
    SQL_QUERY_MAX,
} SQL_QUERY_TYPE;

typedef enum
{
    CONTENT_FIELD_INVALID = -1,
    CONTENT_FIELD_INTEGER = 0,
    CONTENT_FIELD_TEXT,
    CONTENT_FIELD_REAL,
#ifdef __IMS_DB_BINARY_FIELD__
    CONTENT_FIELD_BLOB,
#endif
    CONTENT_FIELD_MAX
} CONTENT_FIELD_TYPE;

struct CONTENT_FIELD
{
    CONTENT_FIELD_TYPE eFieldType;
    AString strFieldName;

    IMS_SINT32 nSize;

    CONTENT_FIELD()
        : eFieldType(CONTENT_FIELD_TEXT)
        , strFieldName(AString::ConstNull())
        , nSize(0)
    {
    }

    CONTENT_FIELD(IN const CONTENT_FIELD& other)
        : eFieldType(other.eFieldType)
        , strFieldName(other.strFieldName)
        , nSize(other.nSize)
    {
    }

    CONTENT_FIELD& operator=(IN const CONTENT_FIELD& other)
    {
        if (this != &other)
        {
            eFieldType = other.eFieldType;
            strFieldName = other.strFieldName;
            nSize = other.nSize;
        }

        return (*this);
    }
};

#endif
