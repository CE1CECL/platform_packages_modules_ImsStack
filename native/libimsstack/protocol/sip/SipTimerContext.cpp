#include "SipTimerContext.h"

SipTimerContext::SipTimerContext()
    : pTxnSipTxnTimers(new SipTxnTimerValues())
    , nTimerOptions(0)
{
}

SipTimerContext::~SipTimerContext()
{
    if (pTxnSipTxnTimers != SIP_NULL)
    {
        delete pTxnSipTxnTimers;
        pTxnSipTxnTimers = SIP_NULL;
    }
}
