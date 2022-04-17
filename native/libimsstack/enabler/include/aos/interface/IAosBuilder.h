#ifndef _INTERFACE_AOS_BUILDER_H_
#define _INTERFACE_AOS_BUILDER_H_

#include "IMSTypeDef.h"

#include "provider/AosStaticProfile.h"

class AString;

class IAosAppContext;
class IAosHandle;
class IAosApplication;
class IAosConnection;
class IAosRegistration;
class IAosNetTracker;
class IAosBlock;
class IAosSubscriber;
class IAosPcscf;
class IAosCallTracker;
class IAosRegStateManager;
class IAosMsgHandler;
class IAosService;
class IAosSubscriberManager;
class IAosNConfiguration;

class IAosBuilder
{
public:
    virtual ~IAosBuilder() {};

    /// AosAppContext
    virtual IAosAppContext* BuildAppContext(IN AosStaticProfile* pProflie) = 0;
    virtual IAosApplication* BuildApp(IN IAosAppContext* piAppContext) = 0;
    virtual IAosHandle* BuildHandle(IN IAosAppContext* piAppContext, IN const AString& strAppId,
            IN const AString& strSrvId) = 0;
    virtual IAosRegistration* BuildRegistration(IN IAosAppContext* piAppContext) = 0;
    virtual IAosSubscriber* BuildSubscriber(IN IAosAppContext* piAppContext) = 0;
    virtual IAosPcscf* BuildPcscf(IN IAosAppContext* piAppContext) = 0;
    virtual IAosBlock* BuildBlock(IN IAosAppContext* piAppContext) = 0;
    virtual IAosConnection* BuildConnection(IN IAosAppContext* piAppContext) = 0;
    virtual IAosNetTracker* BuildNetTracker(IN IAosAppContext* piAppContext) = 0;

    /// AoSProvider
    virtual IAosCallTracker* BuildCallTracker(IN IMS_SINT32 nSlotId) = 0;
    virtual IAosRegStateManager* BuildRegStateManager() = 0;
    virtual IAosMsgHandler* BuildMsgHandler() = 0;
    virtual IAosService* BuildService(IN IMS_SINT32 nSlotId) = 0;
    virtual IAosSubscriberManager* BuildSubscriberManager(IN IMS_SINT32 nSlotId) = 0;
    virtual IAosNConfiguration* BuildNConfiguration() = 0;
};
#endif // _INTERFACE_AOS_BUILDER_H_
