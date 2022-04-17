#ifndef _AOS_HANDLE_EMERGENCY_MTC_H_
#define _AOS_HANDLE_EMERGENCY_MTC_H_

#include "handle/AosHandle.h"
#include "IEventListener.h"

class AosHandleEmergencyMtc
    : public AosHandle
{
public:
    AosHandleEmergencyMtc
    (
        IN IAosAppContext* piAppContext,
        IN CONST AString& strAppId,
        IN CONST AString& strServiceId,
        IN CONST IMS_SINT32 nServiceType
    );
    virtual ~AosHandleEmergencyMtc();

protected:
    virtual void Init();
};
#endif // _AOS_HANDLE_EMERGENCY_MTC_H_
