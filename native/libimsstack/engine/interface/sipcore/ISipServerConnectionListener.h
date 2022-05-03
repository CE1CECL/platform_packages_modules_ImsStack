#ifndef _INTERFACE_SIP_SERVER_CONNECTION_LISTENER_H_
#define _INTERFACE_SIP_SERVER_CONNECTION_LISTENER_H_

class ISipConnectionNotifier;

/**
 * @brief This class provides a listener interface for an incoming SIP requests.
 *
 * @see ISipConnectionNotifier
 */
class ISipServerConnectionListener
{
public:
    /**
     * @brief This method will notify the listener that a new request is received.
     *
     * This method gives the ISipConnectionNotifier instance.\n
     * The user has to call the ISipConnectionNotifier::AcceptAndOpen() to get
     * the ISipServerConnection object that holds the server transaction and the request received.
     *
     * @param piSCN Pointer to ISipConnectionNotifier object carrying ISipServerConnection
     * @param bIsForked Flag to indicate that an incoming request is forked or not
     */
    virtual void ServerConnection_NotifyRequest(IN ISipConnectionNotifier *piSCN,
            IN IMS_BOOL bIsForked = IMS_FALSE) = 0;
};

#endif // _INTERFACE_SIP_SERVER_CONNECTION_LISTENER_H_
