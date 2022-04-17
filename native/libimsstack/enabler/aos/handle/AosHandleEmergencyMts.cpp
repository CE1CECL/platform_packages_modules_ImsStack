#include "ServiceTrace.h"

#include "interface/IAosAppContext.h"

#include "handle/AosHandleEmergencyMts.h"

//__IMS_TRACE_TAG_USER_DECL__("AOS");

/*

Remarks

*/
PUBLIC
AosHandleEmergencyMts::AosHandleEmergencyMts
    (
        IN IAosAppContext* piAppContext,
        IN CONST AString& strAppId,
        IN CONST AString& strServiceId,
        IN CONST IMS_SINT32 nServiceType
    )
    : AosHandle(piAppContext, strAppId, strServiceId, nServiceType)
{
    IMS_TRACE_MEM("AOS_MEM", "AOS_M : [%s] AosHandleEmergencyMts = %" PFLS_u "/%" PFLS_x,
            strAppId.GetStr(), sizeof(AosHandleEmergencyMts), this);
}

/*

Remarks

*/
PUBLIC VIRTUAL
AosHandleEmergencyMts::~AosHandleEmergencyMts()
{
    IMS_TRACE_MEM("AOS_MEM", "AOS_F : [%s] AosHandleEmergencyMts = %" PFLS_u "/%" PFLS_x,
            m_strAppId.GetStr(), sizeof(AosHandleEmergencyMts), this);
}