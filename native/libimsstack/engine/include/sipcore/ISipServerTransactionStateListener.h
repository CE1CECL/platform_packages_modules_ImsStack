/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#ifndef _INTERFACE_SIP_SERVER_TRANSACTION_STATE_LISTENER_H_
#define _INTERFACE_SIP_SERVER_TRANSACTION_STATE_LISTENER_H_

#include "SipServerTransactionState.h"

class SipDialogState;

/*
SIP server transaction state listener interface

Example

See Also
SipServerTransactionState
*/
class ISipServerTransactionStateListener
{
public:
    /*

    Remarks

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
    virtual void ServerTransactionState_ForkedRequestReceived(
            IN SipServerTransactionState* pSTState, IN SipDialogEx* pOrigDialogEx) = 0;

    /*

    Remarks

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
    virtual void ServerTransactionState_RequestCreated(IN SipServerTransactionState* pSTState) = 0;

    /*

    Remarks

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
    virtual void ServerTransactionState_RequestReceived(IN SipServerTransactionState* pSTState) = 0;
};

#endif  // _INTERFACE_SIP_SERVER_TRANSACTION_STATE_LISTENER_H_
