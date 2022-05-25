#ifndef MOCK_I_IMS_AOS_INFO_H_
#define MOCK_I_IMS_AOS_INFO_H_

#include <gmock/gmock.h>

#include "AString.h"

#include "IImsAosInfo.h"

class MockIImsAosInfo : public IImsAosInfo
{
public:
    MOCK_METHOD(AString, GetAssociatedUri, (), (override));
    MOCK_METHOD(IMS_SINT32, GetConnectionType, (), (override));
    MOCK_METHOD(IMS_UINT32, GetImsFeatures, (), (override));
    MOCK_METHOD(IMS_UINT32, GetImsState, (), (override));
    MOCK_METHOD(IMS_SINT32, GetIpcanType, (), (override));
    MOCK_METHOD(AString, GetLastPathHeaderValue, (), (override));
    MOCK_METHOD(AString, GetLocalAddress, (), (override));
    MOCK_METHOD(IMS_UINT32, GetLocalPort, (), (override));
    MOCK_METHOD(IMS_UINT32, GetRegisteredNetworkType, (), (override));
    MOCK_METHOD(AString, GetPathHeaderValue, (), (override));
    MOCK_METHOD(AString, GetPcscfAddress, (), (override));
    MOCK_METHOD(IMS_UINT32, GetPcscfPort, (), (override));
    MOCK_METHOD(IMS_UINT32, GetRegistrationMode, (), (override));
    MOCK_METHOD(AString, GetSupportedHeaderValue, (), (override));
    MOCK_METHOD(AString, GetServiceRouteHeaderValue, (), (override));
    MOCK_METHOD(void, NotifyEmergencyCallState, (IN IMS_BOOL bIsInitialized), (override));
    MOCK_METHOD(void, NotifyPublishState, (IN IMS_BOOL bIsStarted), (override));
    MOCK_METHOD(void, NotifyEmergencySmsState, (IN IMS_BOOL bIsInitialized), (override));
};

#endif  // MOCK_I_IMS_AOS_INFO_H_
