#ifndef _AOS_INFO_H_
#define _AOS_INFO_H_

#include "IImsAosInfo.h"
#include "AString.h"

class IAosAppContext;

class AosInfo
    : public IImsAosInfo
{
public:
    AosInfo(IN IAosAppContext* piContext);
    virtual ~AosInfo();

private:
    // IImsAosInfo
    virtual AString GetAssociatedUri();
    virtual IMS_SINT32 GetConnectionType();
    virtual IMS_UINT32 GetImsFeatures();
    virtual IMS_UINT32 GetImsState();
    virtual IMS_SINT32 GetIpcanType();
    virtual AString GetLastPathHeaderValue();
    virtual AString GetLocalAddress();
    virtual IMS_UINT32 GetLocalPort();
    virtual IMS_UINT32 GetRegisteredNetworkType();
    virtual AString GetPathHeaderValue();
    virtual AString GetPcscfAddress();
    virtual IMS_UINT32 GetPcscfPort();
    virtual IMS_UINT32 GetRegistrationMode();
    virtual AString GetSupportedHeaderValue();
    virtual AString GetServiceRouteHeaderValue();
    virtual void NotifyEmergencyCallState(IN IMS_BOOL bIsInitialized);
    virtual void NotifyPublishState(IN IMS_BOOL bIsStarted);
    virtual void NotifyEmergencySmsState(IN IMS_BOOL bIsInitialized);

    IMS_BOOL IsForbiddenBlock();

private:
    IAosAppContext* m_piContext;
};
#endif // _AOS_INFO_H_
