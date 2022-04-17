#ifndef __SIP_TRANSPORT_PARAMETER_H__
#define __SIP_TRANSPORT_PARAMETER_H__

#include "sip_pf_datatypes.h"

class SipTransportParameter
{
    private:
        /* host =  hostname/IPv4address /IPv6reference */
        SIP_CHAR    *m_pucHostAddress;
        SIP_UINT16    m_usPort;
        SIP_INT32  m_usTranspProtocol;
        SIP_UINT32    m_uiSockId;
        /* Enable = SIP_TRUE and for Disable= SIP_FALSE */
        SIP_BOOL     m_bMulticastEnable;
        /* Time to Live : Value starts from 0-255 only */
        SIP_CHAR    m_cTTL;

        SIP_INT32   m_usTranspIpType;

        SipTransportParameter& operator=(IN const SipTransportParameter &objRHS);
        SipTransportParameter(IN const SipTransportParameter &objRHS);

    public:

        SipTransportParameter()
            : m_pucHostAddress(SIP_NULL)
            , m_usPort(SIP_ZERO)
            , m_usTranspProtocol(SIP_INVALID)
            , m_uiSockId(SIP_ZERO)
            , m_bMulticastEnable(SIP_FALSE)
            , m_cTTL(SIP_ZERO)
            , m_usTranspIpType(SIP_INVALID)
        {}
        SipTransportParameter(SipTransportParameter* pobjTranspParam);
        SipTransportParameter(SIP_CHAR* pucHostAddress,
                SIP_UINT16 usPort, SIP_INT32 eTranspProtocol);
        SipTransportParameter(SIP_CHAR* pucHostAddress,
                SIP_UINT16 usPort, SIP_INT32 eTranspProtocol, SIP_UINT32 uiSockId);
        virtual ~SipTransportParameter()
        {
            if (m_pucHostAddress != SIP_NULL)
            {
                delete[] m_pucHostAddress;
            }
        }
        SIP_BOOL GetMulticastEnable() const
        {
            return m_bMulticastEnable;
        }

        SIP_CHAR GetCTTL() const
        {
            return m_cTTL;
        }

        SIP_INT32 GetTranspIpType() const
        {
            return m_usTranspIpType;
        }

        SIP_INT32 GetTranspProtocol() const
        {
            return m_usTranspProtocol;
        }

        SIP_CHAR *GetHostAddress() const
        {
            return m_pucHostAddress;
        }

        SIP_UINT32 GetSockId() const
        {
            return m_uiSockId;
        }

        SIP_UINT16 GetPort() const
        {
            return m_usPort;
        }

        SIP_VOID setMulticastEnable(SIP_BOOL bMulticastEnable)
        {
            this->m_bMulticastEnable = bMulticastEnable;
        }

        SIP_VOID setCTTL(SIP_CHAR cTTL)
        {
            this->m_cTTL = cTTL;
        }

        SIP_VOID setTanspIpType(SIP_INT32 uiTanspIpType)
        {
            this->m_usTranspIpType = uiTanspIpType;
        }

        SIP_VOID setTranspProtocol(SIP_INT32 uiTranspProtocol)
        {
            this->m_usTranspProtocol = uiTranspProtocol;
        }

        SIP_BOOL setHostAddress(const SIP_CHAR* pucHostAddress);

        SIP_VOID setSockId(SIP_UINT32 uiSockId)
        {
            this->m_uiSockId = uiSockId;
        }

        SIP_VOID setPort(SIP_UINT16 usPort)
        {
            this->m_usPort = usPort;
        }
};

#endif //__SIP_TRANSPORT_PARAMETER_H__
