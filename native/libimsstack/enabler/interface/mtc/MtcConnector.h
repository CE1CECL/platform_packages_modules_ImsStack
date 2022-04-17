#ifndef MTC_CONNECTOR_H_
#define MTC_CONNECTOR_H_

#include "IMSTypeDef.h"

class IMtcCallStateListener;

class MtcConnector
{
public:
    static void AddCallStateListener(IN IMS_SINT32 nSlotId, IN IMtcCallStateListener* pListener);
    static void RemoveCallStateListener(IN IMS_SINT32 nSlotId, IN IMtcCallStateListener* pListener);
};

#endif
