#ifndef INTERFACE_IMS_IWMS_H_
#define INTERFACE_IMS_IWMS_H_

#include "IMSTypeDef.h"
#include "AString.h"

class IWMS
{
public:
    /*

    Connect to WMS

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_RESULT              SUCCESS or FAILURE
    </table>

    */
    virtual IMS_RESULT ConnectWMS(IMS_UINT32 nSmsFormat) = 0;
    virtual void DisconnectWMS() = 0;
    virtual IMS_RESULT Init() = 0;
    virtual void UpdateServiceStatus(IMS_UINT32 status, IN IMS_SINT32 nSlotID) = 0;
    virtual void UpdateSmsFormat(IMS_UINT32 nSmsFormat, IN IMS_SINT32 nSlotID) = 0;
    virtual IMS_UINT32 ReportMtSMS(IMS_UINT32 smsformat, IMS_UINT32 smslength, IN CONST IMS_BYTE* smsdata, IN IMS_SINT32 nSlotID) = 0;
    // Check Mts Retry-After Header
    virtual IMS_RESULT ReportMoStatus(IMS_UINT32 reason, IMS_UINT32 smsformat, IMS_SINT32 nSmsID_, IMS_SINT32 retrytime, IMS_SINT32 nSlotID)= 0;
    virtual IMS_SINT32 GetMsgID(IN CONST IMS_BYTE* smsdata, IN CONST IMS_UINT32 smsformat) = 0;

public:

    // State of Service
    enum
    {
        STATE_READY = 1,
        STATE_LIMITED,
        STATE_NOTREADY
    };

    enum
    {
        SMSFORMAT_3GPP = 1,
        SMSFORMAT_3GPP2,
        SMSFORMAT_INVALID
    };

    /* these are sms mo (mobile originated) status codes of success and failure.
       ims and wms had a discussion for the status codes and we decided to have
       4 status codes. they seems to be fair enough now. if you need, you can
       add more but after a talk between wms and ims.
    */
    enum
    {
        MO_SUCCESS = 1,
        MO_IMS_TEMP_FAILURE = 2,              /* sending the sms failed but it's still ok to retry to send the sms. */
        MO_IMS_PERM_FAILURE = 3,
        /* sending the sms failed and it's not possible to retry to send the sms.
                                                       sms over ims service is still supportive. */
        MO_IMS_LIMITEDSMSSVCREGI = 4,
        /* ims is currently registered with imsi based uri. it means the administrative sms service mode.
                                                       phone is in a situation that sending sms is not allowed. */
        MO_IMS_REQUEST_RETRY_TIMER = 5          // Check Mts Retry-After Header
    };

    /* These are result codes of processing received messages in WMS.
       IMS relays received sms messages to WMS and then WMS processes and gives back
       the result of processing the sms messages. below describes the results.
    */
    enum
    {
        MT_SUCCESS = 1,
        MT_FAILURE = 2,
        MT_SMS_FORMAT_FAILURE = 3,
        MT_SMS_NODATA_FAILURE = 4
    };

};

#endif
