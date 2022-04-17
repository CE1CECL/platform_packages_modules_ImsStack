/*
   Author
   <table>
   date      author                description
   --------  --------------        ----------
   20170110  vijay.nair@           Created
   </table>

   Description

 */

#include "sip_error.h"
#include "SipTrace.h"
#include "sip_debug.h"
#include "platform/sip_pf_string.h"
#include "SipConfiguration.h"
#include "txn/SipTxn.h"
#include "txn/sip_txn_common.h"
#include "txn/SipTxnTimerValues.h"

SipTxnTimerValues::SipTxnTimerValues()
    : m_usT1Value(SIP_ZERO)
    , m_usT2Value(SIP_ZERO)
    , m_usT4Value(SIP_ZERO)
    , m_usTimerA_Value(SIP_ZERO)
    , m_usTimerB_Value(SIP_ZERO)
    , m_usTimerD_Value(SIP_ZERO)
    , m_usTimerE_Value(SIP_ZERO)
    , m_usTimerF_Value(SIP_ZERO)
    , m_usTimerG_Value(SIP_ZERO)
    , m_usTimerH_Value(SIP_ZERO)
    , m_usTimerI_Value(SIP_ZERO)
    , m_usTimerJ_Value(SIP_ZERO)
    , m_usTimerK_Value(SIP_ZERO)
{
    SipConfiguration* pConfig = SipConfiguration::GetInstance();

    m_usT1Value = pConfig->GetT1();
    m_usT2Value = pConfig->GetT2();
    m_usT4Value = pConfig->GetT4();
    m_usTimerA_Value = m_usT1Value;
    m_usTimerB_Value = pConfig->GetTimerB();
    m_usTimerD_Value = pConfig->GetTimerD();
    m_usTimerE_Value = m_usT1Value;
    m_usTimerF_Value = pConfig->GetTimerF();
    m_usTimerG_Value = m_usT1Value;
    m_usTimerH_Value = pConfig->GetTimerH();
    m_usTimerI_Value = pConfig->GetTimerI();
    m_usTimerJ_Value = pConfig->GetTimerJ();
    m_usTimerK_Value = pConfig->GetTimerK();
}

SIP_VOID SipTxnTimerValues::SetTimerValue(SIP_UINT32 usTimerType, SIP_UINT32 usDur)
{
    switch(usTimerType)
    {
        case SipTxn::TIMER1:
        {
            m_usT1Value = usDur;
        }
        break;
        case SipTxn::TIMER2:
        {
            m_usT2Value = usDur;
        }
        break;
        case SipTxn::TIMER4:
        {
            m_usT4Value = usDur;
        }
        break;
        case SipTxn::TIMERA:
        {
            m_usTimerA_Value = usDur;
        }
        break;
        case SipTxn::TIMERB:
        {
            m_usTimerB_Value = usDur;
        }
        break;
        case SipTxn::TIMERD:
        {
            m_usTimerD_Value = usDur;
        }
        break;
        case SipTxn::TIMERE:
        {
            m_usTimerE_Value = usDur;
        }
        break;
        case SipTxn::TIMERF:
        {
            m_usTimerF_Value = usDur;
        }
        break;
        case SipTxn::TIMERG:
        {
            m_usTimerG_Value = usDur;
        }
        break;
        case SipTxn::TIMERH:
        {
            m_usTimerH_Value = usDur;
        }
        break;
        case SipTxn::TIMERI:
        {
            m_usTimerI_Value = usDur;
        }
        break;
        case SipTxn::TIMERJ:
        {
            m_usTimerJ_Value = usDur;
        }
        break;
        case SipTxn::TIMERK:
        {
            m_usTimerK_Value = usDur;
        }
        break;

        default:
            break;
    }
}

SIP_UINT32 SipTxnTimerValues::GetTimerValue(SIP_UINT32 usTimerType) const
{
    switch(usTimerType)
    {
        case SipTxn::TIMER1:
        {
            return m_usT1Value;
        }
        break;
        case SipTxn::TIMER2:
        {
            return m_usT2Value;
        }
        break;
        case SipTxn::TIMER4:
        {
            return m_usT4Value;
        }
        break;
        case SipTxn::TIMERA:
        {
            return m_usTimerA_Value;
        }
        break;
        case SipTxn::TIMERB:
        {
            return m_usTimerB_Value;
        }
        case SipTxn::TIMERD:
        {
            return m_usTimerD_Value;
        }
        break;
        case SipTxn::TIMERE:
        {
            return m_usTimerE_Value;
        }
        break;
        case SipTxn::TIMERF:
        {
            return m_usTimerF_Value;
        }
        break;
        case SipTxn::TIMERG:
        {
            return m_usTimerG_Value;
        }
        break;
        case SipTxn::TIMERH:
        {
            return m_usTimerH_Value;
        }
        break;
        case SipTxn::TIMERI:
        {
            return m_usTimerI_Value;
        }
        break;
        case SipTxn::TIMERJ:
        {
            return m_usTimerJ_Value;
        }
        break;
        case SipTxn::TIMERK:
        {
            return m_usTimerK_Value;
        }
        break;

        default:
            break;
    }
    return SIP_ZERO;
}

SIP_BOOL SipTxnTimerValues::UpdateSipTimers(IN SIP_UINT32 nTimerOptions,
    const IN SipTxnTimerValues* ptrTxnSipTxnTimers)
{
    if (ptrTxnSipTxnTimers == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTIMER,
                            "ptrTxnSipTxnTimers is NULL",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    if (IsTimerSet(nTimerOptions, TV_ALL) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODTIMER,
                            "nTimerOptions is NOT set",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_DEBUG_WARNING(ESIPTRACE_MODTIMER,
                            "UpdateSipTimers nTimerOptions(%X)",nTimerOptions,SIP_ZERO);

    if (IsTimerSet(nTimerOptions, TV_T1) == SIP_TRUE)
    {
        SetTimerValue(SipTxn::TIMER1,ptrTxnSipTxnTimers->m_usT1Value);
    }

    if (IsTimerSet(nTimerOptions, TV_T2) == SIP_TRUE)
    {
        SetTimerValue(SipTxn::TIMER2,ptrTxnSipTxnTimers->m_usT2Value);
    }

    if (IsTimerSet(nTimerOptions, TV_TIMER_B) == SIP_TRUE)
    {
        SetTimerValue(SipTxn::TIMERB,ptrTxnSipTxnTimers->m_usTimerB_Value);
    }

    if (IsTimerSet(nTimerOptions, TV_TIMER_D) == SIP_TRUE)
    {
        SetTimerValue(SipTxn::TIMERD,ptrTxnSipTxnTimers->m_usTimerD_Value);
    }

    if (IsTimerSet(nTimerOptions, TV_TIMER_F) == SIP_TRUE)
    {
        SetTimerValue(SipTxn::TIMERF,ptrTxnSipTxnTimers->m_usTimerF_Value);
    }

    if (IsTimerSet(nTimerOptions, TV_TIMER_H) == SIP_TRUE)
    {
        SetTimerValue(SipTxn::TIMERH,ptrTxnSipTxnTimers->m_usTimerH_Value);
    }

    if (IsTimerSet(nTimerOptions, TV_TIMER_I) == SIP_TRUE)
    {
        SetTimerValue(SipTxn::TIMERI,ptrTxnSipTxnTimers->m_usTimerI_Value);
    }

    if (IsTimerSet(nTimerOptions, TV_TIMER_J) == SIP_TRUE)
    {
        SetTimerValue(SipTxn::TIMERJ,ptrTxnSipTxnTimers->m_usTimerJ_Value);
    }

    if (IsTimerSet(nTimerOptions, SipTxn::TIMERK) == SIP_TRUE)
    {
        SetTimerValue(SipTxn::TIMERK,ptrTxnSipTxnTimers->m_usTimerK_Value);
    }

    PrintSIPTxnTimerValues();
    return SIP_TRUE;
}

SIP_BOOL SipTxnTimerValues::IsTimerSet(IN SIP_UINT32 nTimerOptions,IN SIP_UINT32 nType)
{
    return (((nTimerOptions & nType) != 0) ? SIP_TRUE : SIP_FALSE);
}

SIP_VOID SipTxnTimerValues::PrintSIPTxnTimerValues() const
{
    SIP_CHAR cTimerLog[SIP_TRACE_MAX_SIZE] = {SIP_ZERO};
    SIP_CHAR const *pcTimerVal =
            "(T1|T2|T4|A|B|D|E|F|G|H|I|J|K)=%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d";

    SipPf_Sprintf(cTimerLog, (SIP_CHAR *)pcTimerVal, m_usT1Value, m_usT2Value, m_usT4Value,
        m_usTimerA_Value, m_usTimerB_Value, m_usTimerD_Value, m_usTimerE_Value, m_usTimerF_Value,
        m_usTimerG_Value, m_usTimerH_Value, m_usTimerI_Value, m_usTimerJ_Value, m_usTimerK_Value);

    SIP_DEBUG_WARNING(ESIPTRACE_MODTIMER,"SIPTimer%s", cTimerLog, SIP_ZERO);
}
