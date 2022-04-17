#ifndef EMERGENCY_DIALING_PLAN_H_
#define EMERGENCY_DIALING_PLAN_H_

#include "IMSTypeDef.h"

class AString;
class IMtcContext;

class EmergencyDialingPlan
{
public:
    static AString& GetTranslatedUri(IN IMtcContext& objContext, IN_OUT AString& strNumber);

private:
    static IMS_BOOL IsTestNumber(IN const AString& strNumber);
};

#endif
