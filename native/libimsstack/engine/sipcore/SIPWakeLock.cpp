/*
    Author
    <table>
    date      author                        description
    --------  --------------                ----------
    20111015  hwangoo.park@                 Created
    </table>

    Description
*/

#include "ServiceMemory.h"
#include "ServiceEvent.h"
#include "SIPWakeLock.h"

/*

Remarks

*/
PUBLIC GLOBAL
void SIPWakeLock::Acquire(IN CONST SIPMethod &objMethod, IN IMS_UINT32 /* nTimeout = 0 */)
{
    //---------------------------------------------------------------------------------------------

    if (objMethod.Equals(SIPMethod::INVITE))
    {
        // 3 seconds
        IMS_EVENT_SendEventForSlotId(IMS_EVENT_WAKE_LOCK, 0, 3000, IMS_SLOT_0);
    }
    else if (objMethod.Equals(SIPMethod::UPDATE))
    {
        // 2 seconds
        IMS_EVENT_SendEventForSlotId(IMS_EVENT_WAKE_LOCK, 0, 2000, IMS_SLOT_0);
    }
    else if (objMethod.Equals(SIPMethod::BYE))
    {
        // 2 seconds
        IMS_EVENT_SendEventForSlotId(IMS_EVENT_WAKE_LOCK, 0, 2000, IMS_SLOT_0);
    }
    else if (objMethod.Equals(SIPMethod::ACK)
            || objMethod.Equals(SIPMethod::PRACK)
            || objMethod.Equals(SIPMethod::OPTIONS)
            || objMethod.Equals(SIPMethod::MESSAGE)
            || objMethod.Equals(SIPMethod::REFER)
            || objMethod.Equals(SIPMethod::NOTIFY)
            || objMethod.Equals(SIPMethod::INFO))
    {
        // 1 seconds
        IMS_EVENT_SendEventForSlotId(IMS_EVENT_WAKE_LOCK, 0, 1000, IMS_SLOT_0);
    }
}

/*

Remarks

*/
PUBLIC GLOBAL
IMS_BOOL SIPWakeLock::IsSupported()
{
    //---------------------------------------------------------------------------------------------

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC GLOBAL
void SIPWakeLock::Release()
{
    //---------------------------------------------------------------------------------------------
}
