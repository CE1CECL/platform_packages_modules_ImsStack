#ifndef _INTERFACE_SIP_ERROR_LISTENER_H_
#define _INTERFACE_SIP_ERROR_LISTENER_H_

#include "ISipConnection.h"

/**
 * @brief This class provides a listener interface for notifications
 *        about failure of asynchronous send operations.
 *
 * The application implementing this interface has to register it with
 * a ISipConnection instance using the ISipTransaction::SetErrorListener method.
 *
 * @see ISipConnection
 */
class ISipErrorListener
{
public:
    /**
     * @brief Called when an asynchronous send operation fails or any error occurrs.
     *
     * J180 -> void Error_NotifyError(IN CONST AString &strMessage);
     *
     * @param piSC Pointer to ISipConnection object which error occurrs
     * @param nCode Reason code of error
     * @param strMessage Reason phrase of error\n
     *                   Implementation dependent non-localized information about the error
     */
    virtual void Error_NotifyError(IN ISipConnection *piSC, IN IMS_SINT32 nCode,
            IN CONST AString &strMessage) = 0;
};

#endif // _INTERFACE_SIP_ERROR_LISTENER_H_
