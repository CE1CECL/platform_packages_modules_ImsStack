#include "SipTxnContext.h"

SipTxnContext::SipTxnContext()
    : pSipTimerContext(new SipTimerContext())
    , pTxnContextData(SIP_NULL)
{
}

SipTxnContext::~SipTxnContext()
{
    if (pSipTimerContext != SIP_NULL)
    {
        delete pSipTimerContext;
        pSipTimerContext = SIP_NULL;
    }
}
