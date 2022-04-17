/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20091107  toastops@                 Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "SystemConfig.h"
#include "ServiceNetworkPolicy.h"

class NetworkPolicyHolder
{
public:
    NetworkPolicyHolder();
    ~NetworkPolicyHolder();

public:
    void InitPolicies();
    IMS_BOOL AddPolicy(IN const NetworkPolicy &objPolicy);
    const NetworkPolicy* GetPolicy(IN const AString &strName) const;
    const NetworkPolicy* GetPolicy(IN IMS_SINT32 nAPNType) const;
    void RemovePolicy(IN const AString &strName);
    void RemoveAllPolicies();

private:
    IMSList<NetworkPolicy*> objPolicys;
};

PUBLIC
NetworkPolicyHolder::NetworkPolicyHolder()
    : objPolicys(IMSList<NetworkPolicy*>())
{
    InitPolicies();
}

PUBLIC
NetworkPolicyHolder::~NetworkPolicyHolder()
{
    while (!objPolicys.IsEmpty())
    {
        NetworkPolicy *pPolicy = objPolicys.GetAt(0);

        if (pPolicy != IMS_NULL)
        {
            delete pPolicy;
        }

        objPolicys.RemoveAt(0);
    }
}

PUBLIC
void NetworkPolicyHolder::InitPolicies()
{
    // "ims"
    NetworkPolicy* pPolicy = new NetworkPolicy(IMS_TRUE, "mobile_ims", NetworkPolicy::APN_IMS);
    objPolicys.Append(pPolicy);

    // "emergency"
    pPolicy = new NetworkPolicy(IMS_FALSE, "mobile_emergency", NetworkPolicy::APN_EMERGENCY);
    objPolicys.Append(pPolicy);

    // "internet"
    pPolicy = new NetworkPolicy(IMS_FALSE, "mobile_internet", NetworkPolicy::APN_INTERNET);
    objPolicys.Append(pPolicy);

    // "wifi"
    pPolicy = new NetworkPolicy(IMS_FALSE, "wifi", NetworkPolicy::APN_WIFI);
    objPolicys.Append(pPolicy);
}

PUBLIC
IMS_BOOL NetworkPolicyHolder::AddPolicy(IN const NetworkPolicy &objPolicy)
{
    NetworkPolicy *pNewPolicy = new NetworkPolicy(objPolicy);

    if (pNewPolicy == IMS_NULL)
    {
        return IMS_FALSE;
    }

    if (!objPolicys.Append(pNewPolicy))
    {
        delete pNewPolicy;

        return IMS_FALSE;
    }

    return IMS_TRUE;
}

PUBLIC
const NetworkPolicy* NetworkPolicyHolder::GetPolicy(IN const AString &strName) const
{
    for (IMS_UINT32 i = 0; i < objPolicys.GetSize(); ++i)
    {
        const NetworkPolicy *pPolicy = objPolicys.GetAt(i);

        if (strName.Equals(pPolicy->GetName()))
        {
            return pPolicy;
        }
    }

    return IMS_NULL;
}

PUBLIC
const NetworkPolicy* NetworkPolicyHolder::GetPolicy(IN IMS_SINT32 nAPNType) const
{
    if (nAPNType == NetworkPolicy::APN_NONE)
    {
        for (IMS_UINT32 i = 0; i < objPolicys.GetSize(); ++i)
        {
            const NetworkPolicy *pPolicy = objPolicys.GetAt(i);

            if (pPolicy->IsPrimary())
            {
                return pPolicy;
            }
        }
    }
    else
    {
        for (IMS_UINT32 i = 0; i < objPolicys.GetSize(); ++i)
        {
            const NetworkPolicy *pPolicy = objPolicys.GetAt(i);

            if (pPolicy->GetAPNType() == nAPNType)
            {
                return pPolicy;
            }
        }
    }

    return IMS_NULL;
}

PUBLIC
void NetworkPolicyHolder::RemovePolicy(IN const AString &strName)
{
    for (IMS_UINT32 i = 0; i < objPolicys.GetSize(); ++i)
    {
        NetworkPolicy *pPolicy = objPolicys.GetAt(i);

        if (strName.Equals(pPolicy->GetName()))
        {
            objPolicys.RemoveAt(i);
            delete pPolicy;
            return;
        }
    }
}

PUBLIC
void NetworkPolicyHolder::RemoveAllPolicies()
{
    for (IMS_UINT32 i = 0; i < objPolicys.GetSize(); ++i)
    {
        NetworkPolicy *pPolicy = objPolicys.GetAt(i);

        if (pPolicy != IMS_NULL)
        {
            delete pPolicy;
        }
    }

    objPolicys.Clear();
}



class NetworkServicePolicyPrivate
{
public:
    NetworkServicePolicyPrivate();
    ~NetworkServicePolicyPrivate();

private:
    NetworkServicePolicyPrivate(IN const NetworkServicePolicyPrivate& objRHS);
    NetworkServicePolicyPrivate& operator=(IN const NetworkServicePolicyPrivate& objRHS);

public:
    inline NetworkPolicyHolder* GetHolder(IN IMS_SINT32 nSlotId) const
    {
        if ((nSlotId < IMS_SLOT_0) || (nSlotId >= SystemConfig::GetMaxSimSlot()))
        {
            nSlotId = IMS_SLOT_0;
        }

        return ppHolder[nSlotId];
    }

private:
    NetworkPolicyHolder **ppHolder;
};

PUBLIC
NetworkServicePolicyPrivate::NetworkServicePolicyPrivate()
    : ppHolder(IMS_NULL)
{
    IMS_SINT32 nSimCount = SystemConfig::GetMaxSimSlot();

    ppHolder = new NetworkPolicyHolder*[nSimCount];

    for (IMS_SINT32 i = 0; i < nSimCount; ++i)
    {
        ppHolder[i] = new NetworkPolicyHolder();
    }
}

PUBLIC
NetworkServicePolicyPrivate::~NetworkServicePolicyPrivate()
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



PUBLIC
NetworkPolicy::NetworkPolicy(IN IMS_BOOL bPrimary /* = IMS_FALSE */)
        : m_bPrimary(bPrimary)
        , m_strName(AString::ConstEmpty())
        , m_nApnType(APN_NONE)
{
}

PUBLIC
NetworkPolicy::NetworkPolicy(IN IMS_BOOL bPrimary,
        IN const AString& strName, IN IMS_SINT32 nApnType)
        : m_bPrimary(bPrimary)
        , m_strName(strName)
        , m_nApnType(nApnType)
{

}

PUBLIC
NetworkPolicy::NetworkPolicy(IN const NetworkPolicy& objOther)
        : m_bPrimary(objOther.m_bPrimary)
        , m_strName(objOther.m_strName)
        , m_nApnType(objOther.m_nApnType)
{
}

PUBLIC
NetworkPolicy::~NetworkPolicy()
{
}

PUBLIC
NetworkPolicy& NetworkPolicy::operator=(IN const NetworkPolicy& objOther)
{
    if (this != &objOther)
    {
        m_bPrimary = objOther.m_bPrimary;
        m_strName = objOther.m_strName;
        m_nApnType = objOther.m_nApnType;
    }

    return (*this);
}

PUBLIC GLOBAL
IMS_BOOL NetworkPolicy::IsMobilePolicy(IN const AString& strName)
{
    return strName.StartsWith("mobile");
}

PUBLIC GLOBAL
IMS_BOOL NetworkPolicy::IsMobilePolicy(IN IMS_SINT32 nApnType)
{
    return (nApnType == APN_IMS)
            || (nApnType == APN_EMERGENCY)
            || (nApnType == APN_INTERNET);
}

PUBLIC GLOBAL
IMS_BOOL NetworkPolicy::IsWiFiPolicy(IN const AString& strName)
{
    return strName.StartsWith("wifi");
}

PUBLIC GLOBAL
IMS_BOOL NetworkPolicy::IsWiFiPolicy(IN IMS_SINT32 nApnType)
{
    return (nApnType == APN_WIFI);
}



PRIVATE
NetworkServicePolicy::NetworkServicePolicy()
    : pPrivate(new NetworkServicePolicyPrivate())
{
}

PRIVATE
NetworkServicePolicy::~NetworkServicePolicy()
{
    if (pPrivate != IMS_NULL)
    {
        delete pPrivate;
    }
}

PUBLIC
IMS_BOOL NetworkServicePolicy::AddPolicy(IN const AString &strName,
        IN const NetworkPolicy &objPolicy, IN IMS_SINT32 nSlotId/* = IMS_SLOT_0*/)
{
    NetworkPolicyHolder *pHolder = pPrivate->GetHolder(nSlotId);

    if (pHolder->GetPolicy(strName) != IMS_NULL)
    {
        // It already exists
        return IMS_TRUE;
    }

    return pHolder->AddPolicy(objPolicy);
}

PUBLIC
const NetworkPolicy* NetworkServicePolicy::GetPolicy(
        IN const AString &strName, IN IMS_SINT32 nSlotId/* = IMS_SLOT_0*/) const
{
    NetworkPolicyHolder *pHolder = pPrivate->GetHolder(nSlotId);
    return pHolder->GetPolicy(strName);
}

PUBLIC
const NetworkPolicy* NetworkServicePolicy::GetPolicy(
        IN IMS_SINT32 nAPNType, IN IMS_SINT32 nSlotId/* = IMS_SLOT_0*/) const
{
    NetworkPolicyHolder *pHolder = pPrivate->GetHolder(nSlotId);
    return pHolder->GetPolicy(nAPNType);
}

PUBLIC
void NetworkServicePolicy::RemovePolicy(
        IN const AString &strName, IN IMS_SINT32 nSlotId/* = IMS_SLOT_0*/)
{
    NetworkPolicyHolder *pHolder = pPrivate->GetHolder(nSlotId);
    pHolder->RemovePolicy(strName);
}

PUBLIC
void NetworkServicePolicy::RemoveAllPolicies(IN IMS_SINT32 nSlotId/* = IMS_SLOT_0*/)
{
    NetworkPolicyHolder *pHolder = pPrivate->GetHolder(nSlotId);
    pHolder->RemoveAllPolicies();
}

PUBLIC GLOBAL
NetworkServicePolicy* NetworkServicePolicy::GetInstance()
{
    static NetworkServicePolicy *pNetServicePolicy = IMS_NULL;

    if (pNetServicePolicy == IMS_NULL)
    {
        pNetServicePolicy = new NetworkServicePolicy();
    }

    return pNetServicePolicy;
}
