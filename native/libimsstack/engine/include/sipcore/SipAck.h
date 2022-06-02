/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20140318  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _SIP_ACK_H_
#define _SIP_ACK_H_

#include "ITimer.h"
#include "SipClientTransactionState.h"

class SipAck : public ITimerListener
{
public:
    SipAck(IN SipClientTransactionState* pCTState_, IN IMS_SINT32 nAliveInterval);
    virtual ~SipAck();

private:
    SipAck();
    SipAck(IN CONST SipAck& objRHS);
    SipAck& operator=(IN CONST SipAck& objRHS);

public:
    IMS_BOOL IsSameTransaction(IN ::SipTxnKey* pstTxnKey) const;
    IMS_BOOL IsStrayAck() const;
    void RetransmitMessage();

private:
    // ITimerListener class
    virtual void Timer_TimerExpired(IN ITimer* piTimer);

private:
    RCPtr<SipClientTransactionState> pCTState;
    ITimer* piTimer;
};

#endif  // _SIP_ACK_H_
