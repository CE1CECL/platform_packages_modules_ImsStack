#ifndef _INTERFACE_AOS_CALL_TRACKER_H_
#define _INTERFACE_AOS_CALL_TRACKER_H_

class IAosCallTrackerListener;

class IAosCallTracker
{
public:
    virtual IMS_BOOL IsCSCallActive() const = 0;
    virtual IMS_BOOL IsNormalCallActive() const = 0;
    virtual IMS_BOOL IsEmergencyCallActive() const = 0;

    // IsVideoCallingActive() returns true when
    // a video calling is in the state of OFFHOOK.
    virtual IMS_BOOL IsVideoCallingActive() const = 0;

    virtual IMS_SINT32 GetSlotId() const = 0;
    virtual IMS_UINT32 GetCallState(IN IMS_UINT32 nType) const = 0;
    virtual IMS_UINT32 GetSessionType(IN IMS_UINT32 nType) const = 0;

    virtual void SetCSCallStateWatchMode() = 0;
    virtual void SetActiveCSCallState(IN IMS_UINT32 nActiveCSState) = 0;

    virtual void SetListener(IN IAosCallTrackerListener *piListener) = 0;
    virtual void RemoveListener(IN IAosCallTrackerListener* piListener) = 0;

    enum
    {
        TYPE_CS = 0,
        TYPE_NORMAL,
        TYPE_EMERGENCY
    };

    enum
    {
        STATE_IDLE = 0,
        STATE_TERMINATING = 1,
        STATE_RINGBACK = 2,
        STATE_RINGING = 3,
        STATE_ALERTING = 4,
        STATE_OFFHOOK = 5
    };

    enum
    {
       SESSION_TYPE_NONE = 0x00000000,
       SESSION_TYPE_VOIP = 0x00000001,
       Session_TYPE_VS = 0x00000002,
       SESSION_TYPE_VT = 0x00000004
    };
};
#endif // _INTERFACE_AOS_CALL_TRACKER_H_
