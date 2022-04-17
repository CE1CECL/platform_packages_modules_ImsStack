/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20141119  changik.jeong             Created
    </table>

    Description

*/

#ifndef _INTERFACE_IMS_PHONE_INFO_LOCATION_H_
#define _INTERFACE_IMS_PHONE_INFO_LOCATION_H_

#include "AString.h"


class ILocationProperties
{
public:
    virtual const AString& GetLatitude() const = 0;
    virtual const AString& GetLongitude() const = 0;
    virtual const AString& GetRadius() const = 0;
    virtual const AString& GetShape() const = 0;
    virtual const AString& GetConfidence() const = 0;
    virtual const AString& GetCurrentTime() const = 0;
    virtual const AString& GetMethod() const = 0;
    virtual const AString& GetCountry() const = 0;
    virtual const AString& GetState() const = 0;
    virtual const AString& GetCity() const = 0;
    virtual const AString& GetPostal() const = 0;
    virtual const AString& GetAltitude() const = 0;
    virtual const AString& GetVerticalAccuracy() const = 0;
};

class ILocationInfo
{
public:
    virtual IMS_BOOL StartLocationInfo(IN IMS_UINT32 nUpdateIntervalInSec) = 0;
    virtual void StopLocationInfo() = 0;
    virtual ILocationProperties* GetLocationProperties(IN IMS_SINT32 nType = LOCATION_ALL) = 0;
    virtual IMS_BOOL MakeInstantLocationInfo() = 0;
    virtual void SetDefaultLocationProperties(IN IMS_BOOL bFromUICC = IMS_TRUE) = 0;
    virtual const AString& GetLastKnownCountry() const = 0;

public:
    // Type for getting location properties
    enum
    {
        LOCATION_ALL = 0,
        LOCATION_POSITION_N_COUNTRY = 1,
        LOCATION_POSITION = 2
    };
};

#endif // _INTERFACE_IMS_PHONE_INFO_LOCATION_H_