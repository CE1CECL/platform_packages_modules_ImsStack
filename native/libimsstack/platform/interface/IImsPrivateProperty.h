/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20180828  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _INTERFACE_IMS_IMS_PRIVATE_PROPERTY_H_
#define _INTERFACE_IMS_IMS_PRIVATE_PROPERTY_H_

#include "AString.h"
#include "ImsPrivateProperties.h"

/**
 * Ephemeral properties : cleared whenever IMS process starts
 * Persistent properties : preserved even though device or IMS re-starts
 */
class IImsPrivateProperty
{
public:
    /*
     Gets an ephemeral IMS private property.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strKey                  Key of the property to be retrieved
    nSlotId                 Slot id
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    AString                 Value of the specified property
    </table>
    */
    virtual AString Get(IN const AString &strKey, IN IMS_SINT32 nSlotId) = 0;

    /*
     Gets an ephemeral IMS private property as boolean.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strKey                  Key of the property to be retrieved
    nSlotId                 Slot id
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_BOOL                Value of the specified property
    </table>
    */
    virtual IMS_BOOL GetBoolean(IN const AString &strKey, IN IMS_SINT32 nSlotId) = 0;

    /*
     Gets an ephemeral IMS private property as integer.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strKey                  Key of the property to be retrieved
    nSlotId                 Slot id
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_SINT32              Value of the specified property
    </table>
    */
    virtual IMS_SINT32 GetInt(IN const AString &strKey, IN IMS_SINT32 nSlotId) = 0;

    /*
     Sets an ephemeral IMS private property.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strName                 Name of the property
    strKey                  Key of the property
    nSlotId                 Slot id
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>
    */
    virtual void Set(IN const AString &strKey, IN const AString &strValue,
            IN IMS_SINT32 nSlotId) = 0;

    /*
     Sets an ephemeral IMS private property as boolean.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strKey                  Key of the property
    bValue                  Value of the property
    nSlotId                 Slot id
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>
    */
    virtual void SetBoolean(IN const AString &strKey, IN IMS_BOOL bValue,
            IN IMS_SINT32 nSlotId) = 0;

    /*
     Sets an ephemeral IMS private property as integer.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strKey                  Key of the property
    nValue                  Value of the property
    nSlotId                 Slot id
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>
    */
    virtual void SetInt(IN const AString &strKey, IN IMS_SINT32 nValue,
            IN IMS_SINT32 nSlotId) = 0;

    /*
     Gets a persistent IMS private property.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strKey                  Key of the property to be retrieved
    nSlotId                 Slot id
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    AString                 Value of the specified property
    </table>
    */
    virtual AString GetPersistent(IN const AString &strKey,
            IN IMS_SINT32 nSlotId) = 0;

    /*
     Gets a persistent IMS private property as boolean.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strKey                  Key of the property to be retrieved
    nSlotId                 Slot id
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_BOOL                Value of the specified property
    </table>
    */
    virtual IMS_BOOL GetPersistentBoolean(IN const AString &strKey,
            IN IMS_SINT32 nSlotId) = 0;

    /*
     Gets a persistent IMS private property as integer.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strKey                  Key of the property to be retrieved
    nSlotId                 Slot id
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_SINT32              Value of the specified property
    </table>
    */
    virtual IMS_SINT32 GetPersistentInt(IN const AString &strKey,
            IN IMS_SINT32 nSlotId) = 0;

    /*
     Sets a persistent IMS private property.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strKey                  Key of the property
    strValue                Value of the property
    nSlotId                 Slot id
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>
    */
    virtual void SetPersistent(IN const AString &strKey, IN const AString &strValue,
            IN IMS_SINT32 nSlotId) = 0;

    /*
     Sets a persistent IMS private property as boolean.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strKey                  Key of the property
    bValue                  Value of the property
    nSlotId                 Slot id
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>
    */
    virtual void SetPersistentBoolean(IN const AString &strKey, IN IMS_BOOL bValue,
            IN IMS_SINT32 nSlotId) = 0;

    /*
     Sets a persistent IMS private property as integer.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strKey                  Key of the property
    nValue                  Value of the property
    nSlotId                 Slot id
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>
    */
    virtual void SetPersistentInt(IN const AString &strKey, IN IMS_SINT32 nValue,
            IN IMS_SINT32 nSlotId) = 0;
};

#endif // _INTERFACE_IMS_IMS_PRIVATE_PROPERTY_H_
