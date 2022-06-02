/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20140304  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _SIP_IPSEC_STATE_H_
#define _SIP_IPSEC_STATE_H_

#include "EngineActivity.h"
#include "SipStackHeaders.h"
#include "SipTxnKey.h"
#include "SipTransportAddress.h"
#include "ISipIpSecState.h"

class SipIpSecState : public EngineActivity, public ISipIpSecState
{
private:
    class SA
    {
    public:
        SA();
        SA(IN CONST IPAddress& objIP_U_, IN IMS_SINT32 nPort_UC_, IN IMS_SINT32 nPort_US_,
                IN CONST IPAddress& objIP_P_, IN IMS_SINT32 nPort_PC_, IN IMS_SINT32 nPort_PS_);
        SA(IN CONST SA& objRHS);
        ~SA();

    private:
        SA& operator=(IN CONST SA& objRHS);

    public:
        IMS_BOOL AddTransaction(IN CONST sipcore::SipTxnKey* pTxnKey);
        IMS_BOOL CheckIPAddress(IN CONST SipTransportAddress& objNearEnd,
                IN CONST SipTransportAddress& objFarEnd) const;
        IMS_SINT32 GetSA(IN CONST SipTransportAddress& objNearEnd,
                IN CONST SipTransportAddress& objFarEnd, IN IMS_SINT32 nDirection) const;
        IMS_SINT32 GetState() const;
        IMS_BOOL HasPendingTransaction() const;
        IMS_BOOL RemoveTransaction(IN CONST sipcore::SipTxnKey* pTxnKey);
        void SetState(IN IMS_SINT32 nState);

    private:
        static const IMS_CHAR* StateToString(IN IMS_SINT32 nState);

    public:
        // Direction of SA
        enum
        {
            SA_IN = 1,
            SA_OUT = 2
        };

        // SA pairs
        enum
        {
            // src_port, dst_port, transport, direction
            SA_START = 1,

            SA_PUC_PPS_U_OUT = SA_START,
            SA_PUC_PPS_T_OUT,
            SA_PUS_PPC_T_OUT,
            SA_PPC_PUS_U_IN,
            SA_PPC_PUS_T_IN,
            SA_PPS_PUC_T_IN,

            SA_END
        };

        IPAddress objIP_U;
        IMS_SINT32 nPort_UC;
        IMS_SINT32 nPort_US;
        IPAddress objIP_P;
        IMS_SINT32 nPort_PC;
        IMS_SINT32 nPort_PS;

        IMS_SINT32 nState;
        // For tracking SIP transaction
        IMSList<sipcore::SipTxnKey> objSAStat;
    };

public:
    SipIpSecState();
    virtual ~SipIpSecState();

private:
    SipIpSecState(IN CONST SipIpSecState& objRHS);
    SipIpSecState& operator=(IN CONST SipIpSecState& objRHS);

public:
    IMS_BOOL IsIpSecEnabled() const;
    void NotifyMessageReceived(IN CONST SipTransportAddress& objNearEnd,
            IN CONST SipTransportAddress& objFarEnd, IN ::SipMessage* pstMessage);
    void NotifyMessageSent(IN CONST SipTransportAddress& objNearEnd,
            IN CONST SipTransportAddress& objFarEnd, IN ::SipMessage* pstMessage);
    void NotifyMessageSentFailed(IN ::SipMessage* pstMessage);
    void NotifyTransactionAborted(IN ::SipTxnKey* pstTxnKey);

private:
    // EngineActivity class
    virtual IMS_BOOL DispatchMessage(IN IMSMSG& objMSG);

    // ISipIpSecState class
    virtual void ClearIpSecSa(IN IMS_SINT32 nSAType);
    virtual IMS_SINT32 GetState(IN IMS_SINT32 nSAType) const;
    virtual IMS_BOOL HasPendingTransaction(IN IMS_SINT32 nSAType) const;
    virtual void SetIpSecSa(IN IMS_SINT32 nSAType, IN CONST IPAddress& objIP_U,
            IN IMS_SINT32 nPort_UC, IN IMS_SINT32 nPort_US, IN CONST IPAddress& objIP_P,
            IN IMS_SINT32 nPort_PC, IN IMS_SINT32 nPort_PS);
    virtual void SetListener(IN ISipIpSecStateListener* piListener);

    void NotifyMessageReceivedInternal(IN CONST SipTransportAddress& objNearEnd,
            IN CONST SipTransportAddress& objFarEnd, IN sipcore::SipTxnKey* pTxnKey);
    void NotifyMessageSentInternal(IN CONST SipTransportAddress& objNearEnd,
            IN CONST SipTransportAddress& objFarEnd, IN sipcore::SipTxnKey* pTxnKey);
    void NotifyTransactionAbortedInternal(IN sipcore::SipTxnKey* pTxnKey);

private:
    // Event for message processing
    enum
    {
        AMSG_NOTIFY_STATE_CHANGED = AMSG_USER
    };

    SA* pNewSA;
    SA* pOldSA;

    ISipIpSecStateListener* piListener;
};

#endif  // _SIP_IPSEC_STATE_H_
