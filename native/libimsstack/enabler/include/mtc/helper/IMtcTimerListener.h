#ifndef INTERFACE_MTC_TIMER_LISTENER_H_
#define INTERFACE_MTC_TIMER_LISTENER_H_

class IMtcTimerListener
{
public:
    virtual void OnTimerExpired(IN IMS_SINT32 nType) = 0;
};
#endif
