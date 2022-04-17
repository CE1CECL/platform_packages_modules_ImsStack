#ifndef TERMINATION_HANDLER_H_
#define TERMINATION_HANDLER_H_

#include "FailReason.h"
#include "IMSTypeDef.h"

class ISession;

class TerminationHandler final
{
public:
    explicit TerminationHandler();
    ~TerminationHandler();
    TerminationHandler(const TerminationHandler&) = delete;
    TerminationHandler& operator=(const TerminationHandler&) = delete;

    FailReason Handle(IN const ISession& objSession) const;

private:
    FailReason GetFailReasonFromSessionTerminationReason(IN IMS_SINT32 nTerminationReason) const;
};

#endif
