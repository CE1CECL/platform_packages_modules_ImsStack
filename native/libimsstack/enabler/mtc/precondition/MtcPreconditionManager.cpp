#include "media/IMedia.h"
#include "precondition/MtcPreconditionManager.h"
#include "precondition/SdpPreconditionHelper.h"
#include "media/IMtcMediaManager.h"
#include "call/MtcSession.h"
#include "configuration/MtcConfigurationProxy.h"
#include "Configuration.h"
#include "ISubscriberConfig.h"
#include "ServiceTrace.h"
// #include "define/MtcStringDef.h"
// #include "precondition/QosStringDef.h"

__IMS_TRACE_TAG_COM_MTC__;

PUBLIC
MtcPreconditionManager::MtcPreconditionManager(IN IMtcCallContext& objContext) :
        m_objQosDatas(IMSMap<ISession*, QosData*>()),
        m_objQosTimers(IMSMap<ISession*, QosTimer*>()),
        m_objStatusTables(IMSMap<ISession*, QosStatusTable*>()),
        m_objCapabilities(IMSMap<ISession*, IMS_BOOL>()),
        m_pListener(IMS_NULL),
        m_objContext(objContext)
{
    IMS_TRACE_D("+MtcPreconditionManager Callkey[%" PFLS_x "]", m_objContext.GetCallKey(), 0, 0);
    m_objContext.GetMediaManager().SetQosListener(this);
}

PUBLIC VIRTUAL
MtcPreconditionManager::~MtcPreconditionManager()
{
    IMS_TRACE_D("~MtcPreconditionManager Callkey[%" PFLS_x "]", m_objContext.GetCallKey(), 0, 0);
    m_objContext.GetMediaManager().SetQosListener(IMS_NULL);
    DestroyAll();
}

PUBLIC VIRTUAL
void MtcPreconditionManager::CreateQos(IN ISession* piSession)
{
    IMS_TRACE_D("CreateQos", 0, 0, 0);
    CreateQosData(piSession);
    CreateQosTimer(piSession);
    CreateStatusTable(piSession);

    InitializeCapability(piSession);
    InitializeLocalCurrentStatus(piSession);
}

PUBLIC VIRTUAL
void MtcPreconditionManager::DestroyQos(IN ISession* piSession)
{
    IMS_TRACE_D("DestroyQos", 0, 0, 0);
    RemoveQosData(piSession);
    RemoveQosTimer(piSession);
    RemoveStatusTable(piSession);

    m_objCapabilities.Remove(piSession);
}

PUBLIC VIRTUAL
void MtcPreconditionManager::SetListener(IN IMtcPreconditionListener* pListener)
{
    m_pListener = pListener;
}

PUBLIC VIRTUAL
IMS_BOOL MtcPreconditionManager::IsQosEnabled(IN ISession* piSession, IN QosCheckType eType)
{
    IMS_TRACE_D("IsQosEnabled", 0, 0, 0);

    if (!IsPreconditionSupportedInLocal())
    {
        return IMS_TRUE;
    }

    IMS_UINT32 eMediaTypes = SdpPreconditionHelper::GetMediaTypesBySdp(piSession);

    if (eMediaTypes == MEDIATYPE_NONE)
    {
        IMS_TRACE_D("IsQosEnabled : can't get media types from SDP.", 0, 0, 0);
        return IMS_FALSE;
    }

    IMS_BOOL bAudioEnabled = IMS_TRUE;
    IMS_BOOL bVideoEnabled = IMS_TRUE;
    IMS_BOOL bTextEnabled = IMS_TRUE;

    QosStatusTable* pStatusTable = GetQosStatusTable(piSession);

    // all, remote case
    if (eType != QosCheckType::LOCAL_STATUS)
    {
        if (eMediaTypes & MEDIATYPE_AUDIO)
        {
            bAudioEnabled = pStatusTable->IsCurrentStatusEnabled(
                    SdpMedia::TYPE_AUDIO, SdpPrecondition::STATUS_REMOTE);
        }

        if (eMediaTypes & MEDIATYPE_VIDEO)
        {
            bVideoEnabled = pStatusTable->IsCurrentStatusEnabled(
                    SdpMedia::TYPE_VIDEO, SdpPrecondition::STATUS_REMOTE);
        }

        if (eMediaTypes & MEDIATYPE_TEXT)
        {
            bTextEnabled = pStatusTable->IsCurrentStatusEnabled(
                    SdpMedia::TYPE_TEXT, SdpPrecondition::STATUS_REMOTE);
        }
    }

    // all, local case
    if (eType != QosCheckType::REMOTE_STATUS)
    {
        if (eMediaTypes & MEDIATYPE_AUDIO)
        {
            bAudioEnabled = pStatusTable->IsCurrentStatusEnabled(
                    SdpMedia::TYPE_AUDIO, SdpPrecondition::STATUS_LOCAL) && bAudioEnabled;
        }

        if (eMediaTypes & MEDIATYPE_VIDEO)
        {
            bVideoEnabled = pStatusTable->IsCurrentStatusEnabled(
                    SdpMedia::TYPE_VIDEO, SdpPrecondition::STATUS_LOCAL) && bVideoEnabled;
        }

        if (eMediaTypes & MEDIATYPE_TEXT)
        {
            bTextEnabled = pStatusTable->IsCurrentStatusEnabled(
                    SdpMedia::TYPE_TEXT, SdpPrecondition::STATUS_LOCAL) && bTextEnabled;
        }
    }

    IMS_TRACE_D("IsQosEnabled : audio[%d] video[%d] text[%d]",
            bAudioEnabled, bVideoEnabled, bTextEnabled);

    return (bAudioEnabled && bVideoEnabled && bTextEnabled) ? IMS_TRUE : IMS_FALSE;
}

PUBLIC VIRTUAL
void MtcPreconditionManager::StartQosTimer(IN ISession* piSession,
        IN QosTimerType eType /*= QosTimerType::WAIT_AVAILABLE*/)
{
    QosTimer* pQosTimer = GetQosTimer(piSession);

    if (!pQosTimer)
    {
        return;
    }

    if (!IsPreconditionSupportedInLocal())
    {
        return;
    }

    IMS_SINT32 nDuration = 0;

    if (eType == QosTimerType::WAIT_AVAILABLE)
    {
        nDuration =
                m_objContext.GetConfigurationProxy().GetInt(Feature::DEDICATED_BEARER_WAIT_TIMER);

        if (nDuration < 0)
        {
            IMS_TRACE_D("StartQosTimer : start the timer to enable QoS by force.", 0, 0, 0);
            eType = QosTimerType::FORCE_AVAILABLE;
            nDuration = 1000;
        }
    }
    else if (eType == QosTimerType::GUARD_INACTIVE)
    {
        nDuration = 1000;
    }

    pQosTimer->StartQosTimer(eType, nDuration);
}

PUBLIC VIRTUAL
void MtcPreconditionManager::StopQosTimer(IN ISession* piSession,
        IN QosTimerType eType /*= QosTimerType::WAIT_AVAILABLE*/)
{
    QosTimer* pQosTimer = GetQosTimer(piSession);
    if (pQosTimer == IMS_NULL)
    {
        return;
    }

    pQosTimer->StopQosTimer(eType);
}

PUBLIC VIRTUAL
void MtcPreconditionManager::StopAllQosTimer(IN ISession* piSession)
{
    StopQosTimer(piSession, QosTimerType::WAIT_AVAILABLE);
    StopQosTimer(piSession, QosTimerType::GUARD_INACTIVE);
    StopQosTimer(piSession, QosTimerType::FORCE_AVAILABLE);

}

PUBLIC VIRTUAL
void MtcPreconditionManager::UpdatePreconditionCapability(IN ISession* piSession,
        IN IMS_BOOL bCapability)
{
    IMS_BOOL bLocalCapa = IsPreconditionSupportedInLocal();

    // TODO: check qos attributes in SDP body of received SIP message.
    m_objCapabilities.Add(piSession, (bLocalCapa && bCapability));
}

PUBLIC VIRTUAL
IMS_BOOL MtcPreconditionManager::HasPreconditionCapability(IN ISession* piSession)
{
    return m_objCapabilities.GetValue(piSession);
}

PUBLIC VIRTUAL
void MtcPreconditionManager::UpdatePreconditionAttributes(IN ISession* piSession)
{
    if (!piSession)
    {
        return;
    }

    QosStatusTable* pStatusTable = GetQosStatusTable(piSession);
    if (!pStatusTable)
    {
        return;
    }

    IMS_TRACE_D("UpdatePreconditionAttributes", 0, 0, 0);

    IMS_UINT32 eMediaTypes = SdpPreconditionHelper::GetMediaTypesBySdp(piSession);

    if (eMediaTypes & MEDIATYPE_AUDIO)
    {
        UpdateStatusRecords(piSession, MEDIATYPE_AUDIO);
    }

    if (eMediaTypes & MEDIATYPE_VIDEO)
    {
        UpdateStatusRecords(piSession, MEDIATYPE_VIDEO);
    }

    if (eMediaTypes & MEDIATYPE_TEXT)
    {
        UpdateStatusRecords(piSession, MEDIATYPE_TEXT);
    }

    pStatusTable->RemoveUnusedStatusRecords(eMediaTypes);

    IMSList<IMedia*> lstMedias = piSession->GetMedia();
    IMS_UINT32 nSize = lstMedias.GetSize();

    IMS_TRACE_D("UpdatePreconditionAttributes : media size[%d]", nSize, 0, 0);

    IMS_BOOL bIncludePrecondition = SdpPreconditionHelper::IsPreconditionIncludedInSdp(piSession);

    for (IMS_UINT32 index = 0; index < nSize; index++)
    {
        IMedia* piMedia = lstMedias.GetAt(index);
        if (!piMedia)
        {
            continue;
        }

        if (bIncludePrecondition)
        {
            pStatusTable->UpdateStatusTable(piMedia);
        }
    }
}

PUBLIC VIRTUAL
void MtcPreconditionManager::FormPreconditionSdp(IN ISession* piSession, IN IMS_BOOL bFailure)
{
    if (!HasPreconditionCapability(piSession))
    {
        IMS_TRACE_D("FormPreconditionSdp : UE don't have precondition capability.", 0, 0, 0);
        RemovePreconditionSdp(piSession);
        return;
    }

    IMS_TRACE_D("FormPreconditionSdp", 0, 0, 0);

    if (bFailure)
    {
        SdpPreconditionHelper::FormFailurePreconditionSdp(piSession);
        return;
    }

    QosStatusTable* pStatusTable = GetQosStatusTable(piSession);
    if (pStatusTable == IMS_NULL)
    {
        return;
    }

    IMSList<IMedia*> lstMedias = piSession->GetMedia();
    for (IMS_UINT32 index = 0; index < lstMedias.GetSize(); index++)
    {
        IMedia* piMedia = lstMedias.GetAt(index);
        const SdpMedia* pLocalSdp = GetSdpMedia(piMedia, IMS_FALSE);
        if (pLocalSdp == IMS_NULL)
        {
            continue;
        }

        IMS_SINT32 eSdpMediaType = pLocalSdp->GetType();

        if ((pLocalSdp->GetPort() <= 0) || !pStatusTable->IsStatusRecordsListEmpty(eSdpMediaType))
        {
            continue;
        }

        pStatusTable->CreateStatusRecords(eSdpMediaType);

        IMS_UINT32 eMediaType =
                SdpPreconditionHelper::GetMediaType(pLocalSdp, piMedia->GetState());

        pStatusTable->UpdateLocalCurrentStatus(eSdpMediaType,
                IsReserved(GetQosStatus(piSession, eMediaType)));
    }

    IMS_BOOL bCheckQosWhileCallUpgrade = IMS_TRUE;
    if (m_objContext.GetConfigurationProxy().GetInt(
            Feature::POLICY_FOR_CHECKING_QOS_WHILE_CALL_UPGRADING) != 2)
    {
        bCheckQosWhileCallUpgrade = IMS_FALSE;
    }

    IMS_BOOL bUseConf = (m_objContext.GetCallInfo().ePeerType == PeerType::MT);
    if (piSession->GetState() >= ISession::STATE_ESTABLISHED)
    {
        // TODO: check if the confirmation status should be added when UE uses qos check
        // for upgrading the call.
        bUseConf = IMS_FALSE;
    }

    SdpPreconditionHelper::FormPreconditionSdp(
            piSession, pStatusTable, bCheckQosWhileCallUpgrade, bUseConf);
}

PUBLIC VIRTUAL
void MtcPreconditionManager::RemovePreconditionSdp(IN ISession* piSession)
{
    IMS_TRACE_D("RemovePreconditionSdp", 0, 0, 0);
    SdpPreconditionHelper::RemovePreconditionSdp(piSession);
}

PUBLIC VIRTUAL
IMS_UINT32 MtcPreconditionManager::EnableLocalCurrentStatus(IN ISession* piSession)
{
    IMS_UINT32 eEnabledMediaTypes = MEDIATYPE_NONE;
    QosStatusTable* pStatusTable = GetQosStatusTable(piSession);
    if (pStatusTable == IMS_NULL)
    {
        return eEnabledMediaTypes;
    }

    IMS_UINT32 eMediaTypes = SdpPreconditionHelper::GetMediaTypesBySdp(piSession);
    if (eMediaTypes == MEDIATYPE_NONE)
    {
        eMediaTypes = GetMediaTypesFromCallType();
    }

    if ((eMediaTypes & MEDIATYPE_AUDIO) && !pStatusTable->IsCurrentStatusEnabled(
            SdpMedia::TYPE_AUDIO, SdpPrecondition::STATUS_LOCAL))
    {
        pStatusTable->UpdateLocalCurrentStatus(SdpMedia::TYPE_AUDIO, IMS_TRUE);
        eEnabledMediaTypes |= MEDIATYPE_AUDIO;
    }

    if ((eMediaTypes & MEDIATYPE_VIDEO) && !pStatusTable->IsCurrentStatusEnabled(
            SdpMedia::TYPE_VIDEO, SdpPrecondition::STATUS_LOCAL))
    {
        pStatusTable->UpdateLocalCurrentStatus(SdpMedia::TYPE_VIDEO, IMS_TRUE);
        eEnabledMediaTypes |= MEDIATYPE_VIDEO;
    }

    if ((eMediaTypes & MEDIATYPE_TEXT) && !pStatusTable->IsCurrentStatusEnabled(
            SdpMedia::TYPE_TEXT, SdpPrecondition::STATUS_LOCAL))
    {
        pStatusTable->UpdateLocalCurrentStatus(SdpMedia::TYPE_TEXT, IMS_TRUE);
        eEnabledMediaTypes |= MEDIATYPE_TEXT;
    }

    IMS_TRACE_D("EnableLocalCurrentStatus : Enabled Media Types[%d]", eEnabledMediaTypes, 0, 0);
    return eEnabledMediaTypes;
}

PUBLIC VIRTUAL
void MtcPreconditionManager::EnableRemoteCurrentStatus(IN ISession* piSession)
{
    QosStatusTable* pStatusTable = GetQosStatusTable(piSession);
    if (pStatusTable == IMS_NULL)
    {
        IMS_TRACE_D("EnableRemoteCurrentStatus : can't get StatusTable", 0, 0, 0);
        return;
    }

    IMSList<IMedia*> lstMedias = piSession->GetMedia();
    IMS_UINT32 nSize = lstMedias.GetSize();

    IMS_TRACE_D("EnableRemoteCurrentStatus : media size[%d]", nSize, 0, 0);

    for (IMS_UINT32 index = 0; index < nSize; index++)
    {
        IMedia* piMedia = lstMedias.GetAt(index);
        if (piMedia == IMS_NULL)
        {
            continue;
        }

        const SdpMedia* pRemoteSdp = GetSdpMedia(piMedia, IMS_TRUE);
        if (pRemoteSdp == IMS_NULL)
        {
            continue;
        }

        pStatusTable->EnableRemoteCurrentStatus(pRemoteSdp->GetType());
    }
}

PUBLIC VIRTUAL
void MtcPreconditionManager::QosStatusChanged(IN ISession* piSession, IN QosStatus eStatus,
        IN IMS_UINT32 eMediaType)
{
    if (piSession == IMS_NULL)
    {
        IMS_TRACE_D("QosStatusChanged : ISession is null", 0, 0, 0);
        return;
    }

    // 1. Update QosData
    QosData* pQosData = GetQosData(piSession);
    if (pQosData == IMS_NULL)
    {
        IMS_TRACE_D("QosStatusChanged : QosData is null", 0, 0, 0);
        return;
    }

    QosStatus ePreviousStatus = QosStatus::IDLE;

    if (eMediaType & MEDIATYPE_AUDIO)
    {
        ePreviousStatus = pQosData->GetAudioStatus();
        pQosData->SetAudioStatus(eStatus);
    }

    if (eMediaType & MEDIATYPE_VIDEO)
    {
        ePreviousStatus = pQosData->GetVideoStatus();
        pQosData->SetVideoStatus(eStatus);
    }

    if (eMediaType & MEDIATYPE_TEXT)
    {
        ePreviousStatus = pQosData->GetTextStatus();
        pQosData->SetTextStatus(eStatus);
    }

    IMS_TRACE_D("QosStatusChanged : MediaType[%d] Status[%d]->[%d]",
            eMediaType, ePreviousStatus, eStatus);

    QosStatusTable* pStatusTable = GetQosStatusTable(piSession);
    if (pStatusTable == IMS_NULL)
    {
        IMS_TRACE_D("QosStatusChanged : QosStatusTable is null", 0, 0, 0);
        return;
    }

    IMS_BOOL bLocalCurrReserved = pStatusTable->IsCurrentStatusEnabled(GetSdpMediaType(eMediaType),
            SdpPrecondition::STATUS_LOCAL);
    IMS_BOOL bReserved = (IsDefaultBearerUsed(eMediaType)) ? IMS_TRUE : IsReserved(eStatus);

    if (bReserved != bLocalCurrReserved)
    {
        // 2. Update StatusTable
        pStatusTable->UpdateLocalCurrentStatus(GetSdpMediaType(eMediaType), bReserved);

        // 3. Check QosTimer and start or stop the timer.
        IMS_BOOL bNeedToNotify = CheckQosTimers(piSession, bReserved);
        if (bNeedToNotify)
        {
            NotifyQosStatusToListener(piSession, bReserved, eMediaType);
        }
    }

    QosTimer* pTimer = GetQosTimer(piSession);
    IMS_BOOL bInactiveGuardTimerActivated =
            (pTimer && pTimer->IsQosTimerActivated(QosTimerType::GUARD_INACTIVE)) ?
            IMS_TRUE : IMS_FALSE;

    if (!IsReserved(eStatus) && !bInactiveGuardTimerActivated)
    {
        InitializeStatusForLostQos(eMediaType, pQosData);
    }
}

PUBLIC VIRTUAL
void MtcPreconditionManager::OnWaitTimerExpired(IN QosTimer* pQosTimer)
{
    IMS_TRACE_D("OnWaitTimerExpired", 0, 0, 0);
    NotifyReserveFailedByQosTimerExpired(pQosTimer);
}

PUBLIC VIRTUAL
void MtcPreconditionManager::OnGuardInactiveTimerExpired(IN QosTimer* pQosTimer)
{
    IMS_TRACE_D("OnGuardInactiveTimerExpired", 0, 0, 0);
    NotifyReserveFailedByQosTimerExpired(pQosTimer);
}

PUBLIC VIRTUAL
void MtcPreconditionManager::OnForceAvailableTimerExpired(IN QosTimer* pQosTimer)
{
    IMS_TRACE_D("OnForceAvailableTimerExpired", 0, 0, 0);

    ISession* piSession = IMS_NULL;
    for (IMS_UINT32 index = 0; index < m_objQosTimers.GetSize(); index++)
    {
        QosTimer* pTimer = m_objQosTimers.GetValueAt(index);
        if (pTimer == pQosTimer)
        {
            piSession = m_objQosTimers.GetKeyAt(index);
            break;
        }
    }

    if (piSession == IMS_NULL)
    {
        return;
    }

    QosData *pQosData = GetQosData(piSession);
    if (pQosData == IMS_NULL)
    {
        return;
    }

    IMS_UINT32 eEnabledMediaTypes = EnableLocalCurrentStatus(piSession);
    StopQosTimer(piSession, QosTimerType::WAIT_AVAILABLE);

    if (eEnabledMediaTypes & MEDIATYPE_AUDIO)
    {
        pQosData->SetAudioStatus(QosStatus::AVAILABLE);
        m_pListener->QosReserved(piSession, MEDIATYPE_AUDIO);
    }

    if (eEnabledMediaTypes & MEDIATYPE_VIDEO)
    {
        pQosData->SetVideoStatus(QosStatus::AVAILABLE);
        m_pListener->QosReserved(piSession, MEDIATYPE_VIDEO);
    }

    if (eEnabledMediaTypes & MEDIATYPE_TEXT)
    {
        pQosData->SetTextStatus(QosStatus::AVAILABLE);
        m_pListener->QosReserved(piSession, MEDIATYPE_TEXT);
    }
}

PRIVATE
void MtcPreconditionManager::CreateQosTimer(IN ISession* piSession)
{
    QosTimer* pQosTimer = GetQosTimer(piSession);
    if (pQosTimer != IMS_NULL)
    {
        IMS_TRACE_D("CreateQosTimer : Timer has been already created.", 0, 0, 0);
        return;
    }

    pQosTimer = new QosTimer(this);
    // pQosTimer->SetListener(this);

    m_objQosTimers.Add(piSession, pQosTimer);
    IMS_TRACE_D("CreateQosTimer : QosTimer size [%d]", m_objQosTimers.GetSize(), 0, 0);
}

PRIVATE
void MtcPreconditionManager::RemoveQosTimer(IN ISession* piSession)
{
    QosTimer* pTimer = GetQosTimer(piSession);

    if (pTimer != IMS_NULL)
    {
        delete pTimer;
        m_objQosTimers.Remove(piSession);
    }

    IMS_TRACE_D("RemoveQosTimer : size[%d]", m_objQosTimers.GetSize(), 0, 0);
}

PRIVATE
void MtcPreconditionManager::CreateStatusTable(IN ISession* piSession)
{
    QosStatusTable* pStatusTable = GetQosStatusTable(piSession);
    if (pStatusTable != IMS_NULL)
    {
        IMS_TRACE_D("CreateStatusTable : already have status table.", 0, 0, 0);
        return;
    }

    pStatusTable = new QosStatusTable();
    m_objStatusTables.Add(piSession, pStatusTable);

    IMS_TRACE_D("CreateStatusTable : size[%d]", m_objStatusTables.GetSize(), 0, 0);
}

PRIVATE
void MtcPreconditionManager::RemoveStatusTable(IN ISession* piSession)
{
    QosStatusTable* pStatusTable = GetQosStatusTable(piSession);

    if (pStatusTable != IMS_NULL)
    {
        delete pStatusTable;
        m_objStatusTables.Remove(piSession);
    }

    IMS_TRACE_D("RemoveStatusTable : size[%d]", m_objStatusTables.GetSize(), 0, 0);
}

PRIVATE
void MtcPreconditionManager::CreateQosData(IN ISession* piSession)
{
    QosData* pQosData = GetQosData(piSession);
    if (pQosData != IMS_NULL)
    {
        IMS_TRACE_D("CreateQosData : already have QosData.", 0, 0, 0);
        return;
    }

    pQosData = new QosData();
    m_objQosDatas.Add(piSession, pQosData);

    IMS_TRACE_D("CreateQosData : size[%d]", m_objQosDatas.GetSize(), 0, 0);
}

PRIVATE
void MtcPreconditionManager::RemoveQosData(IN ISession* piSession)
{
    QosData* pQosData = GetQosData(piSession);

    if (pQosData != IMS_NULL)
    {
        delete pQosData;
        m_objQosDatas.Remove(piSession);
    }

    IMS_TRACE_D("RemoveQosData : size[%d]", m_objQosDatas.GetSize(), 0, 0);
}

PRIVATE
void MtcPreconditionManager::DestroyAll()
{
    for (IMS_UINT32 index = 0; index < m_objQosDatas.GetSize(); index++)
    {
        delete m_objQosDatas.GetValueAt(index);
    }

    for (IMS_UINT32 index = 0; index < m_objQosTimers.GetSize(); index++)
    {
        delete m_objQosTimers.GetValueAt(index);
    }

    for (IMS_UINT32 index = 0; index < m_objStatusTables.GetSize(); index++)
    {
        delete m_objStatusTables.GetValueAt(index);
    }
}

PRIVATE
QosData* MtcPreconditionManager::GetQosData(IN ISession* piSession)
{
    IMS_SLONG nIndex = m_objQosDatas.GetIndexOfKey(piSession);

    return (nIndex >= 0) ? m_objQosDatas.GetValueAt(nIndex) : IMS_NULL;
}

PRIVATE
QosTimer* MtcPreconditionManager::GetQosTimer(IN ISession* piSession)
{
    IMS_SLONG nIndex = m_objQosTimers.GetIndexOfKey(piSession);

    return (nIndex >= 0) ? m_objQosTimers.GetValueAt(nIndex) : IMS_NULL;
}

PRIVATE
QosStatusTable* MtcPreconditionManager::GetQosStatusTable(IN ISession* piSession)
{
    IMS_SLONG nIndex = m_objStatusTables.GetIndexOfKey(piSession);

    return (nIndex >= 0) ? m_objStatusTables.GetValueAt(nIndex) : IMS_NULL;
}

PRIVATE
void MtcPreconditionManager::NotifyReserveFailedByQosTimerExpired(IN QosTimer* pQosTimer)
{
    ISession* piSession = IMS_NULL;
    for (IMS_UINT32 index = 0; index < m_objQosTimers.GetSize(); index++)
    {
        QosTimer* pTimer = m_objQosTimers.GetValueAt(index);
        if (pTimer == pQosTimer)
        {
            piSession = m_objQosTimers.GetKeyAt(index);
            break;
        }
    }

    if (piSession == IMS_NULL)
    {
        IMS_TRACE_D("NotifyReserveFailedByQoSTimerExpired : can't find ISession", 0, 0, 0);
        return;
    }

    QosData* pQosData = GetQosData(piSession);
    if (pQosData == IMS_NULL)
    {
        IMS_TRACE_D("NotifyReserveFailedByQoSTimerExpired : can't find QosData", 0, 0, 0);
        return;
    }

    QosLossPolicy eAction = GetActionForQosLoss(piSession);

    IMS_UINT32 eMediaTypes = SdpPreconditionHelper::GetMediaTypesBySdp(piSession);
    InitializeStatusForLostQos(eMediaTypes, pQosData);

    if (eAction != QosLossPolicy::MAINTAIN)
    {
        m_pListener->QosReserveFailed(piSession, eAction);
    }
}

PRIVATE
QosLossPolicy MtcPreconditionManager::GetActionForQosLoss(IN ISession* piSession)
{
    QosLossPolicy eAction = QosLossPolicy::MAINTAIN;

    if (piSession == IMS_NULL)
    {
        return eAction;
    }

    QosStatusTable* pStatusTable = GetQosStatusTable(piSession);
    if (pStatusTable == IMS_NULL)
    {
        IMS_TRACE_D("GetActionForQosLoss : StatusTable is null.", 0, 0, 0);
        return eAction;
    }

    IMS_UINT32 eMediaTypes = SdpPreconditionHelper::GetMediaTypesBySdp(piSession);

    if ((eMediaTypes & MEDIATYPE_AUDIO) && !pStatusTable->IsCurrentStatusEnabled(
            SdpMedia::TYPE_AUDIO, SdpPrecondition::STATUS_LOCAL))
    {
        QosLossPolicy ePartialAction = (IsDefaultBearerUsed(MEDIATYPE_AUDIO)) ?
                QosLossPolicy::MAINTAIN : GetQosLossPolicy(MEDIATYPE_AUDIO);

        if (eAction < ePartialAction)
        {
            eAction = ePartialAction;
        }
    }

    if ((eMediaTypes & MEDIATYPE_VIDEO) && !pStatusTable->IsCurrentStatusEnabled(
            SdpMedia::TYPE_VIDEO, SdpPrecondition::STATUS_LOCAL))
    {
        QosLossPolicy ePartialAction = (IsDefaultBearerUsed(MEDIATYPE_VIDEO)) ?
                QosLossPolicy::MAINTAIN : GetQosLossPolicy(MEDIATYPE_VIDEO);

        if (eAction < ePartialAction)
        {
            eAction = ePartialAction;
        }
    }

    if ((eMediaTypes & MEDIATYPE_TEXT) && !pStatusTable->IsCurrentStatusEnabled(
            SdpMedia::TYPE_TEXT, SdpPrecondition::STATUS_LOCAL))
    {
        QosLossPolicy ePartialAction = (IsDefaultBearerUsed(MEDIATYPE_TEXT)) ?
                QosLossPolicy::MAINTAIN : GetQosLossPolicy(MEDIATYPE_TEXT);

        if (eAction < ePartialAction)
        {
            eAction = ePartialAction;
        }
    }

    IMS_TRACE_D("GetActionForQosLoss : The next Action is %d", eAction, 0, 0);
    return eAction;
}

PRIVATE
void MtcPreconditionManager::InitializeStatusForLostQos(
        IN IMS_UINT32 eMediaTypes, IN QosData* pQosData)
{
    IMS_TRACE_D("InitializeStatusForLostQos : MediaTypes[%d]", eMediaTypes, 0, 0);

    if ((eMediaTypes & MEDIATYPE_AUDIO) && pQosData->GetAudioStatus() == QosStatus::LOST)
    {
        pQosData->SetAudioStatus(QosStatus::IDLE);
    }

    if ((eMediaTypes & MEDIATYPE_VIDEO) && pQosData->GetVideoStatus() == QosStatus::LOST)
    {
        pQosData->SetVideoStatus(QosStatus::IDLE);
    }

    if ((eMediaTypes & MEDIATYPE_TEXT) && pQosData->GetTextStatus() == QosStatus::LOST)
    {
        pQosData->SetTextStatus(QosStatus::IDLE);
    }
}

PRIVATE
void MtcPreconditionManager::UpdateStatusRecords(IN ISession* piSession,
        IN IMS_UINT32 eMediaType)
{
    QosStatusTable* pStatusTable = GetQosStatusTable(piSession);
    if (pStatusTable == IMS_NULL)
    {
        return;
    }

    IMS_TRACE_D("UpdateStatusRecords : MediaType[%d]", eMediaType, 0, 0);
    IMS_SINT32 eSdpMediaType = GetSdpMediaType(eMediaType);

    if (pStatusTable->IsStatusRecordsListEmpty(eSdpMediaType))
    {
        pStatusTable->CreateStatusRecords(eSdpMediaType);
    }

    IMS_BOOL bCheckQosWhileCallUpgrade = IMS_TRUE;
    if (m_objContext.GetConfigurationProxy().GetInt(
            Feature::POLICY_FOR_CHECKING_QOS_WHILE_CALL_UPGRADING) != 2)
    {
        bCheckQosWhileCallUpgrade = IMS_FALSE;
    }

    if ((piSession->GetState() <= ISession::STATE_ESTABLISHED) || bCheckQosWhileCallUpgrade)
    {
        pStatusTable->UpdateLocalCurrentStatus(eSdpMediaType,
                IsReserved(GetQosStatus(piSession, eMediaType)));
    }
}

PRIVATE
IMS_BOOL MtcPreconditionManager::CheckQosTimers(IN ISession* piSession, IN IMS_BOOL bLocalReserved)
{
    IMS_TRACE_D("CheckQosTimers : local reserved [%d]", bLocalReserved, 0, 0);
    QosTimer* pQosTimer = GetQosTimer(piSession);

    if (bLocalReserved) // QosStatus::IDLE or QosStatus::LOST -> QosStatus::AVAILABLE
    {
        if (IsQosEnabled(piSession, QosCheckType::LOCAL_STATUS))
        {
            // UE should stop QoS Timers related to QoS activation when all medias are enabled.
            StopQosTimer(piSession, QosTimerType::WAIT_AVAILABLE);
            StopQosTimer(piSession, QosTimerType::FORCE_AVAILABLE);
        }

        if (pQosTimer && pQosTimer->IsQosTimerActivated(QosTimerType::GUARD_INACTIVE))
        {
            StopQosTimer(piSession, QosTimerType::GUARD_INACTIVE);
            return IMS_FALSE;
        }
    }
    else // QosStatus::IDLE or QosStatus::AVAILABLE -> QosStatus::LOST
    {
        if (pQosTimer && (pQosTimer->IsQosTimerActivated(QosTimerType::WAIT_AVAILABLE) ||
                pQosTimer->IsQosTimerActivated(QosTimerType::FORCE_AVAILABLE)))
        {
            IMS_TRACE_D("CheckQosTimers : There is timer for waiting enable QoS.", 0, 0, 0);
            return IMS_FALSE;
        }
        else
        {
            StartQosTimer(piSession, QosTimerType::GUARD_INACTIVE);
            return IMS_FALSE;
        }
    }

    return IMS_TRUE;
}

PRIVATE
void MtcPreconditionManager::NotifyQosStatusToListener(IN ISession* piSession,
        IN IMS_BOOL bReserved, IN IMS_UINT32 eMediaTypes)
{
    if (bReserved)
    {
        m_pListener->QosReserved(piSession, eMediaTypes);
    }
    else
    {
        QosLossPolicy eAction = GetActionForQosLoss(piSession);

        if (eAction != QosLossPolicy::MAINTAIN)
        {
            m_pListener->QosReserveFailed(piSession, eAction);
        }
    }
}

PRIVATE
IMS_BOOL MtcPreconditionManager::IsReserved(IN QosStatus eStatus)
{
    return (eStatus == QosStatus::AVAILABLE) ? IMS_TRUE : IMS_FALSE;
}


PRIVATE
IMediaDescriptor* MtcPreconditionManager::GetMediaDescriptor(IN IMedia* piMedia)
{
    if (piMedia == IMS_NULL)
    {
        return IMS_NULL;
    }

    if (piMedia->GetUpdateState() == IMedia::UPDATE_MODIFIED)
    {
        return piMedia->GetProposal()->GetMediaDescriptor();
    }
    else
    {
        return piMedia->GetMediaDescriptor();
    }
}

PRIVATE
const SdpMedia* MtcPreconditionManager::GetSdpMedia(IN IMedia* piMedia, IN IMS_BOOL bRemote)
{
    IMediaDescriptor* piMediaDescriptor = GetMediaDescriptor(piMedia);
    if (piMediaDescriptor == IMS_NULL)
    {
        return IMS_NULL;
    }

    if (!bRemote)
    {
        if (piMedia->GetState() == IMedia::STATE_DELETED)
        {
            return IMS_NULL;
        }
        else
        {
            return piMediaDescriptor->GetMediaDescriptionExAsLocal();
        }
    }

    return piMediaDescriptor->GetMediaDescriptionEx();
}

PRIVATE
IMS_SINT32 MtcPreconditionManager::GetSdpMediaType(IN IMS_UINT32 eMediaType)
{
    IMS_SINT32 eSdpMediaType = SdpMedia::TYPE_INVALID;

    switch (eMediaType)
    {
        case MEDIATYPE_AUDIO:
            eSdpMediaType = SdpMedia::TYPE_AUDIO;
            break;
        case MEDIATYPE_VIDEO:
            eSdpMediaType = SdpMedia::TYPE_VIDEO;
            break;
        case MEDIATYPE_TEXT:
            eSdpMediaType = SdpMedia::TYPE_TEXT;
            break;
        default:
            break;
    }

    return eSdpMediaType;
}

PRIVATE
IMS_UINT32 MtcPreconditionManager::GetMediaTypesFromCallType()
{
    IMS_UINT32 eMediaTypes = MEDIATYPE_NONE;
    CallType eCallType = m_objContext.GetCallInfo().eCallType;

    if (eCallType == CallType::UNKNOWN)
    {
        IMS_TRACE_D("GetMediaTypesFromCallType : call type is unknown", 0, 0, 0);
        return eMediaTypes;
    }

    eMediaTypes |= MEDIATYPE_AUDIO;

    if (eCallType == CallType::VT)
    {
        eMediaTypes |= MEDIATYPE_VIDEO;
    }
    else if (eCallType == CallType::RTT)
    {
        eMediaTypes |= MEDIATYPE_TEXT;
    }
    else if (eCallType == CallType::VIDEO_RTT)
    {
        eMediaTypes |= MEDIATYPE_VIDEO;
        eMediaTypes |= MEDIATYPE_TEXT;
    }

    IMS_TRACE_D("GetMediaTypesFromCallType : %d", eMediaTypes, 0, 0);

    return eMediaTypes;
}

PRIVATE
IMS_BOOL MtcPreconditionManager::IsDefaultBearerUsed(IN IMS_UINT32 eMediaType)
{
    IMS_BOOL bUseDefaultBearer = IMS_FALSE;
    if (eMediaType == MEDIATYPE_AUDIO)
    {
        bUseDefaultBearer = !m_objContext.GetConfigurationProxy().Is(
                Feature::DEFAULT_EPS_BEARER_CONTEXT_USAGE_RESTRICTION_ON_CELLULAR) ||
                m_objContext.GetConfigurationProxy().Is(Feature::VOICE_ON_DEFAULT_BEARER_SUPPORTED);
    }
    else if (eMediaType == MEDIATYPE_VIDEO)
    {
        bUseDefaultBearer =
                m_objContext.GetConfigurationProxy().Is(Feature::VIDEO_ON_DEFAULT_BEARER_SUPPORTED);
    }
    else if (eMediaType == MEDIATYPE_TEXT)
    {
        bUseDefaultBearer =
                m_objContext.GetConfigurationProxy().Is(Feature::TEXT_ON_DEFAULT_BEARER_SUPPORTED);
    }

    return bUseDefaultBearer;
}

PRIVATE
void MtcPreconditionManager::InitializeCapability(IN ISession* piSession)
{
    IMS_BOOL bLocalSupport = IsPreconditionSupportedInLocal();
    m_objCapabilities.Add(piSession, bLocalSupport);
}

PRIVATE
void MtcPreconditionManager::InitializeLocalCurrentStatus(IN ISession* piSession)
{
    IMS_UINT32 eMediaTypes = SdpPreconditionHelper::GetMediaTypesBySdp(piSession);
    if (eMediaTypes == MEDIATYPE_NONE)
    {
        eMediaTypes = GetMediaTypesFromCallType();
    }

    QosStatusTable* pStatusTable = GetQosStatusTable(piSession);
    if (!pStatusTable)
    {
        return;
    }

    if (eMediaTypes & MEDIATYPE_AUDIO && IsDefaultBearerUsed(MEDIATYPE_AUDIO))
    {
        pStatusTable->UpdateLocalCurrentStatus(SdpMedia::TYPE_AUDIO, IMS_TRUE);
    }

    if (eMediaTypes & MEDIATYPE_VIDEO && IsDefaultBearerUsed(MEDIATYPE_VIDEO))
    {
        pStatusTable->UpdateLocalCurrentStatus(SdpMedia::TYPE_VIDEO, IMS_TRUE);
    }

    if (eMediaTypes & MEDIATYPE_TEXT && IsDefaultBearerUsed(MEDIATYPE_TEXT))
    {
        pStatusTable->UpdateLocalCurrentStatus(SdpMedia::TYPE_TEXT, IMS_TRUE);
    }
}

PRIVATE
QosLossPolicy MtcPreconditionManager::GetQosLossPolicy(IN IMS_UINT32 eMediaType)
{
    IMS_SINT32 nPolicy = -1;
    if (eMediaType == MEDIATYPE_AUDIO)
    {
        nPolicy = m_objContext.GetConfigurationProxy().GetInt(
                Feature::POLICY_ON_AUDIO_QOS_DEACTIVATION);
    }
    else if (eMediaType == MEDIATYPE_VIDEO)
    {
        nPolicy = m_objContext.GetConfigurationProxy().GetInt(
                Feature::POLICY_ON_VIDEO_QOS_DEACTIVATION);
    }
    else if (eMediaType == MEDIATYPE_TEXT)
    {
        nPolicy = m_objContext.GetConfigurationProxy().GetInt(
                Feature::POLICY_ON_TEXT_QOS_DEACTIVATION);
    }

    switch (nPolicy)
    {
        case 0: // POLICY_TERMINATE_CALL
            return QosLossPolicy::RELEASE;
        case 1: // POLICY_MAINTAIN_CALL
            return QosLossPolicy::MAINTAIN;
        case 2: // POLICY_MODIFY_CALL
            return QosLossPolicy::MODIFY;
        default:
            break;
    }

    return QosLossPolicy::MAINTAIN;
}

PRIVATE
QosStatus MtcPreconditionManager::GetQosStatus(IN ISession* piSession, IN IMS_UINT32 eMediaType)
{
    QosData* pQosData = GetQosData(piSession);
    if (pQosData == IMS_NULL)
    {
        return QosStatus::IDLE;
    }

    if (eMediaType == MEDIATYPE_AUDIO)
    {
        return pQosData->GetAudioStatus();
    }
    else if (eMediaType == MEDIATYPE_VIDEO)
    {
        return pQosData->GetVideoStatus();
    }
    else if (eMediaType == MEDIATYPE_TEXT)
    {
        return pQosData->GetTextStatus();
    }

    return QosStatus::IDLE;
}

PRIVATE
IMS_BOOL MtcPreconditionManager::IsPreconditionSupportedInLocal(
        IN IMS_UINT32 eMediaType /*= MEDIATYPE_NONE*/)
{
    IMS_BOOL bSupport = IMS_FALSE;

    IMS_BOOL bAudioSupport =
            m_objContext.GetConfigurationProxy().Is(Feature::VOICE_QOS_PRECONDITION_SUPPORTED);
    IMS_BOOL bVideoSupport =
            m_objContext.GetConfigurationProxy().Is(Feature::VIDEO_QOS_PRECONDITION_SUPPORTED);
    IMS_BOOL bTextSupport =
            m_objContext.GetConfigurationProxy().Is(Feature::TEXT_QOS_PRECONDITION_SUPPORTED);


    if (eMediaType != MEDIATYPE_NONE)
    {
        if (eMediaType == MEDIATYPE_AUDIO)
        {
            bSupport = bAudioSupport;
        }
        else if (eMediaType == MEDIATYPE_VIDEO)
        {
            bSupport = bVideoSupport;
        }
        else if (eMediaType == MEDIATYPE_TEXT)
        {
            bSupport = bTextSupport;
        }

        IMS_TRACE_D("IsPreconditionSupportedInLocal : MediaType[%d] Result[%d]",
                eMediaType, bSupport, 0);

        return bSupport;
    }

    CallType eCallType = m_objContext.GetCallInfo().eCallType;

    switch (eCallType)
    {
        case CallType::VOIP:
            bSupport = bAudioSupport;
            break;
        case CallType::VT:
            bSupport = bAudioSupport | bVideoSupport;
            break;
        case CallType::RTT:
            bSupport = bAudioSupport | bTextSupport;
            break;
        case CallType::VIDEO_RTT:
            bSupport = bAudioSupport | bVideoSupport | bTextSupport;
            break;
        default:
            break;
    }

    IMS_TRACE_D("IsPreconditionSupportedInLocal : CallType[%d] Result[%d]",
            eCallType, bSupport, 0);
    return bSupport;
}
