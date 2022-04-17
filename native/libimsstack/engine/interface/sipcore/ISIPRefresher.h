/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090302  toastops@                 Created
    </table>

    Description
     This class defines a refresher interface for the refreshable requests.
    The refreshable requests can use a refresher: REGISTER, PUBLISH, SUBSCRIBE.
*/

#ifndef _INTERFACE_SIP_REFRESHER_H_
#define _INTERFACE_SIP_REFRESHER_H_

/*
SIP refresher interface

Example

See Also
SIPClientConnection
*/
class ISIPRefresher
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
    virtual IMS_SINT32 GetState() = 0;

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
    virtual IMS_BOOL Update(IN SIPClientConnection* pSCC) = 0;
};

#endif // _INTERFACE_SIP_REFRESHER_H_
