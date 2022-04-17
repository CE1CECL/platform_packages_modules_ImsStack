/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20170616  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _INTERFACE_IMS_PHONE_INFO_DEVICE_H_
#define _INTERFACE_IMS_PHONE_INFO_DEVICE_H_

#include "AString.h"

class IDeviceInfo
{
public:
    // CDMA : MEID, WCDMA : IMEI
    virtual IMS_BOOL GetDeviceId(IN IMS_SINT32 nSlotId, OUT AString &strDeviceId) const = 0;
    // CDMA : N/A, WCDMA : Software version
    virtual IMS_BOOL GetDeviceSoftwareVersion(
            IN IMS_SINT32 nSlotId, OUT AString &strSV) const = 0;
    virtual IMS_BOOL GetDeviceName(OUT AString &strDeviceName) const = 0;
};

#endif // _INTERFACE_IMS_PHONE_INFO_DEVICE_H_
