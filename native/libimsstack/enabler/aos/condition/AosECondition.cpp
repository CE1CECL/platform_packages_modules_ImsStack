#include "ServiceTrace.h"
#include "interface/IAosAppContext.h"
#include "interface/IAosConditionListener.h"
#include "interface/IAosNetTracker.h"
#include "interface/IAosService.h"
#include "interface/IAosSubscriber.h"
#include "provider/AosProvider.h"
#include "condition/AosBlock.h"
#include "condition/AosECondition.h"

__IMS_TRACE_TAG_USER_DECL__("AOS");

#define APPPROFILE m_strTag.GetStr()

PUBLIC
AosECondition::AosECondition(IN IAosAppContext* piAppContext)
    : AosCondition(piAppContext)
{
    IMS_TRACE_MEM("AOS_MEM", "AOS_M : [%s] AosECondition = %" PFLS_u "/%" PFLS_x, APPPROFILE,
            sizeof(AosECondition), this);
    RemoveListener(LISTENER_CALLTRACKER);
}

PUBLIC VIRTUAL
AosECondition::~AosECondition()
{
    IMS_TRACE_MEM("AOS_MEM", "AOS_F : [%s] AosECondition = %" PFLS_u "/%" PFLS_x, APPPROFILE,
            sizeof(AosECondition), this);
}

PUBLIC VIRTUAL
IMS_BOOL AosECondition::IsReady()
{
    IMS_BOOL bReturn = m_piAppContext->GetBlock()->IsCleared();
    A_IMS_TRACE_I(APPPROFILE, "IsReady(%s)", _TRACE_B_(bReturn), 0, 0);

    return bReturn;
}

PRIVATE VIRTUAL
void AosECondition::AddAosServiceListener()
{
    AosProvider::GetInstance()->GetService(m_nSlotId)->AddListener(
            DYNAMIC_CAST(IAosServicePhoneListener*, this));
}

PRIVATE VIRTUAL
void AosECondition::RemoveAosServiceListener()
{
    AosProvider::GetInstance()->GetService(m_nSlotId)->RemoveListener(
            DYNAMIC_CAST(IAosServicePhoneListener*, this));
}

// IAosBlockListener
PRIVATE VIRTUAL
void AosECondition::Block_Changed(IN IMS_UINT32 nType, IN IMS_UINT32 nParam)
{
    A_IMS_TRACE_I(APPPROFILE, "Block_Changed :: Reason(%s)(%d) - %s",
            AosBlock::BlockReasonToString(nType), nType, (nParam > 0) ? "BLOCK" : "NOT_BLOCK");

    PrintBlockReasons();

    m_piListener->Condition_Changed();
}

// AosServicePhoneListener
PRIVATE VIRTUAL
void AosECondition::ServicePhone_AosStart()
{
    A_IMS_TRACE_D(APPPROFILE, "ServicePhone_AosStart()", 0, 0, 0);
    m_piAppContext->GetBlock()->ResetBlockReason(BLOCK_AOS_INCOMPLETED);
}