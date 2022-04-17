#include "ServiceTrace.h"

#include "interface/IAosAppContext.h"
#include "handle/AosHandleEmergencyMtc.h"

__IMS_TRACE_TAG_USER_DECL__("AOS");

#define APPPROFILE m_strTag.GetStr()

/*

Remarks

*/
PUBLIC
AosHandleEmergencyMtc::AosHandleEmergencyMtc
    (
        IN IAosAppContext* piAosAppContext,
        IN CONST AString& strAppId,
        IN CONST AString& strServiceId,
        IN CONST IMS_SINT32 nServiceType
    )
    : AosHandle(piAosAppContext, strAppId, strServiceId, nServiceType)
{
    IMS_TRACE_MEM("AOS_MEM", "AOS_M : [%s] AosHandleEmergencyMtc = %" PFLS_u "/%" PFLS_x,
            strAppId.GetStr(), sizeof(AosHandleEmergencyMtc), this);
}

/*

Remarks

*/
PUBLIC VIRTUAL
AosHandleEmergencyMtc::~AosHandleEmergencyMtc()
{
    IMS_TRACE_MEM("AOS_MEM", "AOS_F : [%s] AosHandleEmergencyMtc = %" PFLS_u "/%" PFLS_x,
            m_strAppId.GetStr(), sizeof(AosHandleEmergencyMtc), this);
}

/*

Remarks

*/
PROTECTED VIRTUAL
void AosHandleEmergencyMtc::Init()
{
    A_IMS_TRACE_D(APPPROFILE, "Init", 0, 0, 0);
}