/*
*/

#ifndef _AOS_STATIC_PROFILE_H_
#define _AOS_STATIC_PROFILE_H_

#include "IMSList.h"
#include "AString.h"

/// Define a flow Id for registration
enum class AosRegistrationFlowId
{
    NORMAL = 1,
    EMERGENCY,
    RCS
};

/// Define a type for registration
enum class AosRegistrationType
{
    NORMAL = 0,
    EMERGENCY,
    // For fake registration (w/o network interworking)
    FAKE,
    // For only RCS registration
    RCS
};

class AosServiceProfile
{
public:
    AosServiceProfile(IN AString strAppId, IN AString strServiceId)
        : m_strAppId(strAppId)
        , m_ServiceId(strServiceId) {}

    virtual ~AosServiceProfile() {}

    inline AString& GetAppId() {return m_strAppId;}
    inline AString& GetServiceId() {return m_ServiceId;}

public:
    AString m_strAppId;
    AString m_ServiceId;
};

class AosStaticProfile
{
public:
    AosStaticProfile();
    virtual ~AosStaticProfile();

    /// Define a type for aos profile
    enum class Type
    {
        NORMAL = 0,
        EMERGENCY,
        RCS
    };

    void SetProflieType(IN Type eType);
    void AddService(IN AString strAppId, IN AString strServiceId);

    AString& GetId();
    IMS_SINT32 GetConnectionType();
    IMS_UINT32 GetRegistrationFlowId();
    AString& GetRegistrationId();

    Type GetProfileType() const;
    AosRegistrationType GetRegistrationType() const;

    const IMSList<AosServiceProfile*>& GetServiceProfiles() const;

private:
    IMS_BOOL IsWifiTest();

private:
    Type m_eProfileType;
    AosRegistrationType m_eRegistrationType;
    AString m_strId;
    AString m_strRegistrationId;

    IMS_SINT32 m_nConnectionType;
    IMS_UINT32 m_nRegistrationFlowId;
    IMSList<AosServiceProfile*> m_objServiceProfiles;
};

#endif // _AOS_STATIC_PROFILE_H_
