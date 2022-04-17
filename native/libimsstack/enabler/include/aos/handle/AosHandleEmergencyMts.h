#ifndef _AOS_HANDLE_EMERGENCY_MTS_H_
#define _AOS_HANDLE_EMERGENCY_MTS_H_

#include "handle/AosHandle.h"

class AosHandleEmergencyMts
    : public AosHandle
{
// Operation
public:
    AosHandleEmergencyMts
    (
        IN IAosAppContext* piAppContext,
        IN CONST AString& strAppId,
        IN CONST AString& strServiceId,
        IN CONST IMS_SINT32 nServiceType
    );
    virtual ~AosHandleEmergencyMts();

private:
};
#endif // _AOS_HANDLE_EMERGENCY_MTS_H_
