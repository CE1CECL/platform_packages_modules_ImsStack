/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20170704  hwangoo.park@             Created
    </table>

    Description
*/

#ifndef _GEOLOCATION_HELPER_H_
#define _GEOLOCATION_HELPER_H_

#include "GeolocationPidfCreator.h"

class GeolocationHelperPrivate;

class GeolocationHelper
{
private:
    GeolocationHelper();
    ~GeolocationHelper();

    GeolocationHelper(IN const GeolocationHelper& objRHS);
    GeolocationHelper& operator=(IN const GeolocationHelper& objRHS);

public:
    void CreatePidfCreator(IN IMS_SINT32 nSlotId);
    void DestroyPidfCreator(IN IMS_SINT32 nSlotId);
    GeolocationPidfCreator* GetPidfCreator(IN IMS_SINT32 nSlotId);

    static GeolocationHelper* GetInstance();
    // Creates an identifier for Content-ID header field
    static AString CreateContentId(IN IMS_SINT32 nSlotId,
            IN const AString& strDomain = AString::ConstNull());
    // Returns the recent country or newly updated country
    static const AString& GetCountry(IN IMS_SINT32 nSlotId, IN IMS_BOOL bLocationUpdate);

private:
    static GeolocationHelper* pGeolocationHelper;

    GeolocationHelperPrivate* pPrivate;
};

#endif // _GEOLOCATION_HELPER_H_
