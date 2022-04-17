#include "IMSTypeDef.h"
#include "helper/MtcAosConnector.h"
#include "ServiceTrace.h"


__IMS_TRACE_TAG_COM_MTC__;


PUBLIC
MtcAosConnector::MtcAosConnector(IN IImsAos& objImsAos, IN IImsAosInfo& objImsAosInfo) :
        m_objImsAos(objImsAos),
        m_objImsAosInfo(objImsAosInfo)

{
    IMS_TRACE_I("+MtcAosConnector", 0, 0, 0);
}

PUBLIC VIRTUAL
MtcAosConnector::~MtcAosConnector()
{
    IMS_TRACE_I("~MtcAosConnector", 0, 0, 0);
}
