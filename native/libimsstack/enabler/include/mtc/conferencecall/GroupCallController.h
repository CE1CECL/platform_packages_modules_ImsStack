#ifndef GROUP_CALL_CONTROLLER_H_
#define GROUP_CALL_CONTROLLER_H_

#include "conferencecall/ConferenceController.h"
#include "IMSList.h"

class IMtcCallContext;
class IConferenceReference;
class ConfUser;
class MediaInfo;
class SuppService;
struct CallInfo;
struct CallStartOperationParams;

class GroupCallController final :
        public ConferenceController
{
public:
    explicit GroupCallController(IN IMtcCallContext& objContext);
    virtual ~GroupCallController();
    GroupCallController(IN const GroupCallController&) = delete;
    GroupCallController& operator=(IN const GroupCallController&) = delete;

public:
    void OnReferenceStartFailed(IN IConferenceReference* piConfRef) override;

protected:
    void ProcessGroupCall(IN IMSList<ConfUser*>& objUsers, IN CallInfo& objCallInfo,
            IN MediaInfo& objMediaInfo,
            IN IMSMap<IMS_UINT32, SuppService*>& objSuppServices) override;
    void StartConferenceCall(IN CallStartOperationParams* pParams) override;
    void Recover() override;

private:
    void RecoverOnCreating();
    void RecoverOnReferring();
};

#endif
