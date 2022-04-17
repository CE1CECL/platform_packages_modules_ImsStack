/*
    Author
    IMSers
    <table>
    Date      Description
    --------  ----------
    20100302  Created
    </table>

    Description
     This file defines the entry point of IMS service.
*/

#include "ServiceTrace.h"
#include "PlatformProperty.h"
#include "SystemConfigManager.h"
#include "IMSProcess.h"

#include "EngineState.h"
#include "EnablerLoader.h"
#include "ImsProvisioning.h"
#include "IMSCore.h"

__IMS_TRACE_TAG_USER_DECL__("IMSCore");

class __SystemConfigWrapper
{
public:
    inline static void SetConfiguration(
            IN IMS_SINT32 nEvent, IN IMS_SINT32 nCount, IN const __SystemConfig *pstSysConfig)
    {
        SystemConfigManager *pSCM = SystemConfigManager::GetInstance();
        pSCM->UpdateSystemConfig(nEvent, nCount, pstSysConfig);
    }
};

GLOBAL void IMSCore_SetConfiguration(
        IN IMS_SINT32 nEvent, IN IMS_SINT32 nCount, IN const __SystemConfig *pstSysConfig)
{
    __SystemConfigWrapper::SetConfiguration(nEvent, nCount, pstSysConfig);

    if ((nEvent == SystemConfig::EVENT_FEATURE_CHANGED)
            || (nEvent == SystemConfig::EVENT_ALL_CONFIGURATION_CHANGED))
    {
        // no-op
    }
}

GLOBAL void IMSCore_Initialize()
{
    PlatformProperty::Initialize();
}

GLOBAL void IMSCore_InitConfiguration()
{
    ImsProvisioning::Initialize();
}

GLOBAL void IMSCore_Start()
{
    // Starts the platform-specific threads
    PlatformProperty::Start();

    IMSProcess *pProcess = IMSProcess::GetInstance();

    // Starts the main thread (Framework)
    if (!pProcess->Initialize())
    {
        return;
    }

    EngineState::Initialize();
    EnablerLoader::GetInstance()->Init();

    IMS_TRACE_I("ImsStackCore: starts", 0, 0, 0);
}

GLOBAL void IMSCore_Stop()
{
    IMS_TRACE_I("ImsStackCore: stops", 0, 0, 0);

    EngineState::Uninitialize();

    IMSProcess::GetInstance()->Uninitialize();

    PlatformProperty::Stop();
}

GLOBAL void IMSCore_Uninitialize()
{
    PlatformProperty::Uninitialize();
}
