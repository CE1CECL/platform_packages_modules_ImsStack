#ifndef CALLER_INFO_H_
#define CALLER_INFO_H_

#include "AString.h"
#include "IMSTypeDef.h"
#include "call/message/IMtcMessageHandler.h"

class IMessage;
class IMtcCallContext;

enum class OipType
{
    NONE        = 0,
    IDENTITY    = 1,
    RESTRICTED  = 2,
};

/**
 * This class stores caller and callee info.
 */
class ParticipantInfo final :
        public IMtcMessageHandler
{
public:
    explicit ParticipantInfo(IN IMtcCallContext& objContext);
    ~ParticipantInfo();
    ParticipantInfo(IN const ParticipantInfo&) = delete;
    ParticipantInfo& operator=(IN const ParticipantInfo&) = delete;

    // AString GetLocalNumber() const;
    AString GetLocalUri() const;
    AString GetRemoteUri() const;
    AString GetRemoteDisplayName() const;
    OipType GetOipType() const;

    void SetRemoteNumber(IN const AString& strRemoteNumber);
    void SetRemoteUri(IN const AString& strRemoteUri);

    void FormatRequest(IN IMS_UINT32 eMethod, IN_OUT IMessage& objRequest) override;
    void FormatResponse(IN IMS_UINT32 eMethod, IN_OUT IMessage& objResponse) override;
    void HandleRequest(IN IMS_UINT32 eMethod, IN const IMessage& objRequest) override;
    void HandleResponse(IN IMS_UINT32 eMethod, IN const IMessage& objResponse) override;

private:
    static const AString URI_SET_BY_IMS_ENGINE;
    static const AString ANONYMOUS_ADDRESS;
    static const AString ANONYMOUS_DISPLAY_NAME;

    AString GetLocalUriForEmergencyCall() const;
    AString GetRemoteUriForConferenceCall() const;
    OipType GetOipTypeFrom(IN const IMessage& objMessage) const;

    AString GetMcc() const;
    AString GetMnc(IN IMS_UINT32 nLength) const;

    AString m_strRemoteUri;
    AString m_strRemoteDisplayName;
    OipType m_eOipType;

    IMtcCallContext& m_objContext;
};

#endif
