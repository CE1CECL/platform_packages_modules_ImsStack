/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100317  hwangoo.park@             Created
    </table>

    Description
     This class defines a shared dialog state.
    SipDialogState class MUST have this class as its member.
*/

#include "ServiceMemory.h"
#include "SipPrivate.h"
#include "SipDebug.h"
#include "SipManager.h"
#include "SipDialogEx.h"
#include "SipDialogSharedState.h"

__IMS_TRACE_TAG_SIP__;

PUBLIC
SipDialogSharedState::SipDialogSharedState() :
        nSharedState(SHARED_STATE_INIT)
{
}

PUBLIC
SipDialogSharedState::~SipDialogSharedState()
{
#ifdef __IMS_SIP_DEBUG__
    IMS_TRACE_D("Destructor :: SipDialogSharedState (STATE: %d)", nSharedState, 0, 0);
#endif
}

PRIVATE
IMS_BOOL SipDialogSharedState::AddDialog(IN SipDialogEx* pDialogEx)
{
    //---------------------------------------------------------------------------------------------

    if (pDialogEx == IMS_NULL)
    {
        return IMS_FALSE;
    }

    if (nSharedState == SHARED_STATE_TERMINATED)
    {
        IMS_TRACE_D("INVALID STATE : adding a dialog usage for dialog (%s)",
                SipDebug::GetCharA1(pDialogEx->GetDialogState()->GetCallId().GetStr(), 8, '@'), 0,
                0);
        return IMS_FALSE;
    }

    if (!objDialogExs.Append(pDialogEx))
    {
        IMS_TRACE_E(0, "Adding a dialog usage for dialog (%s) failed",
                SipDebug::GetCharA1(pDialogEx->GetDialogState()->GetCallId().GetStr(), 8, '@'), 0,
                0);
        return IMS_FALSE;
    }

    if (nSharedState == SHARED_STATE_INIT)
    {
        nSharedState = SHARED_STATE_ACTIVE;

        // Attach a dialog state : make it a permanent dialog state
        SipManager::GetInstance()->AttachDialogState(pDialogEx->GetDialogState());
    }

    return IMS_TRUE;
}

PRIVATE
void SipDialogSharedState::RemoveDialog(IN SipDialogEx* pDialogEx)
{
    //---------------------------------------------------------------------------------------------

    if (pDialogEx == IMS_NULL)
    {
        return;
    }

    if (nSharedState != SHARED_STATE_ACTIVE)
    {
        IMS_TRACE_D("INVALID STATE : removing a dialog usage for dialog (%s)",
                SipDebug::GetCharA1(pDialogEx->GetDialogState()->GetCallId().GetStr(), 8, '@'), 0,
                0);
        return;
    }

    for (IMS_UINT32 i = 0; i < objDialogExs.GetSize(); ++i)
    {
        SipDialogEx* pTempDialogEx = objDialogExs.GetAt(i);

        if (pTempDialogEx->Equals(pDialogEx))
        {
            objDialogExs.RemoveAt(i);

            if (objDialogExs.IsEmpty())
            {
                nSharedState = SHARED_STATE_TERMINATED;

                // Detach a dialog state : dialog will be destroyed after a few minutes
                SipManager::GetInstance()->DetachDialogState(pDialogEx->GetDialogState());
            }

            return;
        }
    }
}

PRIVATE
SipDialogEx* SipDialogSharedState::GetDialog(IN CONST SipMessageInfo& objMInfo)
{
    //---------------------------------------------------------------------------------------------

    if (nSharedState != SHARED_STATE_ACTIVE)
        return IMS_NULL;

    for (IMS_UINT32 i = 0; i < objDialogExs.GetSize(); ++i)
    {
        SipDialogEx* pDialogEx = objDialogExs.GetAt(i);

        if (pDialogEx->CompareTo(objMInfo))
        {
            return pDialogEx;
        }
    }

    return IMS_NULL;
}

PRIVATE
IMS_BOOL SipDialogSharedState::HasMultipleDialogUsages() const
{
    //---------------------------------------------------------------------------------------------

    if (nSharedState != SHARED_STATE_ACTIVE)
    {
        return IMS_FALSE;
    }

    if (objDialogExs.GetSize() < 2)
    {
        return IMS_FALSE;
    }

    IMS_BOOL bHasInviteUsage = IMS_FALSE;
    IMS_BOOL bHasSubscribeUsage = IMS_FALSE;

    for (IMS_UINT32 i = 0; i < objDialogExs.GetSize(); ++i)
    {
        SipDialogEx* pDialogEx = objDialogExs.GetAt(i);

        if (pDialogEx == IMS_NULL)
        {
            continue;
        }

        if (pDialogEx->IsInviteUsage())
        {
            bHasInviteUsage = IMS_TRUE;
        }
        else
        {
            bHasSubscribeUsage = IMS_TRUE;
        }
    }

    return (bHasInviteUsage && bHasSubscribeUsage);
}
