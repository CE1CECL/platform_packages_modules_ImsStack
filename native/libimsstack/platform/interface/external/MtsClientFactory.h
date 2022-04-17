#ifndef IMS_MTSClIENT_FACTORY_H_
#define IMS_MTSClIENT_FACTORY_H_

#include "IMSTypeDef.h"

class IMtsClient;
class IMtsSCBM;

class MtsClientFactory
{
public:
    MtsClientFactory();
    virtual ~MtsClientFactory();
    static IMtsClient* GetIMtsJavaClient(IN IMS_SINT32 nSlotID);
    static IMtsSCBM* GetIMtsSCBMClient(IN IMS_SINT32 nSlotID);
    static void DestroyIMtsSCBMClient(IN IMS_SINT32 nSlotID);
    static void DestroyIMtsJavaClient(IN IMS_SINT32 nSlotID);
};

#endif
