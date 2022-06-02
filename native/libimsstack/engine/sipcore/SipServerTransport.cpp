/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "ISipHeader.h"
#include "SipPrivate.h"
#include "SipDebug.h"
#include "SipAddress.h"
#include "SipServerTransport.h"

__IMS_TRACE_TAG_SIP__;

PUBLIC
SipServerTransport::SipServerTransport(IN IMS_SINT32 nSlotId,
        IN CONST SipTransportAddress& objTA_NearEnd_, IN CONST SipTransportAddress& objTA_FarEnd_) :
        SipTransport(nSlotId, SipTransport::TYPE_SERVER)
{
    SetAddress(objTA_NearEnd_, TA_NEAR);
    SetAddress(objTA_FarEnd_, TA_FAR);

    // Only for TCP client socket
    // The destination information (address / port) will be updated before sending the response,
    // so if the request is received from TCP connection, it needs to be stored
    // in the transport layer.
    // If the socket is already reserved, the transport layer will use the reserved socket.
    StoreResource();
}

PUBLIC VIRTUAL SipServerTransport::~SipServerTransport()
{
#ifdef __IMS_SIP_DEBUG__
    IMS_TRACE_D("Destructor :: SipServerTransport", 0, 0, 0);
#endif
}

/*

Remarks
 MULTI_REG_SIP_PROFILE
*/
PUBLIC VIRTUAL IMS_BOOL SipServerTransport::FormViaHeader(
        IN_OUT ::SipMessage*& pstMessage, IN CONST SipProfile* /*pSIPProfile = IMS_NULL*/)
{
    // Get the topmost Via header from the message
    SipHeaderBase* pstViaHeader = SipStack::GetHeader(pstMessage, ISipHeader::VIA);
    AString strRPort(Sip::STR_RPORT);
    const SipTransportAddress& objFarEnd = GetAddress(TA_FAR);

    //---------------------------------------------------------------------------------------------

    // Check if the Via header includes a "rport" parameter or not.
    if (SipStack::HasParameter(pstViaHeader, strRPort))
    {
        // Set the actual received port number from the current transport information
        // "rport" parameter : UDP & unprotected port only
        IMS_SINT32 nPortC = GetPortC();
        AString strSentProtocol = SipStack::GetSentProtocolFromVia(pstViaHeader);

        if (strSentProtocol.Contains(Sip::STR_UDP_CAPS) &&
                ((nPortC == 0) || (nPortC == Sip::PORT_UNSPECIFIED) ||
                        (GetPort(TA_NEAR) == nPortC)))
        {
            AString strPort;

            strPort.SetNumber(objFarEnd.GetPort());

            if (!SipStack::SetParameter(pstViaHeader, strRPort, strPort))
            {
                SipStack::FreeHeaderEx(pstViaHeader);
                return IMS_FALSE;
            }
        }
    }

    // Examine the value of the "sent-by" parameter in the top Via header field value.
    // If the host portion of the "sent-by" parameter contains a domain name, or
    // if it contains an IP address that differs from the packet source address,
    // the server MUST add a "received" parameter to that Via header field value.
    AString strHost;
    AString strSentBy = SipStack::GetSentByFromVia(pstViaHeader);
    IMS_SINT32 nPort = Sip::PORT_UNSPECIFIED;

    // Parse the host & port information from "sent-by" field
    SipTransport::ParseHostNPort(strSentBy, strHost, nPort);

    IPAddress objHost;
    IMS_BOOL bNumericIPFormat = objHost.Parse(strHost);

    // Check if the actual received IP & the host in "sent-by" field are same
    if (!bNumericIPFormat || !objHost.Equals(objFarEnd.GetIpAddress()))
    {
        // The address returned by recvfrom is not same as address put in the Via header.
        // Add the "received" parameter in Via header.
        if (!SipStack::SetParameter(
                    pstViaHeader, AString(Sip::STR_RECEIVED), objFarEnd.GetIpAddress().ToString()))
        {
            SipStack::FreeHeaderEx(pstViaHeader);
            return IMS_FALSE;
        }
    }

    SipStack::FreeHeaderEx(pstViaHeader);

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_BOOL SipServerTransport::UpdateDestinationInfo(IN ::SipMessage* pstMessage,
        IN IMS_BOOL /* bRoutingLR = IMS_TRUE */, IN SipAddrSpec* /* pstImplicitRoute = IMS_NULL */)
{
    // For a response which is being sent out, updating the destination info. is done as follows:
    //
    // 1. If the Via header contains a "maddr" parameter, send the response to the IP address
    //   it contains and to the port specified in the "sent-by" field of the topmost Via header.
    // 2. If "maddr" is absent, but the "received" parameter is present,
    //   then send the response to the port indicated in the "sent-by" field of the topmost
    //   Via header (if none given, then a default port is 5060).
    // 3. If neither "maddr" nor "received" parameter are present, the destination info. is set
    //   to the address / port of the topmost Via header.
    SipHeaderBase* pstViaHeader = SipStack::GetHeader(pstMessage, ISipHeader::VIA);
    AString strSentBy = SipStack::GetSentByFromVia(pstViaHeader);
    AString strViaHost;
    IMS_SINT32 nViaPort;

    //---------------------------------------------------------------------------------------------

    // Parse the host & port information from "sent-by" field
    SipTransport::ParseHostNPort(strSentBy, strViaHost, nViaPort);

    SetIpAddress(IPAddress(strViaHost), TA_FAR);
    SetPort(nViaPort, TA_FAR);

    // Now, check if "maddr" & "received" parameter are present
    AString strTemp = SipStack::GetParameter(pstViaHeader, AString(SipAddress::PARAM_MADDR));

    if (strTemp.GetLength() > 0)
    {
        // If "maddr" parameter is found, it is the highest precedence.
        // So, update the destination address as this value.
        SetIpAddress(IPAddress(strTemp), TA_FAR);
    }
    else
    {
        // Find the "received" parameter
        strTemp = SipStack::GetParameter(pstViaHeader, AString(Sip::STR_RECEIVED));

        if (strTemp.GetLength() > 0)
        {
            SetIpAddress(IPAddress(strTemp), TA_FAR);
        }
    }

    // Check if "rport" parameter is present or not
    strTemp = SipStack::GetParameter(pstViaHeader, AString(Sip::STR_RPORT));

    if (strTemp.GetLength() > 0)
    {
        IMS_SINT32 nRPort = strTemp.ToInt32();

        if (nRPort != 0)
            SetPort(nRPort, TA_FAR);
    }

    // Update the destination transport protocol from "sent-protocol" field in the top Via header
    strTemp = SipStack::GetSentProtocolFromVia(pstViaHeader);

    // "SIP/2.0/" : TCP / UDP / TLS / SCTP
    if (strTemp.EndsWith(Sip::STR_TLS_CAPS))
    {
        SetProtocol(SipTransportAddress::PROTOCOL_TLS, TA_FAR);

        // If the port number does not set explicitly,
        // then check the transport protocol & set the port.
        if (GetPort(TA_FAR) == Sip::PORT_UNSPECIFIED)
            SetPort(Sip::PORT_5061, TA_FAR);
    }
    else if (strTemp.EndsWith(Sip::STR_TCP_CAPS))
    {
        SetProtocol(SipTransportAddress::PROTOCOL_TCP, TA_FAR);

        // If the port number does not set explicitly,
        // then check the transport protocol & set the port.
        if (GetPort(TA_FAR) == Sip::PORT_UNSPECIFIED)
            SetPort(Sip::PORT_5060, TA_FAR);
    }
    else
    {
        SetProtocol(SipTransportAddress::PROTOCOL_UDP, TA_FAR);

        // If the port number does not set explicitly,
        // then check the transport protocol & set the port.
        if (GetPort(TA_FAR) == Sip::PORT_UNSPECIFIED)
            SetPort(Sip::PORT_5060, TA_FAR);
    }

    SipStack::FreeHeaderEx(pstViaHeader);

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_SINT32 SipServerTransport::ValidateViaHeader(IN ::SipMessage* pstMessage)
{
    // Get the topmost Via header from the message
    SipHeaderBase* pstViaHeader = SipStack::GetHeader(pstMessage, ISipHeader::VIA);

    //---------------------------------------------------------------------------------------------

    // Check if there is a mismatch between the protocol in the topmost Via
    // and the actual protocol on which it was received.
    //
    // If the topmost Via header has scheme SIP/2.0/TCP, but actually came on UDP,
    // (or vice versa) flag off error. Application SHOULD respond to this with 400 "Bad Request".
    const SipTransportAddress& objNearEnd = GetAddress(TA_NEAR);
    AString strSentProtocol = SipStack::GetSentProtocolFromVia(pstViaHeader);

    // "SIP/2.0/" : 8 characters
    strSentProtocol = strSentProtocol.Mid(8);

    if ((strSentProtocol.EqualsIgnoreCase(Sip::STR_TCP_CAPS) &&
                (objNearEnd.GetProtocol() != SipTransportAddress::PROTOCOL_TCP)) ||
            (strSentProtocol.EqualsIgnoreCase(Sip::STR_TLS_CAPS) &&
                    (objNearEnd.GetProtocol() != SipTransportAddress::PROTOCOL_TLS)) ||
            (strSentProtocol.EqualsIgnoreCase(Sip::STR_UDP_CAPS) &&
                    (objNearEnd.GetProtocol() != SipTransportAddress::PROTOCOL_UDP)))
    {
        // Correct the sent-protocol in Via header
        strSentProtocol = SipStack::GetSentProtocolFromVia(pstViaHeader);

        // "SIP/2.0/" : 8 characters
        strSentProtocol = strSentProtocol.Left(8);

        if (objNearEnd.GetProtocol() == SipTransportAddress::PROTOCOL_TCP)
            strSentProtocol += Sip::STR_TCP_CAPS;
        else if (objNearEnd.GetProtocol() == SipTransportAddress::PROTOCOL_TLS)
            strSentProtocol += Sip::STR_TLS_CAPS;
        else
            strSentProtocol += Sip::STR_UDP_CAPS;

        SipStack::UpdateSentProtocol(pstMessage, strSentProtocol);
        SipStack::FreeHeaderEx(pstViaHeader);

        IMS_TRACE_E(0, "Transport protocol is mismatched in Via header", 0, 0, 0);

        SipPrivate::SetLastError(SipError::VIA_PROTOCOL_MISMATCH);

        return SipPrivate::MESSAGE_INVALID_400;
    }

    // Free the local reference
    SipStack::FreeHeaderEx(pstViaHeader);

    return SipPrivate::MESSAGE_VALID;
}
