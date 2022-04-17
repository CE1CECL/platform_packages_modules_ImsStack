#ifndef QOS_STRING_DEF_H_
#define QOS_STRING_DEF_H_

#include "IMSTypeDef.h"
#include "SdpMedia.h"
#include "SdpAttribute.h"
#include "offeranswer/SdpPrecondition.h"
#include "precondition/QosDef.h"

class QosStringDef
{
public:
    inline static const IMS_CHAR* QOS_PS_MediaType(IN IMS_SINT32 eSdpMediaType)
    {
        switch (eSdpMediaType)
        {
            case SdpMedia::TYPE_AUDIO:
                return "audio";
            case SdpMedia::TYPE_VIDEO:
                return "video";
            case SdpMedia::TYPE_TEXT:
                return "text";
            default:
                return "invalid";
        }
    }

    inline static const IMS_CHAR* QOS_PS_AttrType(IN IMS_SINT32 eAttrType)
    {
        switch (eAttrType)
        {
            case SdpAttribute::CURR:
                return "curr";
            case SdpAttribute::DES:
                return "des";
            case SdpAttribute::CONF:
                return "conf";
            default:
                return "invalid";
        }
    }

    inline static const IMS_CHAR* QOS_PS_CheckType(IN QosCheckType eCheckType)
    {
        switch (eCheckType)
        {
            case QosCheckType::ALL_STATUS:
                return "all";
            case QosCheckType::LOCAL_STATUS:
                return "local";
            case QosCheckType::REMOTE_STATUS:
                return "remote";
            default:
                return "invalid";
        }
    }

    inline static const IMS_CHAR* QOS_PS_DirTag(IN IMS_SINT32 eDirTag)
    {
        switch (eDirTag)
        {
            case SdpPrecondition::DIRECTION_NONE:
                return "none";
            case SdpPrecondition::DIRECTION_SEND:
                return "send";
            case SdpPrecondition::DIRECTION_RECV:
                return "recv";
            case SdpPrecondition::DIRECTION_SENDRECV:
                return "sendrecv";
            default:
                return "invalid";
        }
    }

    inline static const IMS_CHAR* QOS_PS_StrengthTag(IN IMS_SINT32 eStrengthTag)
    {
        switch (eStrengthTag)
        {
            case SdpPrecondition::STRENGTH_MANDATORY:
                return "mandatory";
            case SdpPrecondition::STRENGTH_OPTIONAL:
                return "optional";
            case SdpPrecondition::STRENGTH_NONE:
                return "none";
            case SdpPrecondition::STRENGTH_FAILURE:
                return "failure";
            case SdpPrecondition::STRENGTH_UNKNOWN:
                return "unknown";
            case SdpPrecondition::STRENGTH_NOTUSED:
                return "not used";
            default:
                return "invalid";
        }
    }

    inline static const IMS_CHAR* QOS_PS_LossPolicy(IN QosLossPolicy ePolicy)
    {
        switch (ePolicy)
        {
            case QosLossPolicy::MAINTAIN_ASIS:
                return "maintain-asis";
            case QosLossPolicy::MODIFY:
                return "modify";
            case QosLossPolicy::RELEASE:
                return "release";
            default:
                return "invalid";
        }
    }

    inline static const IMS_CHAR* QOS_PS_Status(IN QosStatus eStatus)
    {
        switch (eStatus)
        {
            case QosStatus::IDLE:
                return "idle";
            case QosStatus::AVAILABLE:
                return "available";
            case QosStatus::LOST:
                return "lost";
            default:
                return "invalid";
        }
    }

    inline static const IMS_CHAR* QOS_PS_TimerType(IN QosTimerType eTimerType)
    {
        switch (eTimerType)
        {
            case QosTimerType::WAIT_AVAILABLE:
                return "timer - wait available";
            case QosTimerType::GUARD_INACTIVE:
                return "timer - guard inactive";
            case QosTimerType::FORCE_AVAILABLE:
                return "timer - force available";
            default:
                return "invalid";
        }
    }
};

#ifndef QOS_PS_MediaType
#define QOS_PS_MediaType(A) \
QosStringDef::QOS_PS_MediaType(A)
#endif

#ifndef QOS_PS_AttrType
#define QOS_PS_AttrType(A) \
QosStringDef::QOS_PS_AttrType(A)
#endif

#ifndef QOS_PS_CheckType
#define QOS_PS_CheckType(A) \
QosStringDef::QOS_PS_CheckType(A)
#endif

#ifndef QOS_PS_DirTag
#define QOS_PS_DirTag(A) \
QosStringDef::QOS_PS_DirTag(A)
#endif

#ifndef QOS_PS_StrengthTag
#define QOS_PS_StrengthTag(A) \
QosStringDef::QOS_PS_StrengthTag(A)
#endif

#ifndef QOS_PS_LossPolicy
#define QOS_PS_LossPolicy(A) \
QosStringDef::QOS_PS_LossPolicy(A)
#endif

#ifndef QOS_PS_Status
#define QOS_PS_Status(A) \
QosStringDef::QOS_PS_Status(A)
#endif

#ifndef QOS_PS_TimerType
#define QOS_PS_TimerType(A) \
QosStringDef::QOS_PS_TimerType(A)
#endif

#endif
