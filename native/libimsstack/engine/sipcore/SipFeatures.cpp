/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20160523  hwangoo.park@             Created
    </table>

    Description

*/

#include "SipConfigProxy.h"
#include "SipFeatures.h"

PUBLIC GLOBAL
IMS_BOOL SipFeatures::IsEventHeaderApplicableForRefer(IN IMS_SINT32/* nSlotId*/)
{
    return IMS_TRUE;
}

PUBLIC GLOBAL
IMS_BOOL SipFeatures::IsHeaderSessionIdRequired(IN IMS_SINT32 nSlotId)
{
    return SipConfigProxy::IsSessionIdHeaderSupported(nSlotId);
}

PUBLIC GLOBAL
IMS_BOOL SipFeatures::IsHostPartValidationRequiredForIncomingRequestRouting(
        IN IMS_SINT32/* nSlotId*/)
{
    return IMS_FALSE;
}

PUBLIC GLOBAL
IMS_BOOL SipFeatures::IsMultipleDialogUsagesRequiredForNonSharedDialog(IN IMS_SINT32/* nSlotId*/)
{
    return IMS_FALSE;
}

PUBLIC GLOBAL
IMS_BOOL SipFeatures::IsPaniHeaderForAckRequired(IN IMS_SINT32/* nSlotId*/)
{
    return IMS_TRUE;
}

PUBLIC GLOBAL
IMS_BOOL SipFeatures::IsSocketOptionRequiredForTcpMaxSeg(IN IMS_SINT32/* nSlotId*/)
{
    return IMS_TRUE;
}

PUBLIC GLOBAL
IMS_BOOL SipFeatures::IsReferSubHeaderSupported(IN IMS_SINT32/* nSlotId*/)
{
    return IMS_TRUE;
}

PUBLIC GLOBAL
IMS_BOOL SipFeatures::IsStandard2XXRetransmissionIntervalRequired(IN IMS_SINT32/* nSlotId*/)
{
    return IMS_TRUE;
}

PUBLIC GLOBAL
IMS_BOOL SipFeatures::IsTransportParameterIgnoredForIncomingRequestRouting(
        IN IMS_SINT32/* nSlotId*/)
{
    return IMS_FALSE;
}

PUBLIC GLOBAL
IMS_BOOL SipFeatures::IsTransportParameterUdpIgnoredForOutgoingRequest(IN IMS_SINT32/* nSlotId*/)
{
    return IMS_FALSE;
}

PUBLIC GLOBAL
IMS_BOOL SipFeatures::IsTransportParameterIgnoredForRegBinding(IN IMS_SINT32/* nSlotId*/)
{
    return IMS_FALSE;
}
