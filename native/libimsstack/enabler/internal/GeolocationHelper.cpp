/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20170704  hwangoo.park@             Created
    </table>

    Description
*/

#include "ServiceMemory.h"
#include "ServicePhoneInfo.h"
#include "ServiceSystemTime.h"
#include "SystemConfig.h"
#include "Configuration.h"
#include "GeolocationHelper.h"

class GeolocationHelperPrivate
{
public:
    GeolocationHelperPrivate();
    ~GeolocationHelperPrivate();

private:
    GeolocationHelperPrivate(IN const GeolocationHelperPrivate& objRHS);
    GeolocationHelperPrivate& operator=(IN const GeolocationHelperPrivate& objRHS);

public:
    inline void CreatePidfCreator(IN IMS_SINT32 nSlotId)
    {
        if ((nSlotId < IMS_SLOT_0) || (nSlotId >= SystemConfig::GetMaxSimSlot()))
        {
            return;
        }

        if (ppCreator[nSlotId] == IMS_NULL)
        {
            ppCreator[nSlotId] = new GeolocationPidfCreator(nSlotId);
        }
    }

    inline void DestroyPidfCreator(IN IMS_SINT32 nSlotId)
    {
        if ((nSlotId < IMS_SLOT_0) || (nSlotId >= SystemConfig::GetMaxSimSlot()))
        {
            return;
        }

        if (ppCreator[nSlotId] != IMS_NULL)
        {
            delete ppCreator[nSlotId];
            ppCreator[nSlotId] = IMS_NULL;
        }
    }

    inline GeolocationPidfCreator* GetPidfCreator(IN IMS_SINT32 nSlotId) const
    {
        if ((nSlotId < IMS_SLOT_0) || (nSlotId >= SystemConfig::GetMaxSimSlot()))
        {
            nSlotId = IMS_SLOT_0;
        }

        return ppCreator[nSlotId];
    }

private:
    GeolocationPidfCreator **ppCreator;
};

PUBLIC
GeolocationHelperPrivate::GeolocationHelperPrivate()
    : ppCreator(IMS_NULL)
{
    IMS_SINT32 nSimCount = SystemConfig::GetMaxSimSlot();

    ppCreator = new GeolocationPidfCreator*[nSimCount];

    for (IMS_SINT32 i = 0; i < nSimCount; ++i)
    {
        ppCreator[i] = IMS_NULL;
    }
}

PUBLIC
GeolocationHelperPrivate::~GeolocationHelperPrivate()
{
    if (ppCreator != IMS_NULL)
    {
        IMS_SINT32 nSimCount = SystemConfig::GetMaxSimSlot();

        for (IMS_SINT32 i = 0; i < nSimCount; ++i)
        {
            if (ppCreator[i] != IMS_NULL)
            {
                delete ppCreator[i];
            }
        }

        delete[] ppCreator;
    }
}



PRIVATE GLOBAL
GeolocationHelper* GeolocationHelper::pGeolocationHelper = IMS_NULL;

PRIVATE
GeolocationHelper::GeolocationHelper()
    : pPrivate(new GeolocationHelperPrivate())
{
}

PRIVATE
GeolocationHelper::~GeolocationHelper()
{
    if (pPrivate != IMS_NULL)
    {
        delete pPrivate;
    }
}

PUBLIC
void GeolocationHelper::CreatePidfCreator(IN IMS_SINT32 nSlotId)
{
    pPrivate->CreatePidfCreator(nSlotId);
}

PUBLIC
void GeolocationHelper::DestroyPidfCreator(IN IMS_SINT32 nSlotId)
{
    pPrivate->DestroyPidfCreator(nSlotId);
}

PUBLIC
GeolocationPidfCreator* GeolocationHelper::GetPidfCreator(IN IMS_SINT32 nSlotId)
{
    return pPrivate->GetPidfCreator(nSlotId);
}

PUBLIC GLOBAL
GeolocationHelper* GeolocationHelper::GetInstance()
{
    if (pGeolocationHelper == IMS_NULL)
    {
        pGeolocationHelper = new GeolocationHelper();
    }

    return pGeolocationHelper;
}

// Creates an identifier for Content-ID header field
PUBLIC GLOBAL
AString GeolocationHelper::CreateContentId(IN IMS_SINT32 nSlotId,
        IN const AString& strDomain/* = AString::ConstNull()*/)
{
    IMS_UINT32 nRandom = IMS_SYS_GetSRandom0();
    IMS_UINT32 nMicroSeconds = IMS_SYS_GetTimeInMicroSeconds();

    AString strContentId;

    if (strDomain.GetLength() > 0)
    {
        strContentId.Sprintf("%05x%05x@%s", nMicroSeconds, nRandom, strDomain.GetStr());
    }
    else
    {
        const ISubscriberConfig* piSubsConfig
                = Configuration::GetInstance()->GetSubscriberConfig(nSlotId);

        strContentId.Sprintf("%05x%05x@%s", nMicroSeconds, nRandom,
                (piSubsConfig == IMS_NULL) ?\
                    "unavailable.com" : piSubsConfig->GetHomeDomainName().GetStr());
    }

    return strContentId;
}

// Returns the recent country or newly updated country
PUBLIC GLOBAL
const AString& GeolocationHelper::GetCountry(IN IMS_SINT32 nSlotId, IN IMS_BOOL bLocationUpdate)
{
    ILocationInfo *piLocationInfo
            = PhoneInfoService::GetPhoneInfoService()->GetLocationInfo(nSlotId);

    if (piLocationInfo == IMS_NULL)
    {
        return AString::ConstNull();
    }

    if (bLocationUpdate)
    {
        ILocationProperties *piLocation =
                piLocationInfo->GetLocationProperties(ILocationInfo::LOCATION_POSITION_N_COUNTRY);

        if (piLocation != IMS_NULL)
        {
            return piLocation->GetCountry();
        }
        else
        {
            return AString::ConstNull();
        }
    }
    else
    {
        return piLocationInfo->GetLastKnownCountry();
    }
}
