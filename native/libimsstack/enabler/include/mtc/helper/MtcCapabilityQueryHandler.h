#ifndef MTC_CAPABILITY_QUERY_HANDLER_H_
#define MTC_CAPABILITY_QUERY_HANDLER_H_

#include "IMSTypeDef.h"

class IMessage;
class ICoreService;
class ICapabilities;
class AString;
class AStringArray;

class MtcCapabilityQueryHandler
{
public:
    static void HandleIncomingCapabilityQuery(IN ICoreService* piService,
            IN ICapabilities* piCapabilities, IN const AString& strAppId,
            IN const AString& strServiceId, IN IMS_SINT32 nSlotId);

private:
    static void SetHeaderForCapabilityQuery(IN ICoreService* piService, IN IMessage* piMessage);
    static void SetBodyForCapabilityQuery(IN ICoreService* piService, IN IMessage* piMessage,
            IN const AString& strAppId, IN const AString& strServiceId, IN IMS_SINT32 nSlotId);
    static void SetSessionLevelDescription(IN ICoreService* piService, OUT AString& strDesc);

    // if EVS is supported by default, this should be removed.
    static AString GetAdjustedCodecList(IN const AStringArray& objAudioCaps);
};

#endif
