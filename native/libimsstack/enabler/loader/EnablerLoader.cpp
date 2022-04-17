/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20170510  hwangoo.park@             Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "ServiceTrace.h"
#include "IMSProcess.h"
#include "SystemConfigManager.h"
#include "EnablerUtils.h"
#include "GeolocationHelper.h"
#include "EnablerFactory.h"
#include "EnablerThread.h"
#include "EnablerLoader.h"

__IMS_TRACE_TAG_USER_DECL__("EnablerLoader");

PRIVATE GLOBAL
EnablerLoader* EnablerLoader::pEnablerLoader = IMS_NULL;

PRIVATE
EnablerLoader::EnablerLoader()
    : pEnablerFactory(IMS_NULL)
    , objEnablerThreads(IMSMap<IMS_SINT32, EnablerThread*>())
{
    pEnablerFactory = new EnablerFactory();

    SystemConfigManager::GetInstance()->AddListener(this);
}

PRIVATE
EnablerLoader::~EnablerLoader()
{
    SystemConfigManager::GetInstance()->RemoveListener(this);

    if (pEnablerFactory != IMS_NULL)
    {
        delete pEnablerFactory;
        pEnablerFactory = IMS_NULL;
    }
}

PUBLIC
void EnablerLoader::Init()
{
    GeolocationHelper::GetInstance();

    // As default, the enabler thread for slot0 is always created.
    CreateAndAddThread(IMS_SLOT_0);

    for (IMS_SINT32 i = IMS_SLOT_1; i < SystemConfig::GetMaxSimSlot(); ++i)
    {
        CreateAndAddThread(i);
    }
}

PUBLIC GLOBAL
void EnablerLoader::CreateInstance()
{
    if (pEnablerLoader == IMS_NULL)
    {
        pEnablerLoader = new EnablerLoader();
    }
}

PUBLIC GLOBAL
void EnablerLoader::DestroyInstance()
{
    if (pEnablerLoader != IMS_NULL)
    {
        delete pEnablerLoader;
        pEnablerLoader = IMS_NULL;
    }
}

PUBLIC GLOBAL
EnablerLoader* EnablerLoader::GetInstance()
{
    if (pEnablerLoader == IMS_NULL)
    {
        CreateInstance();
    }

    return pEnablerLoader;
}

PROTECTED VIRTUAL
void EnablerLoader::SystemConfig_ConfigurationChanged(
        IN IMS_SINT32 nEvent, IN IMS_SINT32 nSlotId/* = IMS_SLOT_ANY*/)
{
    IMS_TRACE_I("SystemConfig :: event=%d, slotId=%d", nEvent, nSlotId, 0);

    if (nEvent == SystemConfig::EVENT_ON_BOOT)
    {
        // Ignore it on boot-up
    }
    else
    {
        ControlEnablers(nSlotId);
    }
}

PRIVATE
void EnablerLoader::CreateAndAddThread(IN IMS_SINT32 nSlotId)
{
    AString strThreadName = EnablerUtils::GetEnablerThreadName(nSlotId);
    EnablerThreadParam objParam(pEnablerFactory, nSlotId);
    IMSProcess *pProcess = IMSProcess::GetInstance();

    pProcess->LoadAppThreadWithParam(strThreadName,
            EnablerLoader::CreateThread, reinterpret_cast<void*>(&objParam), nSlotId);

    EnablerThread *pThread = DYNAMIC_CAST(
            EnablerThread*, pProcess->GetApplicationThread(strThreadName));

    if (pThread != IMS_NULL)
    {
        IMS_TRACE_I("EnablerThread created - %s", strThreadName.GetStr(), 0, 0);

        objEnablerThreads.Add(nSlotId, pThread);
    }
}

PRIVATE
EnablerThread* EnablerLoader::GetEnablerThread(IN IMS_SINT32 nSlotId) const
{
    IMS_SLONG nIndex = objEnablerThreads.GetIndexOfKey(nSlotId);

    if (nIndex < 0)
    {
        return IMS_NULL;
    }

    return objEnablerThreads.GetValueAt(nIndex);
}

PRIVATE
void EnablerLoader::ControlEnablers(IN IMS_SINT32 nSlotId)
{
    SystemConfigManager* pSCM = SystemConfigManager::GetInstance();
    const SystemConfig* pOldConfig = pSCM->GetOldConfig(nSlotId);
    const SystemConfig* pNewConfig = pSCM->GetConfig(nSlotId);
    IMS_SINT32 nCtrlFlags = EnablerThread::CONTROL_NONE;

    if (SystemConfig::IsOperatorChanged(pOldConfig, pNewConfig))
    {
        nCtrlFlags = EnablerThread::CONTROL_ALL;

        if (pNewConfig != IMS_NULL)
        {
            if (pNewConfig->GetOperator().GetLength() == 0)
            {
                nCtrlFlags = EnablerThread::CONTROL_STOP | EnablerThread::CONTROL_DESTROY;
            }
            else if (SystemConfig::IsMultiSimEnabled() && !pNewConfig->IsDds())
            {
                nCtrlFlags = EnablerThread::CONTROL_STOP | EnablerThread::CONTROL_DESTROY;
            }
            else if (pNewConfig->GetServiceFeatures() == 0)
            {
                nCtrlFlags = EnablerThread::CONTROL_STOP | EnablerThread::CONTROL_DESTROY;
            }
        }
    }
    else if (SystemConfig::IsDdsChanged(pOldConfig, pNewConfig))
    {
        if (pNewConfig != IMS_NULL)
        {
            if (pNewConfig->GetOperator().GetLength() == 0)
            {
                // Operator: old/new empty - do not control enabler threads
            }
            else if (pNewConfig->IsDds())
            {
                if (pNewConfig->GetServiceFeatures() == 0)
                {
                    // No services - do not start enabler thread
                }
                else
                {
                    nCtrlFlags = EnablerThread::CONTROL_ALL;
                }
            }
            else
            {
                nCtrlFlags = EnablerThread::CONTROL_STOP | EnablerThread::CONTROL_DESTROY;
            }
        }
    }
    else if (SystemConfig::IsServiceFeatureChanged(pOldConfig, pNewConfig))
    {
        nCtrlFlags = EnablerThread::CONTROL_ALL;

        // If service features are changed and it's non-DDS slot, then stop the enablers.
        // SIM1: empty, SIM2: SIM_LOCKED (newly added) : SIM2 is non-DDS when it's in LOADED
        if (SystemConfig::IsMultiSimEnabled()
                && !SystemConfig::IsMultiImsEnabled()
                && !SystemConfig::IsMultiImsEnabledOnDssv())
        {
            if ((pNewConfig != IMS_NULL) && !pNewConfig->IsDds())
            {
                nCtrlFlags = EnablerThread::CONTROL_STOP | EnablerThread::CONTROL_DESTROY;
            }
        }
    }
    else if (SystemConfig::IsSimMobilityChanged(pOldConfig, pNewConfig))
    {
        nCtrlFlags = EnablerThread::CONTROL_ALL;
    }
    /** Don't stop enablers. It's stopped and started when SIM is inserted.
    else
    {
        bOldNoUicc = (pOldConfig != IMS_NULL) ? pOldConfig->IsNoUicc() : IMS_FALSE;
        bNewNoUicc = (pNewConfig != IMS_NULL) ? pNewConfig->IsNoUicc() : IMS_TRUE;

        if ((!bOldNoUicc && bNewNoUicc) || (bOldNoUicc && !bNewNoUicc))
        {
            nCtrlFlags = EnablerThread::CONTROL_STOP | EnablerThread::CONTROL_START;
        }
    }
    */
    IMS_BOOL bOldNoUicc = (pOldConfig != IMS_NULL) ? pOldConfig->IsNoUicc() : IMS_FALSE;
    IMS_BOOL bNewNoUicc = (pNewConfig != IMS_NULL) ? pNewConfig->IsNoUicc() : IMS_TRUE;

    IMS_TRACE_I("ControlEnablers :: flags=%08X, oldNoUicc=%s, newNoUicc=%s",
            nCtrlFlags, _TRACE_B_(bOldNoUicc), _TRACE_B_(bNewNoUicc));

    if (nCtrlFlags != EnablerThread::CONTROL_NONE)
    {
        EnablerThread* pET = GetEnablerThread(nSlotId);

        if (pET != IMS_NULL)
        {
            pET->ControlEnablers(nCtrlFlags);
        }
    }
}

PRIVATE GLOBAL
IMSAppThread* EnablerLoader::CreateThread(IN void* pvParam)
{
    EnablerThreadParam *pParam = reinterpret_cast<EnablerThreadParam*>(pvParam);

    if (pParam == IMS_NULL)
    {
        return IMS_NULL;
    }

    return new EnablerThread(pParam->pEnablerFactory, pParam->nSlotId);
}
