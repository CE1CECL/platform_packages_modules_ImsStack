/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#ifndef _SIP_TRANSACTION_TIMER_H_
#define _SIP_TRANSACTION_TIMER_H_

#include "ITimer.h"

class SIPTransactionTimer
    : public ITimerListener
{
public:
    SIPTransactionTimer(IN SipTxnTimeoutData *pstData_, IN SipTimerCallback pfnTimerCallback_);
    virtual ~SIPTransactionTimer();

private:
    SIPTransactionTimer(IN CONST SIPTransactionTimer &objRHS);
    SIPTransactionTimer& operator=(IN CONST SIPTransactionTimer &objRHS);

public:
    IMS_BOOL Start(IN IMS_SINT32 nDuration);
    void Stop(OUT SipTxnTimeoutData *&pstData_);

    static void FreeTimer(IN void *pvTimerHandle);
    static void TimerExpired(IN SipTimerType enTimer);

private:
    // ITimerListener
    virtual void Timer_TimerExpired(IN ITimer *piTimer);

private:
    ITimer *piTimer;
    SipTxnTimeoutData *pstData;
    SipTimerCallback pfnTimerCallback;
};

#endif // _SIP_TRANSACTION_TIMER_H_
