/*
*/

#ifndef _AOS_STATIC_CONFIG_H_
#define _AOS_STATIC_CONFIG_H_

#include "IMSList.h"
#include "provider/AosStaticProfile.h"

class AosStaticConfig
{
public:
    AosStaticConfig();
    virtual ~AosStaticConfig();

    static AosStaticConfig* GetInstance();

    IMS_BOOL Create();
    AosStaticProfile* GetProfile(IN const AString& strAppId, IN const AString& strServiceId);
    const IMSList<AosStaticProfile*>& GetProfiles() const;

private:
    IMSList<AosStaticProfile*> m_objProfiles;
};

#endif // _AOS_STATIC_CONFIG_H_
