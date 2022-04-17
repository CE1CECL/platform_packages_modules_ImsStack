#include "Configuration.h"
#include "ServiceMemory.h"
#include "ServiceTrace.h"
#include "ServicePhoneInfo.h"
#include "SystemConfig.h"

#include "IMtsClient.h"
#include "ImsServiceConfig.h"
#include "IPageMessage.h"
#include "IUSMS.h"

#include "utility/MtsStrName.h"
#include "utility/MtsDynamicLoader.h"

#include "MtsApp.h"
#include "MtsCallTracker.h"
#include "MtsClient.h"
#include "MtsService.h"
#include "message/MtsMessageController.h"

__IMS_TRACE_TAG_COM_SMS__;

LOCAL
const IMS_CHAR MTS_APP_NAME[] = "MtsApp";

PUBLIC
MtsApp::MtsApp(IN IMS_SINT32 nSlotId) :
        IMSApp(MTS_APP_NAME),
        m_pMtsClient(IMS_NULL),
        m_pMtsService(IMS_NULL),
        m_pMtsMessageController(IMS_NULL),
        m_pMtsDynamicLoader(IMS_NULL),
        m_pMtsServiceState(IMS_NULL),
        m_pCallTracker(IMS_NULL),
        m_pMtsAppTrm(IMS_NULL),
        m_bTrmBlock(IMS_FALSE)
{
    IMS_TRACE_I("+MtsApp [slot_%d]", nSlotId, 0, 0);
    SetSlotId(nSlotId);

    Configuration::GetInstance()->SetAppConfig(
            ImsServiceConfig::GetAppName(ImsAppId::MTS), nSlotId);

    m_pMtsAppTrm = MtsTrm::GetInstance(m_nSlotId);
    if (m_pMtsAppTrm != IMS_NULL)
    {
        m_pMtsAppTrm->AddListener(this);
    }
}

PUBLIC
MtsApp::~MtsApp()
{
    IMS_TRACE_I( "~MtsApp", 0, 0, 0 );

    //Remove MtsUtils
    DestroyMtsUtils();

    //Remove Mts Service
    RemoveMtsService();
    RemoveMtsClient(m_nSlotId);

    if (m_pMtsMessageController != IMS_NULL)
    {
        delete m_pMtsMessageController;
        m_pMtsMessageController = IMS_NULL;
    }

    if (m_pMtsAppTrm != IMS_NULL)
    {
        m_pMtsAppTrm->RemoveListener(this);
        MtsTrm::DestroyMtsTrm(m_nSlotId);
        m_bTrmBlock = IMS_FALSE;
        m_pMtsAppTrm = IMS_NULL;

    }

    if (m_pCallTracker != IMS_NULL)
    {
        delete m_pCallTracker;
        m_pCallTracker = IMS_NULL;
    }
}

PUBLIC VIRTUAL
void MtsApp::Start()
{
    IMS_TRACE_I("SMS Start : m_nSlotId : [%d]", m_nSlotId, 0, 0);

    //MtsUtils
    /*===================*/
    CreateMtsUtils(m_nSlotId);

    //MtsService
    /*===================*/
    CreateMtsService(m_nSlotId);

    //MtsMessageController
    /*===================*/
    CreateMtsMessageController(m_nSlotId, m_pMtsDynamicLoader);

    //MtsClient
    /*===================*/
    CreateMtsClient(m_nSlotId);
    m_pMtsClient->SetServiceState(m_pMtsServiceState);

    //Connect to WMS
    /*===================*/
    if (!m_pMtsClient->ConnectSC(m_nSlotId))
    {
        IMS_TRACE_I("Java MTS Connection is failed", 0, 0, 0);
    }

    //5. Update IP Config & Make MtsServiceState
    GetSmOverIpConfigInfo(m_nSlotId);

    m_pCallTracker = new MtsCallTracker(m_nSlotId);
    if (m_pCallTracker != IMS_NULL)
    {
        m_pCallTracker->AddListener(this);
    }
}

PUBLIC VIRTUAL
void MtsApp::Stop()
{
    IMS_TRACE_I("SMS Stop : m_nSlotId : [%d]", m_nSlotId, 0, 0);

    if (m_pMtsClient != IMS_NULL)
    {
        IMS_TRACE_I("UpdateMtsServiceState(STATE_NOTREADY)", 0, 0, 0);
        m_pMtsClient->UpdateMtsServiceState(IMtsClient::STATE_NOTREADY, m_nSlotId);
    }

    if (m_pMtsServiceState != IMS_NULL)
    {
        IMS_TRACE_I("SetIMSRegState(IMS_FALSE)", 0, 0, 0);
        m_pMtsServiceState->SetImsRegConnected(IMS_FALSE);
    }

    if (m_pMtsClient != IMS_NULL)
    {
        IMS_TRACE_I("Remove MtsApp in MtsClient (m_nSlotId:%d)", m_nSlotId, 0, 0);
        m_pMtsClient->StopTimer(TIMER_SMS_CLIENT_RETRY);
        m_pMtsClient->DisconnectSC(m_nSlotId);
    }

    if (m_pCallTracker != IMS_NULL)
    {
        m_pCallTracker->RemoveListener(this);
    }
}

PUBLIC GLOBAL
IMSApp* MtsApp::GetInstance(IN IMS_SINT32 nSlotId)
{
    return new MtsApp(nSlotId);
}

PUBLIC
void MtsApp::RequestRegistrationRecovery(IN IMS_SINT32 nRecoveryType)
{
    if (m_pMtsService != IMS_NULL)
    {
        m_pMtsService->RequestRegistrationRecovery(nRecoveryType);
    }
}

PUBLIC VIRTUAL
void MtsApp::RequestRegistrationSwitch(
        IN IUSmsSendRequestParam* /*pToBeSentSms*/, IN IMS_BOOL /*bIsSmsEServiceType*/)
{
    IMS_TRACE_D("MtsApp::RequestRegistrationSwitch", 0, 0, 0);
}

PUBLIC
MtsServiceState* MtsApp::GetMtsServiceState()
{
    MtsServiceState* pMtsServiceState = IMS_NULL;

    if (m_pMtsServiceState != IMS_NULL)
    {
        pMtsServiceState = m_pMtsServiceState;
    }
    return pMtsServiceState;
}

PUBLIC VIRTUAL
void MtsApp::MtsMessageController_NoTransaction()
{
    IMS_TRACE_I("MtsApp::MtsMessageController_NoTransaction()", 0, 0, 0);

    if (m_pMtsServiceState == IMS_NULL)
    {
        IMS_TRACE_E(0, "Fail to get MtsServiceState instance", 0, 0, 0);
        return;
    }

    /*
     * TODO: check why SmsOverIp update is needed here
     * If it does not need, then remove
     */
    IMS_BOOL bSmsOverIpNetwork = IMS_TRUE;

    if (bSmsOverIpNetwork)
    {
        m_pMtsServiceState->SetSmsOverIpState(IMS_TRUE);
    }
    else
    {
        m_pMtsServiceState->SetSmsOverIpState(IMS_FALSE);
    }

    if (m_pMtsMessageController != IMS_NULL)
    {
        m_pMtsMessageController->DeregisterNoTransactionListener(this);
    }
}

PROTECTED VIRTUAL
IMS_BOOL MtsApp::OnPreprocess(IN IMSMSG &/*objMSG*/)
{
    IMS_TRACE_I("MtsApp::OnPreprocess : AppName=%s", GetName().GetStr(), 0, 0);
    return IMS_FALSE;
}

PROTECTED VIRTUAL
IMS_BOOL MtsApp::OnMessage(IN IMSMSG &objMSG)
{
    IMS_TRACE_I("MtsApp::OnMessage : nMSG=%d", objMSG.nMSG, 0, 0);

    if (IUSMS::SMSMO_SEND_REQUEST == objMSG.nMSG)
    {
        IUSmsSendRequestParam* pParam = reinterpret_cast<IUSmsSendRequestParam*>(objMSG.nLparam);

        if (pParam == IMS_NULL)
        {
            IMS_TRACE_E(0, "Send request parameter is invalid", 0, 0, 0);
            m_pMtsClient->ReportTransmissionResult(MtsClient::MO_IMS_PERM_FAILURE,
                    IMtsClient::SMSFORMAT_INVALID, 0, m_nSlotId);
            return IMS_TRUE;
        }

        if (m_pMtsAppTrm != IMS_NULL)
        {
            if ((m_pMtsClient != IMS_NULL) && (!m_pMtsAppTrm->IsReady()))
            {
                m_bTrmBlock = IMS_TRUE;
                m_pMtsClient->UpdateMtsServiceState(IMtsClient::STATE_LIMITED, m_nSlotId);
            }
        }
        else
        {
            m_bTrmBlock = IMS_FALSE;
        }

        m_pMtsMessageController->SendMtsMessage(pParam, IMS_FALSE);

        delete pParam;
    }
    else if (SmsSvcInternal::SMSMT_RECVD == objMSG.nMSG)
    {
        IPageMessage* pParam = reinterpret_cast<IPageMessage*>(objMSG.nLparam);

        if (pParam == IMS_NULL)
        {
            IMS_TRACE_E(0, "pParam for a received message is invalid", 0,0,0);
            return IMS_FALSE;
        }

        if (m_pMtsAppTrm != IMS_NULL)
        {
            if ((m_pMtsClient != IMS_NULL) && (!m_pMtsAppTrm->IsReady()))
            {
                m_bTrmBlock = IMS_TRUE;
                m_pMtsClient->UpdateMtsServiceState(IMtsClient::STATE_LIMITED, m_nSlotId);
            }
            else
            {
                m_bTrmBlock = IMS_FALSE;
            }
        }
        else
        {
            m_bTrmBlock = IMS_FALSE;
        }

        m_pMtsMessageController->ReceiveMtsMessage(pParam, IMS_FALSE);
    }
    else if (objMSG.nMSG == IUSMS::SMS_SERVICE_CONTROL)
    {
        IUSmsServiceControlParam *pParam =
                reinterpret_cast<IUSmsServiceControlParam*>(objMSG.nLparam);

        if (pParam != IMS_NULL)
        {
            if (pParam->nCmd == IUSmsServiceControlParam::CMD_RECOVER_COMM_CHANNEL ||
                pParam->nCmd == IUSmsServiceControlParam::CMD_RECOVER_COMM_CHANNEL_BY_MODEM_RESET)
            {
                IMS_TRACE_I("MtsApp::CMD_RECOVER_COMM_CHANNEL. SlotId[%d]", m_nSlotId, 0, 0);
                if (pParam->nCmd ==
                        IUSmsServiceControlParam::CMD_RECOVER_COMM_CHANNEL_BY_MODEM_RESET)
                {
                    MtsServiceState* pMtsServiceState = m_pMtsDynamicLoader->GetMtsServiceState();
                    if (pMtsServiceState != IMS_NULL)
                    {
                        pMtsServiceState->SetMtsServiceState(IMtsClient::STATE_INIT);
                    }
                }
                m_pMtsClient->SetSCCnxState(IMS_FALSE);

                if (!m_pMtsClient->ConnectSC(m_nSlotId))
                {
                    IMS_TRACE_E(0, "Connecting MtsClient failed", 0, 0, 0);
                    m_pMtsClient->Retry_SCCnx();
                }
            }

            delete pParam;
        }
    }
    else if (objMSG.nMSG == IUSMS::SMS_RECONNECT_SC) // SMS_Patch_0664
    {
        IUSmsReconnectScParam *pParam = reinterpret_cast<IUSmsReconnectScParam*>(objMSG.nLparam);
        if (pParam != IMS_NULL)
        {
            if (!m_pMtsClient->ConnectSC(m_nSlotId))
            {
                IMS_TRACE_E(0, "Re-Connecting MtsClient failed", 0, 0, 0); // It's permanent Error.
            }
            delete pParam;
        }
    }
    else
    {
        IMS_TRACE_E(0, "OnMessage : Unknown message is received!!", 0, 0, 0);
    }

    return IMS_TRUE;
}

PROTECTED VIRTUAL
IMS_BOOL MtsApp::OnPostprocess(IN IMSMSG &/*objMSG*/)
{
    IMS_TRACE_I("MtsApp::OnPostprocess : AppName=%s", GetName().GetStr(), 0, 0);
    return IMS_FALSE;
}

PROTECTED VIRTUAL
IIMSActivityControl* MtsApp::GetController()
{
    IMS_TRACE_I("MtsApp::GetController : AppName=%s", GetName().GetStr(), 0, 0);
    return IMS_NULL;
}

PUBLIC VIRTUAL
IMS_BOOL MtsApp::Control(
        IN IMS_UINT32 nCmdType, IN IMS_UINTP /*nInParam*/, OUT IMS_UINTP* /*pnOutParam*/)
{
    IMS_TRACE_I("Control : App[%s] Cmd[%d]", GetName().GetStr(), nCmdType, 0);
    return IMS_FALSE;
}

PROTECTED
void MtsApp::AddService(IN MtsService* pService)
{
    if (pService == IMS_NULL)
    {
        return;
    }

    m_lstMtsServices.Append(pService);
    AttachService(pService);

    IMS_TRACE_I("AddService : ID[%s] Size[%d]", pService->GetId().GetStr(),
            m_lstMtsServices.GetSize(), 0);
}

PROTECTED
void MtsApp::RemoveMtsService()
{
    IMS_UINT32 nSize = m_lstMtsServices.GetSize();

    for (IMS_UINT32 index = 0; index < nSize; index++)
    {
        MtsService* pService = m_lstMtsServices.GetAt(index);

        if (pService != IMS_NULL)
        {
            DetachService(pService);
            delete pService;
        }
    }
    m_lstMtsServices.RemoveElementsAt(0, nSize);
}

PRIVATE
void MtsApp::GetSmOverIpConfigInfo(IN IMS_SINT32 nSlotId)
{
    /*
     * TODO: check carrier configuration
     * KEY_SMS_OVER_IMS_SUPPORTED_BOOL && KEY_SMS_OVER_IMS_SUPPORTED_RATS_INT_ARRAY
     */
    (void) nSlotId;
    IMS_BOOL bSmsOverIpNetwork = IMS_TRUE;

    MtsServiceState* pMtsServiceState = m_pMtsDynamicLoader->GetMtsServiceState();

    if (pMtsServiceState == IMS_NULL)
    {
        IMS_TRACE_E(0, "Fail to get MtsServiceState instance", 0, 0, 0);
        return;
    }

    pMtsServiceState->SetMtsMessageController(m_pMtsMessageController);

    if (bSmsOverIpNetwork)
    {
        pMtsServiceState->SetSmsOverIpState(IMS_TRUE);
    }
    else
    {
        pMtsServiceState->SetSmsOverIpState(IMS_FALSE);
    }
}

PROTECTED VIRTUAL
void MtsApp::CreateMtsService(IN IMS_SINT32 nSlotId)
{
    IMS_TRACE_I("CreateMtsService nSlotId : [%d]", nSlotId, 0, 0);

    if (m_pMtsDynamicLoader == IMS_NULL)
    {
        IMS_TRACE_E(0, "can't make CreateMtsService", 0, 0, 0);
    }

    m_pMtsService = new MtsService(
            m_pMtsDynamicLoader->GetMtsStrName()->GetMtsAppId(),
            m_pMtsDynamicLoader->GetMtsStrName()->GetMtsServiceId(),
            m_nSlotId,
            m_pMtsDynamicLoader);

    if (m_pMtsService != IMS_NULL)
    {
        AddService(m_pMtsService);
    }
    else
    {
        IMS_TRACE_E(0, "m_pMtsService is NULL", 0, 0, 0);
    }
}

PROTECTED VIRTUAL
void MtsApp::CreateMtsClient(IN IMS_SINT32 nSlotId)
{
    IMS_TRACE_I("CreateMtsClient : [%d]", nSlotId, 0, 0);
    m_pMtsClient = MtsClient::GetInstance(nSlotId);

    if (m_pMtsClient == IMS_NULL)
    {
        IMS_TRACE_E(0, "m_pMtsClient is NULL", 0, 0, 0);
    }

    /*if (m_pMtsClient != IMS_NULL)
    {
        m_pMtsClient->RegisterApp(this);
    }*/
}

PROTECTED VIRTUAL
void MtsApp::RemoveMtsClient(IN IMS_SINT32 nSlotId)
{
    if (m_pMtsClient == IMS_NULL)
    {
        IMS_TRACE_E(0, "m_pMtsClient is already NULL", 0, 0, 0);
        return;
    }

    IMS_TRACE_I("DestroyMtsClient : [%d]", nSlotId, 0, 0);
    m_pMtsClient->ClearTimer();
    MtsClient::DestroyMtsClient(nSlotId);
    m_pMtsClient = IMS_NULL;
}

PROTECTED VIRTUAL
void MtsApp::CreateMtsMessageController(
        IN IMS_SINT32 nSlotId, IN MtsDynamicLoader* pMtsDynamicLoader)
{
    m_pMtsMessageController = new MtsMessageController(nSlotId, pMtsDynamicLoader);

    if (m_pMtsMessageController != IMS_NULL)
    {
        m_pMtsMessageController->RegisterService(m_pMtsService);
    }
    else
    {
        IMS_TRACE_E(0, "Fail to add MtsService in MtsMessageController", 0, 0, 0);
    }
}

PROTECTED VIRTUAL
void MtsApp::CreateMtsUtils(IN IMS_SINT32 nSlotId)
{
    m_pMtsDynamicLoader = new MtsDynamicLoader(nSlotId);

    if (m_pMtsDynamicLoader != IMS_NULL)
    {
        m_pMtsDynamicLoader->Initialize(nSlotId);
        MtsServiceState* pMtsServiceState = m_pMtsDynamicLoader->GetMtsServiceState();
        m_pMtsServiceState = pMtsServiceState;
    }

    if (m_pMtsDynamicLoader == IMS_NULL)
    {
        IMS_TRACE_E(0, "m_pMtsDynamicLoader is NULL", 0, 0, 0);
    }
}

PROTECTED VIRTUAL
void MtsApp::DestroyMtsUtils()
{
    IMS_TRACE_I("DestroyMtsUtils", 0, 0, 0);

    if (m_pMtsServiceState != IMS_NULL)
    {
        m_pMtsServiceState = IMS_NULL;
    }

    if (m_pMtsDynamicLoader != IMS_NULL)
    {
        delete m_pMtsDynamicLoader;
        m_pMtsDynamicLoader = IMS_NULL;
    }
}

PROTECTED VIRTUAL
void MtsApp::CallTracker_StateChanged(IN IMS_UINT32 nType, IN IMS_UINT32 nState)
{
    IMS_TRACE_I("MtsApp::CallTracker_StateChanged, nType = [%d], nState = [%d]", nType, nState, 0);
}

PRIVATE
void MtsApp::SetSlotId(IN IMS_SINT32 nSlotId)
{
    if (nSlotId < 0)
    {
        char byData[94] = {0x30, 0x00};

        if (IMS_StrICmp(byData, "1") == 0)
        {
            m_nSlotId = 1;
        }
        else
        {
            m_nSlotId = 0;
        }
    }
    else
    {
        m_nSlotId = nSlotId;
    }

    IMS_TRACE_I("SetSlotId : [%d]", m_nSlotId, 0, 0);
}


PROTECTED VIRTUAL
void MtsApp::Trm_PriorityChanged()
{
    if (m_pMtsAppTrm->IsReady())
    {
        if (m_pMtsClient != IMS_NULL)
        {
            if (m_bTrmBlock == IMS_TRUE)
            {
                m_pMtsClient->UpdateMtsServiceState(IMtsClient::STATE_READY, m_nSlotId);
                m_bTrmBlock = IMS_FALSE;
            }
        }
    }
}
