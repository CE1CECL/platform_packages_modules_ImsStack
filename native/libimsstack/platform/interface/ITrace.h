
/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090819  YR@                       Created
    </table>

    Description

*/

#ifndef _INTERFACE_IMS_TRACE_H_
#define _INTERFACE_IMS_TRACE_H_

#include <stdarg.h>

#include "ITraceTag.h"

class ITrace
{
public:
    virtual void Out(IN const IMS_CHAR *pszFormat, ...) = 0;

    virtual void Out(IN IMS_SINT32 nCategory, IN const IMS_CHAR *pszTag, IN IMS_UINT32 nModule,
            IN const IMS_CHAR *pszFormat, ...) = 0;

    virtual void OutE(IN IMS_SINT32 nErrorCode, IN const IMS_CHAR *pszFunction, IN IMS_UINT16 nLine,
            IN const IMS_CHAR *pszTag, IN IMS_UINT32 nModule,
            IN const IMS_CHAR *pszFormat, ...) = 0;

    //// For C-style adaptation (ex. protocol stacks, ...)
    virtual void OutV(IN IMS_SINT32 nCategory, IN const IMS_CHAR *pszTag, IN IMS_UINT32 nModule,
            IN const IMS_CHAR *pszFormat, IN va_list args) = 0;

    //// For a large TEXT message (HTTP, MSRP, SDP, SIP, XML, ...)
    virtual void OutText(IN IMS_UINT32 nModule, IN IMS_SINT32 nType,
            IN const IMS_CHAR *pszDescription, IN const IMS_CHAR *pszText,
            IN IMS_UINT32 nTextSize, IN IMS_BOOL bBinaryBody = IMS_FALSE) = 0;

    //// For checking Trace-enabed
    virtual IMS_BOOL IsTraceEnabled(IN IMS_SINT32 nCategory, IN IMS_UINT32 nModule) = 0;

public:
    // Category of TRACE
    enum
    {
        CAT_D = 0x44, // 'D' : debug
        CAT_E = 0x45, // 'E' : error
        CAT_I = 0x49, // 'I' : information

        CAT_E_BASE = 0xFF
    };

    // Category of Text Message
    enum
    {
        TEXT_ANY = 0,
        TEXT_SDP,
        TEXT_SIP,
        TEXT_XML,
        TEXT_MAX
    };
};

#endif // _INTERFACE_IMS_TRACE_H_
