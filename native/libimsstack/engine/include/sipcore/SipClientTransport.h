/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#ifndef _SIP_CLIENT_TRANSPORT_H_
#define _SIP_CLIENT_TRANSPORT_H_

#include "SipTransport.h"

class SipClientTransport : public SipTransport
{
public:
    explicit SipClientTransport(IN IMS_SINT32 nSlotId);
    virtual ~SipClientTransport();

private:
    SipClientTransport(IN CONST SipClientTransport& objRHS);

public:
    // MULTI_REG_SIP_PROFILE
    virtual IMS_BOOL FormViaHeader(
            IN_OUT ::SipMessage*& pstMessage, IN CONST SipProfile* pSIPProfile = IMS_NULL);
    // MULTI_REG_SIP_PROFILE
    virtual IMS_BOOL ReserveResource(IN CONST SipProfile* pSIPProfile = IMS_NULL);
    virtual IMS_BOOL UpdateDestinationInfo(IN ::SipMessage* pstMessage,
            IN IMS_BOOL bRoutingLR = IMS_TRUE, IN SipAddrSpec* pstImplicitRoute = IMS_NULL);
    virtual IMS_SINT32 ValidateViaHeader(IN ::SipMessage* pstMessage);

    void SetExtensionTokenForViaBranch(IN CONST AString& strToken);

protected:
    // ISipSocketListener interface
    virtual void Socket_NotifyError(IN SipSocket* pSocket, IN IMS_SINT32 nErrorCode);

private:
    static IMS_BOOL IsSameHostAndPort(IN SipAddrSpec* pstAddrSpec1, IN SipAddrSpec* pstAddrSpec2);

private:
    SipSocket* pServerSocket;

    // Extension for branch parameter in the Via header
    AString strExtensionTokenForViaBranch;
};

#endif  // _SIP_CLIENT_TRANSPORT_H_
