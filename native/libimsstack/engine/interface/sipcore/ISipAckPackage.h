#ifndef _INTERFACE_SIP_ACK_PACKAGE_H_
#define _INTERFACE_SIP_ACK_PACKAGE_H_

#include "ISipObject.h"

/**
 * @brief This class provides an interface to handle the stray ACK requests.
 */
class ISipAckPackage
    : public ISipObject
{
public:
    /**
     * @brief Removes the stray ACK object.
     */
    virtual void RemoveStrayAcks() = 0;
};

#endif // _INTERFACE_SIP_ACK_PACKAGE_H_
