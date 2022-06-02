#ifndef _SIP_STACK_CALLBACK_H_
#define _SIP_STACK_CALLBACK_H_

#include "ISipUserData.h"
#include "ISipTimerUtil.h"
#include "txn/sip_txn_common.h"

// Definition of function prototypes for SIP stack transaction layer
typedef SIP_BOOL (*SipStack_FetchTransaction)(
        IN SIP_VOID*, IN SIP_INT32, OUT SIP_VOID**, OUT SIP_VOID**);
typedef SIP_BOOL (*SipStack_ReleaseTransaction)(
        IN SIP_VOID*, IN SIP_INT32, OUT SIP_VOID**, OUT SIP_VOID**);
typedef SIP_BOOL (*SipStack_StartTimer)(
        IN SIP_UINT32, IN SipTimerCallback, IN SIP_VOID*, OUT SIP_VOID**);
typedef SIP_BOOL (*SipStack_StopTimer)(IN SIP_VOID*, OUT SIP_VOID**);
typedef SIP_VOID (*SipStack_OnTimerExpired)(IN ISipUserData*, IN SIP_INT32);

typedef SIP_VOID* (*SipStack_CreateAckRequest)(IN SIP_VOID*, IN ISipUserData*);
typedef SIP_VOID (*SipStack_PreProcessMessageSentByStack)(IN SIP_VOID*, IN ISipUserData*);
typedef SIP_VOID (*SipStack_PostProcessMessageSentByStack)(
        IN SIP_VOID*, IN SIP_CHAR*, IN SIP_UINT32, IN ISipUserData*);
typedef SIP_VOID (*SipStack_DisplayTxnKey)(IN SIP_VOID*);

// Definitions for all SIP stack callbacks
struct SipStackCallbacks
{
    SipStack_FetchTransaction pfnFetchTransaction;
    SipStack_ReleaseTransaction pfnReleaseTransaction;
    SipStack_StartTimer pfnStartTimer;
    SipStack_StopTimer pfnStopTimer;
    SipStack_OnTimerExpired pfnOnTimerExpired;
    SipStack_CreateAckRequest pfnCreateAckRequest;
    SipStack_PreProcessMessageSentByStack pfnPreProcessMessageSentByStack;
    SipStack_PostProcessMessageSentByStack pfnPostProcessMessageSentByStack;
    SipStack_DisplayTxnKey pfnDisplayTxnKey;
};

GLOBAL void SipStackCallback_SetCallbacks(IN const SipStackCallbacks& stCallbacks);

#endif  // _SIP_STACK_CALLBACK_H_
