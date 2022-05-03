/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20101120  hwangoo.park@             Created
    </table>

    Description
     This class defines the SIP transaction timer values which are set by the application.
    The default timer values are set according to the 3GPP specification.
*/

#include "ServiceMemory.h"
#include "SipTimerValues.h"



PUBLIC
SipTimerValues::SipTimerValues()
    : nTV_Flags(0)
    , nTV_T1(2000)
    , nTV_T2(16000)
    , nTV_TimerB(128000)
    , nTV_TimerD(128000)
    , nTV_TimerF(128000)
    , nTV_TimerH(128000)
    , nTV_TimerI(17000)
    , nTV_TimerJ(128000)
    , nTV_TimerK(17000)
{
}

PUBLIC
SipTimerValues::SipTimerValues(IN CONST SipTimerValues &objRHS)
    : nTV_Flags(objRHS.nTV_Flags)
    , nTV_T1(objRHS.nTV_T1)
    , nTV_T2(objRHS.nTV_T2)
    , nTV_TimerB(objRHS.nTV_TimerB)
    , nTV_TimerD(objRHS.nTV_TimerD)
    , nTV_TimerF(objRHS.nTV_TimerF)
    , nTV_TimerH(objRHS.nTV_TimerH)
    , nTV_TimerI(objRHS.nTV_TimerI)
    , nTV_TimerJ(objRHS.nTV_TimerJ)
    , nTV_TimerK(objRHS.nTV_TimerK)
{
}

PUBLIC
SipTimerValues::~SipTimerValues()
{
}

PUBLIC
SipTimerValues& SipTimerValues::operator=(IN CONST SipTimerValues &objRHS)
{
    //---------------------------------------------------------------------------------------------

    if (this != &objRHS)
    {
        nTV_Flags = objRHS.nTV_Flags;
        nTV_T1 = objRHS.nTV_T1;
        nTV_T2 = objRHS.nTV_T2;
        nTV_TimerB = objRHS.nTV_TimerB;
        nTV_TimerD = objRHS.nTV_TimerD;
        nTV_TimerF = objRHS.nTV_TimerF;
        nTV_TimerH = objRHS.nTV_TimerH;
        nTV_TimerI = objRHS.nTV_TimerI;
        nTV_TimerJ = objRHS.nTV_TimerJ;
        nTV_TimerK = objRHS.nTV_TimerK;
    }

    return (*this);
}

PUBLIC
IMS_SINT32 SipTimerValues::GetValue(IN IMS_SINT32 nType) const
{
    //---------------------------------------------------------------------------------------------

    if (!IsSet(nType))
    {
        return 0;
    }

    if (nType == TIMER_T1)
    {
        return nTV_T1;
    }
    else if (nType == TIMER_T2)
    {
        return nTV_T2;
    }
    else if (nType == TV_TIMER_B)
    {
        return nTV_TimerB;
    }
    else if (nType == TV_TIMER_D)
    {
        return nTV_TimerD;
    }
    else if (nType == TV_TIMER_F)
    {
        return nTV_TimerF;
    }
    else if (nType == TV_TIMER_H)
    {
        return nTV_TimerH;
    }
    else if (nType == TV_TIMER_I)
    {
        return nTV_TimerI;
    }
    else if (nType == TV_TIMER_J)
    {
        return nTV_TimerJ;
    }
    else if (nType == TV_TIMER_K)
    {
        return nTV_TimerK;
    }

    return 0;
}

PUBLIC
IMS_BOOL SipTimerValues::IsSet(IN IMS_SINT32 nType) const
{
    //---------------------------------------------------------------------------------------------

    return ((this->nTV_Flags & nType) != 0);
}

PUBLIC
void SipTimerValues::SetValue(IN IMS_SINT32 nType, IN IMS_UINT32 nValue)
{
    //---------------------------------------------------------------------------------------------

    this->nTV_Flags |= nType;

    if (nType == TIMER_T1)
    {
        nTV_T1 = nValue;
    }
    else if (nType == TIMER_T2)
    {
        nTV_T2 = nValue;
    }
    else if (nType == TV_TIMER_B)
    {
        nTV_TimerB = nValue;
    }
    else if (nType == TV_TIMER_D)
    {
        nTV_TimerD = nValue;
    }
    else if (nType == TV_TIMER_F)
    {
        nTV_TimerF = nValue;
    }
    else if (nType == TV_TIMER_H)
    {
        nTV_TimerH = nValue;
    }
    else if (nType == TV_TIMER_I)
    {
        nTV_TimerI = nValue;
    }
    else if (nType == TV_TIMER_J)
    {
        nTV_TimerJ = nValue;
    }
    else if (nType == TV_TIMER_K)
    {
        nTV_TimerK = nValue;
    }
}

PUBLIC GLOBAL
SipTimerValues SipTimerValues::CreateTimerValues(IN IMS_SINT32 nT1, IN IMS_SINT32 nT2)
{
    SipTimerValues objTVs;

    //---------------------------------------------------------------------------------------------

    objTVs.nTV_Flags = TV_ALL;

    objTVs.nTV_T1 = nT1;
    objTVs.nTV_T2 = nT2;

    objTVs.nTV_TimerB = nT1 * 64;
    objTVs.nTV_TimerD = nT1 * 64;

    objTVs.nTV_TimerF = nT1 * 64;
    objTVs.nTV_TimerH = nT1 * 64;
    objTVs.nTV_TimerI = nT2 + 1000;
    objTVs.nTV_TimerJ = nT1 * 64;
    objTVs.nTV_TimerK = nT2 + 1000;

    return objTVs;
}
