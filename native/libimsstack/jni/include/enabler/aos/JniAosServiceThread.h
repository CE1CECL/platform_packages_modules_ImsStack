#ifndef _JNI_AOS_SERVICE_THREAD_H_
#define _JNI_AOS_SERVICE_THREAD_H_

#include "BaseServiceThread.h"

class JniAosServiceThread final
    : public BaseServiceThread
{
public:
    JniAosServiceThread();
    virtual ~JniAosServiceThread();

    inline void SetSlotId(IN IMS_SINT32 nSlotId) { m_nSlotId = nSlotId; }

    void NotifyRegistered(IN IMS_SINT32 nNetworkType, IN IMS_UINT32 nFeatureTagBits,
            IN const IMSList<AString>& objFeatureTags);

    void NotifyRegistering(IN IMS_SINT32 nNetworkType, IN IMS_UINT32 nFeatureTagBits,
            IN const IMSList<AString>& objFeatureTags);

    void NotifyDeregistered(IN IMS_SINT32 nReason);
    void NotifyTechnologyChangeFailed(IN IMS_SINT32 nNetworkType, IN IMS_SINT32 nCauseCode);
    void NotifyAssociatedUriChanged(IN const IMSList<AString>& objUris);
    void NotifyCapabilitiesUpdateFailed(IN IMS_UINT32 nCapabilities, IN IMS_SINT32 nNetworkType,
            IN IMS_SINT32 nReason);

    void NotifyAosIsimState(IN IMS_UINT32 nState);
    void NotifyRegEventState(IN IMS_UINT32 nState);
    void RequestPhoneNumberRetry(IN IMS_UINT32 nCommand);
    void RequestWifiService(IN IMS_BOOL bIsOn);

private:
    IMS_SINT32 m_nSlotId;
};

#endif // _JNI_AOS_SERVICE_THREAD_H_