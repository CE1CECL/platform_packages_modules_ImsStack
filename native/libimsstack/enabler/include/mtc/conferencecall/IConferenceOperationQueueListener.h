#ifndef INTERFACE_CONFERENCE_OPERATION_QUEUE_LISTENER_H_
#define INTERFACE_CONFERENCE_OPERATION_QUEUE_LISTENER_H_

class IConferenceOperationQueueListener
{
public:
    virtual void OnOperationReady() = 0;
};

#endif
