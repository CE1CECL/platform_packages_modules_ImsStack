#ifndef _AOS_MESSAGE_HANDLER_H_
#define _AOS_MESSAGE_HANDLER_H_

class AosMessage;
class IAosMsgHandlerListener;

#include "interface/IAosMsgHandler.h"
#include "ITimer.h"

class AosMsgHandler
    : public IAosMsgHandler
    , public ITimerListener
{
public:
    AosMsgHandler();
    virtual ~AosMsgHandler();

public:
    virtual IMS_BOOL SendEmptyMessageDelayed(IN CONST IAosMsgHandlerListener* piListener,
        IN IMS_SINT32 nMessage, IN IMS_SINT32 nDuration);
    virtual void RemoveMessages(IN CONST IAosMsgHandlerListener* piListener,
        IN IMS_SINT32 nMessage);
    virtual IMS_BOOL HasMessages(IN CONST IAosMsgHandlerListener* piListener,
        IN IMS_SINT32 nMessage);

    virtual void Timer_TimerExpired(IN ITimer *piTimer);

private:
    IMS_BOOL HasMessage(IN CONST IAosMsgHandlerListener* piListener,
        IN IMS_SINT32 nMessage, OUT IMS_SINT32& nAt);
    ITimer* StartTimer(IN IMS_SINT32 nDuration);
    void StopTimer(IN ITimer* piTimer);

private:
    IMSMap<ITimer*, AosMessage*> objMessages;
};

#endif // _AOS_MESSAGE_HANDLER_H_
