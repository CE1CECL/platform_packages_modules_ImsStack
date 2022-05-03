/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20130518  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _SIP_ROUTING_REJECT_NOTIFIER_H_
#define _SIP_ROUTING_REJECT_NOTIFIER_H_

#include "IMSList.h"
#include "SipStatusCode.h"
#include "ISipRoutingRejectNotifier.h"

class ISipMessage;
class ISipServerConnection;

class SipRoutingRejectNotifier
    : public ISipRoutingRejectNotifier
{
public:
    SipRoutingRejectNotifier();
    virtual ~SipRoutingRejectNotifier();

private:
    SipRoutingRejectNotifier(IN const SipRoutingRejectNotifier& objRHS);
    SipRoutingRejectNotifier& operator=(IN const SipRoutingRejectNotifier& objRHS);

public:
    /*
     Checks if the SIP routing reject notification is required or not.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_BOOL                IMS_TRUE / IMS_FALSE
    </table>

    */
    IMS_BOOL IsNotificationRequired() const;

    /*
     Notifies the applications that the incoming SIP request will be rejected by the engine.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    piSIPMsg                SIP message to be rejected
    objStatusCode           Status code which will be used for request reject
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>

    */
    void NotifyRequestReject(IN ISipMessage *piSIPMsg,
            IN_OUT SipStatusCode &objStatusCode);

    /*
     Notifies the applications that the incoming SIP request will be rejected by the engine.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    piSSC                   SIP server connection to be rejected
    objStatusCode           Status code which will be used for request reject
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>

    */
    void NotifyRequestReject(IN ISipServerConnection *piSSC,
            IN_OUT SipStatusCode &objStatusCode);
private:
    // ISipRoutingRejectNotifier class
    virtual void AddListener(IN ISipRoutingRejectListener *piListener);
    virtual void RemoveListener(IN ISipRoutingRejectListener *piListener);

private:
    IMSList<ISipRoutingRejectListener*> objListeners;
};

#endif // _SIP_ROUTING_REJECT_NOTIFIER_H_
