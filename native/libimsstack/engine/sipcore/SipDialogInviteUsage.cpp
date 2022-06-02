/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100309  hwangoo.park@             Created
    </table>

    Description
     This class defines an INVITE dialog usage.
        - Created by:
        1) Non-100 provisional responses to INVITE
        2) 200 response to INVITE
        - Destroyed by:
        1) 200 responses to BYE
        2) Certain failure responses to INVITE, UPDATE, PRACK, INFO or BYE
        3) Anything that destroys a dialog and all its usages
*/

#include "ServiceMemory.h"
#include "SipPrivate.h"
#include "SipDialogInviteUsage.h"

__IMS_TRACE_TAG_SIP__;

// clang-format off
PRIVATE GLOBAL const IMS_SINT32
SipDialogInviteUsage::STATE_TABLE[SipDState::STATE_MAX][SipDialogInviteUsage::TRIGGER_MAX] =
{
    // STATE_INIT
    {
        SipDState::STATE_INIT,                 // TRIGGER_INIT
        SipDState::STATE_EARLY,                // TRIGGER_1XX
        SipDState::STATE_CONFIRMED,            // TRIGGER_2XX
        SipDState::STATE_INIT,                 // TRIGGER_NON_2XX
        SipDState::STATE_INIT,                 // TRIGGER_2XX_BYE
        SipDState::STATE_INIT                  // TRIGGER_BYE
    },
    // STATE_TERMINATED
    {
        SipDState::STATE_TERMINATED,           // TRIGGER_INIT
        SipDState::STATE_TERMINATED,           // TRIGGER_1XX
        SipDState::STATE_TERMINATED,           // TRIGGER_2XX
        SipDState::STATE_TERMINATED,           // TRIGGER_NON_2XX
        SipDState::STATE_TERMINATED,           // TRIGGER_2XX_BYE
        SipDState::STATE_TERMINATED            // TRIGGER_BYE
    },
    // STATE_EARLY
    {
        SipDState::STATE_EARLY,                // TRIGGER_INIT
        SipDState::STATE_EARLY,                // TRIGGER_1XX
        SipDState::STATE_CONFIRMED,            // TRIGGER_2XX
        SipDState::STATE_TERMINATED,           // TRIGGER_NON_2XX
        SipDState::STATE_TERMINATED,           // TRIGGER_2XX_BYE
        SipDState::STATE_EARLY                 // TRIGGER_BYE
    },
    // STATE_CONFIRMED
    {
        SipDState::STATE_CONFIRMED,            // TRIGGER_INIT
        SipDState::STATE_CONFIRMED,            // TRIGGER_1XX
        SipDState::STATE_CONFIRMED,            // TRIGGER_2XX
        SipDState::STATE_CONFIRMED,            // TRIGGER_NON_2XX
        SipDState::STATE_TERMINATED,           // TRIGGER_2XX_BYE
        SipDState::STATE_CONFIRMED             // TRIGGER_BYE
    }
};
// clang-format on

PUBLIC
SipDialogInviteUsage::SipDialogInviteUsage(IN SipDialogBase* pDialog_) :
        SipDialogUsage(SipDialogUsage::TYPE_INVITE, pDialog_)
{
}

PUBLIC
SipDialogInviteUsage::SipDialogInviteUsage(IN CONST SipDialogInviteUsage& objRHS) :
        SipDialogUsage(objRHS)
{
}

PUBLIC VIRTUAL SipDialogInviteUsage::~SipDialogInviteUsage() {}

PUBLIC VIRTUAL SipDialogUsage* SipDialogInviteUsage::Clone() const
{
    //---------------------------------------------------------------------------------------------

    return new SipDialogInviteUsage(*this);
}

PUBLIC VIRTUAL IMS_BOOL SipDialogInviteUsage::CompareTo(IN CONST SipMessageInfo& objMInfo) const
{
    const SipMethod& objMethod = objMInfo.GetMethod();

    //---------------------------------------------------------------------------------------------

    // CASE : subscribe usage & register usage
    if (objMethod.Equals(SipMethod::SUBSCRIBE) || objMethod.Equals(SipMethod::REFER) ||
            objMethod.Equals(SipMethod::NOTIFY) || objMethod.Equals(SipMethod::REGISTER))
    {
        return IMS_FALSE;
    }

    return IMS_TRUE;
}

PUBLIC VIRTUAL IMS_SINT32 SipDialogInviteUsage::UpdateUsageDetails(
        IN CONST SipMessageInfo& objMInfo)
{
    //---------------------------------------------------------------------------------------------

    // Update the shared dialog states
    return SipDialogUsage::UpdateUsageDetails(objMInfo);
}

PUBLIC GLOBAL IMS_SINT32 SipDialogInviteUsage::GetNextState(
        IN IMS_SINT32 nState, IN IMS_SINT32 nTrigger)
{
    //---------------------------------------------------------------------------------------------

    if ((nTrigger < TRIGGER_INIT) || (nTrigger >= TRIGGER_MAX))
        return SipDState::STATE_MAX;

    return STATE_TABLE[nState][nTrigger];
}

PROTECTED VIRTUAL IMS_SINT32 SipDialogInviteUsage::GetActionNTrigger(
        IN CONST SipMessageInfo& objMInfo, OUT IMS_SINT32& nTrigger)
{
    ::SipMessage* pstMessage = objMInfo.GetMessage();

    IMS_SINT32 nAction = SipDState::ACTION_TRANSIT_STATE;

    //---------------------------------------------------------------------------------------------

    nTrigger = TRIGGER_INIT;

    if (SipStack::IsRequestMessage(pstMessage))
    {
        if (objMInfo.GetMethod().Equals(SipMethod::BYE))
            nTrigger = TRIGGER_BYE;
    }
    else
    {
        const SipMethod& objMethod = objMInfo.GetMethod();

        nAction = GetActionForResponse(objMInfo);

        IMS_SINT32 nStatusCode = SipStack::GetStatusCode(pstMessage);

        if (SipStatusCode::IsProvisional(nStatusCode))
        {
            nTrigger = TRIGGER_1XX;
        }
        else if (SipStatusCode::IsFinalSuccess(nStatusCode))
        {
            if (objMethod.Equals(SipMethod::BYE))
            {
                nAction = SipDState::ACTION_TRANSIT_STATE;
                nTrigger = TRIGGER_2XX_BYE;
            }
            else
            {
                if (!objMethod.Equals(SipMethod::INVITE))
                {
                    nAction = SipDState::ACTION_IGNORE;
                }

                nTrigger = TRIGGER_2XX;
            }
        }
        else
        {
            if (GetState() == SipDState::STATE_EARLY)
            {
                if (objMethod.Equals(SipMethod::INVITE))
                {
                    nTrigger = TRIGGER_NON_2XX;
                }
            }
            else
            {
                // 4 How to handle in case of BYE ???
                if (!objMethod.Equals(SipMethod::INVITE) && !objMethod.Equals(SipMethod::BYE))
                {
                    nAction = SipDState::ACTION_IGNORE;
                }

                nTrigger = TRIGGER_NON_2XX;
            }
        }
    }

    return nAction;
}

PROTECTED VIRTUAL IMS_BOOL SipDialogInviteUsage::IsUsageTerminated(
        IN IMS_SINT32 nState, IN IMS_SINT32 nTrigger) const
{
    //---------------------------------------------------------------------------------------------

    if (GetNextState(nState, nTrigger) == SipDState::STATE_TERMINATED)
        return IMS_TRUE;

    return IMS_FALSE;
}

PROTECTED VIRTUAL const IMS_CHAR* SipDialogInviteUsage::TriggerToString(
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
        case TRIGGER_2XX_BYE:
            return "TRIGGER_2XX_BYE";
        case TRIGGER_BYE:
            return "TRIGGER_BYE";
        default:
            return SipDialogUsage::TriggerToString(nTrigger);
    }
}
