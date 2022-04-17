#ifndef _IMS_AOS_H_
#define _IMS_AOS_H_

#include "IMSTypeDef.h"
#include "IMSList.h"

class IImsAos;
class AString;

/**
 * @brief This class provides the interface for AoS.
 *
 * @see IImsAos
 */

class ImsAos
{
public:
    /**
     * @brief This class provides the IImsAos interface.
     *
     * @param strAppId an IMS application identifier
     * @param strServiceId an IMS service identifier
     * @param nSlotId Slot id
     * @see IImsAos
     */
    static IImsAos* GetImsAos(IN const AString& strAppId, IN const AString& strServiceId,
            IN IMS_SINT32 nSlotId = IMS_SLOT_0);

    /**
     * @brief This class provides the IImsAos list.
     *
     * @param strAppId an IMS application identifier
     * @param strServiceId an IMS service identifier
     * @param nSlotId Slot id
     * @see IImsAos
     */
    static IMSList<IImsAos*> GetImsAosList(IN const AString& strAppId,
            IN const AString& strServiceId, IN IMS_SINT32 nSlotId = IMS_SLOT_0);

    /**
     * @brief This class provides the IImsAos list.
     *
     * @param strAppId an IMS application identifier
     * @param nSlotId Slot id
     * @see IImsAos
     */
    static IMSList<IImsAos*> GetImsAosList(IN const AString& strAppId,
            IN IMS_SINT32 nSlotId = IMS_SLOT_0);
};

#endif // _IMS_AOS_H_
