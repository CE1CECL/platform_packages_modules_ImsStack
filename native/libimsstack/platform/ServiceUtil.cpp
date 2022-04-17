/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20120105  hwangoo.park@             Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "ServiceUtil.h"
#include "ImsPrivateProperty.h"
#include "PlatformApi.h"
#include "PlatformFactory.h"

PRIVATE
UtilService::UtilService()
{
}

PRIVATE
UtilService::~UtilService()
{
}

/*

Remarks

*/
PUBLIC
IImsPrivateProperty* UtilService::GetPrivateProperty()
{
    ImsPrivateProperty *pPrivateProperty = ImsPrivateProperty::GetInstance();

    IMS_ASSERT(pPrivateProperty != IMS_NULL);

    return pPrivateProperty;
}

/*

Remarks

*/
PUBLIC
ISystemUtil* UtilService::GetSystemUtil()
{
    ISystemUtil *piSysUtil = PlatformFactory::GetSystemUtil();

    IMS_ASSERT(piSysUtil != IMS_NULL);

    return piSysUtil;
}

/*

Remarks

*/
PUBLIC
ISystemProperty* UtilService::GetSystemProperty()
{
    ISystemProperty *piSysProperty = PlatformFactory::GetSystemProperty();

    IMS_ASSERT(piSysProperty != IMS_NULL);

    return piSysProperty;
}

/*

Remarks

*/
PUBLIC
IZLib* UtilService::GetZLib()
{
    IZLib *pZLib = PlatformFactory::GetZLib();

    IMS_ASSERT(pZLib != IMS_NULL);

    return pZLib;
}

/*

Remarks

*/
PUBLIC
void UtilService::SetDebugOn(IN IMS_BOOL bDebugOn)
{
    PlatformApi::SetDebugOn(bDebugOn);
}

/*

Remarks

*/
PUBLIC GLOBAL
UtilService* UtilService::GetUtilService()
{
    static UtilService *pUtilService = IMS_NULL;

    if (pUtilService == IMS_NULL)
    {
        pUtilService = new UtilService();
    }

    return pUtilService;
}

/*

Remarks
 Return value: strOutput (user mode & config-debug-off), strInput (non-user mode)
*/
PUBLIC GLOBAL
const AString& UtilService::GetLogString(IN const AString &strInput,
        IN_OUT AString &strOutput, IN IMS_SINT32 nOutSize,
        IN IMS_CHAR cDelimiter /* = 0 */)
{
    if (IMS_UTIL_SYS_PROP_IS_DEBUG_MODE())
    {
        return strInput;
    }

    if (strInput.GetLength() == 0)
    {
        strOutput = "zzz";
        return strOutput;
    }

    IMS_SINT32 nIndex = AString::NPOS;

    if (cDelimiter > 0)
    {
        nIndex = strInput.GetIndexOf(cDelimiter);
    }

    IMS_BOOL bAddPostFix = IMS_TRUE;

    if (nIndex == AString::NPOS)
    {
        strOutput = strInput.GetSubStr(0, nOutSize);

        if (nOutSize >= strInput.GetLength())
        {
            bAddPostFix = IMS_FALSE;
        }
    }
    else
    {
        if ((nOutSize > 0) && (nOutSize < nIndex))
            strOutput = strInput.GetSubStr(0, nOutSize);
        else
            strOutput = strInput.GetSubStr(0, nIndex);
    }

    if (bAddPostFix)
    {
        strOutput.Append("xxx");
    }

    return strOutput;
}
