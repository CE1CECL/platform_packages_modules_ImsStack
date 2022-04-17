#ifndef __ISIP_DEFNETWORKUTIL_H__
#define __ISIP_DEFNETWORKUTIL_H__

#include "sip_pf_datatypes.h"
#include "ISipNetworkUtil.h"

class SipDefNetworkUtil: public ISipNetworkUtil
{
    public:
        SipDefNetworkUtil();
        ~SipDefNetworkUtil();
    public:
        /**/
        SIP_BOOL SendToNetwork
            (
             SipTransportBuffer     *pobjTranspSipBuffer,
             SipTransportParameter            *pobjFinalTranspParam,
             ISipUserData            *pobjUserData
            );

        /**/
        SIP_BOOL CheckTCPConnection
            (
             SipTransportParameter            *pobjTransportParam,
             ISipUserData            *pobjUserData
            );

        /**/
        SIP_BOOL AbortTransmission
            (
             SipTransportParameter     *pobjTranspParam,
             ISipUserData        *pobjUserData
            );
};

#endif // __ISIP_DEFNETWORKUTIL_H__
