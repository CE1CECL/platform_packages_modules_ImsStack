#include "ServiceTrace.h"

#include "interface/IAosAppContext.h"
#include "interface/IAosHandle.h"
#include "handle/AosHandleUce.h"

//__IMS_TRACE_TAG_USER_DECL__("AOS");

/*

Remarks

*/
PUBLIC
AosHandleUce::AosHandleUce
    (
        IN IAosAppContext* piAppContext,
        IN CONST AString& strAppId,
        IN CONST AString& strServiceId,
        IN CONST IMS_SINT32 nServiceType
    )
    : AosHandle(piAppContext, strAppId, strServiceId, nServiceType)
{
    IMS_TRACE_MEM("AOS_MEM", "AOS_M : [%s] AosHandleUce = %" PFLS_u "/%" PFLS_x,
            strAppId.GetStr(), sizeof(AosHandleUce), this);

    /* @jryou: temp blocked. Defined in VZWAoSHandle.
    AddConfig(CONFIG_LIMITED_MODE | CONFIG_SRLTE_MODE);
    */
}

/*

Remarks

*/
PUBLIC VIRTUAL
AosHandleUce::~AosHandleUce()
{
    IMS_TRACE_MEM("AOS_MEM", "AOS_F : [%s] AosHandleUce = %" PFLS_u "/%" PFLS_x,
            m_strAppId.GetStr(), sizeof(AosHandleUce), this);
}