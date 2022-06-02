/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100309  hwangoo.park@             Created
    </table>

    Description
     This class defines a SUBSCRIBE dialog usage.
        - Created by:
        1) 200 class responses to SUBSCRIBE
        2) 200 class responses to REFER
        3) NOTIFY requests
        - Destroyed by:
        1) 200 class responses to NOTIFY-terminated
        2) NOTIFY or refresh-SUBSCRIBE request timeout
        3) Certain failure responses to NOTIFY or SUBSCRIBE
        4) Expiration without refresh if network issues prevent the terminal NOTIFY from arriving
        5) Anything that destroys a dialog and all its usages
*/

#include "ServiceMemory.h"
#include "SipPrivate.h"
#include "ISipHeader.h"
#include "SipDialogSubscribeUsage.h"

__IMS_TRACE_TAG_SIP__;

// clang-format off
PRIVATE GLOBAL const IMS_SINT32
SipDialogSubscribeUsage::STATE_TABLE[SipDState::STATE_MAX][SipDialogSubscribeUsage::TRIGGER_MAX] =
{
    // STATE_INIT
    {
        SipDState::STATE_INIT,                 // TRIGGER_INIT
        SipDState::STATE_EARLY,                // TRIGGER_1XX
        SipDState::STATE_CONFIRMED,            // TRIGGER_2XX
        SipDState::STATE_INIT,                 // TRIGGER_NON_2XX
        SipDState::STATE_TERMINATED,           // TRIGGER_XXX_NOTIFY_TERMINATED
        SipDState::STATE_CONFIRMED             // TRIGGER_NOTIFY
    },
    // STATE_TERMINATED
    {
        SipDState::STATE_TERMINATED,           // TRIGGER_INIT
        SipDState::STATE_TERMINATED,           // TRIGGER_1XX
        SipDState::STATE_TERMINATED,           // TRIGGER_2XX
        SipDState::STATE_TERMINATED,           // TRIGGER_NON_2XX
        SipDState::STATE_TERMINATED,           // TRIGGER_XXX_NOTIFY_TERMINATED
        SipDState::STATE_TERMINATED            // TRIGGER_NOTIFY
    },
    // STATE_EARLY
    {
        SipDState::STATE_EARLY,                // TRIGGER_INIT
        SipDState::STATE_EARLY,                // TRIGGER_1XX
        SipDState::STATE_CONFIRMED,            // TRIGGER_2XX
        SipDState::STATE_TERMINATED,           // TRIGGER_NON_2XX
        SipDState::STATE_TERMINATED,           // TRIGGER_XXX_NOTIFY_TERMINATED
        SipDState::STATE_CONFIRMED             // TRIGGER_NOTIFY
    },
    // STATE_CONFIRMED
    {
        SipDState::STATE_CONFIRMED,            // TRIGGER_INIT
        SipDState::STATE_CONFIRMED,            // TRIGGER_1XX
        SipDState::STATE_CONFIRMED,            // TRIGGER_2XX
        SipDState::STATE_CONFIRMED,            // TRIGGER_NON_2XX
        SipDState::STATE_TERMINATED,           // TRIGGER_XXX_NOTIFY_TERMINATED
        SipDState::STATE_CONFIRMED             // TRIGGER_NOTIFY
    }
};
// clang-format on

PUBLIC
SipDialogSubscribeUsage::SipDialogSubscribeUsage(IN SipDialogBase* pDialog_) :
        SipDialogUsage(SipDialogUsage::TYPE_SUBSCRIBE, pDialog_),
        nSubState(SUB_STATE_INIT),
        nMethod(METHOD_SUBSCRIBE),
        strEvent(AString::ConstNull()),
        strEventId(AString::ConstNull()),
        nCSeqForNOTIFYWithTerminated(SipPrivate::INVALID_SEQ_NUM)
{
}

PUBLIC
SipDialogSubscribeUsage::SipDialogSubscribeUsage(IN CONST SipDialogSubscribeUsage& objRHS) :
        SipDialogUsage(objRHS),
        nSubState(objRHS.nSubState),
        nMethod(objRHS.nMethod),
        strEvent(objRHS.strEvent),
        strEventId(objRHS.strEventId),
        nCSeqForNOTIFYWithTerminated(objRHS.nCSeqForNOTIFYWithTerminated)
{
}

PUBLIC VIRTUAL SipDialogSubscribeUsage::~SipDialogSubscribeUsage() {}

PUBLIC VIRTUAL IMS_BOOL SipDialogSubscribeUsage::InitDialogUsage(IN CONST SipMessageInfo& objMInfo)
{
    //---------------------------------------------------------------------------------------------

    // For a forked NOTIFY request, checking to NOTIFY method is added
    if (!objMInfo.GetMethod().Equals(SipMethod::SUBSCRIBE) &&
            !objMInfo.GetMethod().Equals(SipMethod::REFER) &&
            !objMInfo.GetMethod().Equals(SipMethod::NOTIFY))
    {
        IMS_TRACE_E(0, "OPERATION NOT ALLOWED : Can't create a subscribe dialog usage for %s",
                objMInfo.GetMethod().ToString().GetStr(), 0, 0);
        return IMS_FALSE;
    }

    if (objMInfo.GetMethod().Equals(SipMethod::REFER))
    {
        nMethod = METHOD_REFER;
        strEvent = Sip::STR_REFER;
        // MULTIPLE_REFER
        strEventId.SetNumber(SipStack::GetCSeqNumber(objMInfo.GetMessage()));
    }
    else
    {
        // Get Event header & set this info.
        if (!SipStack::GetEventHeader(objMInfo.GetMessage(), strEvent, strEventId))
        {
            IMS_TRACE_E(0, "OPERATION FAILED : Getting Event Header failed", 0, 0, 0);
            return IMS_FALSE;
        }
    }

    return SipDialogUsage::InitDialogUsage(objMInfo);
}

PUBLIC VIRTUAL SipDialogUsage* SipDialogSubscribeUsage::Clone() const
{
    //---------------------------------------------------------------------------------------------

    return new SipDialogSubscribeUsage(*this);
}

PUBLIC VIRTUAL IMS_BOOL SipDialogSubscribeUsage::CompareTo(IN CONST SipMessageInfo& objMInfo) const
{
    const SipMethod& objMethod = objMInfo.GetMethod();

    //---------------------------------------------------------------------------------------------

    // Check only NOTIFY request ?????

    if (objMethod.Equals(SipMethod::REFER) && (nMethod != METHOD_REFER))
    {
        return IMS_FALSE;
    }
    else if (objMethod.Equals(SipMethod::SUBSCRIBE) && (nMethod != METHOD_SUBSCRIBE))
    {
        return IMS_FALSE;
    }

    AString strTmpEvent;
    AString strTmpEventId;

    if (!SipStack::GetEventHeader(objMInfo.GetMessage(), strTmpEvent, strTmpEventId))
    {
        IMS_TRACE_E(0, "NOT FOUND : Getting Event header failed (%s)",
                objMethod.ToString().GetStr(), 0, 0);
        return IMS_FALSE;
    }

    if (!this->strEvent.Equals(strTmpEvent))
    {
        IMS_TRACE_I(
                "Event (%s , %s) is not matched", this->strEvent.GetStr(), strTmpEvent.GetStr(), 0);
        return IMS_FALSE;
    }

    // MULTIPLE_REFER
    if ((nMethod == METHOD_REFER) && objMInfo.GetMethod().Equals(SipMethod::REFER) &&
            (objMInfo.GetDirection() == SipMessageInfo::DIRECTION_OUTGOING) &&
            (strTmpEventId.GetLength() == 0))
    {
        strTmpEventId.SetNumber(SipStack::GetCSeqNumber(objMInfo.GetMessage()));
    }

    if (!this->strEventId.Equals(strTmpEventId))
    {
        // MULTIPLE_REFER
        if (nMethod == METHOD_REFER)
        {
            if (strTmpEventId.GetLength() == 0)
            {
                // There is no id parameter in NOTIFY request of the implicit subscription,
                // so, ignore the comparison result.
                IMS_TRACE_I("Event id is not used for 'refer' event package", 0, 0, 0);
                return IMS_TRUE;
            }
            else if (strTmpEventId.Equals("0"))
            {
                IMS_TRACE_I("Event id is not valid. Skip validation check.", 0, 0, 0);
                return IMS_TRUE;
            }
        }

        IMS_TRACE_I("Event id (%s , %s) is not matched", this->strEventId.GetStr(),
                strTmpEventId.GetStr(), 0);
        return IMS_FALSE;
    }

    return IMS_TRUE;
}

PUBLIC VIRTUAL IMS_BOOL SipDialogSubscribeUsage::Equals(IN SipDialogUsage* pDUsage) const
{
    SipDialogSubscribeUsage* pDSubscribeUsage = DYNAMIC_CAST(SipDialogSubscribeUsage*, pDUsage);

    //---------------------------------------------------------------------------------------------

    if (pDSubscribeUsage == IMS_NULL)
    {
        return IMS_FALSE;
    }

    // Check if the shared dialog state equals or not
    if (!SipDialogUsage::Equals(pDSubscribeUsage))
    {
        return IMS_FALSE;
    }

    // Check if the subscribe usage info. equals or not
    if (nMethod != pDSubscribeUsage->nMethod)
    {
        return IMS_FALSE;
    }

    if (!strEvent.Equals(pDSubscribeUsage->strEvent))
    {
        return IMS_FALSE;
    }

    if (!strEventId.Equals(pDSubscribeUsage->strEventId))
    {
        return IMS_FALSE;
    }

    return IMS_TRUE;
}

PUBLIC VIRTUAL AString SipDialogSubscribeUsage::ToString() const
{
    AString strValue;

    strValue.Sprintf("DialogUsage: [%s|%s|%s]", (nMethod == METHOD_REFER) ? "REFER" : "SUBSCRIBE",
            strEvent.GetStr(), strEventId.GetStr());

    return strValue;
}

PUBLIC VIRTUAL IMS_SINT32 SipDialogSubscribeUsage::UpdateUsageDetails(
        IN CONST SipMessageInfo& objMInfo)
{
    ::SipMessage* pstMessage = objMInfo.GetMessage();
    const SipMethod& objMethod = objMInfo.GetMethod();

    //---------------------------------------------------------------------------------------------

    // Update Subscription-State header
    if (SipStack::IsRequestMessage(pstMessage) && objMethod.Equals(SipMethod::NOTIFY))
    {
        AString strSubState;

        if (!SipStack::GetSubscriptionStateHeader(objMInfo.GetMessage(), strSubState))
        {
            IMS_TRACE_E(0, "Getting Subscription-State header failed", 0, 0, 0);
            return SipPrivate::MESSAGE_FAILED;
        }

        if (strSubState.EqualsIgnoreCase(Sip::STR_ACTIVE))
            nSubState = SUB_STATE_ACTIVE;
        else if (strSubState.EqualsIgnoreCase(Sip::STR_PENDING))
            nSubState = SUB_STATE_PENDING;
        else if (strSubState.EqualsIgnoreCase(Sip::STR_TERMINATED))
        {
            nSubState = SUB_STATE_TERMINATED;
            nCSeqForNOTIFYWithTerminated = SipStack::GetCSeqNumber(objMInfo.GetMessage());
        }
    }

    // Update the shared dialog states
    return SipDialogUsage::UpdateUsageDetails(objMInfo);
}

PUBLIC
IMS_BOOL SipDialogSubscribeUsage::InitDialogUsage(IN CONST SipMethod& objMethod)
{
    //---------------------------------------------------------------------------------------------

    if (!objMethod.Equals(SipMethod::SUBSCRIBE) && !objMethod.Equals(SipMethod::REFER) &&
            !objMethod.Equals(SipMethod::NOTIFY))
    {
        IMS_TRACE_E(0, "OPERATION NOT ALLOWED : Can't create a subscribe dialog usage for %s",
                objMethod.ToString().GetStr(), 0, 0);
        return IMS_FALSE;
    }

    if (objMethod.Equals(SipMethod::REFER))
    {
        nMethod = METHOD_REFER;
        strEvent = Sip::STR_REFER;
    }

    return IMS_TRUE;
}

PUBLIC GLOBAL IMS_SINT32 SipDialogSubscribeUsage::GetNextState(
        IN IMS_SINT32 nState, IN IMS_SINT32 nTrigger)
{
    //---------------------------------------------------------------------------------------------

    if ((nTrigger < TRIGGER_INIT) || (nTrigger >= TRIGGER_MAX))
        return SipDState::STATE_MAX;

    return STATE_TABLE[nState][nTrigger];
}

PROTECTED VIRTUAL IMS_SINT32 SipDialogSubscribeUsage::GetActionNTrigger(
        IN CONST SipMessageInfo& objMInfo, OUT IMS_SINT32& nTrigger)
{
    ::SipMessage* pstMessage = objMInfo.GetMessage();

    IMS_SINT32 nAction = SipDState::ACTION_TRANSIT_STATE;

    //---------------------------------------------------------------------------------------------

    nTrigger = TRIGGER_INIT;

    if (SipStack::IsRequestMessage(pstMessage))
    {
        if (objMInfo.GetMethod().Equals(SipMethod::NOTIFY))
        {
            if ((nSubState == SUB_STATE_ACTIVE) || (nSubState == SUB_STATE_PENDING) ||
                    (nSubState == SUB_STATE_TERMINATED))
            {
                nTrigger = TRIGGER_NOTIFY;
            }
        }
    }
    else
    {
        nAction = GetActionForResponse(objMInfo);

        IMS_SINT32 nStatusCode = SipStack::GetStatusCode(pstMessage);

        if (SipStatusCode::IsProvisional(nStatusCode))
        {
            nTrigger = TRIGGER_1XX;
        }
        else if (SipStatusCode::IsFinalSuccess(nStatusCode))
        {
            if (nSubState == SUB_STATE_TERMINATED)
            {
                IMS_UINT32 nCSeq = SipStack::GetCSeqNumber(objMInfo.GetMessage());

                if ((nCSeqForNOTIFYWithTerminated == SipPrivate::INVALID_SEQ_NUM) ||
                        (objMInfo.GetMethod().Equals(SipMethod::NOTIFY) &&
                                (nCSeqForNOTIFYWithTerminated == nCSeq)))
                {
                    nAction = SipDState::ACTION_TRANSIT_STATE;
                    nTrigger = TRIGGER_XXX_NOTIFY_TERMINATED;
                }
                else
                {
                    nTrigger = TRIGGER_2XX;
                }
            }
            else
            {
                nTrigger = TRIGGER_2XX;
            }
        }
        else
        {
            // 4 How to handle in case of NOTIFY with TERMINATED ???

            // A NOTIFY request is considered failed if the response times out,
            // or a non-200 class response code is received which has no "Retry-After" header
            // and no implied further action which can be taken to retry the request
            // (e.g. 401 Unauthorized).
            //
            // If the NOTIFY request fails (as defined above) due to a timeout condition,
            // and the subscription was installed using a soft-state mechanism (such as SUBSCRIBE),
            // the notifier SHOULD remove the subscription.
            //
            // If the NOTIFY request fails (as defined above) due to an error response,
            // and the subscription was installed using a soft-state mechanism,
            // the notifier MUST remove the corresponding subscription.
            //
            // If a NOTIFY request receives a 481 response,
            // the notifier MUST remove the corresponding subscription even if such subscription
            // was installed by non-SUBSCRIBE means (such as an administrative interface).
            nTrigger = TRIGGER_NON_2XX;

            if (nSubState == SUB_STATE_TERMINATED)
            {
                IMS_UINT32 nCSeq = SipStack::GetCSeqNumber(objMInfo.GetMessage());

                if ((nCSeqForNOTIFYWithTerminated == SipPrivate::INVALID_SEQ_NUM) ||
                        (objMInfo.GetMethod().Equals(SipMethod::NOTIFY) &&
                                (nCSeqForNOTIFYWithTerminated == nCSeq)))
                {
                    nAction = SipDState::ACTION_TRANSIT_STATE;
                    nTrigger = TRIGGER_XXX_NOTIFY_TERMINATED;
                }
            }
            else
            {
                if ((nStatusCode != SipStatusCode::SC_401) &&
                        (nStatusCode != SipStatusCode::SC_407))
                {
                    // No "Retry-After" header... ???
                }
            }
        }
    }

    return nAction;
}

PROTECTED VIRTUAL IMS_BOOL SipDialogSubscribeUsage::IsUsageTerminated(
        IN IMS_SINT32 nState, IN IMS_SINT32 nTrigger) const
{
    //---------------------------------------------------------------------------------------------

    if (GetNextState(nState, nTrigger) == SipDState::STATE_TERMINATED)
        return IMS_TRUE;

    return IMS_FALSE;
}

PROTECTED VIRTUAL const IMS_CHAR* SipDialogSubscribeUsage::TriggerToString(
        IN IMS_SINT32 nTrigger) const
{
    //---------------------------------------------------------------------------------------------

    switch (nTrigger)
    {
        case TRIGGER_1XX:
            return "TRIGGER_1XX";
        case TRIGGER_2XX:
            return "TRIGGER_2XX";
        case TRIGGER_NON_2XX:
            return "TRIGGER_NON_2XX";
        case TRIGGER_XXX_NOTIFY_TERMINATED:
            return "TRIGGER_XXX_NOTIFY_TERMINATED";
        case TRIGGER_NOTIFY:
            return "TRIGGER_NOTIFY";
        default:
            return SipDialogUsage::TriggerToString(nTrigger);
    }
}
