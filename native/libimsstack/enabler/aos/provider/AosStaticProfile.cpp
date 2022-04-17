/*

*/

#include "ServiceTrace.h"
#include "ServiceNetworkPolicy.h"
#include "ServiceUtil.h"

#include "provider/AosStaticConfig.h"
#include "provider/AosStaticProfile.h"

//__IMS_TRACE_TAG_USER_DECL__("AOS");

PUBLIC
AosStaticProfile::AosStaticProfile()
    : m_eProfileType(Type::NORMAL)
    , m_eRegistrationType(AosRegistrationType::NORMAL)
    , m_nConnectionType(NetworkPolicy::APN_IMS)
    , m_nRegistrationFlowId(0)
    , m_objServiceProfiles(IMSList<AosServiceProfile*>())
{

}

PUBLIC VIRTUAL
AosStaticProfile::~AosStaticProfile()
{

}

PUBLIC
void AosStaticProfile::SetProflieType(IN Type eType)
{
     if (eType == Type::NORMAL)
    {
        m_strId = AString("aos_normal");
        m_nConnectionType = NetworkPolicy::APN_IMS;
        m_strRegistrationId = AString("aos_normal_reg");
        m_eRegistrationType = AosRegistrationType::NORMAL;
        m_nRegistrationFlowId = static_cast<IMS_UINT32>(AosRegistrationFlowId::NORMAL);

        // For test purpose
        if (IsWifiTest())
        {
            m_nConnectionType = NetworkPolicy::APN_WIFI;
        }
    }
    else if (eType == Type::EMERGENCY)
    {
        m_strId = AString("aos_emergency");
        m_nConnectionType = NetworkPolicy::APN_EMERGENCY;
        m_strRegistrationId = AString("aos_emergency_reg");
        m_eRegistrationType = AosRegistrationType::EMERGENCY;
        m_nRegistrationFlowId = static_cast<IMS_UINT32>(AosRegistrationFlowId::EMERGENCY);
    }
    else if (eType == Type::RCS)
    {
        m_strId = AString("aos_rcs");
        m_nConnectionType = NetworkPolicy::APN_INTERNET;
        m_strRegistrationId = AString("aos_rcs_reg");
        m_eRegistrationType = AosRegistrationType::RCS;
        m_nRegistrationFlowId = static_cast<IMS_UINT32>(AosRegistrationFlowId::RCS);
    }
    else
    {
        m_strId = AString("aos_default");
        m_nConnectionType = NetworkPolicy::APN_NONE;
    }
}

PUBLIC
void AosStaticProfile::AddService(IN AString strAppId, IN AString strServiceId)
{
    AosServiceProfile* pProfile = new AosServiceProfile(strAppId, strServiceId);
    m_objServiceProfiles.Append(pProfile);
}

PUBLIC
AString& AosStaticProfile::GetId()
{
    return m_strId;
}

PUBLIC
IMS_SINT32 AosStaticProfile::GetConnectionType()
{
    return m_nConnectionType;
}

PUBLIC
IMS_UINT32 AosStaticProfile::GetRegistrationFlowId()
{
    return m_nRegistrationFlowId;
}

PUBLIC
AString& AosStaticProfile::GetRegistrationId()
{
    return m_strRegistrationId;
}

PUBLIC
AosStaticProfile::Type AosStaticProfile::GetProfileType() const
{
    return m_eProfileType;
}

PUBLIC
AosRegistrationType AosStaticProfile::GetRegistrationType() const
{
    return m_eRegistrationType;
}

PUBLIC
const IMSList<AosServiceProfile*>& AosStaticProfile::GetServiceProfiles() const
{
   return m_objServiceProfiles;
}

PRIVATE
IMS_BOOL AosStaticProfile::IsWifiTest()
{
    IImsPrivateProperty* piProperty = UtilService::GetUtilService()->GetPrivateProperty();

    return piProperty->GetPersistentInt(ImsPrivateProperties::Persistent::KEY_WIFI_TEST, 0) == 1;
}