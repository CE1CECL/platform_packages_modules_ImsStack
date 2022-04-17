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

#ifndef _JNI_SMS_SERVICE_H_
#define _JNI_SMS_SERVICE_H_

#include "BaseService.h"

class JNISmsServiceThread;

using namespace android;

class JNISmsService
    : public BaseService
{
    public:
        JNISmsService(IN IMS_SINT32 nSimSlot = 0);
        JNISmsService(CBServiceNoti pCBServiceNoti, IN IMS_SINT32 nSimSlot = 0);
        virtual ~JNISmsService();

        virtual int SendData(const Parcel& pParcel);

    private:
        void HandleMessage(int nMsg, const Parcel& pParcel);

    private:
        JNISmsServiceThread* pJNISmsServiceThread;
        IMS_SINT32 m_nSlotID;
        AString m_aStrTargetActivity;
        AString strThreadName;
};

#endif //_JNI_SMS_SERVICE_H_
