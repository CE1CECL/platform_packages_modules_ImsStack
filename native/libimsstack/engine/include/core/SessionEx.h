/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100615  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _SESSION_EX_H_
#define _SESSION_EX_H_

#include "ITimer.h"
#include "Session.h"

class IOnSessionExListener;
class ReliableProvResponseHelper;



class SessionEx
    : public Session
    , public ITimerListener
{
public:
    explicit SessionEx(IN Service *pService_);
    virtual ~SessionEx();

private:
    SessionEx(IN CONST SessionEx &objRHS);
    SessionEx& operator=(IN CONST SessionEx &objRHS);

public:
    IMS_RESULT RespondToEarlyUpdate(IN IMS_SINT32 nStatusCode,
            IN CONST AString &strReason = AString::ConstNull());
    IMS_RESULT RespondToPRAck(IN IMS_SINT32 nStatusCode,
            IN CONST AString &strReason = AString::ConstNull());
    IMS_RESULT SendPRAck();
    IMS_RESULT SendRPR(IN IMS_SINT32 nStatusCode,
            IN CONST AString &strReason = AString::ConstNull(), IN IMS_BOOL bSDP = IMS_TRUE,
            IN IMS_SINT32 nFlags = 0);
    IMS_RESULT UpdateEarlyMedia();
    void SetExListener(IN IOnSessionExListener *piListener);

protected:
    // Activity class
    virtual IMS_BOOL DispatchMessage(IN IMSMSG &objMSG);

    // Method class
    virtual IMS_BOOL NotifySIPRequest(IN ISIPServerConnection *piSSC);
    virtual void NotifySIPResponse(IN ISIPClientConnection *piSCC);
    virtual void NotifySIPError(IN ISIPConnection *piSC, IN IMS_SINT32 nCode,
            IN CONST AString &strMessage);
    virtual IMS_BOOL SendRequestToChallenge(IN ISIPClientConnection *piSCC);

    // IDialogMethod interface
    virtual IMS_BOOL Dialog_NotifyRequest(IN ISIPServerConnection *piSSC);

    // Session class
    virtual Session* CreateSession();
    virtual IMS_RESULT HandleProvisionalResponse(IN ISIPClientConnection *piSCC);
    virtual IMS_RESULT HandleRequestToUPDATE(IN ISIPServerConnection *piSSC);
    virtual IMS_RESULT HandleResponseToUPDATE(IN ISIPClientConnection *piSCC);
    virtual IMS_BOOL HasPendingPRAck() const;
    virtual IMS_BOOL IsEarlyUpdateInProgress() const;

    // ITimerListener
    virtual void Timer_TimerExpired(IN ITimer *piTimer);

private:
    AString AdjustSessionExpiresHeader(
            IN CONST AString &strRequestSE, IN CONST AString &strResponseSE);
    IMS_BOOL CheckNCreateRPRHelper(IN ISIPMessage *piSIPMsg);
    void DestroyRPRHelper();

    void HandleRequestToPRACK(IN ISIPServerConnection *piSSC);
    void HandleResponseToPRACK(IN ISIPClientConnection *piSCC);

    IMS_BOOL IsEarlyUpdateNotificationInProgress() const;
    void SetEarlyUpdateNotificationState(IN IMS_BOOL bInProgress);
    void SetEarlyState(IN IMS_UINT32 nState);

    IMS_BOOL IsIncomingEarlyUpdateReceivedInShortTime() const;
    void SetLastEarlyUpdateCompletedTime(IN IMS_SINT32 nExplicitTime = (-1));

    IMS_BOOL StartPendingUpdateTimer();
    void StopPendingUpdateTimer();

    static const IMS_CHAR* EarlyStateToString(IN IMS_SINT32 nState);

protected:
    enum
    {
        AMSG_SESSIONEX_INVITATION_RECEIVED = AMSG_SESSION_MAX,
        AMSG_SESSIONEX_EARLY_MEDIA_UPDATED,
        AMSG_SESSIONEX_EARLY_MEDIA_UPDATE_FAILED,
        AMSG_SESSIONEX_EARLY_MEDIA_UPDATE_RECEIVED,
        AMSG_SESSIONEX_PRACK_DELIVERED,
        AMSG_SESSIONEX_PRACK_DELIVERY_FAILED,
        AMSG_SESSIONEX_PRACK_RECEIVED,
        AMSG_SESSIONEX_RPR_DELIVERY_FAILED,
        AMSG_SESSIONEX_RPR_RECEIVED,

        AMSG_SESSIONEX_MAX
    };

private:
    enum
    {
        EARLY_STATE_IDLE = 0,
        EARLY_STATE_UPDATE_RECEIVED = 1,
        EARLY_STATE_UPDATE_SENT = 2
    };

    IMS_UINT32 nEarlyState;

    ReliableProvResponseHelper *pRPRHelper;
    IOnSessionExListener *piListener;

    // RACE_CONDITION: 200 OK to UPDATE and incoming UPDATE
    IMS_BOOL bFlag_EarlyUpdateNotificationInProgress;
    ISIPServerConnection *piSSC_PendingUpdate;
    ITimer *piTimer_PendingUpdate;
    IMS_UINT32 nLastEarlyUpdateCompletedTimeSec;
    IMS_UINT32 nLastEarlyUpdateCompletedTimeMicroSec;
};

#endif // _SESSION_EX_H_
