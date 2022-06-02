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
#include "StaticSip.h"
#include "SipPrivate.h"
#include "SipUtils.h"
#include "SipDebug.h"
#include "SipConfigProxy.h"
#include "SipStackHeaders.h"
#include "SipStackState.h"
#include "SipAckPackage.h"
#include "SipFactoryProxy.h"
#include "SipTransportHelper.h"
#include "SipMessageHandler.h"
#include "SipConnectionNotifierImpl.h"
#include "SipClientConnectionImpl.h"
#include "SipManager.h"

__IMS_TRACE_TAG_SIP__;

PRIVATE
SipManager::SipManager() :
        nState(STATE_INACTIVE)
{
    SipPrivate::Init();
}

PRIVATE
SipManager::~SipManager()
{
    CleanUp();
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipManager::AttachDialogState(IN SipDialogState* pDState)
{
    if (nState != STATE_ACTIVE)
    {
        IMS_TRACE_E(0, "... SIP MANAGER IS NOT ACTIVE ...", 0, 0, 0);
        return IMS_FALSE;
    }

    if (pDState == IMS_NULL)
    {
        return IMS_FALSE;
    }

    IMS_TRACE_I("___ Attach::DialogState (%s)",
            SipDebug::GetCharA1(pDState->GetCallId().GetStr(), 8, '@'), 0, 0);

    return objDialogStates.Append(pDState);
}

/*

Remarks

*/
PUBLIC
void SipManager::DetachDialogState(IN SipDialogState* pDState)
{
    if (nState != STATE_ACTIVE)
    {
        IMS_TRACE_E(0, "... SIP MANAGER IS NOT ACTIVE ...", 0, 0, 0);
        return;
    }

    for (IMS_UINT32 i = 0; i < objDialogStates.GetSize(); ++i)
    {
        SipDialogState* pTempDState = objDialogStates.GetAt(i);

        if (pTempDState != IMS_NULL)
        {
#if 1
            if (pTempDState == pDState)
            {
                IMS_TRACE_I("___ Detach::DialogState (%s)",
                        SipDebug::GetCharA1(pDState->GetCallId().GetStr(), 8, '@'), 0, 0);

                objDialogStates.RemoveAt(i);
                return;
            }
#else
            IMS_SINT32 nComparisonResult = pExistingDState->CompareTo(pDState, IMS_NULL, IMS_FALSE);

            switch (nComparisonResult)
            {
                case SipDialogState::MATCHED:
                    IMS_TRACE_I("___ Detach::DialogState (%s)",
                            pExistingDState->GetCallId().GetStr(), 0, 0);
                    objDialogStates.RemoveAt(n);
                    return;
            }
#endif
        }
    }
}

/*

Remarks

*/
PUBLIC
RCPtr<SipDialogState> SipManager::LookupDialogState(IN SipDialogState* pDState,
        IN ::SipMessage* pstMessage, IN IMS_BOOL bCheckForked /* = IMS_FALSE */,
        OUT IMS_BOOL* pbIsForked /* = IMS_NULL */)
{
    if (nState != STATE_ACTIVE)
    {
        IMS_TRACE_E(0, "... SIP MANAGER IS NOT ACTIVE ...", 0, 0, 0);
        return IMS_NULL;
    }

    if (pbIsForked != IMS_NULL)
    {
        (*pbIsForked) = IMS_FALSE;
    }

    for (IMS_UINT32 i = 0; i < objDialogStates.GetSize(); ++i)
    {
        SipDialogState* pTempDState = objDialogStates.GetAt(i);

        if (pTempDState != IMS_NULL)
        {
            IMS_SINT32 nComparisonResult =
                    pTempDState->CompareTo(pDState, pstMessage, bCheckForked);

            switch (nComparisonResult)
            {
                case SipDialogState::MATCHED:
                    return pTempDState;

                case SipDialogState::NOT_MATCHED:
                    break;

                case SipDialogState::MATCHED_DIFFERENT:
                    break;

                case SipDialogState::MATCHED_FORKED_SUBSCRIBE:
                    if (pbIsForked != IMS_NULL)
                    {
                        (*pbIsForked) = IMS_TRUE;
                    }

                    IMS_TRACE_I("___ FORKED NOTIFY RECEIVED ___", 0, 0, 0);
                    return pTempDState;

                case SipDialogState::MATCHED_EARLY_NOTIFY:
                    IMS_TRACE_I("___ EARLY NOTIFY RECEIVED ___", 0, 0, 0);
                    return pTempDState;

                case SipDialogState::MATCHED_OVERLAP_DIALING:
                    break;

                default:
                    break;
            }
        }
    }

    return IMS_NULL;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipManager::AttachConnectionNotifier(IN SipConnectionNotifier* pSCN)
{
    if (nState != STATE_ACTIVE)
    {
        IMS_TRACE_E(0, "... SIP MANAGER IS NOT ACTIVE ...", 0, 0, 0);
        return IMS_FALSE;
    }

    return objSCNs.Append(pSCN);
}

/*

Remarks

*/
PUBLIC
void SipManager::DetachConnectionNotifier(IN SipConnectionNotifier* pSCN)
{
    if (nState != STATE_ACTIVE)
    {
        IMS_TRACE_E(0, "... SIP MANAGER IS NOT ACTIVE ...", 0, 0, 0);
        return;
    }

    for (IMS_UINT32 i = 0; i < objSCNs.GetSize(); ++i)
    {
        SipConnectionNotifier* pTempSCN = objSCNs.GetAt(i);

        if (pTempSCN != IMS_NULL)
        {
            if (pTempSCN == pSCN)
            {
                objSCNs.RemoveAt(i);
                return;
            }
        }
    }
}

/*

Remarks

*/
PUBLIC
SipConnectionNotifier* SipManager::LookupConnectionNotifier(IN CONST SipTransportAddress& objTA,
        IN CONST AString& strFilter /* = AString::ConstNull() */)
{
    (void)strFilter;

    if (nState != STATE_ACTIVE)
    {
        IMS_TRACE_E(0, "... SIP MANAGER IS NOT ACTIVE ...", 0, 0, 0);
        return IMS_NULL;
    }

    for (IMS_UINT32 i = 0; i < objSCNs.GetSize(); ++i)
    {
        SipConnectionNotifier* pTempSCN = objSCNs.GetAt(i);

        if (pTempSCN != IMS_NULL)
        {
            if (pTempSCN->IsSameConnectionNotifier(objTA))
            {
                return pTempSCN;
            }
        }
    }

    return IMS_NULL;
}

/*

Remarks

*/
PUBLIC GLOBAL SipManager* SipManager::GetInstance()
{
    static SipManager* pSIPMngr = IMS_NULL;

    if (pSIPMngr == IMS_NULL)
    {
        pSIPMngr = new SipManager();
    }

    return pSIPMngr;
}

/*

Remarks

*/
PRIVATE
IMS_BOOL SipManager::StartUp()
{
    if (nState == STATE_ACTIVE)
    {
        IMS_TRACE_E(0, "... SIP MANAGER IS ALREADY ACTIVE ...", 0, 0, 0);
        return IMS_TRUE;
    }

    if (nState == STATE_PENDING)
    {
        return IMS_FALSE;
    }

    // Initialize the SIP stack & transaction layer
    SipStackState::GetInstance()->StartUp();

    // For ACK retransmission for 2xx response to INVITE
    SipAckPackage::Init();

    SipDebug::InitLogging();

    nState = STATE_ACTIVE;

    return IMS_TRUE;
}

/*

Remarks

*/
PRIVATE
void SipManager::CleanUp()
{
    if ((nState == STATE_INACTIVE) || (nState == STATE_PENDING))
    {
        IMS_TRACE_E(0, "... SIP MANAGER IS NOT ACTIVE ...", 0, 0, 0);
        return;
    }

    nState = STATE_PENDING;

    // Delete the dialog state info.
    IMS_TRACE_D("___ Number of DialogState (%d)", objDialogStates.GetSize(), 0, 0);

    if (!objDialogStates.IsEmpty())
    {
        for (IMS_UINT32 i = 0; i < objDialogStates.GetSize(); ++i)
        {
            SipDialogState* pDState = objDialogStates.GetAt(i);

            if (pDState != IMS_NULL)
            {
                pDState->RemoveReference();
            }
        }

        objDialogStates.Clear();
    }

    IMS_TRACE_D("___ Number of ConnectionNotifier (%d)", objSCNs.GetSize(), 0, 0);

    if (!objSCNs.IsEmpty())
    {
        for (IMS_UINT32 i = 0; i < objSCNs.GetSize(); ++i)
        {
            SipConnectionNotifier* pSCN = objSCNs.GetAt(i);

            if (pSCN != IMS_NULL)
            {
                delete pSCN;
            }
        }

        objSCNs.Clear();
    }

    SipStackState::GetInstance()->CleanUp();

    nState = STATE_INACTIVE;
}

/*

Remarks

*/
PUBLIC GLOBAL IMS_BOOL StaticSip::Initialize()
{
    if (!SipManager::GetInstance()->StartUp())
    {
        return IMS_FALSE;  // throw exception
    }

    IMS_TRACE_D(">>> SIP ENGINE IS SUCCESSFULLY LOADED <<<", 0, 0, 0);

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC GLOBAL void StaticSip::Uninitialize()
{
    SipManager::GetInstance()->CleanUp();

    IMS_TRACE_D(">>> SIP ENGINE IS SUCCESSFULLY UNLOADED <<<", 0, 0, 0);
}

/*

Remarks

*/
PUBLIC GLOBAL void StaticSip::InitializeForSlot(IN IMS_SINT32 nSlotId)
{
    IMS_TRACE_D("InitializeForSlot :: slotId=%d", nSlotId, 0, 0);

    IMS_SINT32 nOptions = SipPrivate::OPTIONS_E;

    if (SipConfigProxy::IsCompactFormConfigured(nSlotId))
    {
        nOptions |= SipPrivate::OPT_E_SHORTFORM;
    }
    else
    {
        nOptions |= SipPrivate::OPT_E_FULLFORM;
    }

    SipPrivate::Init(nSlotId, nOptions);

    SipUtils::Init(nSlotId);

    SipTransportHelper* pTransportHelper =
            SipFactoryProxy::GetInstance()->GetTransportHelper(nSlotId);

    // Attach the message handler from the network
    pTransportHelper->SetListener(SipMessageHandler::GetInstance());
}

/*

Remarks

*/
PUBLIC GLOBAL void StaticSip::UninitializeForSlot(IN IMS_SINT32 nSlotId)
{
    (void)nSlotId;

    IMS_TRACE_D("UninitializeForSlot :: slotId=%d", nSlotId, 0, 0);
}
