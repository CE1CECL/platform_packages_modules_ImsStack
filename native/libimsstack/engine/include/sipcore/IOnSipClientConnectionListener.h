/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100525  hwangoo.park@             Created
    </table>

    Description
    This class defines a listener interface for an incoming SIP responses.
*/

#ifndef _INTERFACE_ON_SIP_CLIENT_CONNECTION_LISTENER_H_
#define _INTERFACE_ON_SIP_CLIENT_CONNECTION_LISTENER_H_

class SipClientConnection;

/*
SIP client connection listener interface

Example

See Also
SipClientConnection

*/
class IOnSipClientConnectionListener
{
public:
    /*
     This method gives the SipClientConnection instance, which has received a new SIP response.
    The application implementing this listener interface has to call SipClientConnection::Receive()
    to initialize the SipClientConnection object with the new response.

    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    pSCC                    Pointer to SipClientConnection object carrying the response
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>
    */
    virtual void OnClientConnection_NotifyResponse(IN SipClientConnection* pSCC) = 0;

    //// IMS extensions

    /*
     This method gives the SipClientConnection instance, which has received a new SIP response.
    The application implementing this listener interface has to call SipClientConnection::Receive()
    to initialize the SipClientConnection object with the new response.

    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    pSCC                    Pointer to SipClientConnection object carrying the response
    pForkedSCC              Pointer to SipClientConnection object carrying the forked response
    IMS extensions
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>
    */
    virtual void OnClientConnection_NotifyForkedResponse(
            IN SipClientConnection* pSCC, IN SipClientConnection* pForkedSCC) = 0;
};

#endif  // _INTERFACE_ON_SIP_CLIENT_CONNECTION_LISTENER_H_
