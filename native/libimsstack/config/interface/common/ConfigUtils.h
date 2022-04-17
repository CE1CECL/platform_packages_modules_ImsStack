/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20150516  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _INTERFACE_CONFIG_UTILS_H_
#define _INTERFACE_CONFIG_UTILS_H_

#include "AString.h"

class ConfigUtils
{
private:
    ConfigUtils();

public:
    /*
     Returns the string value of Ims configuration.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strConfName             Table name
    nId                     Content key of table; (-1) indicates that it's not used
    pszSection              Section name
    pszKey                  Key value
    strValue                Configured value if success
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_RESULT              If the operation is successfully done, returns IMS_SUCCESS
                            Otherwise, returns IMS_FAILURE or other error value
    </table>
    */
    static IMS_RESULT GetValue(IN const AString &strConfName, IN IMS_SINT32 nId,
            IN const IMS_CHAR *pszSection, IN const IMS_CHAR *pszKey,
            OUT AString &strValue);

    /*
     Returns the string value of Ims configuration.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strConfName             Table name
    nId                     Content key of table; (-1) indicates that it's not used
    pszSection              Section name
    nSectionIndex           Index of section; (-1) indicates that it's not used
    pszKey                  Key value
    nKeyIndex               Index of key; (-1) indicates that it's not used
    strValue                Configured value if success
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_RESULT              If the operation is successfully done, returns IMS_SUCCESS
                            Otherwise, returns IMS_FAILURE or other error value
    </table>
    */
    static IMS_RESULT GetValue(IN const AString &strConfName, IN IMS_SINT32 nId,
            IN const IMS_CHAR *pszSection, IN IMS_SINT32 nSectionIndex,
            IN const IMS_CHAR *pszKey, IN IMS_SINT32 nKeyIndex,
            OUT AString &strValue);

    /*
     Returns the boolean value of Ims configuration.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strConfName             Table name
    nId                     Content key of table; (-1) indicates that it's not used
    pszSection              Section name
    pszKey                  Key value
    bValue                  Configured value if success
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_RESULT              If the operation is successfully done, returns IMS_SUCCESS
                            Otherwise, returns IMS_FAILURE or other error value
    </table>
    */
    static IMS_RESULT GetValueBoolean(IN const AString &strConfName, IN IMS_SINT32 nId,
            IN const IMS_CHAR *pszSection, IN const IMS_CHAR *pszKey,
            OUT IMS_BOOL &bValue);

    /*
     Returns the hexadecimal value of Ims configuration.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strConfName             Table name
    nId                     Content key of table; (-1) indicates that it's not used
    pszSection              Section name
    pszKey                  Key value
    nValue                  Configured value if success
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_RESULT              If the operation is successfully done, returns IMS_SUCCESS
                            Otherwise, returns IMS_FAILURE or other error value
    </table>
    */
    static IMS_RESULT GetValueHexadecimal(IN const AString &strConfName, IN IMS_SINT32 nId,
            IN const IMS_CHAR *pszSection, IN const IMS_CHAR *pszKey,
            OUT IMS_UINT32 &nValue);

    /*
     Returns the integer value of Ims configuration.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strConfName             Table name
    nId                     Content key of table; (-1) indicates that it's not used
    pszSection              Section name
    pszKey                  Key value
    nValue                  Configured value if success
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_RESULT              If the operation is successfully done, returns IMS_SUCCESS
                            Otherwise, returns IMS_FAILURE or other error value
    </table>
    */
    static IMS_RESULT GetValueInt(IN const AString &strConfName, IN IMS_SINT32 nId,
            IN const IMS_CHAR *pszSection, IN const IMS_CHAR *pszKey,
            OUT IMS_SINT32 &nValue);
};

#endif // _INTERFACE_CONFIG_UTILS_H_
