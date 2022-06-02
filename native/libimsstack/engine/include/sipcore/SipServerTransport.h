/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#ifndef _SIP_SERVER_TRANSPORT_H_
#define _SIP_SERVER_TRANSPORT_H_

#include "SipTransport.h"

class SipServerTransport : public SipTransport
{
public:
    SipServerTransport(IN IMS_SINT32 nSlotId, IN CONST SipTransportAddress& objTA_NearEnd_,
            IN CONST SipTransportAddress& objTA_FarEnd_);
    virtual ~SipServerTransport();

private:
    SipServerTransport(IN CONST SipServerTransport& objRHS);
    SipServerTransport& operator=(IN CONST SipServerTransport& objRHS);

public:
    // MULTI_REG_SIP_PROFILE
    virtual IMS_BOOL FormViaHeader(
            IN_OUT ::SipMessage*& pstMessage, IN CONST SipProfile* pSIPProfile = IMS_NULL);
    virtual IMS_BOOL UpdateDestinationInfo(IN ::SipMessage* pstMessage,
            IN IMS_BOOL bRoutingLR = IMS_TRUE, IN SipAddrSpec* pstImplicitRoute = IMS_NULL);
    virtual IMS_SINT32 ValidateViaHeader(IN ::SipMessage* pstMessage);
};

#endif  // _SIP_SERVER_TRANSPORT_H_
