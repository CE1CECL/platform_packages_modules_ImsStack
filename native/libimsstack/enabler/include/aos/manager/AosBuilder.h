#ifndef _AOS_BUILDER_H_
#define _AOS_BUILDER_H_

#include "interface/IAosBuilder.h"

class AosBuilder
    : public IAosBuilder
{
// Operation
public:
    AosBuilder();
    virtual ~AosBuilder();

     /// AosAppContext
    virtual IAosAppContext* BuildAppContext(IN AosStaticProfile* pProflie);
    virtual IAosApplication* BuildApp(IN IAosAppContext* piAppContext);
    virtual IAosHandle* BuildHandle(IN IAosAppContext* piAppContext, IN const AString& strAppId,
            IN const AString& strSrvId);
    virtual IAosRegistration* BuildRegistration(IN IAosAppContext* piAppContext);
    virtual IAosSubscriber* BuildSubscriber(IN IAosAppContext* piAppContext);
    virtual IAosPcscf* BuildPcscf(IN IAosAppContext* piAppContext);
    virtual IAosBlock* BuildBlock(IN IAosAppContext* piAppContext);
    virtual IAosConnection* BuildConnection(IN IAosAppContext* piAppContext);
    virtual IAosNetTracker* BuildNetTracker(IN IAosAppContext* piAppContext);

    /// AoSProvider
    virtual IAosCallTracker* BuildCallTracker(IN IMS_SINT32 nSlotId);
    virtual IAosRegStateManager* BuildRegStateManager();
    virtual IAosMsgHandler* BuildMsgHandler();
    virtual IAosService* BuildService(IN IMS_SINT32 nSlotId);
    virtual IAosSubscriberManager* BuildSubscriberManager(IN IMS_SINT32 nSlotId);
    virtual IAosNConfiguration* BuildNConfiguration();
};
#endif // _AOS_BUILDER_H_
