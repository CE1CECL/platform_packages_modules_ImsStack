/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100517  eunwoo.cho@               Created
    20101010  hwangoo.park@             Modified the input/output parameter in Get & Put method
    </table>

    Description

*/

#ifndef _INTERFACE_CONTENT_CURSOR_H_
#define _INTERFACE_CONTENT_CURSOR_H_

#include "ByteArray.h"

class IContentCursor
{
public:
    // Get Values
    virtual IMS_SINT32 GetInt(IN const AString &strFieldName,
            OUT IMS_BOOL *pbOK = IMS_NULL) const = 0;
    virtual const AString& GetString(IN const AString &strFieldName,
            OUT IMS_BOOL *pbOK = IMS_NULL) const = 0;
    virtual IMS_FLOAT GetFloat(IN const AString &strFieldName,
            OUT IMS_BOOL *pbOK = IMS_NULL) const = 0;
#ifdef __IMS_DB_BINARY_FIELD__
    virtual const ByteArray& GetBinary(IN const AString &strFieldName,
            OUT IMS_BOOL *pbOK = IMS_NULL) const = 0;
    virtual IMS_BOOL GetBinary(IN const AString &strFieldName,
            OUT ByteArray& achOutBinary, OUT IMS_SINT32 &nLength) const = 0;
#endif
    // for update field value
    virtual IMS_BOOL PutValue(IN const AString &strFieldName, IN IMS_SINT32 nValue) = 0;
    virtual IMS_BOOL PutValue(IN const AString &strFieldName, IN IMS_FLOAT fValue) = 0;
    virtual IMS_BOOL PutValue(IN const AString &strFieldName, IN const AString &strValue) = 0;
#ifdef __IMS_DB_BINARY_FIELD__
    virtual IMS_BOOL PutValue(IN const AString &strFieldName, IN const ByteArray &objValue) = 0;
    virtual IMS_BOOL PutValue(IN const AString &strFieldName,
            IN const ByteArray &objValue, IN IMS_SINT32 nLength) = 0;
#endif
    virtual IMS_BOOL Remove(IN const AString &strFieldName) = 0;

    virtual IMS_BOOL IsFirstCursor() const = 0;
    virtual IMS_BOOL IsLastCursor() const = 0;

    virtual IContentCursor* MoveToFirst() = 0;
    virtual IContentCursor* MoveToLast() = 0;
    virtual IContentCursor* MoveToNext() = 0;

    virtual void Clear() = 0;
};

#endif // _INTERFACE_CONTENT_CURSOR_H_
