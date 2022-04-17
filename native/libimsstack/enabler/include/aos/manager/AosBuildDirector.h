#ifndef _AOS_BUILD_DIRECTOR_H_
#define _AOS_BUILD_DIRECTOR_H_

#include "IMSMap.h"

class AString;

class IAosBuilder;
class IAosAppContext;

class IAosConnection;
class IAosNetTracker;
class AosStaticProfile;

class AosBuildDirector
{
public:
    AosBuildDirector(IN IAosBuilder* piBuilder, IN IMS_SINT32 nSlotId);
    virtual ~AosBuildDirector();

    IAosAppContext* ConstructAos(IN AosStaticProfile* pProfile);
    void ConstructProvider();
    void DestructAos();
    void DestructProvider();

private:
    IMS_SINT32 m_nSlotId;
    IAosBuilder* m_piBuilder;

    IMSMap<IMS_SINT32, IAosConnection*> m_objConnection;
    IMSMap<IMS_SINT32, IAosNetTracker*> m_objNetTracker;
    IMSMap<AString, IAosAppContext*> m_objAppContext;
};
#endif // _AOS_BUILD_DIRECTOR_H_
