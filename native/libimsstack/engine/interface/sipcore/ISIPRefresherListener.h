/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090302  toastops@                 Created
    </table>

    Description
     This class defines a listener interface for the refresher.
*/

#ifndef _INTERFACE_SIP_REFRESHER_LISTENER_H_
#define _INTERFACE_SIP_REFRESHER_LISTENER_H_

/*
SIP refresher listener interface

Example

See Also

*/
class ISIPRefresherListener
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
    virtual void Refresher_Refreshed(IN IMS_SINT32 nID, IN IMS_SINT32 nStatusCode,
            IN CONST AString &strReasonPhrase) = 0;
};

#endif // _INTERFACE_SIP_REFRESHER_LISTENER_H_
