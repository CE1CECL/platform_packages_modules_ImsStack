#include "platform/sip_pf_string.h"
#include "transport/SipTransportParameter.h"

SipTransportParameter::SipTransportParameter(SipTransportParameter* pobjTranspParam)
    : m_pucHostAddress(SIP_NULL)
    , m_usPort(SIP_ZERO)
    , m_usTranspProtocol(SIP_INVALID)
    , m_uiSockId(SIP_ZERO)
    , m_bMulticastEnable(SIP_FALSE)
    , m_cTTL(SIP_ZERO)
    , m_usTranspIpType(SIP_INVALID)
{
    if (pobjTranspParam == SIP_NULL){
        return;
    }

    m_pucHostAddress = SipPf_Strdup(pobjTranspParam->m_pucHostAddress);
    m_usPort = pobjTranspParam->m_usPort;
    m_usTranspProtocol = pobjTranspParam->m_usTranspProtocol;
    m_uiSockId = pobjTranspParam->m_uiSockId;
    m_bMulticastEnable= pobjTranspParam->m_bMulticastEnable;
    m_cTTL = pobjTranspParam->m_cTTL;
    m_usTranspIpType = pobjTranspParam->m_usTranspIpType ;
}

SipTransportParameter::SipTransportParameter(SIP_CHAR* pucHostAddress,
        SIP_UINT16 usPort, SIP_INT32 eTranspProtocol)
    : m_pucHostAddress(SIP_NULL)
    , m_usPort(usPort)
    , m_usTranspProtocol(eTranspProtocol)
    , m_uiSockId(SIP_ZERO)
    , m_bMulticastEnable(SIP_FALSE)
    , m_cTTL(SIP_ZERO)
    , m_usTranspIpType(SIP_INVALID)
{
    m_pucHostAddress = SipPf_Strdup(pucHostAddress);
}

SipTransportParameter::SipTransportParameter(SIP_CHAR* pucHostAddress,
        SIP_UINT16 usPort, SIP_INT32 eTranspProtocol, SIP_UINT32 uiSockId)
    : m_pucHostAddress(SIP_NULL)
    , m_usPort(usPort)
    , m_usTranspProtocol(eTranspProtocol)
    , m_uiSockId(uiSockId)
    , m_bMulticastEnable(SIP_FALSE)
    , m_cTTL(SIP_ZERO)
    , m_usTranspIpType(SIP_INVALID)
{
    m_pucHostAddress = SipPf_Strdup(pucHostAddress);
}

SIP_BOOL SipTransportParameter::setHostAddress(const SIP_CHAR* pucHostAddress)
{
    if (m_pucHostAddress != SIP_NULL)
    {
        delete[] m_pucHostAddress;
        m_pucHostAddress = SIP_NULL;
    }

    m_pucHostAddress = SipPf_Strdup(pucHostAddress);

    return SIP_TRUE;
}
