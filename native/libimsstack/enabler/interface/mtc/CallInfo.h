#ifndef CALL_INFO_H_
#define CALL_INFO_H_

#include "AString.h"
#include "IMSTypeDef.h"
#include "IMtcService.h"

enum class CallType
{
    UNKNOWN = 0,
    VOIP = 1,
    VT = 2,
    RTT = 3,
    VIDEO_RTT = 4,
};

enum class PeerType
{
    MO,
    MT,
};

struct CallInfo
{
public:
    CallInfo() :
            eCallType(CallType::VOIP),
            ePeerType(PeerType::MO),
            eServiceType(ServiceType::NORMAL),
            bWifi(IMS_FALSE),
            bEmergency(IMS_FALSE),
            bOffline(IMS_FALSE),
            bUssi(IMS_FALSE),
            bConference(IMS_FALSE),
            bConferenceEnabled(IMS_FALSE),
            bConferenceSubscriptionRequired(IMS_FALSE),
            bRttCapable(IMS_FALSE),
            bVideoCapable(IMS_FALSE),
            strRemoteUri(AString::ConstNull()),
            strSessionIdHeader(AString::ConstNull())
    {
    }

    CallInfo(IN const CallInfo& objRhs) :
            eCallType(objRhs.eCallType),
            ePeerType(objRhs.ePeerType),
            eServiceType(objRhs.eServiceType),
            bWifi(objRhs.bWifi),
            bEmergency(objRhs.bEmergency),
            bOffline(objRhs.bOffline),
            bUssi(objRhs.bUssi),
            bConference(objRhs.bConference),
            bConferenceEnabled(objRhs.bConferenceEnabled),
            bConferenceSubscriptionRequired(objRhs.bConferenceSubscriptionRequired),
            bRttCapable(objRhs.bRttCapable),
            bVideoCapable(objRhs.bVideoCapable),
            strRemoteUri(objRhs.strRemoteUri),
            strSessionIdHeader(objRhs.strSessionIdHeader)
    {
    }

    CallInfo& operator=(IN const CallInfo& objRhs)
    {
        if (this != &objRhs)
        {
            eCallType = objRhs.eCallType;
            ePeerType = objRhs.ePeerType;
            eServiceType = objRhs.eServiceType;
            bWifi = objRhs.bWifi;
            bEmergency = objRhs.bEmergency;
            bOffline = objRhs.bOffline;
            bUssi = objRhs.bUssi;
            bConference = objRhs.bConference;
            bConferenceEnabled = objRhs.bConferenceEnabled;
            bConferenceSubscriptionRequired = objRhs.bConferenceSubscriptionRequired;
            bRttCapable = objRhs.bRttCapable;
            bVideoCapable = objRhs.bVideoCapable;
            strRemoteUri = objRhs.strRemoteUri;
            strSessionIdHeader = objRhs.strSessionIdHeader;
        }

        return *this;
    }

public:
    CallType eCallType;
    PeerType ePeerType;
    ServiceType eServiceType;

    IMS_BOOL bWifi;
    IMS_BOOL bEmergency;
    IMS_BOOL bOffline;
    IMS_BOOL bUssi;
    IMS_BOOL bConference;
    IMS_BOOL bConferenceEnabled;
    IMS_BOOL bConferenceSubscriptionRequired;
    IMS_BOOL bRttCapable;
    IMS_BOOL bVideoCapable;

    AString strRemoteUri;
    AString strSessionIdHeader;
};

#endif
