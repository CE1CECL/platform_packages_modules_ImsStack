#ifndef _AOS_IPSEC_HELPER_H_
#define _AOS_IPSEC_HELPER_H_

#include "registration/AosIpsec.h"

class IRegContact;
class IRegParameter;
class IAosAppContext;

/**
 * @brief This class manages related ipsec information from AosRegistration.
 */
class AosIpsecHelper
    : public IAosIpsecListener
{
public:
    AosIpsecHelper(IN IRegContact* piRegContact, IN IRegParameter* piRegParameter,
            IN IAosAppContext* piAppContext, IN AString& strRegId);
    virtual ~AosIpsecHelper();

private:
    AosIpsecHelper(IN const AosIpsecHelper& objRhs);
    AosIpsecHelper& operator=(IN const AosIpsecHelper& objRhs);

public:
    virtual IMS_BOOL Create(IN IMS_BOOL bInitial);
    virtual void CreateOnChallenging();

    virtual IMS_BOOL SetPcscfPortnSpi();
    virtual IMS_BOOL IsPcscfServerPortDifferent();
    virtual IMS_BOOL UpdatePreloadedRoute(IN const AString& strPcscf);

    virtual IMS_BOOL MakeSas(IN const AString& strPcscf, IN const IPAddress& objIpa,
            IN const ByteArray& objIk, IN const ByteArray& objCk);

    virtual IMS_BOOL ProcessAuthChallenged(IN IMS_SINT32 nAlgorithm);
    virtual void ProcessRegStarted();
    virtual void ProcessRegStartFailed();
    virtual IMS_BOOL ProcessRegUpdated();

    virtual void InitIpsec();
    virtual IMS_BOOL IsEstablished();
    virtual void SetSecurityServerPortInRegContact();

    virtual void IgnoreCurrentPolicyExpired();

protected:
    virtual void SetSecurityServerPortInRegistration();
    virtual void SetUePortnSpi(IN IMS_BOOL bInitial);
    virtual IMS_BOOL SetSecurityClientHeader();
    virtual IMS_BOOL CheckSecurityServerHeader();
    virtual void ProcessPolicyExpired(IN AosIpsec* pIpsec);

    // IAosIpsecListener
    virtual void IPSecPolicyExpired(IN AosIpsec* pIpsec);

    void Destroy();

private:
    void CloseUnsecureTCPSocket();
    void CloseSecureTCPSocket(IN AosIpsec* pIpsec);
    IMS_UINT32 GetValidUePort();
    void DeleteSamePolicy();

protected:
    IRegContact* m_piRegContact;
    IRegParameter* m_piRegParameter;
    IAosAppContext* m_piContext;
    AString m_strRegId;

    AosIpsec* m_pOldIpsec;
    AosIpsec* m_pCurrIpsec;
    AosIpsec* m_pNewIpsec;
    UeIpsecInfo* m_pUeIpsecInfo;

    AString m_strTag;

    static const IMS_UINT32 IPSEC_PORT_INTERVAL = 1000;
    static const IMS_UINT32 IPSEC_UPDATE_GUARD_LIFE_TIME_MILLIS = 30000;
};
#endif // _AOS_IPSEC_HELPER_H_
