/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20150516  hwangoo.park@             Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "ServiceTrace.h"
#include "IConfigBuffer.h"
#include "ConfigLoader.h"
#include "ConfigUtils.h"

__IMS_TRACE_TAG_CONF__;

/*

Remarks

*/
LOCAL
IMS_BOOL configUtils_CaptureSection(IN IConfigBuffer *piBuffer,
        IN const IMS_CHAR *pszSection, IN IMS_SINT32 nIndex = (-1))
{
    if (nIndex < 0)
    {
        return piBuffer->CaptureSection(pszSection);
    }
    else
    {
        return piBuffer->CaptureSection(pszSection, nIndex);
    }
}

/*

Remarks

*/
LOCAL
const AString& configUtils_ReadValue(IN IConfigBuffer *piBuffer,
        IN const IMS_CHAR *pszKey, IN IMS_SINT32 nIndex = (-1))
{
    if (nIndex < 0)
    {
        return piBuffer->ReadValue(pszKey);
    }
    else
    {
        return piBuffer->ReadValue(pszKey, nIndex);
    }
}



/*

Remarks

*/
PUBLIC GLOBAL
IMS_RESULT ConfigUtils::GetValue(IN const AString &strConfName, IN IMS_SINT32 nId,
        IN const IMS_CHAR *pszSection, IN const IMS_CHAR *pszKey,
        OUT AString &strValue)
{
    IConfigBuffer *piBuffer = ConfigLoader::GetConfig(strConfName, (nId >= 0) ? nId : IMS_SLOT_0);

    if (piBuffer == IMS_NULL)
    {
        IMS_TRACE_E(0, "GetValue :: ConfigOpenFailed(%s:%s:%s)",
                strConfName.GetStr(), _TRACE_S_(pszSection), _TRACE_S_(pszKey));
        return IMS_FAILURE;
    }

    // Set the current section
    if (!piBuffer->CaptureSection(pszSection))
    {
        IMS_TRACE_E(0, "GetValue :: CaptureSectionFailed(%s:%s:%s)",
                strConfName.GetStr(), _TRACE_S_(pszSection), _TRACE_S_(pszKey));
        piBuffer->Destroy();
        return IMS_FAILURE;
    }

    // Read the string value
    strValue = piBuffer->ReadValue(pszKey);

    piBuffer->Destroy();

    return IMS_SUCCESS;
}

/*

Remarks

*/
PUBLIC GLOBAL
IMS_RESULT ConfigUtils::GetValue(IN const AString &strConfName, IN IMS_SINT32 nId,
        IN const IMS_CHAR *pszSection, IN IMS_SINT32 nSectionIndex,
        IN const IMS_CHAR *pszKey, IN IMS_SINT32 nKeyIndex,
        OUT AString &strValue)
{
    if ((nSectionIndex < 0) && (nKeyIndex < 0))
    {
        return GetValue(strConfName, nId, pszSection, pszKey, strValue);
    }

    IConfigBuffer *piBuffer = ConfigLoader::GetConfig(strConfName, (nId >= 0) ? nId : IMS_SLOT_0);

    if (piBuffer == IMS_NULL)
    {
        IMS_TRACE_E(0, "GetValue(index) :: ConfigOpenFailed(%s:%s:%s)",
                strConfName.GetStr(), _TRACE_S_(pszSection), _TRACE_S_(pszKey));
        return IMS_FAILURE;
    }

    // Set the current section
    if (!configUtils_CaptureSection(piBuffer, pszSection, nSectionIndex))
    {
        IMS_TRACE_E(0, "GetValue(index) :: CaptureSectionFailed(%s:%s:%s)",
                strConfName.GetStr(), _TRACE_S_(pszSection), _TRACE_S_(pszKey));
        piBuffer->Destroy();
        return IMS_FAILURE;
    }

    // Read the string value
    strValue = configUtils_ReadValue(piBuffer, pszKey, nKeyIndex);

    piBuffer->Destroy();

    return IMS_SUCCESS;
}

/*

Remarks

*/
PUBLIC GLOBAL
IMS_RESULT ConfigUtils::GetValueBoolean(IN const AString &strConfName, IN IMS_SINT32 nId,
        IN const IMS_CHAR *pszSection, IN const IMS_CHAR *pszKey,
        OUT IMS_BOOL &bValue)
{
    IConfigBuffer *piBuffer = ConfigLoader::GetConfig(strConfName, (nId >= 0) ? nId : IMS_SLOT_0);

    if (piBuffer == IMS_NULL)
    {
        IMS_TRACE_E(0, "GetValueBoolean :: ConfigOpenFailed(%s:%s:%s)",
                strConfName.GetStr(), _TRACE_S_(pszSection), _TRACE_S_(pszKey));
        return IMS_FAILURE;
    }

    // Set the current section
    if (!piBuffer->CaptureSection(pszSection))
    {
        IMS_TRACE_E(0, "GetValueBoolean :: CaptureSectionFailed(%s:%s:%s)",
                strConfName.GetStr(), _TRACE_S_(pszSection), _TRACE_S_(pszKey));
        piBuffer->Destroy();
        return IMS_FAILURE;
    }

    // Read the boolean value
    bValue = piBuffer->ReadValueBoolean(pszKey);

    piBuffer->Destroy();

    return IMS_SUCCESS;
}

/*

Remarks

*/
PUBLIC GLOBAL
IMS_RESULT ConfigUtils::GetValueHexadecimal(IN const AString &strConfName, IN IMS_SINT32 nId,
        IN const IMS_CHAR *pszSection, IN const IMS_CHAR *pszKey,
        OUT IMS_UINT32 &nValue)
{
    IConfigBuffer *piBuffer = ConfigLoader::GetConfig(strConfName, (nId >= 0) ? nId : IMS_SLOT_0);

    if (piBuffer == IMS_NULL)
    {
        IMS_TRACE_E(0, "GetValueHexadecimal :: ConfigOpenFailed(%s:%s:%s)",
                strConfName.GetStr(), _TRACE_S_(pszSection), _TRACE_S_(pszKey));
        return IMS_FAILURE;
    }

    // Set the current section
    if (!piBuffer->CaptureSection(pszSection))
    {
        IMS_TRACE_E(0, "GetValueHexadecimal :: CaptureSectionFailed(%s:%s:%s)",
                strConfName.GetStr(), _TRACE_S_(pszSection), _TRACE_S_(pszKey));
        piBuffer->Destroy();
        return IMS_FAILURE;
    }

    // Read the hexadecimal value
    const AString &strValue = piBuffer->ReadValue(pszKey);
    IMS_BOOL bOK = IMS_FALSE;
    IMS_UINT32 nTmpVal = strValue.ToUInt32(&bOK, 16);

    piBuffer->Destroy();

    if (!bOK)
    {
        IMS_TRACE_E(0, "GetValueHexadecimal :: ToUInt32Failed(%s)", strValue.GetStr(), 0, 0);
        return IMS_FAILURE;
    }

    nValue = nTmpVal;

    return IMS_SUCCESS;
}

/*

Remarks

*/
PUBLIC GLOBAL
IMS_RESULT ConfigUtils::GetValueInt(IN const AString &strConfName, IN IMS_SINT32 nId,
        IN const IMS_CHAR *pszSection, IN const IMS_CHAR *pszKey,
        OUT IMS_SINT32 &nValue)
{
    IConfigBuffer *piBuffer = ConfigLoader::GetConfig(strConfName, (nId >= 0) ? nId : IMS_SLOT_0);

    if (piBuffer == IMS_NULL)
    {
        IMS_TRACE_E(0, "GetValueInt :: ConfigOpenFailed(%s:%s:%s)",
                strConfName.GetStr(), _TRACE_S_(pszSection), _TRACE_S_(pszKey));
        return IMS_FAILURE;
    }

    // Set the current section
    if (!piBuffer->CaptureSection(pszSection))
    {
        IMS_TRACE_E(0, "GetValueInt :: CaptureSectionFailed(%s:%s:%s)",
                strConfName.GetStr(), _TRACE_S_(pszSection), _TRACE_S_(pszKey));
        piBuffer->Destroy();
        return IMS_FAILURE;
    }

    // Read the integer value
    nValue = piBuffer->ReadValueInt(pszKey);

    piBuffer->Destroy();

    return IMS_SUCCESS;
}
