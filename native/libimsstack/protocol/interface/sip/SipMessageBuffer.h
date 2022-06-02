#ifndef _SIP_MESSAGE_BUFFER_H_
#define _SIP_MESSAGE_BUFFER_H_

#include "RCObject.h"

class SipMessageBuffer : public RCObject
{
public:
    SipMessageBuffer();
    SipMessageBuffer(IN const SipMessageBuffer& objRHS);
    virtual ~SipMessageBuffer();

private:
    SipMessageBuffer& operator=(IN const SipMessageBuffer& objRHS);

public:
    IMS_BYTE* GetBuffer();
    IMS_BYTE* GetBuffer(IN IMS_SINT32 nSlotId);
    /*
     Returns a maximum buffer length to form a SIP message
    */
    inline IMS_SINT32 GetLength() const { return MAX_MSG_SIZE; }

    static RCPtr<SipMessageBuffer> GetInstance();

public:
    // Max buffer size for raw SIP message
    enum
    {
        MAX_MSG_SIZE = 65535
    };

    // Variable for a temporary message storage to form a SIP message
    IMS_BYTE MESSAGE[MAX_MSG_SIZE];
    // Buffer for dual VoLTE
    IMS_BYTE** ppBuffer;
};

#endif  // _SIP_MESSAGE_BUFFER_H_
