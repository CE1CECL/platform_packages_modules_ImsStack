/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20130518  hwangoo.park@             Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "ISipRoutingRejectListener.h"
#include "SipRoutingRejectNotifier.h"

PUBLIC
SipRoutingRejectNotifier::SipRoutingRejectNotifier()
{
}

PUBLIC VIRTUAL
SipRoutingRejectNotifier::~SipRoutingRejectNotifier()
{
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipRoutingRejectNotifier::IsNotificationRequired() const
{
    return !objListeners.IsEmpty();
}

/*

Remarks

*/
PUBLIC
void SipRoutingRejectNotifier::NotifyRequestReject(IN ISipMessage *piSIPMsg,
        IN_OUT SipStatusCode &objStatusCode)
{
    for (IMS_UINT32 i = 0; i < objListeners.GetSize(); ++i)
    {
        ISipRoutingRejectListener *piListener = objListeners.GetAt(i);

        if (piListener->RoutingReject_NotifyRequest(piSIPMsg, objStatusCode))
        {
            return;
        }
    }
}

/*

Remarks

*/
PUBLIC
void SipRoutingRejectNotifier::NotifyRequestReject(IN ISipServerConnection *piSSC,
        IN_OUT SipStatusCode &objStatusCode)
{
    for (IMS_UINT32 i = 0; i < objListeners.GetSize(); ++i)
    {
        ISipRoutingRejectListener *piListener = objListeners.GetAt(i);

        if (piListener->RoutingReject_NotifyRequest(piSSC, objStatusCode))
        {
            return;
        }
    }
}

/*

Remarks

*/
PRIVATE VIRTUAL
void SipRoutingRejectNotifier::AddListener(IN ISipRoutingRejectListener *piListener)
{
    if (piListener == IMS_NULL)
    {
        return;
    }

    for (IMS_UINT32 i = 0; i < objListeners.GetSize(); ++i)
    {
        ISipRoutingRejectListener *piTmpListener = objListeners.GetAt(i);

        if (piTmpListener == piListener)
        {
            return;
        }
    }

    objListeners.Append(piListener);
}

/*

Remarks

*/
PRIVATE VIRTUAL
void SipRoutingRejectNotifier::RemoveListener(IN ISipRoutingRejectListener *piListener)
{
    if (piListener == IMS_NULL)
    {
        return;
    }

    for (IMS_UINT32 i = 0; i < objListeners.GetSize(); ++i)
    {
        ISipRoutingRejectListener *piTmpListener = objListeners.GetAt(i);

        if (piTmpListener == piListener)
        {
            objListeners.RemoveAt(i);
            return;
        }
    }
}
