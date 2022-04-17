#ifndef CONFERENCE_MANAGER_H_
#define CONFERENCE_MANAGER_H_

#include "IMSMap.h"
#include "conferencecall/IConferenceControllerListener.h"
#include "conferencecall/ConferenceController.h"
#include "call/IMtcCallManager.h"
#include "helper/ObjectAsyncDestroyer.h"
#include "conferencecall/ConferenceDef.h"
#include "call/IMtcCallContext.h"

enum class ConferenceType;
enum class IndividualSessionState;

class ConferenceManager final :
        public IConferenceControllerListener
{
public:
    explicit ConferenceManager();
    ~ConferenceManager();
    ConferenceManager(IN const ConferenceManager&) = delete;
    ConferenceManager& operator=(IN const ConferenceManager&) = delete;

    // IConferenceControllerListener interface implementation
    void OnClosed(IN ConferenceController* pController) override;

    // TODO: when this is called, there might be no IMtcCall? when no callContext available.
    IConferenceController& CreateController(IN IMtcCallContext& objContext,
            IN ConferenceType eType);
    IConferenceController* GetController(IN IMS_UINTP nCallKey) const;

private:
    void ReleaseController(IN ConferenceController* pController);

private:
    IMSMap<CallKey, ConferenceController*> m_objControllers;
    ObjectAsyncDestroyer<ConferenceController> m_objDestroyer;
};

enum class ConferenceType
{
    PARTICIPANT,
    GROUP_CALL, // aka. start conference
    MERGE_CALL,
    EXPAND_CALL
};

enum class IndividualSessionState
{
    IDLE,
    JOINING,
    JOINED,
    INVITED
};

#endif
