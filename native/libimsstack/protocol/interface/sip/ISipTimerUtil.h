#ifndef __ISIPTIMERUTIL_H__
#define __ISIPTIMERUTIL_H__

#include "sip_pf_datatypes.h"

/* TODO: This typedef is also defined in sip_uf_timer.h. pls look for another approach*/
typedef SIP_VOID (*SIPCB_TIMERHANDLER) (SIP_VOID *pvData, SIP_VOID *pvTimerId);

class ISipTimerUtil
{
    public:

        ISipTimerUtil(){};
        virtual ~ISipTimerUtil(){};

        virtual SIP_BOOL StartTimer
            (
             SIP_VOID            **ppvTimerId,
             SIP_UINT32        uiDuration,
             SIP_UINT16        usResetFlag,
             SIPCB_TIMERHANDLER        cbTimerHandler,
             SIP_VOID            *pvData
            )=0;

        virtual SIP_VOID* StopTimer(
                SIP_VOID    *pvTimerId
            )=0;

        virtual SIP_BOOL ResetTimer
            (
             SIP_VOID        *pvTimerId,
             SIP_UINT32    uiNewDuration
            )=0;

        virtual SIP_VOID* StopTimerEx
            (
             SIP_VOID        *pvTimerId
            )=0;
};
#endif //__ISIPTIMERUTIL_H__
