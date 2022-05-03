#ifndef _INTERFACE_SIP_CLIENT_CONNECTION_LISTENER_H_
#define _INTERFACE_SIP_CLIENT_CONNECTION_LISTENER_H_

class ISipClientConnection;

/**
 * @brief This class provides an interface for an incoming SIP responses.
 *
 * @see ISipClientConnection
 */
class ISipClientConnectionListener
{
public:
    /**
     * @brief This method gives the ISipClientConnection instance, which has received
     *        a new SIP response.
     *
     * The application implementing this listener interface has to call
     * ISipClientConnection::Receive() to initialize the ISipClientConnection object
     * with the new response.
     *
     * @param piSCC Pointer to ISipClientConnection object carrying the response
     * @param piForkedSCC Pointer to ISipClientConnection object carrying the forked response
     */
    virtual void ClientConnection_NotifyResponse(IN ISipClientConnection *piSCC,
            IN ISipClientConnection *piForkedSCC = IMS_NULL) = 0;
};

#endif // _INTERFACE_SIP_CLIENT_CONNECTION_LISTENER_H_
