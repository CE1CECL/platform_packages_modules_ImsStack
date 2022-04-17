/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090911  toastops@                 Created
    </table>

    Description

*/

#ifndef _INTERFACE_SERVICE_LISTENER_H_
#define _INTERFACE_SERVICE_LISTENER_H_

class IService;

/*

ServiceListener interface

Example

See Also

*/
class IServiceListener
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
    virtual void Service_RegistrationStarted(IN IService *piService, IN IMS_SINT32 nStatusCode,
            IN IMS_SINT32 nReason) = 0;

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
    virtual void Service_RegistrationUpdated(IN IService *piService, IN IMS_SINT32 nStatusCode,
            IN IMS_SINT32 nReason) = 0;

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
    virtual void Service_RegistrationRemoved(IN IService *piService, IN IMS_SINT32 nStatusCode,
            IN IMS_SINT32 nReason) = 0;

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
    virtual void Service_RegistrationTerminated(IN IService *piService, IN IMS_SINT32 nReason) = 0;
};

#endif // _INTERFACE_SERVICE_LISTENER_H_
