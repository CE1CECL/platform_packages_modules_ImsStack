/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20120105  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _SERVICE_IMS_UTIL_H_
#define _SERVICE_IMS_UTIL_H_

#include "IImsPrivateProperty.h"
#include "ISystemUtil.h"
#include "ISystemProperty.h"
#include "IZLib.h"

class UtilService
{
private:
    UtilService();
    ~UtilService();

public:
    IImsPrivateProperty* GetPrivateProperty();
    ISystemUtil* GetSystemUtil();
    // LOG_EXCLUDING_SERVER_INFO
    ISystemProperty* GetSystemProperty();
    IZLib* GetZLib();

    // Sets the debugging flag
    void SetDebugOn(IN IMS_BOOL bDebugOn);

    static UtilService* GetUtilService();

    // Return value: strOutput (user mode & config-debug-off), strInput (non-user mode)
    static const AString& GetLogString(IN const AString &strInput,
            IN_OUT AString &strOutput, IN IMS_SINT32 nOutSize,
            IN IMS_CHAR cDelimiter = 0 /* no delimiter */);
};

//-------------------------------------------------------------------------------------------------
#define IMS_UTIL_SYS_PROP_IS_SERVER_INFO_HIDDEN_IN_LOG() \
        UtilService::GetUtilService()->GetSystemProperty()->IsServerInfoHiddenInLog()

#define IMS_UTIL_SYS_PROP_IS_DEBUG_MODE() \
        UtilService::GetUtilService()->GetSystemProperty()->IsDebugMode()

#define IMS_UTIL_SYS_PROP_IS_USER_MODE() \
        UtilService::GetUtilService()->GetSystemProperty()->IsUserMode()

#define IMS_UTIL_SYS_PROP_SET_DEBUG_ON(bDebugOn) \
        UtilService::GetUtilService()->SetDebugOn(bDebugOn)

#define IMS_UTIL_ZLIB_Compress(DATA, COMP_DATA) \
        UtilService::GetUtilService()->GetZLib()->Compress(DATA, COMP_DATA)

#define IMS_UTIL_ZLIB_Uncompress(COMP_DATA, DATA) \
        UtilService::GetUtilService()->GetZLib()->Uncompress(COMP_DATA, DATA)

#endif // _SERVICE_IMS_UTIL_H_
