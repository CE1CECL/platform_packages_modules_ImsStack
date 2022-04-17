#include "SipContextUtils.h"

static SipContextUtils *gpObjUtil = SIP_NULL;

SipContextUtils* SipContextUtils::GetInstance()
{
    if (gpObjUtil == SIP_NULL)
    {
        gpObjUtil = new SipContextUtils();
    }

    return gpObjUtil;
}

SIP_VOID SipContextUtils::Destruct()
{
    if (gpObjUtil != SIP_NULL)
    {
        delete gpObjUtil;
        gpObjUtil = SIP_NULL;
    }
}

SipContextUtils::SipContextUtils()
{

}

SipContextUtils::~SipContextUtils()
{

}

SipTxnContext* SipContextUtils::Sip_CreateTxnContext()
{
    return new SipTxnContext();
}

void SipContextUtils::Sip_DestroyTxnContext(IN SipTxnContext *pstContext)
{
    // Destroy SipTxnContext
    if (pstContext != SIP_NULL)
    {
        delete pstContext;
        pstContext = SIP_NULL;
    }
}
