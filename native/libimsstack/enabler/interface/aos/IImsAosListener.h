#ifndef _INTERFACE_IMS_AOS_LISTENER_H_
#define _INTERFACE_IMS_AOS_LISTENER_H_

#include "IMSTypeDef.h"

/**
 * @brief This class provides an listener interface for AoS.
 *
 * The IImsAoSListener is the listener interface between service enabler and AoS enabler. \n
 * It provides the APIs to notify the AoS status related to the registration
 * for the dedicated service.
 *
 * @see IImsAos
 */

class IImsAosListener
{
public:
    /**
     * @brief Indicate that the service is connected.
     *
     * If service is registered and feature is changed with one and more, this API is invoked. \n
     * When this API is invoked by AoS, the enabler should check whether feature is changed or not.
     *
     * @param nFeatures Indicate the features for registration (@see ImsAosFeature)
     * @param nIpcan Indicate the IP connectivity access network where IMS is registered. \n
     *                  - IIPCAN::CATEGORY_MOBILE \n
     *                  - IIPCAN::CATEGORY_WLAN
     */
    virtual void ImsAos_Connected(IN IMS_UINT32 nFeatures, IN IMS_UINT32 nIpcan) = 0;

    /**
     * @brief Indicate that the service is connecting. It means registration is trying
     *        with features for the dedicated service.
     *
     */
    virtual void ImsAos_Connecting() = 0;

    /**
     * @brief Indicate that the service is disconnecting. \n
     *        It means that deregistraiton is tried for the decicated service
     *
     * @param nReason Indicate the reason that results in disconnecting the service.
     *                (@see ImsAosReason)
     */
    virtual void ImsAos_Disconnecting(IN IMS_UINT32 nReason) = 0;

    /**
     * @brief Indicate that the service is disconnected with reason.
     *
     * @param nReason Indicate the reason that results that the service is disconnected.
     *                (@see ImsAosReason)
     */
    virtual void ImsAos_Disconnected(IN IMS_UINT32 nReason) = 0;

    /**
     * @brief Indicate that the service is suspended with reason like out-of-servce
     *        even though registration is kept.
     *
     * Enabler determines whether the service is stopped or not(session is ongoing or not).
     *
     * @param nReason Indicate the reason that results that the service is suspended.
     *                (@see ImsAosReason)
     */
    virtual void ImsAos_Suspended(IN IMS_UINT32 nReason) = 0;

    /**
     * @brief Indicate that the service is resumed without the suspended reasons
     */
    virtual void ImsAos_Resumed() = 0;
};

#endif // _INTERFACE_IMS_AOS_LISTENER_H_
