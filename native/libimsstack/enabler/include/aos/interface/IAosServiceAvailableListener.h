#ifndef _INTERFACE_AOS_SERVICE_AVAILABLE_LISTENER_H_
#define _INTERFACE_AOS_SERVICE_AVAILABLE_LISTENER_H_

/**
 * @brief
 *
 * @see
 */
class IAosServiceAvailableListener
{
public:
    /**
     * @brief Notifies if any service is available under the current conditions.
     *
     * @param
     */
    virtual void ServiceAvailable_Changed() = 0;

    /**
     * @brief Notifies if there are specific commands under the current conditions.
     *
     * @param
     */
    virtual void ServiceAvailable_RequestCommand(IN IMS_UINT32 nCommand,
            IN IMS_UINT32 nReason) = 0;
};

#endif // _INTERFACE_AOS_SERVICE_AVAILABLE_LISTENER_H_