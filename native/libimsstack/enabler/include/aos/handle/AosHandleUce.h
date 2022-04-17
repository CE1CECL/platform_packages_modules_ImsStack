#ifndef _AOS_HANDLE_UCE_H_
#define _AOS_HANDLE_UCE_H_

#include "handle/AosHandle.h"

class AosHandleUce
    : public AosHandle
{
// Operation
public:
    AosHandleUce
    (
        IN IAosAppContext* piAppContext,
        IN CONST AString& strAppId,
        IN CONST AString& strServiceId,
        IN CONST IMS_SINT32 nServiceType
    );
    virtual ~AosHandleUce();

private:
};
#endif // _AOS_HANDLE_UCE_H_
