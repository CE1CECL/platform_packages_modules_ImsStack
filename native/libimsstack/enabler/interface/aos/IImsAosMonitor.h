#ifndef _INTERFACE_IMS_AOS_MONITOR_H_
#define _INTERFACE_IMS_AOS_MONITOR_H_

#include "IMSTypeDef.h"

/**
 * @brief This class provides an monitor interface for AoS.
 *
 * The IImsAosMonitor is the monitor interface between service enabler and AoS enabler. \n
 * It provides the APIs to notify the AoS status related to the registration
 * for the every service and the specific information
 *
 * @see IImsAos, ImsAosParameter.h
 */

class IImsAosMonitor
{
public:
    /**
     * @brief Indicate the services that are registered with features over IPCAN type.
     *
     * @param nServices Indicate the services. (@see ImsAosService)
     * @param nIpcan Indicate the IP connectivity access network where IMS is registered. \n
     *                  - IIPCAN::CATEGORY_MOBILE \n
     *                  - IIPCAN::CATEGORY_WLAN
     */
    virtual void ImsAosMonitor_Connected(IN IMS_UINT32 nServices, IN IMS_UINT32 nIpcan) = 0;

    /**
     * @brief Notify the specific information that is required for any enabler.
     *
     * @param nType Indicate the type for the specific information. \n
     *                  - IImsAosMonitor::TYPE_HANDOVER \n
     *                  - IImsAosMonitor::TYPE_SERVICE_BLOCKED \n
     *                  - IImsAosMonitor::TYPE_IPCAN
     * @param nState Indicate the detailed state for the designated type.
     */
    virtual void ImsAosMonitor_Notify(IN IMS_UINT32 nType, IN IMS_UINT32 nState) = 0;

    /// Types and detailed state of notification
    enum
    {
        /// Handover notification from WIFI to Cellular like LTE or NR
        /// @param nState HANDOVER_WIFI_TO_CELLULAR_START, HANDOVER_WIFI_TO_CELLULAR_COMPLETED
        TYPE_HANDOVER = 1,

        /// Service block notification for the specific enabler
        /// @param nState SERVICE_BLOCKED_REG_OUTAGE
        TYPE_SERVICE_BLOCKED = 2,

        /// IP-CAN type notifiction between mobile and wlan
        /// @param nState IIPCAN::CATEGORY_MOBILE, IIPCAN::CATEGORY_WLAN
        TYPE_IPCAN = 3,

        /// Detailed state for the handover type
        /// Handover is started from WIFI to Cellular
        HANDOVER_WIFI_TO_CELLULAR_START = 100,
        /// Handover is completed from WIFI to Cellular
        HANDOVER_WIFI_TO_CELLULAR_COMPLETED,

        /// Detailed state for the service type blocked
        /// Service is blocked with outage reason phrase in 503 response for reregistration
        /// during active IMS call
        SERVICE_BLOCKED_REG_OUTAGE = 200,
     };
};

#endif // _INTERFACE_IMS_AOS_MONITOR_H_
