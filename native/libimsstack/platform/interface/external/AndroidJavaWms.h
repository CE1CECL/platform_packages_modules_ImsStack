
#ifndef IMS_ANDROID_JAVA_WMS_H_
#define IMS_ANDROID_JAVA_WMS_H_

#include "IMSActivityEx.h"
#include "IIMSActivityControl.h"

#include "IMtsClient.h"
#include "IMtsClientListener.h"

class AString;
class IThread;
class System;

class AndroidJavaWMS
    : public IMtsClient
    , public IMSActivityEx
    , public IIMSActivityControl
{
private:
    AndroidJavaWMS(IN IMS_SINT32 nSlotID = 0);
    virtual ~AndroidJavaWMS(void);

public:
    static AndroidJavaWMS* GetAndroidJavaWMS(IN IMS_SINT32 nSlotID = 0);
    static void DestroyAndroidJavaWMS(IN IMS_SINT32 nSlotID = 0);
    static IMS_BOOL StartUp();
    static void CleanUp();
    virtual IMS_RESULT ConnectSC(IN IMS_UINT32 nSmsFormat, IN IMS_SINT32 nSlotID);
    virtual void DisconnectSC(IN IMS_SINT32 nSlotID);
    virtual IMS_RESULT Init();
    virtual IMS_RESULT Release();
    inline virtual IMSWMS_UINT32 GetFeature() { return FEATURE_NONE; }
    virtual void UpdateServiceStatus(IN IMS_UINT32 status, IN IMS_SINT32 nSlotID);
    virtual void UpdateSmsFormat(IN IMS_UINT32 nSmsFormat, IN IMS_SINT32 nSlotID);
    virtual IMS_UINT32 ReportMtSMS(IN IMS_UINT32 smsformat, IN IMS_UINT32 smslength,
            IN CONST IMS_BYTE* smsdata, IN IMS_SINT32 nSlotID);
    virtual IMS_RESULT ReportMoStatus(IN IMS_UINT32 reason, IN IMS_UINT32 smsformat,
            IN IMS_UINT8 nRetryAfter = 0, IN IMS_SINT32 nSeqId = -1, IN IMS_SINT32 nSlotID = -1);

    // IMSActivityEx class
    virtual IMS_BOOL OnMessage(IN IMSMSG& objMSG);

    // IIMSActivityControl class
    virtual IMS_BOOL Control(IN IMS_UINT32 nCmdType, IN IMS_UINTP /* nInParam */,
        OUT IMS_UINTP * /* pnOutParam */);

    IMS_SINT32 GetMsgID(IN CONST IMS_BYTE* smsdata, IN CONST IMS_UINT32 smsformat);
    void ReleaseWMSClient();

    inline void SetTransportID(IMS_UINT32 tid) { nTransportID = tid; }
    inline IMS_SINT32 GetTransportID() { return nTransportID; }
    inline virtual void SetListener(IN IMtsClientListener *iscl) { (void) iscl; }

private:
    static AndroidJavaWMS* pAndroidJavaWMS[];
    IMS_BOOL isWmsConnected;
    IMS_BOOL isSmsServerReady;
    IMS_SINT32 nClientHandle;
    IMS_UINT32 nTransportID;
    IMS_UINT32 nSmsFormat;
    IMS_UINT32 nStatus;
    IMS_SINT32 mSlotID;
};

#endif
