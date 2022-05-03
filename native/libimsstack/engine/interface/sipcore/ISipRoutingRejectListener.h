#ifndef _INTERFACE_SIP_ROUTING_REJECT_LISTENER_H_
#define _INTERFACE_SIP_ROUTING_REJECT_LISTENER_H_

#include "SipStatusCode.h"

class ISipMessage;
class ISipServerConnection;

/**
 * @brief This class provides a listener interface for receiving notifications about
 *        the reject to the incoming SIP request.
 *
 * @see ISipRoutingRejectNotifier, ISipMessage, ISipServerConnection
 */
class ISipRoutingRejectListener
{
public:
    /**
     * @brief Notifies the application that the incoming SIP request will be rejected
     *        in the sipcore layer.
     *
     * At this moment, the application can overwrite the status code of the rejected request.
     *
     * @param piSIPMsg SIP message to be rejected
     * @param objStatusCode Status code which will be used for request reject\n
     *                      Application can overwrite the status code to be rejected.
     * @return If the application is changed the status code, returns IMS_TRUE.
     *         Otherwise, returns IMS_FALSE.
     */
    virtual IMS_BOOL RoutingReject_NotifyRequest(
            IN ISipMessage *piSIPMsg, IN_OUT SipStatusCode &objStatusCode) = 0;

    /**
     * @brief Notifies the application that the incoming SIP request will be rejected
     *        in the sipcore layer.
     *
     * At this moment, the application can overwrite the status code of the rejected request.
     *
     * @param piSSC SIP server connection to be rejected
     * @param objStatusCode Status code which will be used for request reject\n
     *                      Application can overwrite the status code to be rejected.
     * @return If the application is changed the status code, returns IMS_TRUE.
     *         Otherwise, returns IMS_FALSE.
     */
    virtual IMS_BOOL RoutingReject_NotifyRequest(
            IN ISipServerConnection *piSSC, IN_OUT SipStatusCode &objStatusCode) = 0;
};

#endif // _INTERFACE_SIP_ROUTING_REJECT_LISTENER_H_
