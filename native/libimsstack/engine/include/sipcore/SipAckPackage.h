/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20140318  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _SIP_ACK_PACKAGE_H_
#define _SIP_ACK_PACKAGE_H_

#include "IMSList.h"
#include "SipStackHeaders.h"
#include "ISipAckPackage.h"

class SipClientTransactionState;
class SipAck;
class SIPAckPackagePrivate;

class SipAckPackage : public ISipAckPackage
{
private:
    SipAckPackage(IN CONST AString& strCallId_);

public:
    virtual ~SipAckPackage();

public:
    void AddAck(IN SipClientTransactionState* pCTState, IN IMS_SINT32 nAliveInterval);
    IMS_BOOL IsSamePackage(IN CONST AString& strCallId) const;
    IMS_BOOL NotifyStray2xx(IN ::SipTxnKey* pstTxnKey);

private:
    // ISipObject class
    virtual void Destroy();

    // ISipAckPackage class
    virtual void RemoveStrayAcks();

public:
    static SipAckPackage* CreateAckPackage(IN CONST AString& strCallId);
    static IMS_BOOL HandleStray2xx(IN ::SipMessage* pstMessage);
    static void Init();

private:
    AString strCallId;
    IMSList<SipAck*> objAcks;

    static SIPAckPackagePrivate* pAckPackageP;
};

#endif  // _SIP_ACK_PACKAGE_H_
