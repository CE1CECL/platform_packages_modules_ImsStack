#ifndef _INTERFACE_SIP_ROUTING_REJECT_NOTIFIER_H_
#define _INTERFACE_SIP_ROUTING_REJECT_NOTIFIER_H_

#include "ImsTypeDef.h"

class ISipRoutingRejectListener;

/**
 * @brief This class provides an interface to monitor SIP reject scenario
 *        of an incoming SIP request by sipcore engine.
 *
 * @see ISipRoutingRejectListener
 */
class ISipRoutingRejectNotifier
{
public:
    /**
     * @brief Adds a listener to monitor the incoming SIP request to be just rejected by engine.
     *
     * @param piListener Listener to be added
     */
    virtual void AddListener(IN ISipRoutingRejectListener *piListener) = 0;

    /**
     * @brief Removes a listener to monitor the incoming SIP request to be just rejected by engine.
     *
     * @param piListener Listener to be removed
     */
    virtual void RemoveListener(IN ISipRoutingRejectListener *piListener) = 0;
};

#endif // _INTERFACE_SIP_ROUTING_REJECT_NOTIFIER_H_
