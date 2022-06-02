/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20170621  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _SIP_FACTORY_PROXY_H_
#define _SIP_FACTORY_PROXY_H_

#include "IMSTypeDef.h"

class ISipTokenGenerator;
class SipIpSecState;
class SipMessageTracker;
class SipPacketTracker;
class SipRoutingRejectNotifier;
class SipRtConfigHelper;
class SipTransportHelper;
class SIPFactoryProxyPrivate;

class SipFactoryProxy
{
private:
    SipFactoryProxy();
    ~SipFactoryProxy();

    SipFactoryProxy(IN const SipFactoryProxy& objRHS);
    SipFactoryProxy& operator=(IN const SipFactoryProxy& objRHS);

public:
    SipIpSecState* GetIpSecState(IN IMS_SINT32 nSlotId);
    SipMessageTracker* GetMessageTracker(IN IMS_SINT32 nSlotId);
    SipPacketTracker* GetPacketTracker(IN IMS_SINT32 nSlotId);
    SipRoutingRejectNotifier* GetRoutingRejectNotifier(IN IMS_SINT32 nSlotId);
    SipRtConfigHelper* GetRtConfigHelper(IN IMS_SINT32 nSlotId);
    SipTransportHelper* GetTransportHelper(IN IMS_SINT32 nSlotId);
    void SetTokenGenerator(IN IMS_SINT32 nSlotId, IN ISipTokenGenerator* piTokenGenerator);

    IMS_BOOL IsIpSecStateEnabled(IN IMS_SINT32 nSlotId) const;
    IMS_BOOL IsMessageTrackerEnabled(IN IMS_SINT32 nSlotId) const;
    IMS_BOOL IsPacketTrackerEnabled(IN IMS_SINT32 nSlotId) const;
    IMS_BOOL IsRoutingRejectNotifierEnabled(IN IMS_SINT32 nSlotId) const;

public:
    static void CreateInstance();
    static void DestroyInstance();
    static SipFactoryProxy* GetInstance();

private:
    static SipFactoryProxy* pFactoryProxy;

    SIPFactoryProxyPrivate* pPrivate;
};

#endif  // _SIP_FACTORY_PROXY_H_
