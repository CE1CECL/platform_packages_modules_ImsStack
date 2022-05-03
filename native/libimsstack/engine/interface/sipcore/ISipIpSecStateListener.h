#ifndef _INTERFACE_SIP_IPSEC_STATE_LISTENER_H_
#define _INTERFACE_SIP_IPSEC_STATE_LISTENER_H_

#include "ImsTypeDef.h"

/**
 * @brief This class provides an interface to monitor IPSec SA(Security Association) state.
 */
class ISipIpSecStateListener
{
public:
    /**
     * @brief Notifies the application that IPSec security association's state is changed.
     *
     * @param nSAType SA type to be notified\n
     *                #ISipIpSecState#SA_NEW\n
     *                #ISipIpSecState#SA_OLD
     */
    virtual void IpSecState_StateChanged(IN IMS_SINT32 nSAType) = 0;
};

#endif // _INTERFACE_SIP_IPSEC_STATE_LISTENER_H_
