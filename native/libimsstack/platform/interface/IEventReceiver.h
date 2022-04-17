
/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20101020  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _INTERFACE_IMS_EVENT_RECEIVER_H_
#define _INTERFACE_IMS_EVENT_RECEIVER_H_

#include "IEventReceiverListener.h"

class IEventReceiver
{
public:
    /*

    Unregisters the specified event from the event receiver.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------

    </table>

    */
    virtual void ResetEvent(IN IMS_SINT32 nEvent) = 0;

    /*

    Registers the specified event on the event receiver.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------

    </table>

    */
    virtual void SetEvent(IN IMS_SINT32 nEvent) = 0;

    /*

    Sets the listener for the event notification.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------

    </table>

    */
    virtual void SetListener(IN IEventReceiverListener *piListener) = 0;
};

#endif // _INTERFACE_IMS_EVENT_RECEIVER_H_
