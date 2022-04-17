/*
    Author
    <table>
    date              author                        description
    --------      --------------            ----------
    20140530      hoonsang.yun@           Created
    </table>

    Description
    SMSPhone JNI.
*/

#ifndef _JNI_SMS_SERVICE_THREAD_H_
#define _JNI_SMS_SERVICE_THREAD_H_

#include "BaseService.h"
#include "IMSAppThread.h"

class JNISmsServiceThread
    : public IMSAppThread
{
private:
    JNISmsServiceThread();

public:
    static IMSAppThread* GetInstance();
    virtual ~JNISmsServiceThread();

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

#endif    // _JNI_SMS_SERVICE_THREAD_H_
