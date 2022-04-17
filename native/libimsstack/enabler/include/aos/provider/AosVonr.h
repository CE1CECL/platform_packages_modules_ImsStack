#ifndef _AOS_VONR_H_
#define _AOS_VONR_H_

#include "ITimer.h"
#include "interface/IAosVonr.h"

class IAosAppContext;

class AosVonr
    : public IAosVonr
    , public ITimerListener
{
public:
    AosVonr(IN IAosAppContext *piContext_);
    virtual ~AosVonr();

private:
    AosVonr(IN const AosVonr &objRHS);
    AosVonr& operator=(IN const AosVonr &objRHS);

private:
    // IAosVonr
    virtual IMS_BOOL IsSupported() const;
    virtual IMS_BOOL Set(IN IMS_UINT32 nModule, IN IMS_BOOL bIsSending);

private:
    IMS_BOOL IsAvailableCellular();

    IMS_BOOL IsStarted();
    IMS_BOOL IsStarted(IN IMS_UINT32 nModule);

    IMS_BOOL IsWLAN() const;

    void ProcessEndDelayTimerExpired();

    void SetImsSignaling(IN IMS_BOOL bIsActive);

    void Start(IN IMS_UINT32 nModule);
    void Stop(IN IMS_UINT32 nModule);

    void StartTimer(IN IMS_UINT32 nType, IN IMS_UINT32 nDuration);
    void StopTimer(IN IMS_UINT32 nType);
    void StopAllTimers();

    // ITimerListener
    virtual void Timer_TimerExpired(IN ITimer *piTimer);
    static const IMS_CHAR* TimerToString(IN IMS_UINT32 nType);

    enum
    {
        TIMER_END_DELAY = 0
    };

private:
    IAosAppContext *piContext;
    IMS_SINT32 nSlotId;
    IMS_UINT32 nStartedModules;
    IMS_BOOL bIsStartUpdated;

    IVoNR *piVoNR;
    ITimer *piEndDelayTimer;

    AString strTag;

    static const IMS_UINT32 TIMER_END_DELAY_INTERVAL = 1000;
};
#endif // _AOS_VONR_H_
