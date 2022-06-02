/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20170621  hwangoo.park@             Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "SystemConfig.h"
#include "SipIpSecState.h"
#include "SipKeepAliveHelper.h"
#include "SipMessageTracker.h"
#include "SipPacketTracker.h"
#include "SipRoutingRejectNotifier.h"
#include "SipRtConfigHelper.h"
#include "SipTransportHelper.h"
#include "SipFactoryProxy.h"

PRIVATE GLOBAL SipFactoryProxy* SipFactoryProxy::pFactoryProxy = IMS_NULL;

class SIPFactoryHolder
{
public:
    SIPFactoryHolder();
    ~SIPFactoryHolder();

private:
    SIPFactoryHolder(IN const SIPFactoryHolder& objRHS);
    SIPFactoryHolder& operator=(IN const SIPFactoryHolder& objRHS);

public:
    inline SipIpSecState* GetIpSecState()
    {
        if (pIPSecState == IMS_NULL)
        {
            pIPSecState = new SipIpSecState();
        }

        return pIPSecState;
    }

    inline SipMessageTracker* GetMessageTracker()
    {
        if (pMessageTracker == IMS_NULL)
        {
            pMessageTracker = new SipMessageTracker();
        }

        return pMessageTracker;
    }

    inline SipPacketTracker* GetPacketTracker()
    {
        if (pPacketTracker == IMS_NULL)
        {
            pPacketTracker = new SipPacketTracker();
        }

        return pPacketTracker;
    }

    inline SipRoutingRejectNotifier* GetRoutingRejectNotifier()
    {
        if (pRoutingRejectNotifier == IMS_NULL)
        {
            pRoutingRejectNotifier = new SipRoutingRejectNotifier();
        }

        return pRoutingRejectNotifier;
    }

    inline SipRtConfigHelper* GetRtConfigHelper()
    {
        if (pRTConfigHelper == IMS_NULL)
        {
            pRTConfigHelper = new SipRtConfigHelper();
        }

        return pRTConfigHelper;
    }

    inline SipTransportHelper* GetTransportHelper()
    {
        if (pTransportHelper == IMS_NULL)
        {
            pTransportHelper = new SipTransportHelper();
        }

        return pTransportHelper;
    }

    inline void SetTokenGenerator(IN ISipTokenGenerator* piTokenGenerator)
    {
        this->piTokenGenerator = piTokenGenerator;
    }

    inline IMS_BOOL IsIpSecStateEnabled() const
    {
        return (pIPSecState != IMS_NULL) && pIPSecState->IsIpSecEnabled();
    }
    inline IMS_BOOL IsMessageTrackerEnabled() const
    {
        return (pMessageTracker != IMS_NULL) && pMessageTracker->IsMessageTrackerEnabled();
    }
    inline IMS_BOOL IsPacketTrackerEnabled() const
    {
        return (pPacketTracker != IMS_NULL) && pPacketTracker->IsPacketTrackerEnabled();
    }
    inline IMS_BOOL IsRoutingRejectNotifierEnabled() const
    {
        return (pRoutingRejectNotifier != IMS_NULL) &&
                pRoutingRejectNotifier->IsNotificationRequired();
    }

private:
    void Clear();

private:
    SipIpSecState* pIPSecState;
    SipMessageTracker* pMessageTracker;
    SipPacketTracker* pPacketTracker;
    SipRoutingRejectNotifier* pRoutingRejectNotifier;
    SipRtConfigHelper* pRTConfigHelper;
    SipTransportHelper* pTransportHelper;
    ISipTokenGenerator* piTokenGenerator;
};

PUBLIC
SIPFactoryHolder::SIPFactoryHolder() :
        pIPSecState(IMS_NULL),
        pMessageTracker(IMS_NULL),
        pPacketTracker(IMS_NULL),
        pRoutingRejectNotifier(IMS_NULL),
        pRTConfigHelper(IMS_NULL),
        pTransportHelper(IMS_NULL),
        piTokenGenerator(IMS_NULL)
{
}

PUBLIC
SIPFactoryHolder::~SIPFactoryHolder()
{
    Clear();
}

PRIVATE
void SIPFactoryHolder::Clear()
{
    if (pIPSecState != IMS_NULL)
    {
        delete pIPSecState;
        pIPSecState = IMS_NULL;
    }

    if (pMessageTracker != IMS_NULL)
    {
        delete pMessageTracker;
        pMessageTracker = IMS_NULL;
    }

    if (pPacketTracker != IMS_NULL)
    {
        delete pPacketTracker;
        pPacketTracker = IMS_NULL;
    }

    if (pRoutingRejectNotifier != IMS_NULL)
    {
        delete pRoutingRejectNotifier;
        pRoutingRejectNotifier = IMS_NULL;
    }

    if (pTransportHelper != IMS_NULL)
    {
        delete pTransportHelper;
        pTransportHelper = IMS_NULL;
    }

    if (pRTConfigHelper != IMS_NULL)
    {
        delete pRTConfigHelper;
        pRTConfigHelper = IMS_NULL;
    }

    piTokenGenerator = IMS_NULL;
}

class SIPFactoryProxyPrivate
{
public:
    SIPFactoryProxyPrivate();
    ~SIPFactoryProxyPrivate();

private:
    SIPFactoryProxyPrivate(IN const SIPFactoryProxyPrivate& objRHS);
    SIPFactoryProxyPrivate& operator=(IN const SIPFactoryProxyPrivate& objRHS);

public:
    inline SIPFactoryHolder* GetHolder(IN IMS_SINT32 nSlotId) const
    {
        if ((nSlotId < IMS_SLOT_0) || (nSlotId >= SystemConfig::GetMaxSimSlot()))
        {
            nSlotId = IMS_SLOT_0;
        }

        return ppHolder[nSlotId];
    }

private:
    SIPFactoryHolder** ppHolder;
};

PUBLIC
SIPFactoryProxyPrivate::SIPFactoryProxyPrivate() :
        ppHolder(IMS_NULL)
{
    IMS_SINT32 nSimCount = SystemConfig::GetMaxSimSlot();

    ppHolder = new SIPFactoryHolder*[nSimCount];

    for (IMS_SINT32 i = 0; i < nSimCount; ++i)
    {
        ppHolder[i] = new SIPFactoryHolder();
    }
}

PUBLIC
SIPFactoryProxyPrivate::~SIPFactoryProxyPrivate()
{
    if (ppHolder != IMS_NULL)
    {
        IMS_SINT32 nSimCount = SystemConfig::GetMaxSimSlot();

        for (IMS_SINT32 i = 0; i < nSimCount; ++i)
        {
            if (ppHolder[i] != IMS_NULL)
            {
                delete ppHolder[i];
            }
        }

        delete[] ppHolder;
    }
}

PRIVATE
SipFactoryProxy::SipFactoryProxy() :
        pPrivate(new SIPFactoryProxyPrivate())
{
}

PRIVATE
SipFactoryProxy::~SipFactoryProxy()
{
    if (pPrivate != IMS_NULL)
    {
        delete pPrivate;
    }
}

/*

Remarks

*/
PUBLIC
SipIpSecState* SipFactoryProxy::GetIpSecState(IN IMS_SINT32 nSlotId)
{
    SIPFactoryHolder* pHolder = pPrivate->GetHolder(nSlotId);
    return pHolder->GetIpSecState();
}

/*

Remarks

*/
PUBLIC
SipMessageTracker* SipFactoryProxy::GetMessageTracker(IN IMS_SINT32 nSlotId)
{
    SIPFactoryHolder* pHolder = pPrivate->GetHolder(nSlotId);
    return pHolder->GetMessageTracker();
}

/*

Remarks

*/
PUBLIC
SipPacketTracker* SipFactoryProxy::GetPacketTracker(IN IMS_SINT32 nSlotId)
{
    SIPFactoryHolder* pHolder = pPrivate->GetHolder(nSlotId);
    return pHolder->GetPacketTracker();
}

/*

Remarks

*/
PUBLIC
SipRoutingRejectNotifier* SipFactoryProxy::GetRoutingRejectNotifier(IN IMS_SINT32 nSlotId)
{
    SIPFactoryHolder* pHolder = pPrivate->GetHolder(nSlotId);
    return pHolder->GetRoutingRejectNotifier();
}

/*

Remarks

*/
PUBLIC
SipRtConfigHelper* SipFactoryProxy::GetRtConfigHelper(IN IMS_SINT32 nSlotId)
{
    SIPFactoryHolder* pHolder = pPrivate->GetHolder(nSlotId);
    return pHolder->GetRtConfigHelper();
}

/*

Remarks

*/
PUBLIC
SipTransportHelper* SipFactoryProxy::GetTransportHelper(IN IMS_SINT32 nSlotId)
{
    SIPFactoryHolder* pHolder = pPrivate->GetHolder(nSlotId);
    return pHolder->GetTransportHelper();
}

/*

Remarks

*/
PUBLIC
void SipFactoryProxy::SetTokenGenerator(
        IN IMS_SINT32 nSlotId, IN ISipTokenGenerator* piTokenGenerator)
{
    SIPFactoryHolder* pHolder = pPrivate->GetHolder(nSlotId);
    pHolder->SetTokenGenerator(piTokenGenerator);
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipFactoryProxy::IsIpSecStateEnabled(IN IMS_SINT32 nSlotId) const
{
    SIPFactoryHolder* pHolder = pPrivate->GetHolder(nSlotId);
    return pHolder->IsIpSecStateEnabled();
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipFactoryProxy::IsMessageTrackerEnabled(IN IMS_SINT32 nSlotId) const
{
    SIPFactoryHolder* pHolder = pPrivate->GetHolder(nSlotId);
    return pHolder->IsMessageTrackerEnabled();
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipFactoryProxy::IsPacketTrackerEnabled(IN IMS_SINT32 nSlotId) const
{
    SIPFactoryHolder* pHolder = pPrivate->GetHolder(nSlotId);
    return pHolder->IsPacketTrackerEnabled();
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipFactoryProxy::IsRoutingRejectNotifierEnabled(IN IMS_SINT32 nSlotId) const
{
    SIPFactoryHolder* pHolder = pPrivate->GetHolder(nSlotId);
    return pHolder->IsRoutingRejectNotifierEnabled();
}

PUBLIC GLOBAL void SipFactoryProxy::CreateInstance()
{
    if (pFactoryProxy == IMS_NULL)
    {
        pFactoryProxy = new SipFactoryProxy();
    }
}

PUBLIC GLOBAL void SipFactoryProxy::DestroyInstance()
{
    if (pFactoryProxy != IMS_NULL)
    {
        delete pFactoryProxy;
        pFactoryProxy = IMS_NULL;
    }
}

PUBLIC GLOBAL SipFactoryProxy* SipFactoryProxy::GetInstance()
{
    if (pFactoryProxy == IMS_NULL)
    {
        CreateInstance();
    }

    return pFactoryProxy;
}
