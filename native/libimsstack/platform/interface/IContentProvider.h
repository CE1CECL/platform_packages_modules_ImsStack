/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100517  eunwoo.cho@               Created
    20101010  hwangoo.park@             Added/Modified the method
                                        MOD: CreateTable() / DropTable()
                                        ADD: AddTable() / RemoveTable()
                                        REM: RecognizeTable()
    </table>

Description

*/

#ifndef _INTERFACE_CONTENT_PROVIDER_H_
#define _INTERFACE_CONTENT_PROVIDER_H_

#include "IContentCursor.h"
#include "IContentTable.h"

#include "AString.h"

typedef enum _QueryOption
{
    OPTION_NULL = 0,
    OPTION_COLLATE_NOCASE = 1,
} QueryOption;

class IContentProvider
{
public:
    // Operations for table
    virtual IMS_BOOL CreateTable(IN const AString& strTable,
            IN IContentTable* piContentValues) = 0;
    virtual IContentTable* CreateTableEx(IN const AString &strTable,
            IN const AString &strColumn = AString::ConstNull()) = 0;
    //hsyun - start 111004
    virtual IContentTable* CreateTableEx(IN const AString &strTable,
            IN const AString &strColumn, IN const AString &strValue) = 0;
    //hsyun - end
    virtual void DropTable(IN const AString& strTable) = 0;
    virtual IMS_BOOL AddTable(IN const AString& strTable, IN IContentTable* piContentTable) = 0;
    virtual void RemoveTable(IN const AString& strTable) = 0;

    // "insert" operations
    virtual IMS_BOOL InsertData(IN const AString& strTable, IN IContentCursor* piCursor) = 0;

    // "update" operations
    virtual IMS_BOOL UpdateData(IN const AString& strTable,
            IN IContentCursor* piCursorSet, IN IContentCursor* piCursorWhere) = 0;
    virtual IMS_BOOL UpdateData(IN const AString& strTable,
            IN IContentCursor* piCursorSet, IN const AString& strWhere) = 0;

    // "delete" operations
    virtual IMS_BOOL DeleteData(IN const AString& strTable,
            IN const AString& strWhere) = 0;
    virtual IMS_BOOL DeleteData(IN const AString& strTable,
            IN IContentCursor* piCursor) = 0;

    // "select" operations
    //4 "select * from strTable where strWhere"
    virtual IContentCursor* ManagedQuery(IN const AString& strTable,
            IN const AString& strWhere) = 0;
    //4 "select objColumnNames from strTable where strWhere"
    virtual IContentCursor* ManagedQuery(IN const AString& strTable,
            IN const IMSList<AString>& objColumnNames, IN const AString& strWhere) = 0;
    //4 "select * from strTable where objCursor"
    virtual IContentCursor* ManagedQuery(IN const AString& strTable,
            IN IContentCursor* piCursor) = 0;
    //4 "select objColumnNames from strTable where objCursor"
    virtual IContentCursor* ManagedQuery(IN const AString& strTable,
            IN const IMSList<AString>& objColumnNames, IN IContentCursor* piCursor) = 0;

    //4 If option is set, that option will not be applied from the second query.
    virtual void SetQueryOption(IN QueryOption _eQueryOption) = 0;
};

#endif // _INTERFACE_CONTENT_PROVIDER_H_
