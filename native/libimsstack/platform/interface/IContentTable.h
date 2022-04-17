/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100517  eunwoo.cho@
    </table>

    Description

*/

#ifndef _INTERFACE_CONTENT_TABLE_H_
#define _INTERFACE_CONTENT_TABLE_H_

#include "AString.h"

/*
    --------------------------------------
    FILED_TYPE | FIELD_NAME
    --------------------------------------
Ex)
    Integer    |    "Age"
    Text    |    "Name"
*/
class IContentTable
{
public:
    virtual IMS_BOOL AddColumnInteger(IN const AString &strFieldName) = 0;
    virtual IMS_BOOL AddColumnText(IN const AString &strFieldName) = 0;
#ifdef __IMS_DB_BINARY_FIELD__
    virtual IMS_BOOL AddColumnBlob(IN const AString &strFieldName) = 0;
#endif
    virtual IMS_BOOL AddColumnReal(IN const AString &strFieldName) = 0;

    virtual IMS_BOOL DeleteColumn(IN const AString &strFieldName) = 0;

    virtual void SetPrimaryKey(IN const AString& strFieldName) = 0;

    virtual void Clear() = 0;
};

#endif // _INTERFACE_CONTENT_TABLE_H_
