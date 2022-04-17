#ifndef _AOS_LOG_H_
#define _AOS_LOG_H_

#include "interface/AosInternalMsgDef.h"

class AosLog
{
public:
    AosLog();
    virtual ~AosLog();

    // Application Log
    static const IMS_CHAR* AppMessageToString(IN IMS_UINT32 nType);
    static const IMS_CHAR* AppPendingToString(IN IMS_UINT32 nType);
    static const IMS_CHAR* AppRequestToString(IN IMS_UINT32 nType);
    static const IMS_CHAR* AppStateToString(IN IMS_UINT32 nType);
    static const IMS_CHAR* AppTimerToString(IN IMS_UINT32 nType);

    // Registration Log
    static const IMS_CHAR* RegMessageToString(IN IMS_UINT32 nType);
    static const IMS_CHAR* RegModeToString(IN IMS_UINT32 nType);
    static const IMS_CHAR* RegPendingToString(IN IMS_UINT32 nType);
    static const IMS_CHAR* RegReasonToString(IN IMS_SINT32 nType);
    static const IMS_CHAR* RegStateToString(IN IMS_UINT32 nType);
    static const IMS_CHAR* RegTimerToString(IN IMS_UINT32 nType);

    // Event Log
    static const IMS_CHAR* EventToString(IN IMS_SINT32 nEvent);
};
#endif // _AOS_LOG_H_
