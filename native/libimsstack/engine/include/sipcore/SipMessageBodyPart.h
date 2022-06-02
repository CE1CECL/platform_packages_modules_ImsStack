/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#ifndef _SIP_MESSAGE_BODY_PART_H_
#define _SIP_MESSAGE_BODY_PART_H_

#include "ISipMessageBodyPart.h"
#include "SipStackHeaders.h"
#include "SipUnknownHeaders.h"

class SipMessageBodyPart : public ISipMessageBodyPart
{
public:
    SipMessageBodyPart(IN IMS_BOOL bSDPBody_ = IMS_FALSE);
    SipMessageBodyPart(IN SipMsgBody* pstMsgBody_, IN IMS_BOOL bSDPBody_ = IMS_FALSE);
    virtual ~SipMessageBodyPart();

private:
    SipMessageBodyPart(IN CONST SipMessageBodyPart& objRHS);

public:
    SipMessageBodyPart& operator=(IN CONST SipMessageBodyPart& objRHS);

public:
    // ISipObject interface
    virtual void Destroy();
    // ISipMessageBodyPart interface
    virtual ISipMessageBodyPart* Clone() const;
    virtual void CopyFrom(IN CONST ISipMessageBodyPart* piBodyPart);
    virtual AString GetHeader(
            IN IMS_SINT32 nType, IN CONST AString& strName = AString::ConstNull()) const;
    virtual void SetHeader(IN IMS_SINT32 nType, IN CONST AString& strValue,
            IN CONST AString& strName = AString::ConstNull());
    inline virtual const ByteArray& GetContent() const { return objContent; }
    virtual void SetContent(IN CONST ByteArray& objContent);

    IMS_BOOL FormMessageBody();
    void SetHeader(IN SipHeaderBase* pstHeader,
            IN IMS_SINT32 nType = ISipMessageBodyPart::CONTENT_UNKNOWN);
    inline SipMsgBody* GetMessageBody() const { return pstMsgBody; }
    inline IMS_BOOL IsSdpBodyPart() const { return bSDPBody; }

private:
    IMS_BOOL ExtractProperties();

private:
    IMS_BOOL bSDPBody;
    SipMsgBody* pstMsgBody;
    SipUnknownHeaders objOtherMimeHeaders;

    ByteArray objContent;
};

#endif  // _SIP_MESSAGE_BODY_PART_H_
