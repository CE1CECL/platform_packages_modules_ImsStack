/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20130801  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _INTERFACE_IMS_SYSTEM_PROPERTY_H_
#define _INTERFACE_IMS_SYSTEM_PROPERTY_H_

#include "AString.h"

class ISystemProperty
{
public:
    /*
     Gets the system property.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strName                 Name of the system property to be retrieved
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    AString                 Value of the specified system property
    </table>
    */
    virtual AString Get(IN const AString &strName) = 0;

    /*
     Sets the system property.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strName                 Name of the system property
    strValue                Value of the system property
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_BOOL                IMS_TRUE/IMS_FALSE
    </table>
    */
    virtual IMS_BOOL Set(IN const AString &strName, IN const AString &strValue) = 0;

    //// FOR READ ONLY PROPERTIES

    /*
     Returns the modem chipset vendor.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    AString                 Modem chipset vendor (qct / mtk / ...)
    </table>
    */
    virtual const AString& GetChipsetVendor() const = 0;

    /*
     Checks if the debug mode is on or not.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_TRUE                The debug mode is on
    IMS_FALSE               The debug mode is off
    </table>
    */
    virtual IMS_BOOL IsDebugMode() const = 0;

    /*
     Checks if the product_overlayfs is supported or not.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_TRUE                product_overlayfs supports
    IMS_FALSE               product_overlayfs not supports
    </table>
    */
    virtual IMS_BOOL IsProductOverlayFs() const = 0;

    /*
     Checks if the server info. (routing info.) MUST be hidden or not.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_TRUE                Server info. MUST be hidden in the log
    IMS_FALSE               Server info. will not be hidden in the log
    </table>
    */
    virtual IMS_BOOL IsServerInfoHiddenInLog() const = 0;

    /*
     Checks if the build type is user mode or not.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_TRUE                The build type is user mode
    IMS_FALSE               The build type is not user mode
    </table>
    */
    virtual IMS_BOOL IsUserMode() const = 0;
};

#endif // _INTERFACE_IMS_SYSTEM_PROPERTY_H_
