#ifndef JNI_SMS_SCBM_SERVICE_THREAD_H_
#define JNI_SMS_SCBM_SERVICE_THREAD_H_

#include "BaseService.h"
#include "IMSAppThread.h"

class JNISmsSCBMServiceThread
    : public IMSAppThread
{
private:
    JNISmsSCBMServiceThread();

public:
    static IMSAppThread* GetInstance();
    virtual ~JNISmsSCBMServiceThread();

    IMS_SINT32 SetCallback(IN IMS_SINTP nNativeObj
                                , IN CBServiceNoti pCBServiceNoti
                                , IN IMS_SINT32 nSimSlot = 0);
    IMS_SINT32 GetSimSlot();

protected:
    virtual IMS_BOOL Initialize();
    virtual void Uninitialize();

    virtual IMS_BOOL OnStart(IN IMSMSG &objMSG);
    virtual IMS_BOOL OnTerminate(IN IMSMSG &objMSG);
    virtual IMS_BOOL OnMessage(IN IMSMSG &objMSG);

private:
    IMS_UINTP nNativeObj;
    CBServiceNoti pCBServiceNoti;
    IMS_SINT32 m_nSimSlot;
};

#endif
