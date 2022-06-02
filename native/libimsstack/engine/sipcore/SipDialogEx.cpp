/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100323  hwangoo.park@             Created
    </table>

    Description
     This class defines an extended SIP dialog.
    It has a dialog usage to support a multiple dialog usage in SIP.
*/

#include "ServiceMemory.h"
#include "SipPrivate.h"
#include "SipDebug.h"
#include "SipDialogInviteUsage.h"
#include "SipDialogSubscribeUsage.h"
#include "SipDialogEx.h"

__IMS_TRACE_TAG_SIP__;

PUBLIC
SipDialogEx::SipDialogEx(IN SipDialogState* pDState_) :
        SipDialogBase(pDState_),
        bFlag_PermanentDialog(IMS_FALSE),
        bFlag_InitWithDelay(IMS_FALSE),
        bFlag_IsDialogTerminated(IMS_FALSE),
        pDialogUsage(IMS_NULL)
{
}

PUBLIC
SipDialogEx::SipDialogEx(IN CONST SipDialogEx& objRHS) :
        SipDialogBase(objRHS),
        bFlag_PermanentDialog(objRHS.bFlag_PermanentDialog),
        bFlag_InitWithDelay(objRHS.bFlag_InitWithDelay),
        bFlag_IsDialogTerminated(objRHS.bFlag_IsDialogTerminated),
        pDialogUsage(IMS_NULL)
{
    // NOTE: If reference count is not used, you MUST implement this copy constructor.
}

PUBLIC VIRTUAL SipDialogEx::~SipDialogEx()
{
    if (bFlag_PermanentDialog && !bFlag_IsDialogTerminated)
    {
        SipDialogState* pDState = GetDialogState();

        if (pDState != IMS_NULL)
        {
            pDState->RemoveDialogUsage(this);
        }
    }

    if (pDialogUsage != IMS_NULL)
    {
        delete pDialogUsage;
        pDialogUsage = IMS_NULL;
    }

#ifdef __IMS_SIP_DEBUG__
    IMS_TRACE_D("Destructor :: SipDialogEx (%s)",
            SipDebug::GetCharA1(GetDialogState()->GetCallId().GetStr(), 8, '@'), 0, 0);
#endif
}

PUBLIC
SipDialogEx& SipDialogEx::operator=(IN CONST SipDialogEx& objRHS)
{
    //---------------------------------------------------------------------------------------------

    if (this != &objRHS)
    {
        SipDialogBase::operator=(objRHS);
        bFlag_PermanentDialog = objRHS.bFlag_PermanentDialog;
        bFlag_InitWithDelay = objRHS.bFlag_InitWithDelay;
        bFlag_IsDialogTerminated = objRHS.bFlag_IsDialogTerminated;
        // NOTE: If reference count is not used, this MUST be newly created.
        if (objRHS.pDialogUsage == IMS_NULL)
        {
            pDialogUsage = IMS_NULL;
        }
        else
        {
            pDialogUsage = objRHS.pDialogUsage->Clone();
        }
    }

    return (*this);
}

// For an initial requests
PUBLIC
IMS_BOOL SipDialogEx::InitDialog(IN CONST SipMethod& objMethod)
{
    //---------------------------------------------------------------------------------------------

    if (objMethod.Equals(SipMethod::INVITE))
    {
        pDialogUsage = new SipDialogInviteUsage(this);
    }
    else if (objMethod.Equals(SipMethod::SUBSCRIBE) || objMethod.Equals(SipMethod::REFER))
    {
        SipDialogSubscribeUsage* pSubscribeUsage = new SipDialogSubscribeUsage(this);

        if (pSubscribeUsage == IMS_NULL)
        {
            return IMS_FALSE;
        }

        if (!pSubscribeUsage->InitDialogUsage(objMethod))
        {
            IMS_TRACE_E(0, "Initializing a dialog usage (%s) failed", objMethod.ToString().GetStr(),
                    0, 0);

            delete pSubscribeUsage;
            return IMS_FALSE;
        }

        pDialogUsage = pSubscribeUsage;
    }
    else
    {
        pDialogUsage = new SipDialogUsage(this);
    }

    if (pDialogUsage == IMS_NULL)
    {
        IMS_TRACE_E(
                0, "Instantiating a dialog usage (%s) failed", objMethod.ToString().GetStr(), 0, 0);
        return IMS_FALSE;
    }

    // Before sending a request message to the network,
    // if the method, "InitDialogWithDelay(...)" is invoked, this flag will be used to identify
    // whether the operation will be performed or not.
    bFlag_InitWithDelay = IMS_TRUE;

    return IMS_TRUE;
}

// For a dialog request or incoming requests
PUBLIC
IMS_BOOL SipDialogEx::InitDialog(IN CONST SipMessageInfo& objMInfo)
{
    const SipMethod& objMethod = objMInfo.GetMethod();

    //---------------------------------------------------------------------------------------------

    if (objMethod.Equals(SipMethod::INVITE))
    {
        pDialogUsage = new SipDialogInviteUsage(this);
    }
    // For a forked NOTIFY request, adds NOTIFY method checking
    else if (objMethod.Equals(SipMethod::SUBSCRIBE) || objMethod.Equals(SipMethod::REFER) ||
            objMethod.Equals(SipMethod::NOTIFY))
    {
        pDialogUsage = new SipDialogSubscribeUsage(this);
    }
    else
    {
        pDialogUsage = new SipDialogUsage(this);
    }

    if (pDialogUsage == IMS_NULL)
    {
        IMS_TRACE_E(
                0, "Instantiating a dialog usage (%s) failed", objMethod.ToString().GetStr(), 0, 0);
        return IMS_FALSE;
    }

    if (!pDialogUsage->InitDialogUsage(objMInfo))
    {
        IMS_TRACE_E(
                0, "Initializing a dialog usage (%s) failed", objMethod.ToString().GetStr(), 0, 0);

        delete pDialogUsage;
        pDialogUsage = IMS_NULL;

        return IMS_FALSE;
    }

    return IMS_TRUE;
}

PUBLIC
IMS_BOOL SipDialogEx::InitDialogWithDelay(IN CONST SipMessageInfo& objMInfo)
{
    //---------------------------------------------------------------------------------------------

    // If we don't need to initialize a dialog, it will be ignored in here.
    if (!bFlag_InitWithDelay)
    {
        return IMS_TRUE;
    }

    if (pDialogUsage == IMS_NULL)
    {
        return IMS_FALSE;
    }

    return pDialogUsage->InitDialogUsage(objMInfo);
}

PUBLIC
IMS_BOOL SipDialogEx::CompareTo(IN CONST SipMessageInfo& objMInfo) const
{
    //---------------------------------------------------------------------------------------------

    if (pDialogUsage == IMS_NULL)
    {
        return IMS_FALSE;
    }

    return pDialogUsage->CompareTo(objMInfo);
}

PUBLIC
IMS_BOOL SipDialogEx::Equals(IN SipDialogEx* pDialogEx) const
{
    //---------------------------------------------------------------------------------------------

    if (pDialogEx == IMS_NULL)
    {
        return IMS_FALSE;
    }

    if ((pDialogUsage == IMS_NULL) || (pDialogEx->pDialogUsage == IMS_NULL))
    {
        return IMS_FALSE;
    }

    return pDialogUsage->Equals(pDialogEx->pDialogUsage);
}

PUBLIC
IMS_BOOL SipDialogEx::IsInviteUsage() const
{
    //---------------------------------------------------------------------------------------------

    if (pDialogUsage == IMS_NULL)
    {
        return IMS_FALSE;
    }

    return (pDialogUsage->GetType() == SipDialogUsage::TYPE_INVITE);
}

PUBLIC
IMS_BOOL SipDialogEx::IsDialogTerminated() const
{
    //---------------------------------------------------------------------------------------------

    return bFlag_IsDialogTerminated;
}

PUBLIC
void SipDialogEx::TerminateDialogUsage()
{
    IMS_TRACE_I("TerminateDialogUsage :: %s",
            ((pDialogUsage != IMS_NULL) ? pDialogUsage->ToString().GetStr() : "__NULL__"), 0, 0);

    OnTerminated();
}

PUBLIC
IMS_SINT32 SipDialogEx::UpdateDialogDetails(IN CONST SipMessageInfo& objMInfo)
{
    //---------------------------------------------------------------------------------------------

    if (pDialogUsage == IMS_NULL)
    {
        return SipPrivate::MESSAGE_FAILED;
    }

    return pDialogUsage->UpdateUsageDetails(objMInfo);
}

PUBLIC GLOBAL SipDialogEx* SipDialogEx::CreateDialog(IN CONST SipMethod& objMethod)
{
    SipDialogState* pDState = new SipDialogState();

    //---------------------------------------------------------------------------------------------

    if (pDState == IMS_NULL)
    {
        IMS_TRACE_E(
                0, "Instantiating a dialog state (%s) failed", objMethod.ToString().GetStr(), 0, 0);
        return IMS_NULL;
    }

    SipDialogEx* pDialogEx = CreateDialog(pDState, objMethod);

    if (pDialogEx == IMS_NULL)
    {
        delete pDState;

        IMS_TRACE_E(0, "Instantiating a dialog (%s) failed", objMethod.ToString().GetStr(), 0, 0);
        return IMS_NULL;
    }

    return pDialogEx;
}

PUBLIC GLOBAL SipDialogEx* SipDialogEx::CreateDialog(
        IN SipDialogState* pDState, IN CONST SipMethod& objMethod)
{
    SipDialogEx* pDialogEx = new SipDialogEx(pDState);

    //---------------------------------------------------------------------------------------------

    if (pDialogEx == IMS_NULL)
    {
        IMS_TRACE_E(0, "Instantiating a dialog (%s) failed", objMethod.ToString().GetStr(), 0, 0);
        return IMS_NULL;
    }

    if (!pDialogEx->InitDialog(objMethod))
    {
        delete pDialogEx;
        return IMS_NULL;
    }

    return pDialogEx;
}

PUBLIC GLOBAL SipDialogEx* SipDialogEx::CreateDialog(
        IN SipDialogState* pDState, IN CONST SipMessageInfo& objMInfo)
{
    SipDialogEx* pDialogEx = new SipDialogEx(pDState);

    //---------------------------------------------------------------------------------------------

    if (pDialogEx == IMS_NULL)
    {
        IMS_TRACE_E(0, "Instantiating a dialog (%s) failed",
                objMInfo.GetMethod().ToString().GetStr(), 0, 0);
        return IMS_NULL;
    }

    if (!pDialogEx->InitDialog(objMInfo))
    {
        delete pDialogEx;
        return IMS_NULL;
    }

    return pDialogEx;
}

PROTECTED VIRTUAL IMS_BOOL SipDialogEx::OnInit()
{
    //---------------------------------------------------------------------------------------------

    if (bFlag_PermanentDialog == IMS_FALSE)
    {
        SipDialogState* pDState = GetDialogState();

        if (pDState == IMS_NULL)
        {
            return IMS_FALSE;
        }

        if (!pDState->AddDialogUsage(this))
        {
            return IMS_FALSE;
        }

        bFlag_PermanentDialog = IMS_TRUE;
    }

    return IMS_TRUE;
}

PROTECTED VIRTUAL void SipDialogEx::OnTerminated()
{
    //---------------------------------------------------------------------------------------------

    if (bFlag_PermanentDialog == IMS_TRUE)
    {
        SipDialogState* pDState = GetDialogState();

        if (pDState != IMS_NULL)
        {
            pDState->RemoveDialogUsage(this);
        }

        bFlag_PermanentDialog = IMS_FALSE;
    }

    bFlag_IsDialogTerminated = IMS_TRUE;
}

PROTECTED VIRTUAL IMS_SINT32 SipDialogEx::OnUpdateDialogDetails(IN CONST SipMessageInfo& objMInfo,
        IN IMS_SINT32 nUsage, IN IMS_SINT32 nAction, IN IMS_SINT32 nTrigger)
{
    IMS_SINT32 nUsageState = SipDState::STATE_MAX;

    //---------------------------------------------------------------------------------------------

    switch (nUsage)
    {
        case SipDialogUsage::TYPE_INVITE:
            nUsageState = SipDialogInviteUsage::GetNextState(GetState(), nTrigger);
            break;

        case SipDialogUsage::TYPE_SUBSCRIBE:
            nUsageState = SipDialogSubscribeUsage::GetNextState(GetState(), nTrigger);
            break;

        default:
            break;
    }

    SipDialogState* pDState = GetDialogState();

    if (pDState == IMS_NULL)
    {
        return SipPrivate::MESSAGE_FAILED;
    }

    return pDState->UpdateDialogDetails(objMInfo, nUsageState, nAction, nTrigger);
}
