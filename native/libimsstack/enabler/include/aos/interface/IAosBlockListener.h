#ifndef _INTERFACE_AOS_BLOCK_LISTENER_H_
#define _INTERFACE_AOS_BLOCK_LISTENER_H_

#include "IMSTypeDef.h"

class IAosBlockListener
{
public:
    virtual void Block_Changed(IN IMS_UINT32 nType = 0, IN IMS_UINT32 nParam = 0) = 0;
};
#endif // _INTERFACE_AOS_BLOCK_LISTENER_H_
