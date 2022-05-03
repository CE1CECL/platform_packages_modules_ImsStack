/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20110528  hwangoo.park@             Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "SipRtConfig.h"



// LogMask class for SIP run-time (or real-time) configuration
PUBLIC
SipRtConfig::LogMask::LogMask()
    : Base()
    , nValue(LogMask::I_NONE)
{
}

PUBLIC
SipRtConfig::LogMask::~LogMask()
{
}



// SocketOption class for SIP run-time (or real-time) configuration
PUBLIC
SipRtConfig::SocketOption::SocketOption()
    : Base()
    , nValue(0)
    , objIP(IPAddress::NONE)
    , nPort(0)
{
}

PUBLIC
SipRtConfig::SocketOption::SocketOption(IN CONST SipRtConfig::SocketOption &objRHS)
    : Base(objRHS)
    , nValue(objRHS.nValue)
    , objIP(objRHS.objIP)
    , nPort(objRHS.nPort)
{
}

PUBLIC VIRTUAL
SipRtConfig::SocketOption::~SocketOption()
{
}

/*

Remarks

*/
PUBLIC
SipRtConfig::SocketOption& SipRtConfig::SocketOption::operator=(
        IN CONST SipRtConfig::SocketOption &objRHS)
{
    //---------------------------------------------------------------------------------------------

    if (this != &objRHS)
    {
        nValue = objRHS.nValue;
        objIP = objRHS.objIP;
        nPort = objRHS.nPort;
    }

    return (*this);
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL SipRtConfig::SocketOption::Equals(IN CONST SipRtConfig::Base &objOther) const
{
    const SipRtConfig::SocketOption *pSO
            = DYNAMIC_CAST(const SipRtConfig::SocketOption*, &objOther);

    //---------------------------------------------------------------------------------------------

    return (pSO != IMS_NULL) ? (objIP.Equals(pSO->objIP) && (nPort == pSO->nPort)) : IMS_FALSE;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipRtConfig::SocketOption::IsGlobalOption() const
{
    //---------------------------------------------------------------------------------------------

    return ((objIP.Equals(IPAddress::NONE) || objIP.Equals(IPAddress::IPv6NONE)) && (nPort == 0));
}



// IpQos class for IP-level QoS configuration
PUBLIC
SipRtConfig::IpQos::IpQos()
    : Base()
    , nValue(0)
    , objIP(IPAddress::NONE)
    , nPort(0)
{
}

PUBLIC
SipRtConfig::IpQos::IpQos(IN CONST SipRtConfig::IpQos &objRHS)
    : Base(objRHS)
    , nValue(objRHS.nValue)
    , objIP(objRHS.objIP)
    , nPort(objRHS.nPort)
{
}

PUBLIC VIRTUAL
SipRtConfig::IpQos::~IpQos()
{
}

/*

Remarks

*/
PUBLIC
SipRtConfig::IpQos& SipRtConfig::IpQos::operator=(IN CONST SipRtConfig::IpQos &objRHS)
{
    //---------------------------------------------------------------------------------------------

    if (this != &objRHS)
    {
        nValue = objRHS.nValue;
        objIP = objRHS.objIP;
        nPort = objRHS.nPort;
    }

    return (*this);
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL SipRtConfig::IpQos::Equals(IN CONST SipRtConfig::Base &objOther) const
{
    const SipRtConfig::IpQos *pIPQoS = DYNAMIC_CAST(const SipRtConfig::IpQos*, &objOther);

    //---------------------------------------------------------------------------------------------

    return (pIPQoS != IMS_NULL) ?
            (objIP.Equals(pIPQoS->objIP) && (nPort == pIPQoS->nPort)) : IMS_FALSE;
}



// Header class for an additional SIP header control
PUBLIC
SipRtConfig::Header::Header()
    : Base()
    , strName(AString::ConstNull())
    , strParameter(AString::ConstNull())
{
}

PUBLIC
SipRtConfig::Header::Header(IN CONST SipRtConfig::Header &objRHS)
    : Base(objRHS)
    , strName(objRHS.strName)
    , strParameter(objRHS.strParameter)
{
}

PUBLIC VIRTUAL
SipRtConfig::Header::~Header()
{
}

/*

Remarks

*/
PUBLIC
SipRtConfig::Header& SipRtConfig::Header::operator=(IN CONST SipRtConfig::Header &objRHS)
{
    //---------------------------------------------------------------------------------------------

    if (this != &objRHS)
    {
        strName = objRHS.strName;
        strParameter = objRHS.strParameter;
    }

    return (*this);
}

/*

Remarks

*/
PUBLIC VIRTUAL
IMS_BOOL SipRtConfig::Header::Equals(IN CONST SipRtConfig::Base &objOther) const
{
    const SipRtConfig::Header *pHeader = DYNAMIC_CAST(const SipRtConfig::Header*, &objOther);

    //---------------------------------------------------------------------------------------------

    if (pHeader == IMS_NULL)
    {
        return IMS_FALSE;
    }

    return strName.EqualsIgnoreCase(pHeader->strName);
}

// IpSecSa class to do the SA's validity check
PUBLIC
SipRtConfig::IpSecSa::IpSecSa()
    : Base()
    , nPortPC(0)
    , nPortPS(0)
    , objIPP(IPAddress::NONE)
    , nPortUC(0)
    , nPortUS(0)
    , objIPU(IPAddress::NONE)
{
}

PUBLIC
SipRtConfig::IpSecSa::IpSecSa(IN CONST SipRtConfig::IpSecSa &objRHS)
    : Base(objRHS)
    , nPortPC(objRHS.nPortPC)
    , nPortPS(objRHS.nPortPS)
    , objIPP(objRHS.objIPP)
    , nPortUC(objRHS.nPortUC)
    , nPortUS(objRHS.nPortUS)
    , objIPU(objRHS.objIPU)
{
}

PUBLIC VIRTUAL
SipRtConfig::IpSecSa::~IpSecSa()
{
}

PUBLIC
SipRtConfig::IpSecSa& SipRtConfig::IpSecSa::operator=(IN CONST SipRtConfig::IpSecSa &objRHS)
{
    //---------------------------------------------------------------------------------------------

    if (this != &objRHS)
    {
        nPortPC = objRHS.nPortPC;
        nPortPS = objRHS.nPortPS;
        objIPP = objRHS.objIPP;
        nPortUC = objRHS.nPortUC;
        nPortUS = objRHS.nPortUS;
        objIPU = objRHS.objIPU;
    }

    return (*this);
}

PUBLIC VIRTUAL
IMS_BOOL SipRtConfig::IpSecSa::Equals(IN CONST SipRtConfig::Base &objOther) const
{
    const SipRtConfig::IpSecSa *pIPSecSA = DYNAMIC_CAST(const SipRtConfig::IpSecSa*, &objOther);

    //---------------------------------------------------------------------------------------------

    if (pIPSecSA == IMS_NULL)
    {
        return IMS_FALSE;
    }

    return (nPortPC == pIPSecSA->nPortPC)
            && (nPortPS == pIPSecSA->nPortPS)
            && objIPP.Equals(pIPSecSA->objIPP)
            && (nPortUC == pIPSecSA->nPortUC)
            && (nPortUS == pIPSecSA->nPortUS)
            && objIPU.Equals(pIPSecSA->objIPU);
}

PUBLIC
IMS_BOOL SipRtConfig::IpSecSa::IsEmpty() const
{
    //---------------------------------------------------------------------------------------------

    return (nPortPC == 0) && (nPortPS == 0) && (nPortUC == 0) && (nPortUS == 0);
}

// TcpPortRange class to provision TCP port range
PUBLIC
SipRtConfig::TcpPortRange::TcpPortRange()
    : Base()
    , nPortStart(0)
    , nPortEnd(0)
{
}

PUBLIC
SipRtConfig::TcpPortRange::TcpPortRange(IN CONST SipRtConfig::TcpPortRange &objRHS)
    : Base(objRHS)
    , nPortStart(objRHS.nPortStart)
    , nPortEnd(objRHS.nPortEnd)
{
}

PUBLIC VIRTUAL
SipRtConfig::TcpPortRange::~TcpPortRange()
{
}

PUBLIC
SipRtConfig::TcpPortRange& SipRtConfig::TcpPortRange::operator=(
        IN CONST SipRtConfig::TcpPortRange &objRHS)
{
    //---------------------------------------------------------------------------------------------

    if (this != &objRHS)
    {
        nPortStart = objRHS.nPortStart;
        nPortEnd = objRHS.nPortEnd;
    }

    return (*this);
}

PUBLIC VIRTUAL
IMS_BOOL SipRtConfig::TcpPortRange::Equals(IN CONST SipRtConfig::Base &objOther) const
{
    const SipRtConfig::TcpPortRange *pPortRange
            = DYNAMIC_CAST(const SipRtConfig::TcpPortRange*, &objOther);

    //---------------------------------------------------------------------------------------------

    if (pPortRange == IMS_NULL)
    {
        return IMS_FALSE;
    }

    return (nPortStart == pPortRange->nPortStart)
            && (nPortEnd == pPortRange->nPortEnd);
}

// RegContactAddress class to provision RegContact's information
PUBLIC
SipRtConfig::RegContactAddress::RegContactAddress()
    : Base()
    , strCallId(AString::ConstNull())
    , objUri(SipAddress::ConstNull())
{
}

PUBLIC
SipRtConfig::RegContactAddress::RegContactAddress(IN CONST SipRtConfig::RegContactAddress &objRHS)
    : Base(objRHS)
    , strCallId(objRHS.strCallId)
    , objUri(objRHS.objUri)
{
}

PUBLIC VIRTUAL
SipRtConfig::RegContactAddress::~RegContactAddress()
{
}

PUBLIC
SipRtConfig::RegContactAddress& SipRtConfig::RegContactAddress::operator=(
        IN CONST SipRtConfig::RegContactAddress &objRHS)
{
    //---------------------------------------------------------------------------------------------

    if (this != &objRHS)
    {
        strCallId = objRHS.strCallId;
        objUri = objRHS.objUri;
    }

    return (*this);
}

PUBLIC VIRTUAL
IMS_BOOL SipRtConfig::RegContactAddress::Equals(IN CONST SipRtConfig::Base &objOther) const
{
    const SipRtConfig::RegContactAddress *pRegContactA
            = DYNAMIC_CAST(const SipRtConfig::RegContactAddress*, &objOther);

    //---------------------------------------------------------------------------------------------

    if (pRegContactA == IMS_NULL)
    {
        return IMS_FALSE;
    }

    return strCallId.Equals(pRegContactA->strCallId);
}
