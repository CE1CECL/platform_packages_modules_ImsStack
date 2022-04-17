/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20181204  dongo.yi@                 Created
    </table>

    Description

*/

// TODO: this class will be deprecated.

#include "ServiceMemory.h"
#include "ServiceThread.h"
#include "ServiceTrace.h"

#include "configuration/ConfigDef.h"
#include "configuration/MtcConfigurationProxy.h"

#include "conferencecall/ConferenceConfigurationWrapper.h"
#include "MtcDef.h"
#include "MtcContextRepository.h"
#include "IMtcContext.h"


__IMS_TRACE_TAG_COM_MTC__;

#ifdef _PUBLIC_METHOD_
#endif

PUBLIC GLOBAL
IMS_BOOL ConferenceConfiguration::IsSupported()
{
    return IsConferenceFeatureFlagged(FLAG_CONFERENCE);
}

PUBLIC GLOBAL
IMS_BOOL ConferenceConfiguration::IsConferenceSubscriptionRequired()
{
    return IsConferenceFeatureFlagged(FLAG_CONFERENCE_SUBSCRIPTION);
}

PUBLIC GLOBAL
IMS_BOOL ConferenceConfiguration::IsReferSubscriptionRequired()
{
    return IsConferenceFeatureFlagged(FLAG_REFER_SUBSCRIPTION);
}

PUBLIC GLOBAL
IMS_BOOL ConferenceConfiguration::IsSubscriptionOutDialog()
{
    return IsConferenceFeatureFlagged(FLAG_SUBSCRIPTION_OUTDIALOG);
}

PUBLIC GLOBAL
IMS_BOOL ConferenceConfiguration::IsReferredByRequired()
{
    return IsConferenceFeatureFlagged(FLAG_REFERRED_BY);
}

PUBLIC GLOBAL
IMS_BOOL ConferenceConfiguration::IsAddUserParameter()
{
    return IsConferenceFeatureFlagged(FLAG_ADD_USER_PARAMETER);
}

PUBLIC GLOBAL
IMS_BOOL ConferenceConfiguration::IsPackageVersionCheckRequired()
{
    return IsConferenceFeatureFlagged(FLAG_PACKAGE_VERSION_CHECK);
}

PUBLIC GLOBAL
IMS_BOOL ConferenceConfiguration::IsImplicitRoutingRequired()
{
    return IsConferenceFeatureFlagged(FLAG_IMPLICIT_ROUTING);
}

PUBLIC GLOBAL
IMS_BOOL ConferenceConfiguration::IsSubscriptionFirst()
{
    return IsConferenceFeatureFlagged(FLAG_SUBSCRIPTION_FIRST);
}

PUBLIC GLOBAL
IMS_BOOL ConferenceConfiguration::IsPaidPreferred()
{
    return IsConferenceFeatureFlagged(FLAG_PAID_PREFERRED);
}

PUBLIC GLOBAL
IMS_BOOL ConferenceConfiguration::IsReUseReferToUri()
{
    return IsConferenceFeatureFlagged(FLAG_REUSE_REFER_TO_URI_IN_INVITE);
}

PUBLIC GLOBAL
IMS_BOOL ConferenceConfiguration::IsReferUsed()
{
    return IsConferenceFeatureFlagged(FLAG_USE_REFER);
}

PUBLIC GLOBAL
IMS_BOOL ConferenceConfiguration::IsDisconnectingStatusUsed()
{
    return IsConferenceFeatureFlagged(FLAG_USE_DISCONNECTING_STATUS);
}

PUBLIC GLOBAL
IMS_BOOL ConferenceConfiguration::IsReferToExHeaderUsed()
{
    return IsConferenceFeatureFlagged(FLAG_USE_REFER_TO_EX_HDR);
}

PUBLIC GLOBAL
IMS_BOOL ConferenceConfiguration::IsSubscriptionForParticipantRequired()
{
    return IsConferenceFeatureFlagged(FLAG_SUBSCRIPTION_FOR_PARTICIPANT);
}

PUBLIC GLOBAL
IMS_SINT32 ConferenceConfiguration::GetWaitTimeInitiation()
{
    return 2000;
}

PUBLIC GLOBAL
IMS_SINT32 ConferenceConfiguration::GetWaitTimeNotifyActive()
{
    return 2000;
}

PUBLIC GLOBAL
IMS_SINT32 ConferenceConfiguration::GetWaitTimeNotifyTerminated()
{
    // if this value is less than 0, no Un-Subscription
    return 3000;
}

PUBLIC GLOBAL
IMS_SINT32 ConferenceConfiguration::GetWaitTimeSipFrag()
{
    return 2000;
}

PUBLIC GLOBAL
IMS_SINT32 ConferenceConfiguration::GetReferTypeForInvite()
{
    return MtcContextRepository::GetContext(ThreadService::GetCurrentSlotId())
            ->GetConfigurationProxy().GetInt(Feature::CONFERENCE_INVITING_REFER_TYPE);
}

#ifdef _PRIVATE_METHOD_
#endif

PRIVATE GLOBAL
IMS_BOOL ConferenceConfiguration::IsConferenceFeatureFlagged(IN IMS_UINT32 nFeature)
{
    IMS_SINT32 nSlotId = ThreadService::GetCurrentSlotId();
    // TODO, MTC BUILD
    UNUSED_PARAM(nSlotId);
    const AString& strFeatures = "0xffffffff";
    IMS_UINT32 nConferenceFeatures = strFeatures.ToUInt32(IMS_NULL, 16);

    return ((nConferenceFeatures&  nFeature) != 0);
}
