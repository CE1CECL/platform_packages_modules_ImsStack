/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20140318  hwangoo.park@             Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "ServiceTrace.h"
#include "SipDebug.h"
#include "SipAck.h"
#include "SipAckPackage.h"

__IMS_TRACE_TAG_SIP__;

class SIPAckPackagePrivate
{
public:
    inline SIPAckPackagePrivate() {}

    inline ~SIPAckPackagePrivate()
    {
        for (IMS_UINT32 i = 0; i < objAckPackages.GetSize(); ++i)
        {
            SipAckPackage* pPackage = objAckPackages.GetAt(i);

            if (pPackage != IMS_NULL)
            {
                delete pPackage;
            }
        }

        objAckPackages.Clear();
    }

public:
    IMSList<SipAckPackage*> objAckPackages;
};

PRIVATE GLOBAL SIPAckPackagePrivate* SipAckPackage::pAckPackageP = IMS_NULL;

PRIVATE
SipAckPackage::SipAckPackage(IN CONST AString& strCallId_) :
        strCallId(strCallId_)
{
}

PUBLIC VIRTUAL SipAckPackage::~SipAckPackage()
{
    for (IMS_UINT32 i = 0; i < objAcks.GetSize(); ++i)
    {
        SipAck* pAck = objAcks.GetAt(i);

        if (pAck != IMS_NULL)
        {
            delete pAck;
        }
    }

    objAcks.Clear();
}

/*

Remarks

*/
PUBLIC
void SipAckPackage::AddAck(IN SipClientTransactionState* pCTState, IN IMS_SINT32 nAliveInterval)
{
    //---------------------------------------------------------------------------------------------

    if (pCTState == IMS_NULL)
    {
        return;
    }

    ::SipTxnKey* pstTxnKey = pCTState->GetTxnKey();

    if (pstTxnKey == IMS_NULL)
    {
        IMS_TRACE_E(0, "SipAckPackage :: no txn key", 0, 0, 0);
        return;
    }

    for (IMS_UINT32 i = 0; i < objAcks.GetSize(); ++i)
    {
        SipAck* pAck = objAcks.GetAt(i);

        if (pAck == IMS_NULL)
        {
            continue;
        }

        if (pAck->IsSameTransaction(pstTxnKey))
        {
            // ACK already exists in the ACK package
            IMS_TRACE_D("SipAckPackage :: SipAck already exists (%s)",
                    SipStack::TxnKey_GetViaBranch(pstTxnKey), 0, 0);
            return;
        }
    }

    IMS_TRACE_D("SipAckPackage :: SipAck (%s, %d)", SipStack::TxnKey_GetViaBranch(pstTxnKey),
            nAliveInterval, 0);

    objAcks.Append(new SipAck(pCTState, nAliveInterval));
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipAckPackage::IsSamePackage(IN CONST AString& strCallId) const
{
    //---------------------------------------------------------------------------------------------

    return this->strCallId.Equals(strCallId);
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipAckPackage::NotifyStray2xx(IN ::SipTxnKey* pstTxnKey)
{
    //---------------------------------------------------------------------------------------------

    for (IMS_UINT32 i = 0; i < objAcks.GetSize(); ++i)
    {
        SipAck* pAck = objAcks.GetAt(i);

        if (pAck == IMS_NULL)
        {
            continue;
        }

        if (pAck->IsStrayAck())
        {
            continue;
        }

        if (pAck->IsSameTransaction(pstTxnKey))
        {
            pAck->RetransmitMessage();
            return IMS_TRUE;
        }
    }

    return IMS_FALSE;
}

/*

Remarks

*/
PRIVATE VIRTUAL void SipAckPackage::Destroy()
{
    //---------------------------------------------------------------------------------------------

    if (pAckPackageP == IMS_NULL)
    {
        IMS_TRACE_D("SipAckPackage (%s) is destroyed",
                SipDebug::GetCharA1(strCallId.GetStr(), 8, '@'), 0, 0);
        delete this;
        return;
    }

    for (IMS_UINT32 i = 0; i < pAckPackageP->objAckPackages.GetSize(); ++i)
    {
        SipAckPackage* pPackage = pAckPackageP->objAckPackages.GetAt(i);

        if (pPackage == IMS_NULL)
        {
            continue;
        }

        if (pPackage->IsSamePackage(strCallId))
        {
            IMS_TRACE_D("SipAckPackage (%s) is destroyed",
                    SipDebug::GetCharA1(strCallId.GetStr(), 8, '@'), 0, 0);

            pAckPackageP->objAckPackages.RemoveAt(i);
            delete this;
            break;
        }
    }

    IMS_TRACE_D("SipAckPackage :: size=%d", pAckPackageP->objAckPackages.GetSize(), 0, 0);
}

/*

Remarks

*/
PRIVATE VIRTUAL void SipAckPackage::RemoveStrayAcks()
{
    IMS_UINT32 nTotalCount = objAcks.GetSize();

    //---------------------------------------------------------------------------------------------

    for (IMS_UINT32 i = 0; i < objAcks.GetSize();)
    {
        SipAck* pAck = objAcks.GetAt(i);

        if (pAck == IMS_NULL)
        {
            objAcks.RemoveAt(i);
            continue;
        }

        if (pAck->IsStrayAck())
        {
            objAcks.RemoveAt(i);
            delete pAck;
        }
        else
        {
            ++i;
        }
    }

    if (objAcks.GetSize() < nTotalCount)
    {
        IMS_TRACE_D("SipAckPackage :: ACK (%d >> %d)", nTotalCount, objAcks.GetSize(), 0);
    }
}

/*

Remarks

*/
PUBLIC GLOBAL SipAckPackage* SipAckPackage::CreateAckPackage(IN CONST AString& strCallId)
{
    //---------------------------------------------------------------------------------------------

    for (IMS_UINT32 i = 0; i < pAckPackageP->objAckPackages.GetSize(); ++i)
    {
        SipAckPackage* pPackage = pAckPackageP->objAckPackages.GetAt(i);

        if (pPackage == IMS_NULL)
        {
            continue;
        }

        if (pPackage->IsSamePackage(strCallId))
        {
            IMS_TRACE_D("SipAckPackage :: RE-USE (%s)",
                    SipDebug::GetCharA1(strCallId.GetStr(), 8, '@'), 0, 0);
            return pPackage;
        }
    }

    SipAckPackage* pNewPackage = new SipAckPackage(strCallId);

    pAckPackageP->objAckPackages.Append(pNewPackage);

    IMS_TRACE_D(
            "SipAckPackage (%s) is created", SipDebug::GetCharA1(strCallId.GetStr(), 8, '@'), 0, 0);

    return pNewPackage;
}

/*

Remarks

*/
PUBLIC GLOBAL void SipAckPackage::Init()
{
    //---------------------------------------------------------------------------------------------

    if (pAckPackageP == IMS_NULL)
    {
        pAckPackageP = new SIPAckPackagePrivate();
    }
}

/*

Remarks

*/
PUBLIC GLOBAL IMS_BOOL SipAckPackage::HandleStray2xx(IN ::SipMessage* pstMessage)
{
    //---------------------------------------------------------------------------------------------

    if (pAckPackageP == IMS_NULL)
    {
        return IMS_FALSE;
    }

    ::SipTxnKey* pstTxnKey = SipStack::CreateTxnKey(pstMessage, SipStack::SIP_TXN_MSG_RECEIVED);

    if (pstTxnKey == IMS_NULL)
    {
        IMS_TRACE_D("SipAckPackage :: Stray 2xx is discarded", 0, 0, 0);
        return IMS_FALSE;
    }

    AString strCallId(SipStack::TxnKey_GetCallId(pstTxnKey));
    IMS_BOOL bStray2xxHandled = IMS_FALSE;

    for (IMS_UINT32 i = 0; i < pAckPackageP->objAckPackages.GetSize(); ++i)
    {
        SipAckPackage* pPackage = pAckPackageP->objAckPackages.GetAt(i);

        if (pPackage == IMS_NULL)
        {
            continue;
        }

        if (pPackage->IsSamePackage(strCallId))
        {
            bStray2xxHandled = pPackage->NotifyStray2xx(pstTxnKey);

            if (!bStray2xxHandled)
            {
                IMS_TRACE_D("SipAckPackage :: ACK (%s) is not handled",
                        SipDebug::GetCharA1(strCallId.GetStr(), 8, '@'), 0, 0);
            }
            break;
        }
    }

    SipStack::FreeTxnKey(pstTxnKey);

    return bStray2xxHandled;
}
