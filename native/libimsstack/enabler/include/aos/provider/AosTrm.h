#ifndef _AOS_TRM_H_
#define _AOS_TRM_H_

#include "ITimer.h"
#include "ITRM.h"
#include "interface/IAosTrm.h"

class AosTrm
    : public IAosTrm
    , public ITRMListener
    , public ITimerListener
{
public:
    AosTrm(IN IMS_SINT32 nSlotId_);
    virtual ~AosTrm();

    virtual void SetListener(IN IAosTrmListener *piListener);
    virtual void RemoveListener(IN IAosTrmListener *piListener);

    virtual IMS_BOOL IsReady();
    virtual IMS_BOOL IsTRMSupported();
    virtual void Set(IN IMS_UINT32 nType, IN IMS_BOOL bStart);
    virtual void SetEmegency(IN IMS_UINT32 nType, IN IMS_BOOL bStart);
    virtual void SetIPCAN(IN IN IMS_UINT32 nCategory);

private:
    void Start(IN IMS_UINT32 nType);
    IMS_BOOL IsStarted();
    IMS_BOOL IsStarted(IN IMS_UINT32 nType);
    void Stop(IN IMS_UINT32 nType);

    void StartTimer(IN IMS_UINT32 nDuration);
    void StopTimer();

    virtual void ProcessTimerExpired();

    // ITRMListener
    virtual void NotifyServicePriorityChanged();

    // ITimerListener Interface
    virtual void Timer_TimerExpired(IN ITimer *piTimer);

private:
    IMS_SINT32 nSlotId;
    IMS_UINT32 nServices;
    IMS_UINT32 nIPCANCategory;
    IMS_BOOL bIsStartUpdated;
    IMS_BOOL bIsEmergencyStartUpdated;

    ITRM *piTRM;
    ITimer *piStopTimer;
    IMSList<IAosTrmListener*> objListeners;

    AString strTag;

    static const IMS_UINT32 TIME_STOP_DELAY = 1000;
};
#endif // _AOS_TRM_H_
