/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20110528  hwangoo.park@             Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "SipFeatures.h"
#include "SIPFactoryProxy.h"
#include "SIPIPSecState.h"
#include "SIPKeepAliveHelper.h"
#include "SIPMessageTracker.h"
#include "SIPPacketTracker.h"
#include "SipRoutingRejectNotifier.h"
#include "SIPRTConfigHelper.h"
#include "SIPTransportHelper.h"
#include "SIPUtil.h"
#include "SipFactory.h"



/*

Remarks

*/
PUBLIC GLOBAL
ISipKeepAliveHelper* SipFactory::CreateKeepAliveHelper(IN IMS_SINT32 nSlotId/* = IMS_SLOT_0*/)
{
    return new SIPKeepAliveHelper(nSlotId);
}

/*

Remarks

*/
PUBLIC GLOBAL
void SipFactory::GenerateCallId(IN const AString &strHost, OUT AString &strCallId)
{
    strCallId = SIPUtil::GenerateCallId(strHost);
}

/*

Remarks
 HEADER_REQ_SESSION-ID
*/
PUBLIC GLOBAL
void SipFactory::GenerateSessionId(IN IMS_SINT32 nSlotId,
        IN const AString &strCallId, OUT AString &strSessionId)
{
    if (SipFeatures::IsHeaderSessionIdRequired(nSlotId))
    {
        strSessionId = SIPUtil::GenerateSessionId(nSlotId, strCallId);
        return;
    }

    strSessionId = AString::ConstNull();
}

/*

Remarks

*/
PUBLIC GLOBAL
ISipIpSecState* SipFactory::GetIpSecState(IN IMS_SINT32 nSlotId/* = IMS_SLOT_0*/)
{
    return SIPFactoryProxy::GetInstance()->GetIPSecState(nSlotId);
}

/*

Remarks

*/
PUBLIC GLOBAL
ISipMessageTracker* SipFactory::GetMessageTracker(IN IMS_SINT32 nSlotId/* = IMS_SLOT_0*/)
{
    return SIPFactoryProxy::GetInstance()->GetMessageTracker(nSlotId);
}

/*

Remarks

*/
PUBLIC GLOBAL
ISipRoutingRejectNotifier* SipFactory::GetRoutingRejectNotifier(
        IN IMS_SINT32 nSlotId/* = IMS_SLOT_0*/)
{
    return SIPFactoryProxy::GetInstance()->GetRoutingRejectNotifier(nSlotId);
}

/*

Remarks

*/
PUBLIC GLOBAL
ISipRtConfigHelper* SipFactory::GetRtConfigHelper(IN IMS_SINT32 nSlotId/* = IMS_SLOT_0*/)
{
    return SIPFactoryProxy::GetInstance()->GetRtConfigHelper(nSlotId);
}

/*

Remarks

*/
PUBLIC GLOBAL
ISipTransportHelper* SipFactory::GetTransportHelper(IN IMS_SINT32 nSlotId/* = IMS_SLOT_0*/)
{
    return SIPFactoryProxy::GetInstance()->GetTransportHelper(nSlotId);
}

/*

Remarks

*/
PUBLIC GLOBAL
ISipPacketTracker* SipFactory::GetPacketTracker(IN IMS_SINT32 nSlotId/* = IMS_SLOT_0*/)
{
    return SIPFactoryProxy::GetInstance()->GetPacketTracker(nSlotId);
}

/*

Remarks

*/
PUBLIC GLOBAL
void SipFactory::SetTokenGenerator(IN IMS_SINT32 nSlotId,
        IN ISipTokenGenerator *piTokenGenerator)
{
    SIPFactoryProxy::GetInstance()->SetTokenGenerator(nSlotId, piTokenGenerator);
}
