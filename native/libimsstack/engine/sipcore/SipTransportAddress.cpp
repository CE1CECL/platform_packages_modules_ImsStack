/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100322  hwangoo.park@             Change the class name from SIPTransportTuple
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "SipPrivate.h"
#include "SipTransportAddress.h"

// TODO:: selects a default transport protocol
PUBLIC
SipTransportAddress::SipTransportAddress() :
        nProtocol(PROTOCOL_UDP),
        nPort(Sip::PORT_UNSPECIFIED),
        objIPAddress(IPAddress::NONE)
{
}

PUBLIC
SipTransportAddress::SipTransportAddress(IN CONST SipTransportAddress& objRHS) :
        nProtocol(objRHS.nProtocol),
        nPort(objRHS.nPort),
        objIPAddress(objRHS.objIPAddress)
{
}

PUBLIC
SipTransportAddress::SipTransportAddress(
        IN IMS_SINT32 nProtocol_, IN IMS_SINT32 nPort_, IN CONST AString& strAddress_) :
        nProtocol(nProtocol_),
        nPort(nPort_)
{
    objIPAddress.Parse(strAddress_);
}

PUBLIC
SipTransportAddress::~SipTransportAddress() {}

/*

Remarks

*/
PUBLIC
SipTransportAddress& SipTransportAddress::operator=(IN CONST SipTransportAddress& objRHS)
{
    //---------------------------------------------------------------------------------------------

    if (this != &objRHS)
    {
        nProtocol = objRHS.nProtocol;
        nPort = objRHS.nPort;
        objIPAddress = objRHS.objIPAddress;
    }

    return (*this);
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipTransportAddress::Equals(IN CONST SipTransportAddress& objTA) const
{
    //---------------------------------------------------------------------------------------------

    if (nProtocol != objTA.nProtocol)
        return IMS_FALSE;

    if (nPort != objTA.nPort)
        return IMS_FALSE;

    if (!objIPAddress.Equals(objTA.objIPAddress))
        return IMS_FALSE;

    return IMS_TRUE;
}
