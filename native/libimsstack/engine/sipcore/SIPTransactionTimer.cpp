/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "ServiceTimer.h"
#include "Connector.h"
#include "SIPPrivate.h"
#include "SIPStackHeaders.h"
#include "SIPStackState.h"
#include "SIPTransactionTimer.h"

__IMS_TRACE_TAG_SIP__;

PUBLIC
SIPTransactionTimer::SIPTransactionTimer(IN SipTxnTimeoutData *pstData_,
        IN SipTimerCallback pfnTimerCallback_)
    : piTimer(IMS_NULL)
    , pstData(pstData_)
    , pfnTimerCallback(pfnTimerCallback_)
{
}

PUBLIC
SIPTransactionTimer::~SIPTransactionTimer()
{
    //---------------------------------------------------------------------------------------------

    if (piTimer != IMS_NULL)
    {
        piTimer->KillTimer();
        TimerService::GetTimerService()->DestroyTimer(piTimer);

        piTimer = IMS_NULL;
    }

#ifdef __IMS_SIP_DEBUG__
    IMS_TRACE_D("Destructor :: SIPTransactionTimer", 0, 0, 0);
#endif
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SIPTransactionTimer::Start(IN IMS_SINT32 nDuration)
{
    //---------------------------------------------------------------------------------------------

    if (piTimer != IMS_NULL)
    {
        piTimer->KillTimer();
        TimerService::GetTimerService()->DestroyTimer(piTimer);

        piTimer = IMS_NULL;
    }

    piTimer = TimerService::GetTimerService()->CreateTimer();

    if (piTimer == IMS_NULL)
    {
        IMS_TRACE_E(0, "Opening a transaction timer failed", 0, 0, 0);
        return IMS_FALSE;
    }

    piTimer->SetTimer(nDuration, this);
    /*
    {
        piTimer->Close();
        piTimer = IMS_NULL;

        IMS_TRACE_E(0, "Starting a transaction timer failed", 0, 0, 0);
        return IMS_FALSE;
    }*/

    IMS_TRACE_I(">>> START TIMER - TIMER (%p), TYPE (%s), DURATION (%d)",
            piTimer, SIPStack::GetTimerTypeAsString(pstData), nDuration);

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
void SIPTransactionTimer::Stop(OUT SipTxnTimeoutData *&pstData_)
{
    //---------------------------------------------------------------------------------------------

    if (piTimer != IMS_NULL)
    {
        IMS_TRACE_I(">>> STOP TIMER - TIMER (%p), TYPE (%s)",
                piTimer, SIPStack::GetTimerTypeAsString(pstData), 0);

        piTimer->KillTimer();
        TimerService::GetTimerService()->DestroyTimer(piTimer);

        piTimer = IMS_NULL;
    }

    pstData_ = pstData;

    pstData = IMS_NULL;
    pfnTimerCallback = IMS_NULL;
}

/*

Remarks

*/
PUBLIC GLOBAL
void SIPTransactionTimer::FreeTimer(IN void *pvTimerHandle)
{
    SIPTransactionTimer *pTimer = static_cast<SIPTransactionTimer*>(pvTimerHandle);

    //---------------------------------------------------------------------------------------------

    delete pTimer;
}

/*

Remarks

*/
PUBLIC GLOBAL
void SIPTransactionTimer::TimerExpired(IN SipTimerType enTTimer)
{
    //---------------------------------------------------------------------------------------------

    (void) enTTimer;

    IMS_TRACE_I("___ EXPIRED TIMER - TYPE (%s) ___",
            SIPStack::GetTimerTypeAsString(enTTimer), 0, 0);
}

/*

Remarks

*/
PRIVATE VIRTUAL
void SIPTransactionTimer::Timer_TimerExpired(IN ITimer *piTimer)
{
    //---------------------------------------------------------------------------------------------

    if (this->piTimer == IMS_NULL)
    {
        return;
    }

    if (this->piTimer != piTimer)
    {
        return;
    }

    IMS_TRACE_I(">>> TIMER TIMEOUT - TIMER (%p), TYPE (%s)",
            this->piTimer, SIPStack::GetTimerTypeAsString(pstData), 0);

    this->piTimer->KillTimer();
    TimerService::GetTimerService()->DestroyTimer(this->piTimer);
    this->piTimer = IMS_NULL;

    if (pfnTimerCallback != IMS_NULL)
    {
        SIPStack::InvokeTimerCallback(pfnTimerCallback, pstData, reinterpret_cast<IMS_PVOID>(this));
    }
}
