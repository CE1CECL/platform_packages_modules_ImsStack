#ifndef INTERFACE_CONFERENCE_CONTROLLER_H_
#define INTERFACE_CONFERENCE_CONTROLLER_H_

class ConferenceParticipantList;

class IConferenceController
{
public:
    virtual ~IConferenceController() {}
    virtual void ProcessCommand(IN IMS_UINT32 nCmd, IN IMSList<ConfUser*>& objUsers,
            IN CallInfo& objCallInfo, IN MediaInfo& objMediaInfo,
            IN IMSMap<IMS_UINT32, SuppService*>& objSuppServices) = 0;
    virtual IMS_SINT32 GetState() const = 0;
    virtual IMS_UINT32 GetCallStatusInConference(IN CallKey nKey) const = 0;
};

#endif
