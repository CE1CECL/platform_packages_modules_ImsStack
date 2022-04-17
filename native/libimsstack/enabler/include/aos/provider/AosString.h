#ifndef _AOS_STRING_H_
#define _AOS_STRING_H_

#include "IMSTypeDef.h"

class AosString
{
public:
    static const IMS_CHAR STR_SEC_AGREE[];
    static const IMS_CHAR STR_APPLICATION[];
    static const IMS_CHAR STR_3GPP_IMS_XML[];
    static const IMS_CHAR STR_FEATURE_CAPS[];
    static const IMS_CHAR STR_VERSTAT_FEATURE[];
    static const IMS_CHAR STR_USSI_FEATURE[];
    static const IMS_CHAR STR_RTT_FEATURE[];
    static const IMS_CHAR STR_ACCESS_TYPE_FEATURE[];
    static const IMS_CHAR STR_ACCESS_TYPE_CELLULAR[];
    static const IMS_CHAR STR_ACCESS_TYPE_CELLULAR2[];
    static const IMS_CHAR STR_ACCESS_TYPE_WLAN[];
    static const IMS_CHAR STR_ACCESS_TYPE_WLAN1[];
    static const IMS_CHAR STR_P_LAST_ACCESS_NETWORK_INFO[];
};

class FeatureTags
{
public:
    static const IMS_CHAR* STANDALONE_MSG[4];
    static const IMS_CHAR CHAT_IM[];
    static const IMS_CHAR CHAT_SESSION[];
    static const IMS_CHAR FILE_TRANSFER[];
    static const IMS_CHAR FILE_TRANSFER_VIA_SMS[];
    static const IMS_CHAR CALL_COMPOSER_ENRICHED_CALLING[];
    static const IMS_CHAR CALL_COMPOSER_VIA_TELEPHONY[];
    static const IMS_CHAR POST_CALL[];
    static const IMS_CHAR SHARED_MAP[];
    static const IMS_CHAR SHARED_SKETCH[];
    static const IMS_CHAR GEO_PUSH[];
    static const IMS_CHAR GEO_PUSH_VIA_SMS[];
    static const IMS_CHAR CHATBOT_COMMUNICATION_USING_SESSION[];
    static const IMS_CHAR CHATBOT_COMMUNICATION_USING_STANDALONE_MSG[];
    static const IMS_CHAR CHATBOT_VERSION_SUPPORTED[];
    static const IMS_CHAR CHATBOT_VERSION_V2_SUPPORTED[];
    static const IMS_CHAR CHATBOT_ROLE[];
    static const IMS_CHAR MMTEL[];
    static const IMS_CHAR VIDEO[];
    static const IMS_CHAR PRESENCE[];
};

#endif // _AOS_STRING_H_
