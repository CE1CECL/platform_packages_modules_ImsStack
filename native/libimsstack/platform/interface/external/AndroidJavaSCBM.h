#ifndef IMS_ANDROID_JAVA_SCBM_H_
#define IMS_ANDROID_JAVA_SCBM_H_

#include "IMSActivityEx.h"

#include "IMtsSCBM.h"

class AString;
class IThread;
class System;

class AndroidJavaSCBM
    : public IMtsSCBM
    , public IMSActivityEx
{
private:
    AndroidJavaSCBM();
    virtual ~AndroidJavaSCBM(void);

public:
    static AndroidJavaSCBM* GetAndroidJavaSCBM();
    static void DestroyAndroidJavaSCBM(IN IMS_SINT32 nSlotID = 0);
    static IMS_BOOL StartUp();
    static void CleanUp();

    virtual IMS_RESULT ConnectSCBM(IN IMS_SINT32 nSlotID);
    virtual void DisconnectSCBM(IN IMS_SINT32 nSlotID);
    virtual void RequestRATSelection(IN IMS_UINT32 isSCBMMode);
    virtual void RequestExitRATSelection();
    virtual void RequestSetE911State();
    // IMSActivityEx class
    virtual IMS_BOOL OnMessage(IN IMSMSG& objMSG);
private:
//    IMS_BOOL isWmsConnected;
    IMS_BOOL isSmsScbmServerReady;
//    IMS_SINT32 nClientHandle;
//    IMS_UINT32 nTransportID;
//    IMS_UINT32 nSmsFormat;
//    IMS_UINT32 nStatus;
};

#endif
