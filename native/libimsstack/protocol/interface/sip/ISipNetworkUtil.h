#ifndef __ISIPNETWORKUTIL_H__
#define __ISIPNETWORKUTIL_H__

#include "sip_pf_datatypes.h"
#include "ISipUserData.h"
#include "transport/SipTransportBuffer.h"
#include "transport/SipTransportParameter.h"

class ISipNetworkUtil
{
    public:

        ISipNetworkUtil(){};
        virtual ~ISipNetworkUtil(){};

        virtual SIP_BOOL SendToNetwork
            (
             SipTransportBuffer    *pobjTranspSipBuffer,
             SipTransportParameter        *pobjFinalTranspParam,
             ISipUserData        *pobjUserData
            )=0;

        virtual SIP_BOOL CheckTCPConnection
            (
             SipTransportParameter        *pobjTransportParam,
             ISipUserData        *pobjUserData
            )=0;

        virtual SIP_BOOL AbortTransmission
            (
             SipTransportParameter     *pobjTranspParam,
             ISipUserData        *pobjUserData
            )=0;
};

#endif // __ISIPNETWORKUTIL_H__
