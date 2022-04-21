#ifndef CALLER_INFO_H_
#define CALLER_INFO_H_

#include "AString.h"
#include "IMSTypeDef.h"
#include "helper/MtcSupplementaryService.h"

class IMessage;
class IMtcCallContext;

/**
 * This class stores caller and callee info.
 */
class ParticipantInfo final
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

private:
    static const AString URI_SET_BY_IMS_ENGINE;
    static const AString ANONYMOUS_ADDRESS;
    static const AString ANONYMOUS_DISPLAY_NAME;

    AString GetLocalUriForEmergencyCall() const;
    AString GetRemoteUriForConferenceCall() const;

    AString GetMcc() const;
    AString GetMnc(IN IMS_UINT32 nLength) const;

    AString m_strRemoteUri;
    AString m_strRemoteDisplayName;

    IMtcCallContext& m_objContext;
};

#endif
