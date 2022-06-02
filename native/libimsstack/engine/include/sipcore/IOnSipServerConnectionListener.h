/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100525  hwangoo.park@             Created
    </table>

    Description
     This class defines a listener interface for an incoming SIP requests.
*/

#ifndef _INTERFACE_ON_SIP_SERVER_CONNECTION_LISTENER_H_
#define _INTERFACE_ON_SIP_SERVER_CONNECTION_LISTENER_H_

class SipConnectionNotifier;

/*
SIP server connection listener interface

Example

See Also
SipConnectionNotifier

*/
class IOnSipServerConnectionListener
{
public:
    /*
     This method will notify the listener that a new request is received. This method gives the
    SipConnectionNotifier instance. The user has to call the SipConnectionNotifier::AcceptAndOpen()
    to get the SipServerConnection object that holds the server transaction
    and the request received.

    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    pSCN                    Pointer to SipConnectionNotifier object carrying SipServerConnection
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>
    */
    virtual void OnServerConnection_NotifyRequest(IN SipConnectionNotifier* pSCN) = 0;

    //// IMS extensions

    /*
     This method will notify the listener that a new request is received. This method gives the
    SipConnectionNotifier instance. The user has to call the SipConnectionNotifier::AcceptAndOpen()
    to get the SipServerConnection object that holds the server transaction
    and the request received.

    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    pSCN                    Pointer to SipConnectionNotifier object carrying SipServerConnection
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>
    */
    virtual void OnServerConnection_NotifyForkedRequest(IN SipConnectionNotifier* pSCN) = 0;
};

#endif  // _INTERFACE_ON_SIP_SERVER_CONNECTION_LISTENER_H_
