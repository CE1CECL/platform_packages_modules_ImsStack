#ifndef MOCK_I_IMS_AOS_MONITOR_H_
#define MOCK_I_IMS_AOS_MONITOR_H_

#include <gmock/gmock.h>

#include "IMSTypeDef.h"

#include "IImsAosMonitor.h"

class MockIImsAosMonitor : public IImsAosMonitor
{
public:
    MOCK_METHOD(void, ImsAosMonitor_Connected, (IN IMS_UINT32 nServices, IN IMS_UINT32 nIpcan),
            (override));
    MOCK_METHOD(
            void, ImsAosMonitor_Notify, (IN IMS_UINT32 nType, IN IMS_UINT32 nState), (override));
};

#endif  // MOCK_I_IMS_AOS_MONITOR_H_
