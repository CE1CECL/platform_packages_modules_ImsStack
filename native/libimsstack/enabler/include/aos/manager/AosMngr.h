#ifndef _AOS_MNGR_H_
#define _AOS_MNGR_H_

#include "IMSList.h"
#include "IMSMap.h"

class AString;
class AosBuildDirector;
class IAosHandle;
class IAosBuilder;
class IAosAppContext;

class AosMngr
{
public:
    AosMngr(IN IMS_SINT32 nSlotId);
    virtual ~AosMngr();

private:
    AosMngr(IN const AosMngr& objRHS);
    AosMngr& operator=(IN const AosMngr& objRHS);

public:
    IAosHandle* GetAosHandle(IN const AString& strAppId, IN const AString& strSrvId);
    IMSList<IAosHandle*> GetAllAosHandles(IN const AString& strAppId,
            IN const AString& strSrvId);
    IMSList<IAosHandle*> GetAllAosHandles(IN const AString& strAppId);

    IAosAppContext* GetAosAppContext(IN const AString& strProfileId);

private:
    void CreateStaticConfig();
    void CreateAoS();
    void DestroyAoS();

    IAosBuilder* AosBuilderFactory();

private:
    IMS_SINT32 m_nSlotId;
    IMSList<AString> m_objAppId;
    IMSMap<AString, IAosAppContext*> m_objAppContext;
    AosBuildDirector* m_pBuildDirector;
};
#endif // _AOS_MNGR_H_
