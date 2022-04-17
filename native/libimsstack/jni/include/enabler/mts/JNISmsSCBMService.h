#ifndef JNI_SMS_SCBM_SERVICE_H_
#define JNI_SMS_SCBM_SERVICE_H_

#include "BaseService.h"

class JNISmsSCBMServiceThread;

using namespace android;

class JNISmsSCBMService
    : public BaseService
{
    public:
        JNISmsSCBMService(IN IMS_SINT32 nSimSlot = 0);
        JNISmsSCBMService(CBServiceNoti pCBServiceNoti, IN IMS_SINT32 nSimSlot = 0);
        virtual ~JNISmsSCBMService();

        virtual int SendData(const Parcel& pParcel);

    private:
        void HandleMessage(int nMsg, const Parcel& pParcel);

    private:
        JNISmsSCBMServiceThread* pJNISmsSCBMServiceThread;
        IMS_SINT32 m_nSlotID;
        AString m_aStrTargetActivity;
        AString strThreadName;
};

#endif
