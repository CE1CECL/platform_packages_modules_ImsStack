/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090305  lovil@                    Created
    20090831  yhrhee@                   Adjust file location and class re-organize
                                        for layered approach
    </table>

    Description
    It's a timer interface provided by Platform interface layer.
    It can be used after allocating a timer interface from timer service.
*/

#ifndef _INTERFACE_IMS_CORE_TIMER_H_
#define _INTERFACE_IMS_CORE_TIMER_H_

#include "IMSTypeDef.h"

class ICoreTimerListener;

// Application creates ICoreTimer object using TimerService
// and can use SetTimer/KillTimer.
class ICoreTimer
{
public:
    /*
    Remarks
     CoreTimer is different from a generic timer interface.
     Generic timer is exipred by using its own thread, so there may be an error
     with thread context switching time.
     Generally, it will be around 20ms. CoreTimer can be expired with more correct time,
     but programmer needs to be considered how it works.
     The callback which notifies timer expiration will be invoked using the thread of timer
     service, so application programmer needs to consider thread-safe control.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    nDuration               Timer duration (milli-seconds)
    piListener              Listener interface pointer, supplier should implement its listener
                            to receive notification of the timer expiration.
    bRepeat                 Repeat or one shot
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_UINTP               Timer id to identify this object
    </table>
    */
    virtual IMS_UINTP SetTimer(IN IMS_UINT32 nDuration, IN ICoreTimerListener *piListener,
            IN IMS_BOOL bRepeat = IMS_FALSE) = 0;

    virtual void KillTimer() = 0;
};

class ICoreTimerListener
{
public:
    virtual void CoreTimer_TimerExpired(IN ICoreTimer *piCoreTimer) = 0;
};

#endif // _INTERFACE_IMS_CORE_TIMER_H_
