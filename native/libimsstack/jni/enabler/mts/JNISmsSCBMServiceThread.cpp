#include <utils/String8.h>

#define IMS_STL_USE

#include "JNISmsSCBMServiceThread.h"
#include "ServiceMemory.h"
#include "ServiceTrace.h"
#include "IUSmsSCBMService.h"

using namespace android;

__IMS_TRACE_TAG_USER_DECL__("JNISmsSCBMServiceThread");

PRIVATE
JNISmsSCBMServiceThread::JNISmsSCBMServiceThread()
    : nNativeObj(0)
    , pCBServiceNoti(NULL)
    , m_nSimSlot(0)
{
    IMS_TRACE_I("JNISmsSCBMServiceThread::JNISmsSCBMServiceThread : ", 0, 0, 0);
}

IMSAppThread* JNISmsSCBMServiceThread::GetInstance()
{
    return new JNISmsSCBMServiceThread();
}

PUBLIC VIRTUAL
JNISmsSCBMServiceThread::~JNISmsSCBMServiceThread()
{
    IMS_TRACE_I("JNISmsSCBMServiceThread::~JNISmsSCBMServiceThread : ", 0, 0, 0);
}

PUBLIC VIRTUAL
IMS_SINT32 JNISmsSCBMServiceThread::SetCallback(IN IMS_SINTP nNativeObj
                                            , IN CBServiceNoti pCBServiceNoti
                                            , IN IMS_SINT32 nSimSlot /*= 0*/)
{
    IMS_TRACE_D("JNISmsSCBMServiceThread, SetCallback : SimSlot[%d]", nSimSlot, 0, 0);
    this->nNativeObj = nNativeObj;
    this->pCBServiceNoti = pCBServiceNoti;
    this->m_nSimSlot = nSimSlot;
    return 1;
}
PUBLIC VIRTUAL
IMS_SINT32 JNISmsSCBMServiceThread::GetSimSlot()
{
    IMS_TRACE_D("GetSimSlot[%d]", m_nSimSlot, 0, 0);
    return this->m_nSimSlot;
}
PROTECTED VIRTUAL
IMS_BOOL JNISmsSCBMServiceThread::Initialize()
{
    IMS_TRACE_I("JNISmsSCBMServiceThread::Initialize : ", 0, 0, 0);

    return IMS_TRUE;
}

PROTECTED VIRTUAL
void JNISmsSCBMServiceThread::Uninitialize()
{
    IMS_TRACE_I("JNISmsSCBMServiceThread::Uninitialize : ", 0, 0, 0);
}

PROTECTED VIRTUAL
IMS_BOOL JNISmsSCBMServiceThread::OnStart(IN IMSMSG &objMSG)
{
    IMS_TRACE_I("JNISmsSCBMServiceThread::OnStart : %d", objMSG.GetName(), 0, 0);

    return IMS_TRUE;
}

PROTECTED VIRTUAL
IMS_BOOL JNISmsSCBMServiceThread::OnTerminate(IN IMSMSG &objMSG)
{
    IMS_TRACE_I("JNISmsSCBMServiceThread::OnTerminate : %d", objMSG.GetName(), 0, 0);

    return IMS_TRUE;
}

PROTECTED VIRTUAL
IMS_BOOL JNISmsSCBMServiceThread::OnMessage(IN IMSMSG &objMSG)
{
    IMS_TRACE_I("JNISmsSCBMServiceThread::OnMessage : "
            "MSG [ %d ], wParam [ %" PFLS_u " ], lParam [ %" PFLS_u " ]",
            objMSG.nMSG, objMSG.nWparam, objMSG.nLparam);

    Parcel parcel;
    parcel.writeInt32(objMSG.nMSG);

    switch( objMSG.nMSG )
    {
        case IUSmsSCBMService::CREATE_SCBM_MANAGER:
        {
            IMS_TRACE_I("IUSmsSCBMService::CREATE_SCBM_MANAGER", 0, 0, 0);
        }
        break;
        case IUSmsSCBMService::DESTROY_SCBM_MANAGER:
        {
            IMS_TRACE_I("IUSmsSCBMService::DESTROY_MTS_MANAGER", 0, 0, 0);
        }
        break;
        case IUSmsSCBMService::REQUEST_SMS_RAT_SELECTION:
        {
            IMS_TRACE_I("JNISmsSCBMServiceThread::REQUEST_SMS_RAT_SELECTION", 0, 0, 0);

            IUSmsServiceUpdateSCBMParam *pParam =
                    reinterpret_cast<IUSmsServiceUpdateSCBMParam*>(objMSG.nLparam);
            if(pParam != IMS_NULL)
            {
                parcel.writeInt32(pParam->nScbmMode);
                delete pParam;
            }
        }
        break;
        case IUSmsSCBMService::REQUEST_SMS_EXIT_RAT_SELECTION:
        {
            IMS_TRACE_I("JNISmsSCBMServiceThread::REQUEST_SMS_EXIT_RAT_SELECTION", 0, 0, 0);
        }
        break;
        default:
        {
            IMS_TRACE_E(0, "JNISmsSCBMServiceThread::OnMessage : unknown message = %d",
                    objMSG.nMSG , 0, 0);
            return IMS_TRUE;
        }
        break;
    }

    if( pCBServiceNoti != NULL )
    {
        IMS_TRACE_I("JNISmsSCBMServiceThread::pCBServiceNoti is Not Null", 0, 0, 0);
        pCBServiceNoti(nNativeObj, parcel);
    }
    else
    {
        IMS_TRACE_E(0, "[ERROR]JNISmsServiceThread::OnMessage : call back is NULL", 0, 0, 0);
    }

    return IMS_TRUE;
}
