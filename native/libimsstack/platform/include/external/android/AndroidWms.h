/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20101013  changik.jeong@            Created
    20160614  jaeyong1.park@             modify for IMS5
    20181011  sungkwang2.lee@           multi-instance qct client
    </table>

    Description
*/
#ifndef _IMS_ANDROID_WMS_H_
#define _IMS_ANDROID_WMS_H_


#include "IMtsClient.h"
#include "ImsWmsLiteTypeDef.h"


class AndroidWMS
    : public IMtsClient
{
private:
    AndroidWMS(IN IMSWMS_SINT32 nSlotID);
    virtual ~AndroidWMS();

public:
    static AndroidWMS* GetAndroidWMS(IN IMSWMS_SINT32 nSlotID);
    static void DestroyAndroidWMS(IN IMSWMS_SINT32 nSlotID);
    static IMSWMS_BOOL StartUp();
    static void CleanUp();
    virtual IMSWMS_RESULT ConnectSC(IN IMSWMS_UINT32 nSmsFormat, IN IMSWMS_SINT32 nSlotID);
    virtual void DisconnectSC(IN IMSWMS_SINT32 nSlotID);
    virtual IMSWMS_RESULT Init();
    virtual IMSWMS_RESULT Release();

    virtual IMSWMS_UINT32 GetFeature();
    virtual void UpdateServiceStatus(IN IMSWMS_UINT32 status, IN IMSWMS_SINT32 nSlotID);
    virtual void UpdateSmsFormat(IN IMSWMS_UINT32 nSmsFormat, IN IMSWMS_SINT32 nSlotID);
    virtual IMSWMS_UINT32 ReportMtSMS(IN IMSWMS_UINT32 smsformat, IN IMSWMS_UINT32 smslength,
        IN CONST IMSWMS_BYTE* smsdata, IN IMSWMS_SINT32 nSlotID);
    virtual IMSWMS_RESULT ReportMoStatus(IN IMSWMS_UINT32 reason, IN IMSWMS_UINT32 smsformat,
        IN IMSWMS_UINT8 nRetryAfter = 0, IN IMSWMS_SINT32 nSeqId = -1, IN IMSWMS_SINT32 nSlotID = -1);

    IMSWMS_SINT32 GetMsgID(IN CONST IMSWMS_BYTE* smsdata, IN CONST IMSWMS_UINT32 smsformat);
    void ReleaseWMSClient();
    bool androidWms_IsWmsConnected();
    void androidWms_SetWmsConnectionState(bool connected);

    inline void SetTransportID(IN IMSWMS_UINT32 tid) { nTransportID = tid; }
    inline IMSWMS_SINT32 GetTransportID() { return nTransportID; }

    void SetListener(IN IMtsClientListener *iscl);
    IMtsClientListener* GetListener();

private:
    static AndroidWMS* pAndroidWMS[];
    IMSWMS_SINT32 m_nSlotID;

    IMSWMS_BOOL isWmsConnected;
    IMSWMS_BOOL isSmsServerReady;
    IMSWMS_SINT32 nClientHandle;
    IMSWMS_UINT32 nTransportID;
    IMSWMS_UINT32 nSmsFormat;
    IMSWMS_UINT32 nStatus;
    IMtsClientListener *piListener;
};

#endif // _IMS_ANDROID_WMS_H_
