#ifndef INTERFACE_INTERFACE_HOLDER_LISTENER_H_
#define INTERFACE_INTERFACE_HOLDER_LISTENER_H_

class IInterfaceHolderListener
{
public:
    virtual void OnSessionInterfaceCleared() = 0;
    virtual void OnReferenceInterfaceCleared() = 0;
    virtual void OnSubscriptionInterfaceCleared() = 0;
};

#endif
