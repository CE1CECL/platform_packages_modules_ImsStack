/*
   Author
   <table>
   date      author                description
   --------  --------------        ----------
   20170110  vijay.nair@           Created
   </table>

   Description

 */

#ifndef _SIP_TXN_TIMER_VALUES_H
#define _SIP_TXN_TIMER_VALUES_H

#include "sip_pf_datatypes.h"

class SipTxnTimerValues
{
    /* T1: RTT Estimate */
    SIP_UINT32    m_usT1Value;

    /* The maximum retransmit interval for non-INVITE Req and INVITE resp*/
    SIP_UINT32    m_usT2Value;

    /* Maximum duration a message will remain in the network */
    SIP_UINT32    m_usT4Value;

    SIP_UINT32    m_usTimerA_Value;
    SIP_UINT32    m_usTimerB_Value;
    SIP_UINT32    m_usTimerD_Value;
    SIP_UINT32    m_usTimerE_Value;
    SIP_UINT32    m_usTimerF_Value;
    SIP_UINT32    m_usTimerG_Value;
    SIP_UINT32    m_usTimerH_Value;
    SIP_UINT32    m_usTimerI_Value;
    SIP_UINT32    m_usTimerJ_Value;
    SIP_UINT32    m_usTimerK_Value;

public:
    SipTxnTimerValues();
    virtual ~SipTxnTimerValues(){}

    SIP_VOID SetTimerValue(SIP_UINT32 usTimerType, SIP_UINT32 usDur);
    SIP_UINT32 GetTimerValue(SIP_UINT32 usTimerType) const;
    SIP_BOOL UpdateSipTimers(IN SIP_UINT32 nTimerOptions,
        const IN SipTxnTimerValues* ptrTxnSipTxnTimers);

private:
    SIP_BOOL IsTimerSet(IN SIP_UINT32 nTimerOptions,IN SIP_UINT32 nType);
    SIP_VOID PrintSIPTxnTimerValues() const;

    enum
    {
        TV_T1 = 0x0001,
        TV_T2 = 0x0002,
        TV_TIMER_B = 0x0004,
        TV_TIMER_D = 0x0008,
        TV_TIMER_F = 0x0010,
        TV_TIMER_H = 0x0020,
        TV_TIMER_I = 0x0040,
        TV_TIMER_J = 0x0080,
        TV_TIMER_K = 0x0100,
        TV_ALL = 0x01FF
    };
};

#endif
