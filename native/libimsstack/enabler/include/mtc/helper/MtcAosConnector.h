#ifndef MTC_AOS_CONNECTOR_H_
#define MTC_AOS_CONNECTOR_H_

#include "AString.h"
#include "IMSTypeDef.h"
#include "IImsAos.h"
#include "IImsAosInfo.h"

class MtcAosConnector
{
public:
    MtcAosConnector(IN IImsAos& objImsAos, IN IImsAosInfo& objImsAosInfo);
    virtual ~MtcAosConnector();
    MtcAosConnector(IN const MtcAosConnector&) = delete;
    MtcAosConnector& operator=(IN const MtcAosConnector&) = delete;

    // IImsAos interface wrappers.
    inline IMS_UINT32 GetFeatures() { return m_objImsAos.GetFeatures(); }
    inline IMS_UINT32 GetSuspendedReason()  { return m_objImsAos.GetSuspendedReason(); }
    inline IMS_BOOL IsFeatureConnected(IN IMS_UINT32 nFeature)
    { return m_objImsAos.IsFeatureConnected(nFeature); }
    inline IMS_BOOL IsImsConnected() { return m_objImsAos.IsImsConnected(); }
    inline IMS_BOOL IsImsSuspended() { return m_objImsAos.IsImsSuspended(); }
    inline void UpdateFeature(IN IMS_UINT32 nFeatures) { m_objImsAos.UpdateFeature(nFeatures); }
    inline IMS_BOOL Control(IN IMS_UINT32 nType) { return m_objImsAos.Control(nType); }

    // IImsAosInfo interface wrappers.
    inline AString GetAssociatedUri() { return m_objImsAosInfo.GetAssociatedUri(); }
    inline IMS_UINT32 GetConnectionType() { return m_objImsAosInfo.GetConnectionType(); }
    inline IMS_UINT32 GetImsState() { return m_objImsAosInfo.GetImsState(); }
    inline IMS_UINT32 GetIpcanType() { return m_objImsAosInfo.GetIpcanType(); }
    inline AString GetLastPathHeaderValue() { return m_objImsAosInfo.GetLastPathHeaderValue(); }
    inline AString GetLocalAddress() { return m_objImsAosInfo.GetLocalAddress(); }
    inline IMS_UINT32 GetLocalPort() { return m_objImsAosInfo.GetLocalPort(); }
    inline IMS_UINT32 GetRegisteredNetworkType()
    { return m_objImsAosInfo.GetRegisteredNetworkType(); }
    inline AString GetPathHeaderValue() { return m_objImsAosInfo.GetPathHeaderValue(); }
    inline AString GetPcscfAddress() { return m_objImsAosInfo.GetPcscfAddress(); }
    inline IMS_UINT32 GetPcscfPort() { return m_objImsAosInfo.GetPcscfPort(); }
    inline IMS_UINT32 GetRegistrationMode() { return m_objImsAosInfo.GetRegistrationMode(); }
    inline AString GetSupportedHeaderValue() { return m_objImsAosInfo.GetSupportedHeaderValue(); }
    inline AString GetServiceRouteHeaderValue()
    { return m_objImsAosInfo.GetServiceRouteHeaderValue(); }

    inline void NotifyEmergencyCallState(IN IMS_BOOL bIsInitialized)
    { return m_objImsAosInfo.NotifyEmergencyCallState(bIsInitialized); }

private:
    IImsAos& m_objImsAos;
    IImsAosInfo& m_objImsAosInfo;
};

#endif
