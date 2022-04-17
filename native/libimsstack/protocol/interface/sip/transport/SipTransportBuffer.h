#ifndef __SIP_TRANSPORT_BUFFER_H__
#define __SIP_TRANSPORT_BUFFER_H__

#include "sip_pf_datatypes.h"

class SipTransportBuffer
{

    /* Raw Message */
    SIP_CHAR    *m_pucSipBuffer;
    SIP_UINT32    m_usSipBufferLen;

    SipTransportBuffer& operator=(IN const SipTransportBuffer &objRHS);
    SipTransportBuffer(IN const SipTransportBuffer &objRHS);

    public:

    SipTransportBuffer()
        : m_pucSipBuffer(SIP_NULL)
        , m_usSipBufferLen(SIP_ZERO)
    {}
    SipTransportBuffer(SIP_CHAR* pucSipBuffer, SIP_UINT32 usSipBufferLen)
        : m_pucSipBuffer(pucSipBuffer)
        , m_usSipBufferLen(usSipBufferLen)
    {}
    /* Free Message Buffer */
    virtual ~SipTransportBuffer()
    {
        if (m_pucSipBuffer != SIP_NULL)
        {
            delete[] m_pucSipBuffer;
        }
    }
    SIP_CHAR* GetSipBuffer() const {
        return m_pucSipBuffer;
    }
    SIP_UINT32 GetSipBufferLen() const{
        return m_usSipBufferLen;
    }
};

#endif //__SIP_TRANSPORT_BUFFER_H__
