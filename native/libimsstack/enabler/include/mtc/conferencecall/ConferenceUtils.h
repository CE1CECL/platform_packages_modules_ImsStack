#ifndef CONFERENCE_UTILS_H_
#define CONFERENCE_UTILS_H_

#include "IMSTypeDef.h"
#include "AString.h"

class ConferenceUtils
{
public:
    static const AString& GetUserPart(IN const AString& strUri, OUT AString& strUserPart);
};

#endif
