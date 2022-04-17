#ifndef MERGE_CONTROLLER_H_
#define MERGE_CONTROLLER_H_

#include "conferencecall/ConferenceController.h"
#include "IMSList.h"

class IMtcCallContext;
class MediaInfo;
class SuppService;
class ConfUser;
struct CallInfo;
struct CallStartOperationParams;

class MergeController final :
        public ConferenceController
{
public:
    explicit MergeController(IN IMtcCallContext& objContext);
    virtual ~MergeController();
    MergeController(IN const MergeController&) = delete;
    MergeController& operator=(IN const MergeController&) = delete;

protected:
    void ProcessMerge(IN IMSList<ConfUser*>& objUsers, IN CallInfo& objCallInfo,
            IN MediaInfo& objMediaInfo,
            IN IMSMap<IMS_UINT32, SuppService*>& objSuppServices) override;
    void StartConferenceCall(IN CallStartOperationParams* pParams) override;
    IMS_BOOL IsStartFinalSipfragWaitTimer() const override;
    void Recover() override;
    void OnIndividualCallTerminated(IN IMS_UINTP nCallKey) override;

private:
    void ProcessMergeWithoutRefer(IN IMSList<ConfUser*>& objUsers, IN CallInfo& objCallInfo,
            IN MediaInfo& objMediaInfo, IN IMSMap<IMS_UINT32, SuppService*>& objSuppServices);
    void UpdateUserStateBySessionTerminated(IN IMS_UINTP nCallKey);
    void RecoverOnCreating();
    void RecoverOnReferring();
    void RecoverOnSubscribing();
    IMS_BOOL RecoverOnConferenceCallFailed();
    void ClearIndividualCallOnMergeFailed();
};

#endif
