/*
    Author
    <table>
    date              author                        description
    --------      --------------            ----------
    20140530      hoonsang.yun@           Created
    </table>

    Description
    SmsService JNI.
*/

#include <utils/String8.h>

#define IMS_STL_USE

#include "JNISmsServiceThread.h"
#include "ServiceMemory.h"
#include "ServiceTrace.h"
#include "IUSmsService.h"

using namespace android;

__IMS_TRACE_TAG_USER_DECL__("JNISmsServiceThread");

PRIVATE
JNISmsServiceThread::JNISmsServiceThread()
    : nNativeObj(0)
    , pCBServiceNoti(NULL)
    , m_nSimSlot(0)
{
    IMS_TRACE_I("JNISmsServiceThread::JNISmsServiceThread : ", 0, 0, 0);
}

IMSAppThread* JNISmsServiceThread::GetInstance()
{
    return new JNISmsServiceThread();
}

PUBLIC VIRTUAL
JNISmsServiceThread::~JNISmsServiceThread()
{
    IMS_TRACE_I("JNISmsServiceThread::~JNISmsServiceThread : ", 0, 0, 0);
}

PUBLIC VIRTUAL
IMS_SINT32 JNISmsServiceThread::SetCallback(IN IMS_SINTP nNativeObj
                                            , IN CBServiceNoti pCBServiceNoti
                                            , IN IMS_SINT32 nSimSlot /*= 0*/)
{
    IMS_TRACE_D("SetCallback : SimSlot[%d]", nSimSlot, 0, 0);
    this->nNativeObj = nNativeObj;
    this->pCBServiceNoti = pCBServiceNoti;
    this->m_nSimSlot = nSimSlot;
    return 1;
}
PUBLIC VIRTUAL
IMS_SINT32 JNISmsServiceThread::GetSimSlot()
{
    IMS_TRACE_D("GetSimSlot[%d]", m_nSimSlot, 0, 0);
    return this->m_nSimSlot;
}
PROTECTED VIRTUAL
IMS_BOOL JNISmsServiceThread::Initialize()
{
    IMS_TRACE_I("JNISmsServiceThread::Initialize : ", 0, 0, 0);

    return IMS_TRUE;
}

PROTECTED VIRTUAL
void JNISmsServiceThread::Uninitialize()
{
    IMS_TRACE_I("JNISmsServiceThread::Uninitialize : ", 0, 0, 0);
}

PROTECTED VIRTUAL
IMS_BOOL JNISmsServiceThread::OnStart(IN IMSMSG &objMSG)
{
    IMS_TRACE_I("JNISmsServiceThread::OnStart : %d", objMSG.GetName(), 0, 0);

    return IMS_TRUE;
}

PROTECTED VIRTUAL
IMS_BOOL JNISmsServiceThread::OnTerminate(IN IMSMSG &objMSG)
{
    IMS_TRACE_I("JNISmsServiceThread::OnTerminate : %d", objMSG.GetName(), 0, 0);

    return IMS_TRUE;
}

PROTECTED VIRTUAL
IMS_BOOL JNISmsServiceThread::OnMessage(IN IMSMSG &objMSG)
{
    IMS_TRACE_I("JNISmsServiceThread::OnMessage : MSG [ %d ], wParam [ %" PFLS_u " ], lParam [ %" PFLS_u " ]",
            objMSG.nMSG, objMSG.nWparam, objMSG.nLparam);

    Parcel parcel;
    parcel.writeInt32(objMSG.nMSG);

    switch( objMSG.nMSG )
    {
        case IUSmsService::CREATE_MTS_MANAGER:
        {
            IUSmsServiceCreateMtsManagerParam* pParam = reinterpret_cast<IUSmsServiceCreateMtsManagerParam*>(objMSG.nLparam);
            if (pParam != IMS_NULL)
            {
                parcel.writeInt32(pParam->nSmsFormat);
                delete pParam;
            }
        }
        break;
        case IUSmsService::DESTROY_MTS_MANAGER:
        {
            IMS_TRACE_I("IUSmsService::DESTROY_MTS_MANAGER", 0, 0, 0);
        }
        break;
        case IUSmsService::UPDATE_MTS_SVC_STATUS:
        {
            IUSmsServiceUpdateServiceStatusParam *pParam = reinterpret_cast<IUSmsServiceUpdateServiceStatusParam*>(objMSG.nLparam);
            if(pParam != IMS_NULL)
            {
                parcel.writeInt32(pParam->nImsStatus);
                parcel.writeInt32(pParam->nSlotId);
                delete pParam;
            }
        }
        break;
        case IUSmsService::UPDATE_MTS_SMS_FORMAT:
        {
            IUSmsServiceUpdateSMSFormatParam *pParam = reinterpret_cast<IUSmsServiceUpdateSMSFormatParam*>(objMSG.nLparam);
            if(pParam != IMS_NULL)
            {
                parcel.writeInt32(pParam->nSmsFormat);
                parcel.writeInt32(pParam->nSlotId);
                delete pParam;
            }
        }
        break;
        case IUSmsService::REPORT_MTS_MO_STATUS:
        {
            IUSmsServiceReportMOStatusParam* pParam = reinterpret_cast<IUSmsServiceReportMOStatusParam*>(objMSG.nLparam);
            if( pParam != IMS_NULL)
            {
                parcel.writeInt32(pParam->nReason);
                parcel.writeInt32(pParam->nSmsFormat);
                parcel.writeInt32(pParam->nRetryAfter);
                parcel.writeInt32(pParam->nSeqId);
                parcel.writeInt32(pParam->nSlotId);
                delete pParam;
            }
        }
        break;
        case IUSmsService::REPORT_MTS_MT_SMS:
        {
            IUSmsServiceReportMTSMSParam* pParam = reinterpret_cast<IUSmsServiceReportMTSMSParam*>(objMSG.nLparam);
            if(pParam != IMS_NULL)
            {
                parcel.writeInt32(pParam->nSmsFormat);
                parcel.writeString16(android::String16(pParam->objData.ToString().GetStr()));
                parcel.writeInt32(pParam->nSlotId);
                delete pParam;
            }
        }
        break;
        default:
        {
            IMS_TRACE_E(0, "JNISmsServiceThread::OnMessage : unknown message = %d", objMSG.nMSG , 0, 0);
            return IMS_TRUE;
        }
        break;
    }

    if( pCBServiceNoti != NULL )
    {
        pCBServiceNoti(nNativeObj, parcel);
    }
    else
    {
        IMS_TRACE_E(0, "[ERROR]JNISmsServiceThread::OnMessage : call back is NULL", 0, 0, 0);
    }

    return IMS_TRUE;
}
