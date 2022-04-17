#ifndef _AOS_FACTORY_H_
#define _AOS_FACTORY_H_

#include "IMSMap.h"
#include "AString.h"

class IMutex;
class ImsAosManager;

/**
 * @brief This is the factory class for AoS.
 */

class AosFactory
{
public:
    AosFactory();
    virtual ~AosFactory();

    static ImsAosManager* GetManager(IN IMS_SINT32 nSlotId = IMS_SLOT_0);
    static void Start(IN IMS_SINT32 nSlotId = IMS_SLOT_0);
    static void Stop(IN IMS_SINT32 nSlotId = IMS_SLOT_0);

private:
    static AosFactory* m_gpAosFactory;
    static IMutex* m_gpiLock;

    /// <slot-id, ImsAosManager>
    static IMSMap<IMS_SINT32, ImsAosManager*> m_objManagers;
};

#endif //_AOS_FACTORY_H_
