/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100323  hwangoo.park@             Created
    </table>

    Description
     This class defines a base class for SIP dialog class.
*/

#ifndef _SIP_DIALOG_BASE_H_
#define _SIP_DIALOG_BASE_H_

#include "SipDialogState.h"

class SipDialogBase : public RCObject
{
public:
    explicit SipDialogBase(IN SipDialogState* pDState_);
    SipDialogBase(IN CONST SipDialogBase& objRHS);
    virtual ~SipDialogBase();

private:
    SipDialogBase();

public:
    SipDialogBase& operator=(IN CONST SipDialogBase& objRHS);

public:
    virtual IMS_BOOL OnInit() = 0;
    virtual void OnTerminated() = 0;
    virtual IMS_SINT32 OnUpdateDialogDetails(IN CONST SipMessageInfo& objMInfo,
            IN IMS_SINT32 nUsage, IN IMS_SINT32 nAction, IN IMS_SINT32 nTrigger) = 0;

    SipDialogState* GetDialogState() const;
    IMS_SINT32 GetState() const;

    static IMS_BOOL IsDialogCreatable(IN CONST SipMethod& objMethod);

private:
    // Shared dialog information among all dialog usages
    RCPtr<SipDialogState> pDState;
};

#endif  // _SIP_DIALOG_BASE_H_
