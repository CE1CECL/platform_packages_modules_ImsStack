/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20091107  toastops@                 Created
    </table>

    Description

*/

#ifndef _SERVICE_IMS_NETWORK_POLICY_H_
#define _SERVICE_IMS_NETWORK_POLICY_H_

#include "AString.h"

class NetworkServicePolicyPrivate;

class NetworkPolicy
{
public:
    explicit NetworkPolicy(IN IMS_BOOL bPrimary = IMS_FALSE);
    NetworkPolicy(IN IMS_BOOL bPrimary, IN const AString& strName, IN IMS_SINT32 nApnType);
    NetworkPolicy(IN const NetworkPolicy& objOther);
    ~NetworkPolicy();

public:
    NetworkPolicy& operator=(IN const NetworkPolicy& objOther);

public:
    inline IMS_SINT32 GetAPNType() const
    { return m_nApnType; }
    inline IMS_SINT32 GetApnType() const
    { return m_nApnType; }
    inline const AString& GetName() const
    { return m_strName; }
    inline IMS_BOOL IsMobilePolicy() const
    { return IsMobilePolicy(m_nApnType); }
    inline IMS_BOOL IsPrimary() const
    { return m_bPrimary; }

    static IMS_BOOL IsMobilePolicy(IN const AString& strName);
    static IMS_BOOL IsMobilePolicy(IN IMS_SINT32 nApnType);
    static IMS_BOOL IsWiFiPolicy(IN const AString& strName);
    static IMS_BOOL IsWiFiPolicy(IN IMS_SINT32 nApnType);

public:
    // APN types
    enum
    {
        APN_NONE = (-1),

        // Mobile
        APN_IMS = 1,
        APN_INTERNET = 2,
        APN_EMERGENCY = 9,
        APN_MOBILE_MAX,

        // WiFi
        APN_WIFI = 21,
        APN_WIFI_MAX
    };

private:
    // Flag to inidicate that the policy is a primary or not
    IMS_BOOL m_bPrimary;
    // Unique name to identify a network
    AString m_strName;
    // APN type to identify the network which the application want to use
    IMS_SINT32 m_nApnType;
};

class NetworkServicePolicy
{
private:
    NetworkServicePolicy();
    ~NetworkServicePolicy();

    NetworkServicePolicy(IN const NetworkServicePolicy& objRHS);
    NetworkServicePolicy& operator=(IN const NetworkServicePolicy& objRHS);

public:
    IMS_BOOL AddPolicy(IN const AString &strName,
            IN const NetworkPolicy &objPolicy, IN IMS_SINT32 nSlotId = IMS_SLOT_0);
    const NetworkPolicy* GetPolicy(IN const AString &strName,
            IN IMS_SINT32 nSlotId = IMS_SLOT_0) const;
    const NetworkPolicy* GetPolicy(IN IMS_SINT32 nAPNType,
            IN IMS_SINT32 nSlotId = IMS_SLOT_0) const;
    void RemovePolicy(IN const AString &strName,
            IN IMS_SINT32 nSlotId = IMS_SLOT_0);
    void RemoveAllPolicies(IN IMS_SINT32 nSlotId = IMS_SLOT_0);

    static NetworkServicePolicy* GetInstance();

private:
    NetworkServicePolicyPrivate *pPrivate;
};

#endif // _SERVICE_IMS_NETWORK_POLICY_H_
