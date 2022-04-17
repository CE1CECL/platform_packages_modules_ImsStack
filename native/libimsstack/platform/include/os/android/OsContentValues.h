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
#ifndef OS_CONTENT_VALUES_H_
#define OS_CONTENT_VALUES_H_

#include "OsContentCursor.h"
/*
    - OsContentCursor List
    - A Table
Ex)
                             Age        | Weight    | Name
    (OsContentCursor)  33        | 70        | "Amy"
    (OsContentCursor)    30        | 80        | "James"
*/

class OsContentValues
{
public:
    OsContentValues();
    virtual ~OsContentValues();

public:
    inline IMS_SINT32 GetCurrentIndex()
    { return nCursorIndex; }
    IMS_SINT32 GetCursorSize() const;

    IMS_BOOL IsFirstCursor() const;
    IMS_BOOL IsLastCursor() const;

    OsContentCursor* GetNextCursor();
    OsContentCursor* GetFirstCursor();
    OsContentCursor* GetLastCursor();

    OsContentCursor* SetCurrentCursorIndex(IN IMS_SINT32 nIndex);
    OsContentCursor* GetCursor(IN IMS_SINT32 nIndex);

    IMS_BOOL InsertCursor(IN OsContentCursor& objCursor);
    IMS_BOOL DeleteCursor(IN IMS_SINT32 nIndex);
    void Clear();

private:
    IMS_SINT32 nCursorIndex;
    IMSList<OsContentCursor> objCursorList;
};

#endif
