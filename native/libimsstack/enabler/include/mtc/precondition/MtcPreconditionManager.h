#ifndef MTC_PRECONDITION_MANAGER_H_
#define MTC_PRECONDITION_MANAGER_H_

#include "IMSMap.h"
#include "IMSTypeDef.h"

#include "call/IMtcCallContext.h"
#include "MtcDef.h"
#include "media/IMediaDescriptor.h"
#include "media/IMediaQosEventListener.h"
#include "precondition/IMtcPreconditionListener.h"
#include "precondition/IMtcPreconditionManager.h"
#include "precondition/QosStatusTable.h"
#include "precondition/QosTimer.h"
#include "precondition/QosDef.h"

class QosData
{
public:
    inline QosData() :
        eAudioStatus(QosStatus::IDLE),
        eVideoStatus(QosStatus::IDLE),
        eTextStatus(QosStatus::IDLE)
    {
    }

    inline virtual ~QosData()
    {
    }

public:
    inline QosStatus GetAudioStatus() { return eAudioStatus; }
    inline QosStatus GetVideoStatus() { return eVideoStatus; }
    inline QosStatus GetTextStatus() { return eTextStatus; }

    inline void SetAudioStatus(IN QosStatus eStatus) { eAudioStatus = eStatus; }
    inline void SetVideoStatus(IN QosStatus eStatus) { eAudioStatus = eStatus; }
    inline void SetTextStatus(IN QosStatus eStatus) { eAudioStatus = eStatus; }

private:
    QosData& operator=(IN const QosData &objRHS);

private:
    QosStatus eAudioStatus;
    QosStatus eVideoStatus;
    QosStatus eTextStatus;
};

class MtcPreconditionManager :
        public IMtcPreconditionManager,
        public IMediaQosEventListener,
        public IQosTimerListener
{
public:
    MtcPreconditionManager(IN IMtcCallContext& objContext);
    virtual ~MtcPreconditionManager();

private:
    MtcPreconditionManager(IN CONST MtcPreconditionManager &objRHS);
    MtcPreconditionManager& operator=(IN CONST MtcPreconditionManager &objRHS);

public:
    virtual void CreateQos(IN ISession* piSession) override;
    virtual void DestroyQos(IN ISession* piSession) override;
    virtual void SetListener(IN IMtcPreconditionListener* pListener) override;
    virtual IMS_BOOL IsQosEnabled(IN ISession* piSession, IN QosCheckType eType) override;

    virtual void StartQosTimer(IN ISession* piSession,
            IN QosTimerType eType = QosTimerType::WAIT_AVAILABLE) override;
    virtual void StopQosTimer(IN ISession* piSession,
            IN QosTimerType eType = QosTimerType::WAIT_AVAILABLE) override;
    virtual void StopAllQosTimer(IN ISession* piSession) override;

    virtual void UpdatePreconditionCapability(IN ISession* piSession,
            IN IMS_BOOL bCapability) override;
    virtual IMS_BOOL HasPreconditionCapability(IN ISession* piSession) override;
    virtual void UpdatePreconditionAttributes(IN ISession* piSession) override;
    virtual void FormPreconditionSdp(IN ISession* piSession, IN IMS_BOOL bFailure) override;
    virtual void RemovePreconditionSdp(IN ISession* piSession) override;
    virtual IMS_UINT32 EnableLocalCurrentStatus(IN ISession* piSession) override;
    virtual void EnableRemoteCurrentStatus(IN ISession* piSession) override;

public:
    virtual void QosStatusChanged(IN ISession* piSession, IN QosStatus eStatus,
            IN IMS_UINT32 eMediaType) override;

    virtual void OnWaitTimerExpired(IN QosTimer* pQosTimer);
    virtual void OnGuardInactiveTimerExpired(IN QosTimer* pQosTimer);
    virtual void OnForceAvailableTimerExpired(IN QosTimer* pQosTimer);

private:
    void CreateQosTimer(IN ISession* piSession);
    void RemoveQosTimer(IN ISession* piSession);

    void CreateStatusTable(IN ISession* piSession);
    void RemoveStatusTable(IN ISession* piSession);

    void CreateQosData(IN ISession* piSession);
    void RemoveQosData(IN ISession* piSession);

    void DestroyAll();

    QosData* GetQosData(IN ISession* piSession);
    QosTimer* GetQosTimer(IN ISession* piSession);
    QosStatusTable* GetQosStatusTable(IN ISession* piSession);

    void NotifyReserveFailedByQosTimerExpired(IN QosTimer* pQosTimer);
    QosLossPolicy GetActionForQosLoss(IN ISession* piSession);
    void InitializeStatusForLostQos(IN IMS_UINT32 eMediaTypes, IN QosData* pQosData);
    void UpdateStatusRecords(IN ISession* piSession, IN IMS_UINT32 eMediaType);

    // IMS_SINT32 CheckQoSRelatedToSession(IN ISession* pISession, IN IMS_SINT32 eMediaType);
    IMS_BOOL CheckQosTimers(IN ISession* piSession, IN IMS_BOOL bLocalReserved);
    void NotifyQosStatusToListener(IN ISession* piSession, IN IMS_BOOL bReserved,
            IN IMS_UINT32 eMediaTypes);

    IMS_BOOL IsReserved(IN QosStatus eStatus);
    // IMS_BOOL IsQciMatched(IN IMS_SINT32 nQCI, IN IMS_SINT32 eMediaType);
    IMediaDescriptor* GetMediaDescriptor(IN IMedia* piMedia);
    const SdpMedia* GetSdpMedia(IN IMedia* piMedia, IN IMS_BOOL bRemote);

    IMS_SINT32 GetSdpMediaType(IN IMS_UINT32 eMediaType);
    IMS_UINT32 GetMediaTypesFromCallType();
    IMS_BOOL IsDefaultBearerUsed(IN IMS_UINT32 eMediaType);
    void InitializeCapability(IN ISession* piSession);
    void InitializeLocalCurrentStatus(IN ISession* piSession);
    QosLossPolicy GetQosLossPolicy(IN IMS_UINT32 eMediaType);
    QosStatus GetQosStatus(IN ISession* piSession, IN IMS_UINT32 eMediaType);

    IMS_BOOL IsPreconditionSupportedInLocal(IN IMS_UINT32 eMediaType = MEDIATYPE_NONE);

private:
    IMSMap<ISession*, QosData*> m_objQosDatas;
    IMSMap<ISession*, QosTimer*> m_objQosTimers;
    IMSMap<ISession*, QosStatusTable*> m_objStatusTables;
    IMSMap<ISession*, IMS_BOOL> m_objCapabilities;
    IMtcPreconditionListener* m_pListener;
    IMtcCallContext& m_objContext;
};

#endif
